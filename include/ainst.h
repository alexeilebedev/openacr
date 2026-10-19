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
// Header: include/ainst.h
//

#include "include/gen/ainst_gen.h"
#include "include/gen/ainst_gen.inl.h"

namespace ainst { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/ainst/ainst.cpp
    //

    // Select the installations, then do the one thing the command line asked for.
    // The actions share their whole body: each is the same generated script,
    // printed whole or as its function library alone, run, run under a buildroot
    // and packaged, or staged under scratch roots to see which packages still build.
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:ainst

    // -------------------------------------------------------------------
    // cpp/ainst/aws.cpp
    //

    // Install version 2 of the aws command line client.
    // The vendor's own installer takes both of its destinations as options, so it
    // stages into a buildroot exactly as it installs into a live system.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_aws(ainst::FExtpkg &extpkg); // dev.extpkg:aws

    // -------------------------------------------------------------------
    // cpp/ainst/buildkit.cpp
    //

    // Install the buildkit daemon and its client, and the unit that runs the daemon.
    // buildkit is versioned by moby and nerdctl by containerd, so they are two
    // packages: one package carries one version, and a package holding both could
    // be moved to a new release of neither.
    // The unit wants containerd and is PartOf it.  A containerd restart, which the
    // kubelet routine performs, then restarts buildkitd with it.  On a rebooted host
    // containerd's first start loses a race with loopback and the retry five
    // seconds later succeeds.  With Wants that first failure leaves buildkit's own
    // start job to run; Requires would fail it and leave buildkit stopped.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_buildkit(ainst::FExtpkg &extpkg); // dev.extpkg:buildkit

    // -------------------------------------------------------------------
    // cpp/ainst/certbot.cpp
    //

    // Install certbot and its nginx plugin into a virtualenv of their own.
    // A virtualenv records the absolute path it was created at in every script it
    // writes, so it cannot be built in a buildroot and moved.  The whole
    // installation is therefore a post step, and its rpm is that scriptlet.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_certbot(ainst::FExtpkg &extpkg); // dev.extpkg:certbot

    // -------------------------------------------------------------------
    // cpp/ainst/chrome.cpp
    //

    // Install google chrome from the vendor apt repository.
    // The distro's own chromium and firefox on 24.04 are stub packages that
    // pre-depend on snapd and install a snap, which a container has no daemon to
    // run, so neither is a browser anywhere this image is used.  Chrome ships a
    // real deb, and headless is a flag on it rather than a separate build.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_chrome(ainst::FExtpkg &extpkg); // dev.extpkg:chrome

    // -------------------------------------------------------------------
    // cpp/ainst/docker.cpp
    //

    // Install the docker engine from the vendor's apt repository.
    // The vendor publishes a script meant to be piped into a shell, and all it does
    // is add one repository, so the key is an artifact with a sha256 and the
    // sources.list line is written directly.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_docker(ainst::FExtpkg &extpkg); // dev.extpkg:docker

    // -------------------------------------------------------------------
    // cpp/ainst/gitlab_runner.cpp
    //

    // Install gitlab-runner, the agent that runs this project's CI jobs.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_gitlab_runner(ainst::FExtpkg &extpkg); // dev.extpkg:gitlab_runner

    // -------------------------------------------------------------------
    // cpp/ainst/glow.cpp
    //

    // Install glow, which renders markdown in a terminal.
    // The release archive wraps its files in one directory named for the release
    // and the architecture, glow_3.0.0_Linux_x86_64 for one, and the glob reaches
    // the binary inside whichever directory the fetched archive carries, so the
    // script spells neither and -update can move the version on its own.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_glow(ainst::FExtpkg &extpkg); // dev.extpkg:glow

    // -------------------------------------------------------------------
    // cpp/ainst/go.cpp
    //

