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
// Target: atf_ci (exe) -- Normalization tests (see citest table)
// Exceptions: yes
// Source: cpp/atf_ci/depscan.cpp
//
// The depscan cijob.  It matches the third-party dependencies of the tree
// against public advisory databases, so an advisory published today against a
// release the tree already uses fails the job without any change to the code.
// osv-scanner reads the lockfiles and poms, and the NVD answers for each library
// vendored as source, listed in dev.vendorlib.  Each advisory is printed with
// its dev.vulnsev band, and only a band marked to fail fails the job.  An
// advisory accepted after review is a dev.vulnaccept row; it passes both checks
// until the row's date.
// Each check leaves the answers it judged under temp/depscan.

#include "include/algo.h"
#include "include/atf_ci.h"
#include "include/lib_json.h"

// -----------------------------------------------------------------------------

// Return the path of file NAME in the directory the depscan job writes to.
static tempstr GetDepscanPath(strptr name) {
    return tempstr() << "temp/depscan/" << name;
}

// Write the osv-scanner configuration to PATH, one ignored advisory per
// dev.vulnaccept row with the row's date and its comment as the reason.
static void WriteOsvConfig(strptr path) {
    cstring text;
    ind_beg(atf_ci::_db_vulnaccept_curs, vulnaccept, atf_ci::_db) {
        text << "[[IgnoredVulns]]\n";
        text << "id = ";
        lib_json::JsonSerializeString(vulnaccept.vulnaccept, text);
        text << "\nignoreUntil = ";
        algo::UnTime_PrintSpec(vulnaccept.until, text, "%Y-%m-%d");
        text << "\nreason = ";
        lib_json::JsonSerializeString(vulnaccept.comment, text);
        text << "\n\n";
    }ind_end;
    StringToFile(text, path);
}

// Return the dev.vulnsev band of an advisory with CVSS base score CVSS.  A score
// falls in the scored band with the highest lower bound at or below it, and an
// empty or unreadable CVSS falls in the band named unscored.  Return NULL when
// no band matches.
static atf_ci::FVulnsev *FindVulnsev(strptr cvss) {
    algo::U32Dec2 score;
    bool scored = cvss != "" && algo::U32Dec2_ReadStrptrMaybe(score, cvss);
    atf_ci::FVulnsev *ret = NULL;
    ind_beg(atf_ci::_db_vulnsev_curs, vulnsev, atf_ci::_db) {
        bool unscored = vulnsev.vulnsev == "unscored";
        if (!scored && unscored) {
            ret = &vulnsev;
        } else if (scored && !unscored
                   && vulnsev.cvss.value <= score.value
                   && (!ret || ret->cvss.value < vulnsev.cvss.value)) {
            ret = &vulnsev;
        }
    }ind_end;
    return ret;
}

// Print one atf_ci.vuln line for advisory ID with CVSS base score CVSS, followed
// by the key-value text WHERE that names what the advisory is filed against, and
// return whether it fails the scan.  An advisory that ACCEPTED marks as accepted
// passes, and so does one whose band does not fail.  An advisory that matches no
// band fails, so a gap in dev.vulnsev cannot pass a finding unseen.
static bool ReportVuln(strptr id, strptr cvss, bool accepted, strptr where) {
    atf_ci::FVulnsev *vulnsev = FindVulnsev(cvss);
    bool fail = !accepted && (!vulnsev || vulnsev->fail);
    prlog("atf_ci.vuln"
          << Keyval("id", id)
          << Keyval("cvss", cvss)
          << Keyval("severity", vulnsev ? strptr(vulnsev->vulnsev) : strptr("none"))
          << Keyval("accepted", Bool(accepted))
          << Keyval("fail", Bool(fail))
          << "  " << where); // WHERE began empty, so its first Keyval carries no separator
    return fail;
}

