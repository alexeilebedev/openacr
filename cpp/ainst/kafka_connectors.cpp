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
// Source: cpp/ainst/kafka_connectors.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install the s3 and iceberg connectors kafka connect loads from /opt/kafka/plugins.
// Iceberg reads its version from version.txt when that file is present, and
// otherwise derives one from the newest tag by bumping the minor and appending
// -SNAPSHOT -- so a tree at apache-iceberg-1.11.0 builds itself as
// 1.12.0-SNAPSHOT, which is neither the release nor a name this can predict.
// The apache source release carries version.txt for that reason, and so does
// this.
void ainst::extpkg_kafka_connectors(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,"aiven=$(ainst_root /opt/kafka/plugins/aiven-kafka-connect-s3)");
    AddStage(extpkg,"confl=$(ainst_root /opt/kafka/plugins/confluent-kafka-connect-s3)");
    AddStage(extpkg,"ainst_mkdir /opt/kafka/plugins/aiven-kafka-connect-s3");
    AddStage(extpkg,"ainst_mkdir /opt/kafka/plugins/confluent-kafka-connect-s3");
    AddStage(extpkg,"ainst_mkdir /opt/kafka/plugins/iceberg");
    AddStage(extpkg,tempstr()<<"unzip -oq "<<GetExtpkgsrcVar(extpkg,"aiven_sink")<<" -d \"$aiven\"");
    AddStage(extpkg,tempstr()<<"unzip -oq "<<GetExtpkgsrcVar(extpkg,"aiven_source")<<" -d \"$aiven\"");
    AddStage(extpkg,tempstr()<<"unzip -oq "<<GetExtpkgsrcVar(extpkg,"confluent_sink")<<" -d \"$confl\"");
    AddStage(extpkg,tempstr()<<"unzip -oq "<<GetExtpkgsrcVar(extpkg,"confluent_source")<<" -d \"$confl\"");
    AddStage(extpkg,tempstr()<<"ice=$(ainst_unpack "<<GetExtpkgsrcVar(extpkg,"iceberg")<<")/iceberg-apache-iceberg-1.11.0");
    AddStage(extpkg,"echo 1.11.0 > \"$ice/version.txt\"");
    AddStage(extpkg,"(cd \"$ice\" && ./gradlew build -x test -x integrationTest)");
    AddStage(extpkg,"unzip -oq \"$ice/kafka-connect/kafka-connect-runtime/build/distributions"
             "/iceberg-kafka-connect-runtime-1.11.0.zip\" -d \"$(ainst_root /opt/kafka/plugins/iceberg)\"");
    AddStage(extpkg,"(cd \"$ice\" && ./gradlew :iceberg-open-api:shadowJar)");
    AddStage(extpkg,"cp \"$ice\"/open-api/build/libs/iceberg-open-api-test-fixtures-runtime-*.jar"
             " \"$(ainst_root /opt/kafka/plugins/iceberg)\"");
    AddStage(extpkg,"find \"$(ainst_root /opt/kafka/plugins)\" -type d -exec chmod 755 {} \\;");
    AddStage(extpkg,"find \"$(ainst_root /opt/kafka/plugins)\" -type f -exec chmod 644 {} \\;");
    AddPost(extpkg,"chown -R 1101:1101 /opt/kafka");
}
