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
// Source: cpp/ainst/script.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Shell-safe spelling of KEY, for use inside a generated identifier.
// An installation key or an artifact name may carry a character bash will not
// accept in a function or variable name, so one rule serves both.
static algo::tempstr GetSymbol(algo::strptr key) {
    algo::tempstr ret;
    frep_(i,key.n_elems) {
        char c = key[i];
        bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
        ret << (ok ? c : '_');
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Shell-safe spelling of INST's own key, which names its generated functions.
// inst_<symbol>_stage, inst_<symbol>_post and inst_<symbol> are the three, and
// the rpm driver names the first of them from another file.
algo::tempstr ainst::GetExtpkgSymbol(ainst::FExtpkg &extpkg) {
    return GetSymbol(extpkg.extpkg);
}

// -----------------------------------------------------------------------------

// The release PKG is pinned to, or 0 when nothing pins it.
// The version lives in dev.extpkgver rather than beside the package name because
// dev.extpkg is compiled into this program: a version bump would otherwise
// rewrite generated source and rebuild the tree, where what actually changed is
// one row of data.
algo::strptr ainst::GetExtpkgVersion(ainst::FExtpkg &extpkg) {
    algo::strptr ret("0");
    if (extpkg.c_extpkgver && ch_N(extpkg.c_extpkgver->version)) {
        ret = extpkg.c_extpkgver->version;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Shell expression for the local path of artifact NAME of INST, quoted.
// The download step assigns the variable this names, so an install line reads
// bytes that already matched their sha256 and can reach nothing else.
// An artifact the table does not carry is fatal here rather than an empty
// string in the script: an empty path fails much later and says nothing about
// which installation named it.
algo::tempstr ainst::GetExtpkgsrcVar(ainst::FExtpkg &extpkg, algo::strptr name) {
    ainst::FExtpkgsrc *extpkgsrc = ainst::ind_extpkgsrc_Find(dev::Extpkgsrc_Concat_extpkg_name(extpkg.extpkg,name));
    vrfy(extpkgsrc, algo::tempstr()<<"ainst.no_extpkgsrc"
         <<Keyval("inst",extpkg.extpkg)
         <<Keyval("name",name)
         <<Keyval("comment","installation has no artifact of that name"));
    algo::tempstr ret;
    ret << "\"$art_" << GetSymbol(extpkg.extpkg) << "_" << GetSymbol(name) << "\"";
    return ret;
}

// -----------------------------------------------------------------------------

// Append one line to the part of INST that places files under the staging root.
// This is what an rpm packages, so a line here writes through ainst_install,
// ainst_mkdir or ainst_root and never names an absolute destination of its own.
void ainst::AddStage(ainst::FExtpkg &extpkg, algo::strptr line) {
    extpkg.stage << "    " << line << eol;
}

// -----------------------------------------------------------------------------

// Write the lines that place CONTENT at PATH under the staging root.
// The content travels through a quoted heredoc, so what reaches the file is
// what the caller wrote and the shell expands nothing on the way.
void ainst::AddStageFile(ainst::FExtpkg &extpkg, algo::strptr path, algo::strptr content) {
    extpkg.stage << "    ainst_write " << algo::strptr_ToBash(path) << " <<'AINST_FILE_EOF'" << eol;
    extpkg.stage << content;
    if (content.n_elems > 0 && content[content.n_elems-1] != '\n') {
        extpkg.stage << eol;
    }
    extpkg.stage << "AINST_FILE_EOF" << eol;
}

// -----------------------------------------------------------------------------

// Write the lines that place the payload at SRC at PATH under the staging root.
// A payload longer than a line or two -- an init script, a kernel module, a
// systemd unit -- lives under conf/ainst rather than inside the install
// function, because it is shell or C or yaml and a C++ translation unit is the
// wrong home for any of them.  The generated script carries the bytes, so
// reading the file here costs the script nothing: what travels to an image is
// still one self-contained program.
void ainst::AddStageFileFrom(ainst::FExtpkg &extpkg, algo::strptr path, algo::strptr src) {
    ainst::AddStageFile(extpkg,path,algo::FileToString(src,algo_FileFlags__throw));
}

// -----------------------------------------------------------------------------

// Write the lines that place the payload at SRC at the shell path expression PATH.
void ainst::AddStageTextFrom(ainst::FExtpkg &extpkg, algo::strptr path, algo::strptr src) {
    ainst::AddStageText(extpkg,path,algo::FileToString(src,algo_FileFlags__throw));
}

// -----------------------------------------------------------------------------

// Write the lines that place CONTENT at the shell path expression PATH.
// Where StageFile writes something the installation installs, this writes
// something it only builds from -- a source file replaced inside an unpacked
// tree -- so it does not reach through the staging root.  PATH is written into
// the script as given, so a caller that needs quoting supplies it.
void ainst::AddStageText(ainst::FExtpkg &extpkg, algo::strptr path, algo::strptr content) {
    extpkg.stage << "    cat > " << path << " <<'AINST_TEXT_EOF'" << eol;
    extpkg.stage << content;
    if (content.n_elems > 0 && content[content.n_elems-1] != '\n') {
        extpkg.stage << eol;
    }
    extpkg.stage << "AINST_TEXT_EOF" << eol;
}

// -----------------------------------------------------------------------------

// Append one line to the part of INST that acts on the live system.
// Adding a user, enabling a unit and refreshing the linker cache all belong
// here: none of them can be staged into a directory, and the rpm carries these
// lines as its %post.
void ainst::AddPost(ainst::FExtpkg &extpkg, algo::strptr line) {
    extpkg.post << "    " << line << eol;
}

// -----------------------------------------------------------------------------

// Write the vocabulary every generated install function is written against.
// The frontend is set once here so that no apt call in any installation can stop
// to ask: on a host that already carries a package's conffile, dpkg would
// otherwise prompt, read end-of-file, and leave the package unconfigured.
// Six verbs carry the whole of it.  ainst_fetch answers with the path of one
// verified artifact and ainst_unpack with the directory an archive unpacks
// into; ainst_install places a file under the staging root, ainst_write writes
// one from stdin, ainst_mkdir creates a directory, and ainst_root turns a
// destination into a path under that root.  Applying $ROOT in those four rather
// than in each installation is what lets one function serve a live install and
// an rpm build.
void ainst::WritePreamble(algo::cstring &out) {
    out << "#!/bin/bash" << eol;
    out << "# Generated by ainst.  Edit cpp/ainst/<extpkg>.cpp and the dev.extpkg tables." << eol;
    out << "set -euo pipefail" << eol;
    out << "export DEBIAN_FRONTEND=noninteractive" << eol;
    out << eol;
    out << "ROOT=${ROOT:-" << ainst::_db.cmdline.root << "}" << eol;
    out << "AINST_CACHE=${AINST_CACHE:-" << ainst::_db.cmdline.cache << "}" << eol;
    out << R"SH(
# Answer with the local path of one artifact, fetching it once.
# The cache is keyed by content, so the same bytes are fetched once however many
# installations name them, and a partial download is never promoted.
function ainst_fetch {
    local url=$1 sha=$2 dest="$AINST_CACHE/$2"
    if [[ ! -f $dest ]]; then
        mkdir -p "$AINST_CACHE"
        curl -fsSL "$url" -o "$dest.part"
        if ! echo "$sha  $dest.part" | sha256sum -c - >/dev/null; then
            echo "ainst.badsha  url:$url  want:$sha" >&2
            rm -f "$dest.part"
            exit 1
        fi
        mv "$dest.part" "$dest"
    fi
    echo "$dest"
}

# Answer with PATH under the staging root, creating the directory it sits in.
# A caller that writes through a program of its own -- gpg -o, a redirect -- gets
# the same guarantee ainst_install and ainst_write make, so a buildroot that has
# only just been created is not a place where those writes fail.
function ainst_root {
    mkdir -p "$ROOT$(dirname "$1")"
    echo "$ROOT$1"
}

# Place SRC at DEST under the staging root, with mode MODE.
function ainst_install {
    mkdir -p "$ROOT$(dirname "$3")"
    install -m "$1" "$2" "$ROOT$3"
}

# Create DIR under the staging root.
function ainst_mkdir {
    mkdir -p "$ROOT$1"
}

# Write stdin to PATH under the staging root, creating its parent directory.
function ainst_write {
    mkdir -p "$ROOT$(dirname "$1")"
    cat > "$ROOT$1"
}

# Unpack ARCHIVE into a directory of its own and answer with that directory.
# Nothing is stripped from the paths inside, so a caller names the archive's own
# top-level directory where it has one and the directory itself where it does not.
# The format is read from the first bytes rather than from the name: the cache
# names every file by its sha256, so no name here carries a suffix, and one url
# in this tree serves a zip from a path ending in "archive".
function ainst_unpack {
    local dir magic
    dir=$(mktemp -d)
    magic=$(head -c 2 "$1")
    case "$magic" in
        PK) unzip -q "$1" -d "$dir" ;;
        *)  tar -xf "$1" -C "$dir" ;;
    esac
    echo "$dir"
}
)SH";
}

// -----------------------------------------------------------------------------

// Write the fetch of every artifact of INST, one shell variable per artifact.
// Nothing else in the script reaches the network, so an installation's whole
// download is these lines and its sha256 checks are all of them.
void ainst::WriteFetch(ainst::FExtpkg &extpkg, algo::cstring &out) {
    ind_beg(ainst::extpkg_zd_extpkgsrc_curs,extpkgsrc,extpkg) {
        out << "    art_" << GetSymbol(extpkg.extpkg) << "_" << GetSymbol(name_Get(extpkgsrc))
            << "=$(ainst_fetch " << algo::strptr_ToBash(extpkgsrc.url)
            << " " << algo::strptr_ToBash(extpkgsrc.sha256) << ")" << eol;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Write INST as three shell functions: its stage, its post, and the two in order.
// A live install calls the third; an rpm build calls the first under a buildroot
// and carries the second as its %post, which is why they are kept apart here.
static void WriteExtpkg(ainst::FExtpkg &extpkg, algo::cstring &out) {
    tempstr sym(GetSymbol(extpkg.extpkg));
    out << eol;
    out << "# -----------------------------------------------------------------------------" << eol;
    out << "# " << extpkg.extpkg << " " << ainst::GetExtpkgVersion(extpkg);
    if (ch_N(extpkg.comment)) {
        out << " -- " << extpkg.comment;
    }
    out << eol;
    out << "inst_" << sym << "_stage() {" << eol;
    ainst::WriteFetch(extpkg,out);
    out << extpkg.stage;
    out << "    :" << eol;
    out << "}" << eol;
    out << eol;
    out << "inst_" << sym << "_post() {" << eol;
    out << extpkg.post;
    out << "    :" << eol;
    out << "}" << eol;
    out << eol;
    out << "inst_" << sym << "() {" << eol;
    out << "    inst_" << sym << "_stage" << eol;
    out << "    inst_" << sym << "_post" << eol;
    out << "}" << eol;
}

// -----------------------------------------------------------------------------

// Write the preamble and the functions of every selected installation.
// This is a library of shell functions and calls none of them, which is what
// both the driver that runs an install and the one that builds an rpm start
// from -- they differ only in what they call and under which root.
void ainst::WriteFunc(algo::cstring &out) {
    ainst::WritePreamble(out);
    ind_beg(ainst::_db_zd_sel_extpkg_curs,extpkg,ainst::_db) {
        step_Call(extpkg,extpkg);
        WriteExtpkg(extpkg,out);
    }ind_end;
}

// -----------------------------------------------------------------------------

// Assemble the script performing every selected installation, in table order.
// The functions come first and the calls last, so the text reads as a program.
void ainst::WriteScript(algo::cstring &out) {
    ainst::WriteFunc(out);
    out << eol;
    out << "# -----------------------------------------------------------------------------" << eol;
    ind_beg(ainst::_db_zd_sel_extpkg_curs,extpkg,ainst::_db) {
        out << "inst_" << GetSymbol(extpkg.extpkg) << eol;
    }ind_end;
}