    // Install the go toolchain under /opt/go, with go and gofmt on the path.  Each
    // release unpacks into its own versioned directory and a "current" symlink names
    // the one in force, so an upgrade moves that symlink.
    //
    // The links in /usr/local/bin need no GOROOT: go resolves the symlinks above its
    // own binary and reads the root off the path it lands on.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_go(ainst::FExtpkg &extpkg); // dev.extpkg:go

    // -------------------------------------------------------------------
    // cpp/ainst/grpcurl.cpp
    //

    // Install grpcurl, a grpc client for the command line.
    // The release is a deb carrying one binary, so it is unpacked rather than given
    // to dpkg: the file lands under the staging root either way, and nothing here
    // needs a package manager's database.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_grpcurl(ainst::FExtpkg &extpkg); // dev.extpkg:grpcurl

    // -------------------------------------------------------------------
    // cpp/ainst/java.cpp
    //

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
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_java(ainst::FExtpkg &extpkg); // dev.extpkg:java

    // -------------------------------------------------------------------
    // cpp/ainst/jq.cpp
    //

    // Install jq into $PREFIX/bin from the upstream release, a static binary per
    // system and architecture.  The release replaces the distribution's package, so
    // Linux and macOS run the same version.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_jq(ainst::FExtpkg &extpkg); // dev.extpkg:jq

    // -------------------------------------------------------------------
    // cpp/ainst/kafka.cpp
    //

    // Install the kafka broker under /opt/kafka, beside the jmx exporter it runs with.
    // Each release unpacks under its own versioned directory and a "current"
    // symlink names the one in force, so an upgrade is a symlink move.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_kafka(ainst::FExtpkg &extpkg); // dev.extpkg:kafka

    // -------------------------------------------------------------------
    // cpp/ainst/kafka_connectors.cpp
    //

    // Install the s3 and iceberg connectors kafka connect loads from /opt/kafka/plugins.
    // Iceberg reads its version from version.txt when that file is present, and
    // otherwise derives one from the newest tag by bumping the minor and appending
    // -SNAPSHOT -- so a tree at apache-iceberg-1.11.0 builds itself as
    // 1.12.0-SNAPSHOT, which is neither the release nor a name this can predict.
    // The apache source release carries version.txt for that reason, and so does
    // this.  Gradle builds only the two outputs installed below: a whole-tree build
    // also format-checks and tests every module, and opens more files than a
    // container build allows.  The build writes git.properties into its jars from
    // the .git directory of a checkout, and a tarball has none, so that task is
    // skipped.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_kafka_connectors(ainst::FExtpkg &extpkg); // dev.extpkg:kafka_connectors

    // -------------------------------------------------------------------
    // cpp/ainst/kafkaui.cpp
    //

    // Install the kafka-ui web console and the account it runs as.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_kafkaui(ainst::FExtpkg &extpkg); // dev.extpkg:kafkaui

    // -------------------------------------------------------------------
    // cpp/ainst/kcat.cpp
    //

    // Install kcat, a kafka producer and consumer for the command line.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_kcat(ainst::FExtpkg &extpkg); // dev.extpkg:kcat

    // -------------------------------------------------------------------
    // cpp/ainst/kdi.cpp
    //

    // Install kafka-delta-ingest as /usr/bin/kdi, built from the commit its artifact names.
    // The project publishes no tag and no release, so a commit is the only pin
    // available and it is a stronger one than a tag, which upstream can move.
    // The rust toolchain is fetched, used, and removed again: nothing downstream of
    // this installation wants cargo on the machine.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_kdi(ainst::FExtpkg &extpkg); // dev.extpkg:kdi

    // -------------------------------------------------------------------
    // cpp/ainst/librdkafka.cpp
    //

    // Install librdkafka, the C kafka client library, into $PREFIX.  On Linux the
    // distribution's copy is removed, so nothing links against a second version.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_librdkafka(ainst::FExtpkg &extpkg); // dev.extpkg:librdkafka

    // -------------------------------------------------------------------
    // cpp/ainst/linux_tools.cpp
    //

