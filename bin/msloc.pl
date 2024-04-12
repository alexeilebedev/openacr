#!/usr/bin/env perl
# Copyright (C) 2023 AlgoRND
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
# run this tool from root to get line count statistics

(-f ".ffroot") or die "please run from root directory\n";

print "--------------------------------------------------------------------------------\n";
print "non-test, non-auto-generated lines of code\n";
system(q+sloc.pl `ff -d cpp -d include -p '!gen/|sql/' -l`+);