// Print one atf_ci.vuln line for each advisory in osv-scanner's json report
// PATH, naming the package, its release and the file that pins it.  Return
// whether any of them fails the scan.  osv-scanner drops the advisories that
// dev.vulnaccept accepts, and merges the aliases of one advisory into a group.
static bool ReportOsvVuln(strptr path) {
    lib_json::FParser parser;
    bool parsed = lib_json::JsonParse(parser, FileToString(path)) && parser.root_node;
    vrfy(parsed, tempstr() << "atf_ci.depscan_badjson" << Keyval("path", path));
    bool fail = false;
    lib_json::FNode *results = lib_json::node_GetArray(parser.root_node, "results");
    if (results) {
        ind_beg(lib_json::node_c_child_curs, result, *results) {
            lib_json::FNode *packages = lib_json::node_GetArray(&result, "packages");
            if (packages) {
                ind_beg(lib_json::node_c_child_curs, package, *packages) {
                    lib_json::FNode *groups = lib_json::node_GetArray(&package, "groups");
                    if (groups) {
                        ind_beg(lib_json::node_c_child_curs, group, *groups) {
                            lib_json::FNode *ids = lib_json::node_GetArray(&group, "ids");
                            tempstr where;
                            where << Keyval("ecosystem", lib_json::strptr_Get(&package, "package.ecosystem"))
                                  << Keyval("package", lib_json::strptr_Get(&package, "package.name"))
                                  << Keyval("version", lib_json::strptr_Get(&package, "package.version"))
                                  << Keyval("source", lib_json::strptr_Get(&result, "source.path"));
                            strptr id = ids && c_child_N(*ids) > 0 ? strptr(c_child_qFind(*ids, 0).value) : strptr("");
                            fail = ReportVuln(id, lib_json::strptr_Get(&group, "max_severity"), false, where) || fail;
                        }ind_end;
                    }
                }ind_end;
            }
        }ind_end;
    }
    return fail;
}

// Scan every tracked lockfile and pom with osv-scanner, naming each file so the
// scan covers what version control holds and nothing a build installed.  The
// report is temp/depscan/osv.json.  The citest fails on an advisory whose
// dev.vulnsev band fails and that no dev.vulnaccept row accepts, and when
// osv-scanner cannot run.
void atf_ci::citest_depscan_lockfile() {
    tempstr config(GetDepscanPath("osv-scanner.toml"));
    tempstr report(GetDepscanPath("osv.json"));
    CreateDirRecurse(GetDepscanPath(""));
    WriteOsvConfig(config);
    // the manifest names osv-scanner reads, across the ecosystems it covers
    algo_lib::Regx manifest;
    Regx_ReadSql(manifest, "(pnpm-lock.yaml|package-lock.json|yarn.lock|Cargo.lock|go.mod|pom.xml"
                 "|requirements%.txt|poetry.lock|uv.lock|Pipfile.lock|gradle.lockfile|Gemfile.lock|composer.lock)", true);
    tempstr cmd;
    cmd << "osv-scanner scan source --format json"
        << " --config " << strptr_ToBash(config)
        << " --output-file " << strptr_ToBash(report);
    ind_beg(atf_ci::_db_gitfile_curs, gitfile, atf_ci::_db) {
        if (Regx_Match(manifest, algo::StripDirName(gitfile.gitfile))) {
            cmd << " -L " << strptr_ToBash(gitfile.gitfile);
        }
    }ind_end;
    int status = SysCmd(cmd, FailokQ(true));
    // osv-scanner exits 0 when it finds nothing, 1 when it finds an advisory and
    // 128 when it finds no package; any other code is an error it printed above
    int rc = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    bool fail = false;
    if (rc == 1) {
        fail = ReportOsvVuln(report);
    }
    strptr comment = rc == 1 ? strptr("Upgrade the package, or accept the advisory with a dev.vulnaccept row")
        : rc == 128 ? strptr("osv-scanner found no package in the tracked manifests")
        : strptr("osv-scanner failed; read its output above, and install it with ainst osv_scanner -install");
    vrfy(rc == 0 || (rc == 1 && !fail), tempstr() << (rc == 1 ? "atf_ci.depscan_vuln" : "atf_ci.depscan_fail")
         << Keyval("rc", rc)
         << Keyval("report", report)
         << Keyval("comment", comment));
}