    // Install cpupower and the other kernel tools for the running kernel.
    // The generic metapackage linux-tools-generic tracks Ubuntu's newest generic
    // kernel and moves linux-image-generic with it, so installing it on a host that
    // runs another kernel -- a vendor's HWE kernel, say -- installs a second kernel,
    // rebuilds every initramfs and dkms module, and makes the next boot run a kernel
    // nobody chose.  The tools of the kernel that is running are the ones cpupower
    // execs, so that package is the one installed.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_linux_tools(ainst::FExtpkg &extpkg); // dev.extpkg:linux_tools

    // -------------------------------------------------------------------
    // cpp/ainst/mc.cpp
    //

    // Install mc, the minio client.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_mc(ainst::FExtpkg &extpkg); // dev.extpkg:mc

    // -------------------------------------------------------------------
    // cpp/ainst/minio.cpp
    //

    // Install the minio object store server.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_minio(ainst::FExtpkg &extpkg); // dev.extpkg:minio

    // -------------------------------------------------------------------
    // cpp/ainst/mosquitto.cpp
    //

    // Install the mosquitto command-line clients into $PREFIX/bin, built from source
    // with TLS and linked against a static libmosquitto, so they need no library of
    // their own at run time.  The broker is left out.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_mosquitto(ainst::FExtpkg &extpkg); // dev.extpkg:mosquitto

    // -------------------------------------------------------------------
    // cpp/ainst/nats.cpp
    //

    // Install the nats command line client into $PREFIX/bin.
    // The Linux release is a deb and it is unpacked rather than installed, because
    // dpkg-deb is present in every image this runs in while unzip is not, and
    // because the package would otherwise land the binary in /usr/local/bin.  The
    // macOS release is a zip, which macOS unpacks itself.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_nats(ainst::FExtpkg &extpkg); // dev.extpkg:nats

    // -------------------------------------------------------------------
    // cpp/ainst/nats_server.cpp
    //

    // Install the nats-server broker into $PREFIX/bin from the upstream release:
    // a deb on Linux, a tarball on macOS.  The deb is unpacked rather than
    // installed, because dpkg-deb is present in every image this runs in while
    // unzip is not, and because the package also carries a /usr/local/bin symlink
    // of the same binary.
    // natsusr owns the running broker: omcli reaches the node over ssh as that
    // account, and nats-server keeps its JetStream store under that home.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_nats_server(ainst::FExtpkg &extpkg); // dev.extpkg:nats_server

    // -------------------------------------------------------------------
    // cpp/ainst/nerdctl.cpp
    //

    // Install nerdctl, the containerd command line client.
    // The builder it drives is the separate buildkit package, which every image
    // installing nerdctl installs beside it.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_nerdctl(ainst::FExtpkg &extpkg); // dev.extpkg:nerdctl

    // -------------------------------------------------------------------
    // cpp/ainst/nodejs.cpp
    //

    // Install node, npm and npx under /opt/node from the nodejs.org release, point
    // /opt/node/current at it, and link each command into $PREFIX/bin.  The release
    // tarball is the same build on Linux and macOS, so both run one version.  The
    // post step removes a Debian nodejs and nodesource's apt line and keyring from the
    // system it installs onto: the deb owns /usr/bin/node, and an upgrade of it, or
    // an apt install that pulls nodejs in, would put its own node back over the link.
    // Purging the deb deletes the paths it owned, so a purge links the commands again.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_nodejs(ainst::FExtpkg &extpkg); // dev.extpkg:nodejs

    // -------------------------------------------------------------------
    // cpp/ainst/oauth2_proxy.cpp
    //

    // Install oauth2-proxy, the reverse proxy nginx authenticates through.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_oauth2_proxy(ainst::FExtpkg &extpkg); // dev.extpkg:oauth2_proxy

    // -------------------------------------------------------------------
    // cpp/ainst/ofed.cpp
    //

