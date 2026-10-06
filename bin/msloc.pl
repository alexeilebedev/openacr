#!/usr/bin/env perl
# Copyright (C) 2023-2024 AlgoRND
# Copyright (C) 2013-2014 NYSE | Intercontinental Exchange
# Copyright (C) 2008-2012 AlgoEngineering LLC
#
# License: Apache
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Contacting ICE: <https://www.theice.com/contact>
# run this tool from root to get line count statistics

(-f ".ffroot") or die "please run from root directory\n";

print "--------------------------------------------------------------------------------\n";
print "non-test, non-auto-generated lines of code\n";
system(q+sloc.pl `ff -d cpp -d include -p '!gen/|sql/' -l`+);