// Return the CVSS base score of the NVD vulnerability item ITEM, taken from the
// newest CVSS version the NVD scored it with, or an empty string when unscored.
static strptr GetNvdCvss(lib_json::FNode &item) {
    strptr ret;
    lib_json::FNode *metric = lib_json::node_GetArray(&item, "cve.metrics.cvssMetricV40");
    if (!metric) {
        metric = lib_json::node_GetArray(&item, "cve.metrics.cvssMetricV31");
    }
    if (!metric) {
        metric = lib_json::node_GetArray(&item, "cve.metrics.cvssMetricV30");
    }
    if (!metric) {
        metric = lib_json::node_GetArray(&item, "cve.metrics.cvssMetricV2");
    }
    if (metric && c_child_N(*metric) > 0) {
        ret = lib_json::strptr_Get(&c_child_qFind(*metric, 0), "cvssData.baseScore");
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Fetch URL from the NVD into file PATH and parse it into PARSER.  The NVD
// asks a client without an API key to leave six seconds between requests.
static void FetchNvd(strptr url, strptr path, lib_json::FParser &parser) {
    algo::SleepMsec(6000);
    SysCmd(tempstr() << "curl -fsS --retry 3 -o " << strptr_ToBash(path) << " " << strptr_ToBash(url), FailokQ(false));
    bool parsed = lib_json::JsonParse(parser, FileToString(path)) && parser.root_node;
    vrfy(parsed, tempstr() << "atf_ci.depscan_badjson" << Keyval("path", path));
}

// Look up the release of each dev.vendorlib row in the NVD and print one
// atf_ci.vuln line per CVE that the NVD marks as vulnerable in that release.
// The NVD answers an unknown CPE name with no CVE at all, which would read as a
// clean library.  So the CPE dictionary is asked first, and a release missing
// from it fails the citest, as does an answer that holds no CVE list.  A CVE
// whose dev.vulnsev band fails also fails the citest, unless a dev.vulnaccept
// row accepts it and has not lapsed.
void atf_ci::citest_depscan_vendorlib() {
    CreateDirRecurse(GetDepscanPath(""));
    algo::UnTime now = algo::CurrUnTime();
    bool vuln = false;
    bool broken = false;
    ind_beg(atf_ci::_db_vendorlib_curs, vendorlib, atf_ci::_db) {
        tempstr cpe;
        cpe << vendorlib.cpe << ":" << vendorlib.version;
        lib_json::FParser cpe_parser;
        FetchNvd(tempstr() << "https://services.nvd.nist.gov/rest/json/cpes/2.0?cpeMatchString=" << cpe
                 , GetDepscanPath(tempstr() << "nvd-cpe-" << vendorlib.vendorlib << ".json")
                 , cpe_parser);
        strptr total = lib_json::strptr_Get(cpe_parser.root_node, "totalResults");
        bool known = total != "" && total != "0";
        lib_json::FParser cve_parser;
        lib_json::FNode *cves = NULL;
        if (known) {
            // isVulnerable drops the CVEs that name the library only as a platform
            FetchNvd(tempstr() << "https://services.nvd.nist.gov/rest/json/cves/2.0?cpeName=" << cpe << ":*:*:*:*:*:*:*&isVulnerable"
                     , GetDepscanPath(tempstr() << "nvd-cve-" << vendorlib.vendorlib << ".json")
                     , cve_parser);
            cves = lib_json::node_GetArray(cve_parser.root_node, "vulnerabilities");
        }
        if (!cves) {
            prlog((known ? "atf_ci.depscan_badanswer" : "atf_ci.depscan_unknown_cpe")
                  << Keyval("vendorlib", vendorlib.vendorlib)
                  << Keyval("cpe", cpe)
                  << Keyval("totalResults", total)
                  << Keyval("comment", known
                            ? "The NVD answer holds no CVE list; read it under temp/depscan"
                            : "The NVD has no such CPE; correct dev.vendorlib"));
        }
        if (cves) {
            ind_beg(lib_json::node_c_child_curs, item, *cves) {
                strptr id = lib_json::strptr_Get(&item, "cve.id");
                atf_ci::FVulnaccept *vulnaccept = atf_ci::ind_vulnaccept_Find(id);
                bool accepted = vulnaccept && now < vulnaccept->until;
                tempstr where;
                where << Keyval("vendorlib", vendorlib.vendorlib)
                      << Keyval("version", vendorlib.version);
                vuln = ReportVuln(id, GetNvdCvss(item), accepted, where) || vuln;
            }ind_end;
        }
        broken = broken || !cves;
    }ind_end;
    vrfy(!vuln && !broken, tempstr() << (broken ? "atf_ci.depscan_fail" : "atf_ci.depscan_vuln")
         << Keyval("comment", broken
                   ? "The NVD lookup failed for a dev.vendorlib row; see the line above"
                   : "Upgrade the library, or accept the CVE with a dev.vulnaccept row"));
}