    // Add NVIDIA's DOCA apt repository for the release that EXTPKG's dev.extpkgver row
    // pins, keyed by the keyring EXTPKG's "key" source fetches and checks.
    // The repository path carries the release, so the version reaches apt from the
    // table and from nowhere else; ofed and ofed_kernel each pin it, and a host that
    // installs both reads one repository when the two rows agree.  An aarch64 machine
    // reads NVIDIA's arm64-sbsa tree, which publishes the same packages.
    void AddDocaRepo(ainst::FExtpkg &extpkg);

    // Install the DOCA-OFED user space stack and xlio from NVIDIA's apt repository:
    // rdma-core, the verbs providers and tools, and libxlio.  Every image that talks
    // to a Mellanox card takes this package, a container as well as a host, since it
    // carries no kernel driver; a host that runs OFED's drivers adds ofed_kernel.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_ofed(ainst::FExtpkg &extpkg); // dev.extpkg:ofed

    // -------------------------------------------------------------------
    // cpp/ainst/ofed_kernel.cpp
    //

    // Install the DOCA-OFED kernel drivers on a host: mlx5, the RDMA core, srp, iser,
    // xpmem and the mft driver, built by DKMS for every kernel whose headers the host
    // holds, together with the user space they are released with.
    // A container never takes this package, since it runs on its host's kernel.  A
    // host with a Mellanox HCA runs OFED's drivers rather than the kernel's own, and
    // a kernel the drivers do not build for fails this install, so a host's kernel
    // moves only to a release whose DOCA-OFED builds for it.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_ofed_kernel(ainst::FExtpkg &extpkg); // dev.extpkg:ofed_kernel

    // -------------------------------------------------------------------
    // cpp/ainst/osv_scanner.cpp
    //

    // Install osv-scanner, the dependency scanner the depscan cijob runs.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_osv_scanner(ainst::FExtpkg &extpkg); // dev.extpkg:osv_scanner

    // -------------------------------------------------------------------
    // cpp/ainst/pnpm.cpp
    //

    // Install pnpm into $PREFIX with the npm of the nodejs package.  pnpm switches
    // itself to the version each ts package's packageManager field names, so this
    // copy only bootstraps it.  npm writes outside any staging root, so the install
    // runs as a post step on the target system.  An earlier install put pnpm under
    // /usr/local on Linux too, where it would stay first on PATH and answer the
    // probe with its old version, so on a host whose prefix is elsewhere that copy
    // is removed.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_pnpm(ainst::FExtpkg &extpkg); // dev.extpkg:pnpm

    // -------------------------------------------------------------------
    // cpp/ainst/postgresql.cpp
    //

    // Install the postgresql server and client, and the libpq headers the tree builds against.
    // The auto-created "main" cluster is suppressed before the packages configure,
    // because postgresql-common runs initdb from its own postinst otherwise.
    // libpq-dev carries no version suffix: it tracks the distribution's libpq5, and
    // cpp/ctxledger/pg.cpp includes a header that ships in neither versioned package.
    // On macOS the same server comes from Homebrew, as the mac image's postgresql@16
    // formula, so the install stops there with a line that names it.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_postgresql(ainst::FExtpkg &extpkg); // dev.extpkg:postgresql

    // -------------------------------------------------------------------
    // cpp/ainst/redis.cpp
    //

    // Install redis-cli and redis-benchmark into $PREFIX/bin, built from source with
    // TLS.  The server is left out.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_redis(ainst::FExtpkg &extpkg); // dev.extpkg:redis

    // -------------------------------------------------------------------
    // cpp/ainst/redpanda.cpp
    //

    // Install the redpanda broker from the vendor's apt repository, which Google
    // Artifact Registry serves and signs.  The keyring and list keep the names an
    // earlier repository used, so staging over an older install replaces its source
    // line instead of leaving one apt can no longer reach.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_redpanda(ainst::FExtpkg &extpkg); // dev.extpkg:redpanda

    // -------------------------------------------------------------------
    // cpp/ainst/rpm.cpp
    //

