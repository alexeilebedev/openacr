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
// Source: cpp/ainst/update.cpp
//

#include "include/algo.h"
#include "include/ainst.h"
#include "include/lib_curl.h"
#include "include/lib_json.h"

// -----------------------------------------------------------------------------

// Fetch URL and answer with the body, empty when the request did not answer 200.
// A vendor that is down, rate-limiting, or has renamed its repository all arrive
// here as an empty body, and the caller then reports the package as unreadable
// rather than as current -- which is the distinction that matters, since
// "nothing newer" and "could not ask" look identical to whoever runs -update.
static algo::tempstr GetBody(algo::strptr url) {
    lib_curl::FRequest req;
    lib_curl::FResponse resp;
    req.url = url;
    req.total_timeout_sec = 30;
    algo::tempstr ret;
    if (lib_curl::Curl(req,resp) && resp.code == 200) {
        ret << resp.body;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Strip whatever decoration a release tag carries around its version.
// One release is spelled 3.11.0, v3.11.0 and vma_9.8.84 by three upstreams, and
// what belongs in a url is the spelling that url already uses -- so a tag is
// reduced to the part from its first digit onward, and the decoration comes back
// when the new version is substituted into the url the package already has.
static algo::tempstr GetTagVersion(algo::strptr tag) {
    int beg = 0;
    while (beg < tag.n_elems && !(tag[beg] >= '0' && tag[beg] <= '9')) {
        beg++;
    }
    algo::tempstr ret;
    ret << algo::RestFrom(tag,beg);
    return ret;
}

// -----------------------------------------------------------------------------

// True when version A orders after version B, comparing dot-separated numbers.
// A string sort puts 3.9.0 after 3.11.0, which would pick the older release, so
// each component is compared as the number it spells; a component that spells no
// number compares as zero, which is what makes 1.0-rc1 lose to 1.0.
static bool NewerVersionQ(algo::strptr a, algo::strptr b) {
    bool ret = false;
    bool decided = false;
    algo::StringIter ai(a);
    algo::StringIter bi(b);
    while (!decided) {
        algo::strptr aw = algo::GetTokenChar(ai,'.');
        algo::strptr bw = algo::GetTokenChar(bi,'.');
        if (aw == "" && bw == "") {
            decided = true;
        } else {
            u32 an = 0;
            u32 bn = 0;
            (void)u32_ReadStrptrMaybe(an,aw);
            (void)u32_ReadStrptrMaybe(bn,bw);
            if (an != bn) {
                ret = an > bn;
                decided = true;
            }
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Read the newest release of PKG from github, and record it as PKG's newver.
// The releases endpoint is asked first, because a release is what a maintainer
// declares current.  A repository that publishes none -- and several of these do
// not -- answers 404 there, so the tags endpoint supplies the newest tag
// instead, ordered by version rather than by the order github returns them.
void ainst::upstreamtype_github(ainst::FExtpkg &extpkg) {
    algo::strptr repo = algo::Pathcomp(extpkg.c_extpkgver->upstream,":RR");
    algo::tempstr tag;
    lib_json::FParser relparser;
    algo::tempstr body(GetBody(algo::tempstr()<<"https://api.github.com/repos/"<<repo<<"/releases/latest"));
    if (ch_N(body) && lib_json::JsonParse(relparser,body) && lib_json::JsonParse(relparser,"")) {
        tag << lib_json::strptr_Get(relparser.root_node,"tag_name");
    }
    lib_json::FParser tagparser;
    if (ch_N(tag) == 0) {
        body = GetBody(algo::tempstr()<<"https://api.github.com/repos/"<<repo<<"/tags?per_page=100");
        if (ch_N(body) && lib_json::JsonParse(tagparser,body) && lib_json::JsonParse(tagparser,"")) {
            algo::tempstr best;
            ind_beg(lib_json::node_c_child_curs,child,*tagparser.root_node) {
                algo::strptr name = lib_json::strptr_Get(&child,"name");
                algo::tempstr version(GetTagVersion(name));
                if (ch_N(version) && NewerVersionQ(version,best)) {
                    best = version;
                    tag = name;
                }
            }ind_end;
        }
    }
    extpkg.newver = GetTagVersion(tag);
}

// -----------------------------------------------------------------------------

// Answer with TEXT, every occurrence of FROM replaced by TO.
// A url carries its version more than once -- once in the release path and again
// in the file name -- so every occurrence moves, not the first.
static algo::tempstr GetSubst(algo::strptr text, algo::strptr from, algo::strptr to) {
    algo::tempstr ret;
    int i = 0;
    while (i < text.n_elems) {
        int at = algo::FindStr(algo::RestFrom(text,i),from);
        if (at < 0) {
            ret << algo::RestFrom(text,i);
            i = text.n_elems;
        } else {
            ret << algo::qGetRegion(text,i,at) << to;
            i += at + from.n_elems;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Fetch URL and answer with the sha256 of what arrives, empty when the fetch failed.
// sha256sum is what the generated download step checks with, so computing the
// hash with the same program is what makes the two agree by construction.
// pipefail is set because without it a failed curl still leaves sha256sum
// exiting 0, and what would be recorded is the hash of an empty stream.
static algo::tempstr GetUrlSha256(algo::strptr url) {
    algo::tempstr path;
    path << ainst::_db.cmdline.outdir << "/update.sha256";
    CreateDirRecurse(Pathcomp(path,"/RL"));
    algo::tempstr pipeline;
    pipeline << "set -o pipefail; curl -fsSL --max-time 600 " << algo::strptr_ToBash(url) << " | sha256sum";
    algo::tempstr cmd;
    cmd << "bash -c " << algo::strptr_ToBash(pipeline) << " > " << algo::strptr_ToBash(path);
    algo::tempstr ret;
    if (SysCmd(cmd,algo::FailokQ(true),algo::DryrunQ(false)) == 0) {
        algo::tempstr out(algo::FileToString(path,algo::FileFlags()));
        ret << algo::Pathcomp(algo::Trimmed(out)," LL");
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Write TUPLE through acr, so the row reaches the ssimfile in sorted order.
// ainst reads these tables and does not own their files, and acr is what knows
// where a row belongs and how to quote it.
static void WriteTuple(algo::strptr tuple) {
    algo::tempstr path;
    path << ainst::_db.cmdline.outdir << "/update.ssim";
    CreateDirRecurse(Pathcomp(path,"/RL"));
    algo::StringToFile(tuple,path);
    command::acr acr;
    acr.replace = true;
    acr.write = true;
    acr.report = false;
    algo::tempstr cmd;
    cmd << command::acr_ToCmdline(acr) << " < " << algo::strptr_ToBash(path);
    SysCmd(cmd,algo::FailokQ(false),algo::DryrunQ(ainst::_db.cmdline.dry_run),algo::EchoQ(true));
}

// -----------------------------------------------------------------------------

// Move PKG to the release its upstream reader found, rewriting each of its sources.
// A source url carries the version literally, so the new url is the old one with
// the old version substituted -- which needs no template syntax and is right for
// every spelling an upstream uses, v3.11.0 and 3.11.0 alike.
// A source whose url does not carry the version at all cannot be rebuilt this
// way, and one whose new url does not answer would leave the package pinned to
// bytes nobody has seen; either one abandons the whole package rather than
// writing half an update, because a package pinned to two releases at once
// installs neither.
static void ApplyUpdate(ainst::FExtpkg &extpkg) {
    algo::strptr oldver = extpkg.c_extpkgver->version;
    algo::tempstr tuples;
    int n_bad = 0;
    // A package whose own script spells the version cannot be moved by rewriting
    // rows, because the other spelling is C++.  Every source build has one: kcat
    // unpacks into kcat-1.7.1, spdk clones --branch v25.05.1, and neither
    // follows a extpkgsrc url anywhere.  Moving the pin alone would leave the table
    // claiming a release the script does not install, which nothing downstream
    // would notice.
    if (algo::FindStr(extpkg.stage,oldver) >= 0 || algo::FindStr(extpkg.post,oldver) >= 0) {
        prlog("ainst.verinsrc"
              <<Keyval("extpkg",extpkg.extpkg)
              <<Keyval("version",oldver)
              <<Keyval("comment","cpp/ainst names this version too; move both or neither"));
        n_bad++;
    }
    ind_beg(ainst::extpkg_zd_extpkgsrc_curs,extpkgsrc,extpkg) {
        algo::tempstr url;
        if (algo::FindStr(extpkgsrc.url,oldver) < 0) {
            prlog("ainst.nosubst"
                  <<Keyval("extpkgsrc",extpkgsrc.extpkgsrc)
                  <<Keyval("version",oldver)
                  <<Keyval("comment","url does not carry the version, so a new one cannot be derived"));
            n_bad++;
        } else {
            url << GetSubst(extpkgsrc.url,oldver,extpkg.newver);
            algo::tempstr sha256(GetUrlSha256(url));
            if (ch_N(sha256) != 64) {
                prlog("ainst.nofetch"
                      <<Keyval("extpkgsrc",extpkgsrc.extpkgsrc)
                      <<Keyval("url",url)
                      <<Keyval("comment","the url the new release would use does not answer"));
                n_bad++;
            } else {
                dev::Extpkgsrc out;
                extpkgsrc_CopyOut(extpkgsrc,out);
                out.url = url;
                out.sha256 = sha256;
                tuples << out << eol;
            }
        }
    }ind_end;
    if (n_bad > 0) {
        prlog("ainst.skip"
              <<Keyval("extpkg",extpkg.extpkg)
              <<Keyval("nbad",n_bad)
              <<Keyval("comment","left at its current release; a package pinned to two releases installs neither"));
        algo_lib::_db.exit_code=1;
    } else {
        dev::Extpkgver extpkgver_out;
        extpkgver_CopyOut(*extpkg.c_extpkgver,extpkgver_out);
        extpkgver_out.version = extpkg.newver;
        tuples << extpkgver_out << eol;
        WriteTuple(tuples);
        prlog("ainst.update"
              <<Keyval("extpkg",extpkg.extpkg)
              <<Keyval("from",oldver)
              <<Keyval("to",extpkg.newver));
    }
}

// -----------------------------------------------------------------------------

// Ask PKG's upstream for a newer release and take it when there is one.
// Nothing newer and nowhere to look are different answers and are reported as
// different lines: a package this cannot follow stays visible instead of reading
// as up to date.
// A release that is older than the pin is refused rather than applied.  It is
// not a hypothetical: edenhill/kcat is pinned to tag 1.7.1 while the newest
// thing it marks as a release is 1.7.0, so following "latest" would walk the
// package backwards onto bytes nobody chose.
static void UpdateOne(ainst::FExtpkg &extpkg) {
    ainst::FExtpkgver *extpkgver = extpkg.c_extpkgver;
    algo::strptr scheme = extpkgver ? algo::Pathcomp(extpkgver->upstream,":LL") : algo::strptr();
    ainst::FUpstreamtype *upstreamtype = ainst::ind_upstreamtype_Find(scheme);
    if (!extpkgver || ch_N(extpkgver->upstream) == 0) {
        prlog("ainst.noupstream"
              <<Keyval("extpkg",extpkg.extpkg)
              <<Keyval("comment","no dev.extpkgver upstream, so releases cannot be looked up"));
    } else if (!upstreamtype) {
        prlog("ainst.badupstream"
              <<Keyval("extpkg",extpkg.extpkg)
              <<Keyval("scheme",scheme)
              <<Keyval("comment","no dev.upstreamtype row reads this kind of locator"));
        algo_lib::_db.exit_code=1;
    } else {
        step_Call(*upstreamtype,extpkg);
        if (ch_N(extpkg.newver) == 0) {
            prlog("ainst.unreadable"
                  <<Keyval("extpkg",extpkg.extpkg)
                  <<Keyval("upstream",extpkgver->upstream)
                  <<Keyval("comment","upstream named no release; it may be down, rate-limiting, or renamed"));
            algo_lib::_db.exit_code=1;
        } else if (extpkg.newver == algo::strptr(extpkgver->version)) {
            prlog("ainst.current"<<Keyval("extpkg",extpkg.extpkg)<<Keyval("version",extpkgver->version));
        } else if (!NewerVersionQ(extpkg.newver,extpkgver->version)) {
            prlog("ainst.notnewer"
                  <<Keyval("extpkg",extpkg.extpkg)
                  <<Keyval("have",extpkgver->version)
                  <<Keyval("upstream",extpkg.newver)
                  <<Keyval("comment","what upstream calls current is older than the pin; left alone"));
        } else {
            // The package's own script has to exist before it can be searched
            // for the version, and nothing else in an -update run builds it.
            step_Call(extpkg,extpkg);
            ApplyUpdate(extpkg);
        }
    }
}

// -----------------------------------------------------------------------------

// Look for a newer release of every selected package, and record what is found.
// One package at a time, writing each as it goes, so a run interrupted halfway
// leaves the packages it already moved correctly pinned.
void ainst::UpdateExtpkg() {
    ind_beg(ainst::_db_zd_sel_extpkg_curs,extpkg,ainst::_db) {
        UpdateOne(extpkg);
    }ind_end;
}
