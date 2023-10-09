#!/usr/bin/env perl
# Copyright (C) 2008-2012 AlgoEngineering LLC
# Copyright (C) 2013-2014 NYSE | Intercontinental Exchange
# Copyright (C) 2023 AlgoRND
#
# License: GPL
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# Contacting ICE: <https://www.theice.com/contact>
# zfs snapshot manager.

use strict;
use warnings;

use Date::Parse;
use POSIX;
use Cwd 'abs_path';
use File::Basename;
use Getopt::Long;
our $my_dir = dirname(abs_path($0));
require "$my_dir/functions.pl";

my $usage = "$0 <cmd>
    snapshot        take snapshots of all pools
    filter          filter snapshots
    maintenance     snapshot + filter

    -verbose        extra printouts
    -dry_run        don't create/destroy snapshots. just print
    ";

our $dry_run;
our $verbose;
GetOptions(
    "verbose"   => \$verbose
    , "dry_run" => \$dry_run
    )
    or die "$0: GetOptions error. Usage:\n$usage";

my $err = 0;

sub get_pools {
    my @pools=();
    for my $line(split(/\n/,`zpool list`)) {
        my $name = (split(/\s+/,$line))[0];
        if ($name eq "NAME") {
            next;
        }
        # only accept valid identifiers
        if ($name !~ /^[a-zA-Z0-9_]+$/) {
            next;
        }
        push(@pools,$name);
    }
    return @pools;
}

sub take_snapshots {
    for my $pool (@_) {
        my $date = strftime("%Y-%m-%d",localtime());
        my $cmd = "zfs snapshot -r $pool\@snap_$date";
        if ($dry_run) {
            print "(dry run) $cmd\n";
        } else {
            sys_cmd_fail_ok($cmd);
        }
    }
}

sub sort2_inplace_reverse {
    my ($keys,$ary)=@_;
    scalar(@$keys) == scalar(@$ary) or die "sort2: arrays must be of same length\n";
    my $N=scalar(@$keys);
    my @idxs = sort { $keys->[$a] <=> $keys->[$b] } 0..scalar($N)-1;
    @$ary = reverse(@{$ary}[@idxs]);
    @$keys = reverse(@{$keys}[@idxs]);
}

#
# our snapshots follow a formula: @snap_YYYY-mm-dd
# this function obtains a list of current snapshots and groups it by
# dataset.
# it then sorts snapshots within each group by date.
# the age of each snapshot in days is determined, and a snapshot is kept
# if it fits the budget
#
# the budget is given by an exponential function that determines the maximum number of snapshots
# of a given age.
#
# for power=0.7, there can be at most
# 62 snapshots of age 365 days
# 10 snapshots of age 31 days
#
# at least k snapshots are always kept around.
#
#
sub filter_snapshots() {
    my $KEEP_SNAPS = 30;
    my $DECAY = 0.9;
    my $SNAPSHOT_FREQUENCY = 60*60;
    $verbose && print "figuring out snapshots...\n";
    my %snapshots=();
    for my $line(split(/\n/,`zfs list -t snapshot`)) {
        my $name = (split(/\s+/,$line))[0];
        if ($name eq "NAME") {
            next;
        }
        # only accept valid identifiers
        if ($name !~ /^[a-zA-Z0-9_@\/-]+$/) {
            print "skipping identifier [$name]\n";
            next;
        }
        if ($name =~ /(.*)\@snap_(\d\d\d\d-\d\d-\d\d)/) {
            push(@{$snapshots{$1}}, $2);
        }
    }
    foreach my $key(keys(%snapshots)) {
        my @ary = @{$snapshots{$key}};
        my @unix_stamps;
        foreach my $date(@ary) {
            my $dt = Date::Parse::str2time($date);
            $dt or die "Unexpected internal error. Error parsing date $date";
            push(@unix_stamps,$dt);
        }
        sort2_inplace_reverse(\@unix_stamps,\@ary);
        my $now=time();
        my $position = 0;
        for (my $i=0; $i<scalar(@ary); $i++) {
            my $age = ($now - $unix_stamps[$i]) / $SNAPSHOT_FREQUENCY;
            if ($age<0) {
                die "problem with timestamps";
            }
            my $limit = POSIX::pow($age, $DECAY);
            my $too_old = ($position >= $KEEP_SNAPS && $position > $limit);
            $verbose && print "snapshot $key: age $age, position $position, limit $limit\n";
            if ($too_old) {
                print "    snapshot $key : $ary[$i] : too old.\n";
                if (!$dry_run) {
                    sys_cmd("zfs destroy $key\@snap_$ary[$i]");
                }
            } else {
                $verbose && print "    snapshot $key : $ary[$i] : ok\n";
                $position++;
            }
        }
    }
    #print join("\n",@snapshots);
}

my $cmd=shift;
$cmd or die $usage;

if ($cmd eq "snapshot") {
    take_snapshots(get_pools());
    exit 0;
}

if ($cmd eq "list") {
    my $date = strftime("%Y-%m-%d",localtime());
    print "date is $date\n";
    my @pools = get_pools();
    print "@pools\n";
    exit 0;
}

if ($cmd eq "filter") {
    filter_snapshots();
    exit 0;
}

if ($cmd eq "maintenance") {
    take_snapshots(get_pools());
    filter_snapshots();
    print "---------------------------------------- Current Snapshots ----------------------------------------\n";
    sys_cmd("zfs list -t snapshot");
    exit 0;
}

die $usage;