    // Assemble the script that packages every selected installation as an rpm.
    // Each one stages into a buildroot of its own and rpmbuild turns that directory
    // into a package, so an rpm carries exactly the files a live install would have
    // placed -- the same shell function produced both, called under a different
    // root.
    void WriteRpmScript(algo::cstring &out);

    // -------------------------------------------------------------------
    // cpp/ainst/rsync.cpp
    //

    // Install rsync into $PREFIX, built from source.  The optional checksum,
    // compression and OpenSSL libraries are left out, so the build needs none of
    // their headers and comes out the same on every system.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_rsync(ainst::FExtpkg &extpkg); // dev.extpkg:rsync

    // -------------------------------------------------------------------
    // cpp/ainst/rust.cpp
    //

    // Install the rust toolchain under /opt/rust, with cargo, rustc, rustfmt and
    // clippy on the path.  Each release unpacks into its own versioned directory and
    // a "current" symlink names the one in force, so an upgrade moves that symlink.
    //
    // The links in /usr/local/bin need no wrapper: a rust binary reads its own
    // resolved path out of /proc/self/exe and finds its toolchain above it.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_rust(ainst::FExtpkg &extpkg); // dev.extpkg:rust

    // -------------------------------------------------------------------
    // cpp/ainst/script.cpp
    //

    // Shell-safe spelling of INST's own key, which names its generated functions.
    // inst_<symbol>_stage, inst_<symbol>_post and inst_<symbol> are the three, and
    // the rpm driver names the first of them from another file.
    algo::tempstr GetExtpkgSymbol(ainst::FExtpkg &extpkg);

    // The release PKG is pinned to, or 0 when nothing pins it.
    // The version lives in dev.extpkgver rather than beside the package name because
    // dev.extpkg is compiled into this program: a version bump would otherwise
    // rewrite generated source and rebuild the tree, where what actually changed is
    // one row of data.
    algo::strptr GetExtpkgVersion(ainst::FExtpkg &extpkg);

    // Shell expression for the local path of artifact NAME of INST, quoted.
    // The download step assigns the variable this names, so an install line reads
    // bytes that already matched their sha256 and can reach nothing else.  One
    // variable serves every architecture: the download step picks the row of the
    // machine it runs on.
    // An artifact the table does not carry is fatal here rather than an empty
    // string in the script: an empty path fails much later and says nothing about
    // which installation named it.
    algo::tempstr GetExtpkgsrcVar(ainst::FExtpkg &extpkg, algo::strptr name);

    // Append one line to the part of INST that places files under the staging root.
    // This is what an rpm packages, so a line here writes through ainst_install,
    // ainst_mkdir or ainst_root and never names an absolute destination of its own.
    void AddStage(ainst::FExtpkg &extpkg, algo::strptr line);

    // Write the lines that place CONTENT at PATH under the staging root.
    // The content travels through a quoted heredoc, so what reaches the file is
    // what the caller wrote and the shell expands nothing on the way.
    void AddStageFile(ainst::FExtpkg &extpkg, algo::strptr path, algo::strptr content);

    // Write the lines that place the payload at SRC at PATH under the staging root.
    // A payload longer than a line or two -- an init script, a kernel module, a
    // systemd unit -- lives under conf/ainst rather than inside the install
    // function, because it is shell or C or yaml and a C++ translation unit is the
    // wrong home for any of them.  The generated script carries the bytes, so
    // reading the file here costs the script nothing: what travels to an image is
    // still one self-contained program.
    void AddStageFileFrom(ainst::FExtpkg &extpkg, algo::strptr path, algo::strptr src);

    // Write the lines that place the payload at SRC at the shell path expression PATH.
    void AddStageTextFrom(ainst::FExtpkg &extpkg, algo::strptr path, algo::strptr src);

