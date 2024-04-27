#!/usr/bin/env perl
# Copyright (C) 2023-2024 AlgoRND
# Copyright (C) 2013-2014 NYSE | Intercontinental Exchange
# Copyright (C) 2008-2012 AlgoEngineering LLC
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
# usage: sloc.pl <list of files>
# LOC = line of code; i.e. non-comment line
# this program is just a variation on wc -l. the most important
# number it produces if 'total non-comment lines'.
# if list of files is not specified, and ff is installed, it is used.
# if no ff, sloc will consider all files under current directory,
# filtering out non-text files with -T
# PER FILE metrics use total # of files, so they don't make sense
# 'for files of type XX'.
# interestign links:
# http://vimeo.com/9270320
# the curly braces count approximate number of basic blocks
# total # of chars is the 'reading cost' of the code.
# warnings about few unique lines of code indicate a copy-paste problem.

@files = @ARGV;
@files = split(/\n/, `ff -l 2>/dev/null`) if !@files;
@files = split(/\n/, `find . -type f | grep -v '.svn'`) if !@files;
my %counts;
my %linehist;
my %wordhist;
my %struct;
sub bool { return $_[0] ? 1:0; }
sub trim { $_=$_[0]; s/\s+$//; s/^\s+//; $_; }
$nlines = \$counts{'011.total non-comment lines'};
for (@files) {
    next if !-T;
    chomp;
    $ext           = (m/\.([\.a-zA-Z]+)$/ ? $1 : undef);
    my $fname      = $_;
    open R, "<$fname" or print "!$\n";
    $n_uniq_flines = 0;
    $n_flines      = 0;
    while (<R>) {
        s/\t/    /g;
        $counts{'010.total text lines'}    += 1;
        $cmt                                = bool(m!^\s*(//|#|/\*)!);
        $counts{'050.total chars'}         += length($_);
        $counts{'041.comment lines'}       += $cmt;
        $counts{'043.#includes'}           += bool(/^\s*#?include/);
        $counts{'044.#define'}             += () = /\#define\b/;
        next if $cmt;
        $$nlines                           += 1;

        $counts{"022.4x+ indented lines"}   += bool(/^\s{4}/);
        $counts{"023.8x+ indented lines"}   += bool(/^\s{8}/);
        $counts{"024.12x+ indented lines"}  += bool(/^\s{12}/);
        $counts{"025.16x+ indented lines"}  += bool(/^\s{16}/);
        $counts{"026.32x+ indented lines"}  += bool(/^\s{32}/);

        $counts{"027.lines > 120 chars"}   += bool(length($_) > 120);
        $counts{"028.lines > 80 chars"}    += bool(length($_) > 80);

        $counts{'060.curly braces'}        += () = m/{|}/;
        $counts{'062.parentheses'}         += () = m/\(|\)/;
        $counts{'063.()'}                  += () = m/\(\)/;
        $counts{'064.leading _s'}          += () = m/\b_[^g]/;

        $counts{'070.virtual'}             += () = /\bvirtual\b/;
        $counts{'071.template'}            += () = /\btemplate\b|\bTTU?i?\b/;
        $counts{'072.public/private'}      += () = /\b(public|private|protected)\b/;

        $counts{'100.enum declarations'}   += bool(/enum/ && !/;/);

        $counts{'120.prlog/prerr'}         += () = /\bprlog\b|\bprerr\b/;
        $counts{'120.vrfy/throw'}          += () = /\bvrfy\b|\bthrow\b/;
        $counts{'121.bck/assert'}          += () = /\b(bck|assert)\b/;
        $counts{'122.static assert'}       += () = /static_assert|static_assert/;
        $counts{'123.if'}                  += () = /\bif\b/;
        $counts{'124.switch'}              += () = /\bswitch\b/;
        $counts{'124.break/continue'}      += () = /\bbreak|continue\b/;
        $counts{'125.while/for'}           += () = /\bwhile\b|\bfor\b/;

        if (length($_) > 6) {
            $n_uniq_flines                 += !$linehist{trim($_)};
            $linehist{trim($_)}            += 1;
            $n_flines                      += 1;
        }
        $struct{$2}++   if /(struct|class|interface)\s+(\w+)/ && !/typedef/;
        map { $wordhist{$_}++ if /^\w/ } split(/[^\w]+/,$_);
    }
    if ($n_flines > 100 && $n_uniq_flines < $n_flines / 3) {
        print STDERR "WARNING: $n_flines lines, only $n_uniq_flines unique in $fname\n";
    }
    if ($ext && length($ext) < 6) {
        $counts{"014.    in '$ext'"}   += $n_uniq_flines;
    }
}

# count word frequency
my @wcount;
while (($k,$v) = each %wordhist) { push(@wcount, [$k,$v]) if $v>0; }
@wcount = sort { $b->[1] <=> $a->[1] } @wcount;

my @lcount;
while (($k,$v) = each %linehist) { push(@lcount, [trim($k),$v]) if $v>0; }
@lcount = sort { $b->[1] <=> $a->[1] } @lcount;

print "- unique lines are those with at least 10 chars\n";
my $nfiles = @files;
$counts{'009.total files'}                   = $nfiles;
$counts{'013.unique non-comment lines'}      = scalar(keys %linehist);
$counts{'091.distinct words'}                = scalar(@wcount);
$counts{'090.struct declarations'}           = scalar(keys %struct);

# -----------------------------------------------------------------------------
# print main screen

print sprintf("%-30s %-15s %-15s %-15s\n", "METRIC", "COUNT", "% PER LOC", "PER FILE");
if (!$nfiles || !$$nlines) { die "empty set\n"; }
for $k (sort keys %counts) {
    $k =~ /^\d+\.(.*)/;
    $c = $counts{$k};
    print sprintf("%-30s %-15s %-15.3f %-15.3f\n", $1, $c, $c * 100.0 / $$nlines, $c / $nfiles);
}
print "------------------------------------------------------------\n";
print "frequent words:\n", join(", ", map { "$_->[0] ($_->[1])" } @wcount[0..50]), "\n";
print "------------------------------------------------------------\n";
print "infrequent words:\n", join(", ", map { "$_->[0] ($_->[1])" } (reverse @wcount)[0..50]), "\n";
print "------------------------------------------------------------\n";
print "frequent lines:\n", join("\n", map { "$_->[0] ($_->[1])" } @lcount[0..20]), "\n";
