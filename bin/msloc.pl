#!/usr/bin/env perl
#
# AOS
# (C) AlgoEngineering LLC 2008-2012
# (C) NYSE | Intercontinental Exchange 2013-2014
#
#
# run this tool from root to get line count statistics
#

(-f ".ffroot") or die "please run from root directory\n";

print "--------------------------------------------------------------------------------\n";
print "non-test, non-auto-generated lines of code\n";
system(q+sloc.pl `ff -d cpp -d include -p '!gen/|sql/' -l`+);