    // Write the lines that place CONTENT at the shell path expression PATH.
    // Where StageFile writes something the installation installs, this writes
    // something it only builds from -- a source file replaced inside an unpacked
    // tree -- so it does not reach through the staging root.  PATH is written into
    // the script as given, so a caller that needs quoting supplies it.
    void AddStageText(ainst::FExtpkg &extpkg, algo::strptr path, algo::strptr content);

    // Append one line to the part of INST that acts on the live system.
    // Adding a user, enabling a unit and refreshing the linker cache all belong
    // here: none of them can be staged into a directory, and the rpm carries these
    // lines as its %post.
    void AddPost(ainst::FExtpkg &extpkg, algo::strptr line);

    // Write the vocabulary every generated install function is written against.
    // The frontend is set once here so that no apt call in any installation can stop
    // to ask: on a host that already carries a package's conffile, dpkg would
    // otherwise prompt, read end-of-file, and leave the package unconfigured.
    // Eight verbs carry the whole of it.  ainst_fetch answers with the path of one
    // verified artifact, ainst_noarch stops an install whose artifact has no build
    // for the machine, ainst_tmpdir answers with a fresh working directory, and
    // ainst_unpack answers with the directory an archive unpacks into; ainst_install places a file under the staging root, ainst_write writes
    // one from stdin, ainst_mkdir creates a directory, and ainst_root turns a
    // destination into a path under that root.  Applying $ROOT in those four rather
    // than in each installation is what lets one function serve a live install and
    // an rpm build.
    // Every working directory lives under one scratch directory, $AINST_TMP, which
    // the script removes when it exits, however it exits.  An image build runs the
    // script in one layer, so an unpacked source tree, a compiler toolchain or a
    // cargo build left behind would ship in the image; removing them in one place
    // covers every package, including the ones written later.
    void WritePreamble(algo::cstring &out);

    // Write the fetch of every artifact of INST, one shell variable per artifact.
    // Nothing else in the script reaches the network, so an installation's whole
    // download is these lines and its sha256 checks are all of them.
    //
    // An artifact built for any machine has one row and one fetch.  An artifact
    // built per architecture has a row for each, keyed <name>.<arch>, and the fetch
    // picks the row of the machine the script runs on, so one script serves every
    // architecture.  A machine with no row of its own stops the install by name.
    // Mixing the two would let an architecture nobody built for fetch another's
    // bytes, so a name whose rows disagree is refused here.
    void WriteFetch(ainst::FExtpkg &extpkg, algo::cstring &out);

    // Write the preamble and the functions of every selected installation.
    // This is a library of shell functions and calls none of them, which is what
    // both the driver that runs an install and the one that builds an rpm start
    // from -- they differ only in what they call and under which root.
    void WriteFunc(algo::cstring &out);

    // Assemble the script performing every selected installation, in table order.
    // The functions come first and the calls last, so the text reads as a program.
    void WriteScript(algo::cstring &out);

    // Assemble the script that stages every selected installation under a scratch
    // root of its own and reports which ones fail.
    // A stage fetches the artifacts, checks their sha256 and builds, and writes only
    // under $ROOT, so the check exercises everything an install depends on from
    // outside while touching nothing on the host.  Each stage runs in a subshell
    // with its own errexit and its own scratch directory, removed once the package
    // is reported, so one failure ends that package alone and a large build does
    // not hold its disk for the rest of the run.  The driver
    // prints one ainst.check line per package, the log tail of a failure, and exits
    // non-zero when any package failed.
    void WriteCheckScript(algo::cstring &out);

    // -------------------------------------------------------------------
    // cpp/ainst/shfmt.cpp
    //

    // Install shfmt, the shell formatter bin/bash-indent drives.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_shfmt(ainst::FExtpkg &extpkg); // dev.extpkg:shfmt

    // -------------------------------------------------------------------
    // cpp/ainst/spark.cpp
    //

    // Set up the spark account's home and add the jars the image reads and writes s3 and delta through.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_spark(ainst::FExtpkg &extpkg); // dev.extpkg:spark

    // -------------------------------------------------------------------
    // cpp/ainst/spdk.cpp
    //

