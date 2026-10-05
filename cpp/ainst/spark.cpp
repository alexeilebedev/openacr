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
// Source: cpp/ainst/spark.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Set up the spark account's home and add the jars the image reads and writes s3 and delta through.
void ainst::extpkg_spark(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,tempstr()<<"ainst_install 644 "<<GetExtpkgsrcVar(extpkg,"delta_spark")<<" /opt/spark/jars/io.delta_delta-spark_2.13-4.0.0.jar");
    AddStage(extpkg,tempstr()<<"ainst_install 644 "<<GetExtpkgsrcVar(extpkg,"delta_storage")<<" /opt/spark/jars/io.delta_delta-storage-4.0.0.jar");
    AddStage(extpkg,tempstr()<<"ainst_install 644 "<<GetExtpkgsrcVar(extpkg,"antlr")<<" /opt/spark/jars/org.antlr_antlr4-runtime-4.13.1.jar");
    AddStage(extpkg,tempstr()<<"ainst_install 644 "<<GetExtpkgsrcVar(extpkg,"hadoop_aws")<<" /opt/spark/jars/org.apache.hadoop_hadoop-aws-3.4.0.jar");
    AddStage(extpkg,tempstr()<<"ainst_install 644 "<<GetExtpkgsrcVar(extpkg,"awssdk")<<" /opt/spark/jars/software.amazon.awssdk_bundle-2.23.19.jar");
    AddStage(extpkg,tempstr()<<"ainst_install 644 "<<GetExtpkgsrcVar(extpkg,"wildfly")<<" /opt/spark/jars/org.wildfly.openssl_wildfly-openssl-1.1.3.Final.jar");
    AddStage(extpkg,tempstr()<<"ainst_install 644 "<<GetExtpkgsrcVar(extpkg,"postgresql")<<" /opt/spark/jars/postgresql-42.7.4.jar");
    AddStageFileFrom(extpkg,"/home/spark/.bashrc","conf/ainst/spark/bashrc");
    AddPost(extpkg,"chmod 700 /home/spark");
    AddPost(extpkg,"usermod -d /home/spark -s /bin/bash spark");
    AddPost(extpkg,"for d in conf examples python jars bin sbin; do ln -sf \"/opt/spark/$d\" \"/home/spark/$d\" || true; done");
    AddPost(extpkg,"echo '[[ -z \"$BASH\" ]] || cd ~' >> /home/spark/.bash_profile || true");
    AddPost(extpkg,"chown -R 185:185 /opt/spark/jars /home/spark");
}
