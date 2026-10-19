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
// Source: cpp/ainst/java.cpp
//

#include "include/algo.h"
#include "include/ainst.h"


// -----------------------------------------------------------------------------
// Install a Java runtime under /opt/java, point /opt/java/current at its home,
// and put its java on PATH.  The runtime is Temurin's build of the OpenJDK
// release the distribution ships, so Linux and macOS run the same one.  A macOS
// runtime is a bundle whose home is Contents/Home, so /opt/java/current names
// the home on both, and JAVA_HOME=/opt/java/current serves either.
//
// On Linux /usr/bin/java is the distribution's alternatives link, which a JDK
// deb also claims, and a build tool such as maven finds its javac through it.
// So the runtime joins the alternatives at the lowest priority: it is the java
// on PATH where nothing else provides one, and a JDK the host also carries stays
// the default.  On macOS $PREFIX is /usr/local, which no package manager owns,
// and the java there is a plain link.
void ainst::extpkg_java(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,tempstr()<<"dir=$(ainst_unpack "<<GetExtpkgsrcVar(extpkg,"tgz")<<")/jdk-17.0.20+8-jre");
    AddStage(extpkg,"ainst_mkdir /opt/java");
    AddStage(extpkg,"rm -rf \"$(ainst_root /opt/java/jdk-17.0.20+8-jre)\"");
    AddStage(extpkg,"cp -R \"$dir\" \"$(ainst_root /opt/java/)\"");
    AddStage(extpkg,"chmod -R a+rX \"$(ainst_root /opt/java/jdk-17.0.20+8-jre)\"");
    AddStage(extpkg,"home=/opt/java/jdk-17.0.20+8-jre");
    AddStage(extpkg,"if [[ $DARWIN == Y ]]; then home=$home/Contents/Home; fi");
    AddStage(extpkg,"ln -sfn \"$home\" \"$(ainst_root /opt/java/current)\"");
    AddStage(extpkg,"if [[ $DARWIN == Y ]]; then");
    AddStage(extpkg,"    ainst_mkdir \"$PREFIX/bin\"");
    AddStage(extpkg,"    ln -sfn /opt/java/current/bin/java \"$(ainst_root \"$PREFIX/bin/java\")\"");
    AddStage(extpkg,"fi");
    AddPost(extpkg,"if [[ $DARWIN == N ]]; then update-alternatives --install /usr/bin/java java /opt/java/current/bin/java 1; fi");
}