    // Install the storage performance development kit, built from the release
    // dev.extpkgver pins, so -update moves the clone with the version it records.
    // This is the one installation that clones rather than fetching an artifact:
    // spdk carries its dependencies as git submodules, and a release tarball from
    // github carries none of them.  The build dependencies -- libaio-dev,
    // libcunit1-dev, libnuma-dev, libssl-dev, uuid-dev, meson, python3-pyelftools
    // -- are debs of the image that names this package, as they are for nginx and
    // vma, so a host that builds it declares them the same way.  The clone and its
    // build tree, two gigabytes, live in a scratch directory that goes when the
    // script exits.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_spdk(ainst::FExtpkg &extpkg); // dev.extpkg:spdk

    // -------------------------------------------------------------------
    // cpp/ainst/ssm_plugin.cpp
    //

    // Install the aws session-manager-plugin, which "aws ssm start-session" drives.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_ssm_plugin(ainst::FExtpkg &extpkg); // dev.extpkg:ssm_plugin

    // -------------------------------------------------------------------
    // cpp/ainst/tla.cpp
    //

    // Install the TLA+ tools jar as /opt/tla/tla2tools-<version>.jar and point
    // /opt/tla/tla2tools.jar at it, so atla finds the pinned release at one path
    // and the probe tells one release from another.  The jar runs on the runtime of
    // the java package.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_tla(ainst::FExtpkg &extpkg); // dev.extpkg:tla

    // -------------------------------------------------------------------
    // cpp/ainst/tsc_freq_khz.cpp
    //

    // Install the tsc_freq_khz kernel module, which exports the kernel's boot-calibrated TSC rate.
    // Stock kernels never expose tsc_khz, and an EC2 instance has no cpufreq sysfs
    // either, so without this module userspace has no authoritative rate to
    // calibrate against and algo::CpuHz falls back to guessing.
    // The source is vendored verbatim from the archived trailofbits/tsc_freq_khz
    // (GPL-2.0, commit f71e4ad, 2019-09-24).  dkms rebuilds it for every future
    // kernel and modules-load.d loads it at boot.
    // The Makefile recipes use the one-line "target: ; command" form, which make
    // accepts without the leading TAB a conventional recipe needs -- a TAB would
    // not survive this repo's indent pass.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_tsc_freq_khz(ainst::FExtpkg &extpkg); // dev.extpkg:tsc_freq_khz

    // -------------------------------------------------------------------
    // cpp/ainst/update.cpp
    //

    // Read the newest release of PKG from github, and record it as PKG's newver.
    // The releases endpoint is asked first, because a release is what a maintainer
    // declares current.  A repository that publishes none -- and several of these do
    // not -- answers 404 there, so the tags endpoint supplies the newest tag
    // instead, ordered by version rather than by the order github returns them.
    //     (user-implemented function, prototype is in amc-generated header)
    // void upstreamtype_github(ainst::FExtpkg &extpkg); // dev.upstreamtype:github

    // Look for a newer release of every selected package, and record what is found.
    // One package at a time, writing each as it goes, so a run interrupted halfway
    // leaves the packages it already moved correctly pinned.
    void UpdateExtpkg();

    // -------------------------------------------------------------------
    // cpp/ainst/vma.cpp
    //

    // Install mellanox libvma, built from the release tag named in its artifact url.
    // The script this replaced fetched refs/heads/master.zip, which names a
    // different tree every day; 9.8.84 is the newest release upstream publishes and
    // is newer than the OFED beside it.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_vma(ainst::FExtpkg &extpkg); // dev.extpkg:vma

    // -------------------------------------------------------------------
    // cpp/ainst/wstunnel.cpp
    //

    // Install wstunnel, which carries a tcp connection over a websocket.
    //     (user-implemented function, prototype is in amc-generated header)
    // void extpkg_wstunnel(ainst::FExtpkg &extpkg); // dev.extpkg:wstunnel
}
