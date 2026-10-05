// Copyright (C) 2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Target: ainst (exe) -- Install third-party software described by dev.extpkg
// Exceptions: yes
// Source: cpp/ainst/kafka.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install the kafka broker under /opt/kafka, beside the jmx exporter it runs with.
// Each release unpacks under its own versioned directory and a "current"
// symlink names the one in force, so an upgrade is a symlink move.
void ainst::extpkg_kafka(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,tempstr()<<"dir=$(ainst_unpack "<<GetExtpkgsrcVar(extpkg,"tgz")<<")");
    AddStage(extpkg,"ainst_mkdir /opt/kafka/versions");
    AddStage(extpkg,"cp -r \"$dir/kafka_2.13-3.9.0\" \"$(ainst_root /opt/kafka/versions)/\"");
    AddStage(extpkg,"chmod -R a+rX \"$(ainst_root /opt/kafka/versions/kafka_2.13-3.9.0)\"");
    AddStage(extpkg,"ln -sfn /opt/kafka/versions/kafka_2.13-3.9.0 \"$(ainst_root /opt/kafka/current)\"");
    AddStage(extpkg,tempstr()<<"ainst_install 644 "<<GetExtpkgsrcVar(extpkg,"prom")<<" /opt/kafka/prometheus/jmx_prometheus_javaagent-0.3.1.jar");
    AddStage(extpkg,"ln -sfn /opt/kafka/prometheus/jmx_prometheus_javaagent-0.3.1.jar \"$(ainst_root /opt/kafka/prometheus/current.jar)\"");
}
