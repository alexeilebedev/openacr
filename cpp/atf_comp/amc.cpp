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
// Target: atf_comp (exe) -- Component test runner: spawn processes and diff the log against a reference
// Exceptions: yes
// Source: cpp/atf_comp/amc.cpp
//
// Each test drives amc over a controlled universe and pins its output.
// Four kinds of test live here. A rejection pin (the Bad* tests) gives amc
// a universe carrying one schema defect and pins the amc.<tag> error and
// the nonzero exit code. A positive pin gives amc a well-formed universe
// and pins the generated functions a query selects (MinmaxNative,
// FdecGetScale, PmaskGlobal, CursUnrequested, ...). A write-failure pin
// (TableWriteAcrFail, OutfileWriteFail) gives amc a well-formed universe
// and a broken output destination, and pins the failure naming each path.
// A generation pass (SchemaClosure) gives amc the whole schema closure the
// live tree declares as its input, and pins that every namespace generates
// without error.
// Fixture universes load from test/amc/ via -in_dir -- a single file, or a
// directory when the universe includes side-loaded table data (gconst
// values resolve from the -in_dir root when it names a directory; for a
// single-file universe they side-load from the default data/ root, and amc
// prints the amc.sideload notice on the first such load). Except for the
// write-failure pins, every invocation passes the empty -out_dir: (write
// nothing); a query argument additionally keeps amc in query mode, and a
// test whose query is not itself the subject uses the bogus query zzz,
// which matches nothing.

#include "include/algo.h"
#include "include/atf_comp.h"

// Give amc a field of reftype Ptrary without its dmmeta.ptrary record, and check
// that amc reports amc.missing_ptrary, modifies no file and exits nonzero.
void atf_comp::comptest_amc_MissingPtrary() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.c_row  arg:algo_lib.FRow  reftype:Ptrary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Ptrary.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Ptrary.oncecurs  dflt:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Dump with acr_in the schema closure that the live tree declares as amc's own
// input, feed it to amc on stdin, and check that amc generates every namespace the
// tree defines without error.
// Every other test in this file gives amc a fixture of a handful of ctypes.  Such a
// fixture reaches only the generator modules that its ctypes need.  The empty
// -out_dir: keeps this run from writing the code it generates.
// NOTE: the test runs with memcheck:none because amc over the whole schema
// closure is too slow under valgrind.
void atf_comp::comptest_amc_SchemaClosure() {
    atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N < <($bindir/acr_in amc -data)");
}

// Give amc a big-endian u16 field in a universe without the u16 csize row, and
// check that amc.bigend_size names the missing dmmeta.csize row.
// The type's width is unknown here, and u16 itself supports big-endian storage.  A
// diagnostic that prescribed a width change would send the user to fix a width that
// is already right.
void atf_comp::comptest_amc_BadBigendNosize() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:N  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.big  arg:u16  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.FDb.big  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a big-endian field of reftype Inlary on a ctype with a fieldwise
// constructor, and check that amc reports amc.bad_bigend and exits nonzero.
// amc reports the rejection and continues generating to the end of the run.  Each
// further schema error adds to the exit code.
void atf_comp::comptest_amc_BadBigendReftype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:u16  ctor:N");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:N  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.Rec  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.big  arg:u16  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.big  min:4  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.Rec.big  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a big-endian u32 field whose bltin row sets bigendok:N, and check that
// amc reports amc.bigend_deadend.
// The u32 width has a byteswap primitive.  The bltin row still forbids storing u32
// byteswapped, and the Val accessors would swap it anyway.  The check that reads
// the flag runs apart from the width check, so it needs its own pin.
void atf_comp::comptest_amc_BadBigendDeadend() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:N  bigendok:N  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.big  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.FDb.big  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a big-endian u128 field whose bltin row claims bigendok, and check that
// amc reports amc.bigend_width.
// Byteswap primitives exist for 16, 32 and 64 bits.  The Val accessors would call
// be128toh and htobe128, which do not exist.  The width check rejects the field
// whatever the bltin row claims.
void atf_comp::comptest_amc_BadBigendU128() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.big  arg:u128  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.FDb.big  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc one field for each of the four ways to pair reftype Bitfld with a
// dmmeta.bitfld record, and check that amc reports amc.missing_record for both
// mismatched fields in one run.
// Reftype Bitfld and the dmmeta.bitfld record imply each other.  A Bitfld field
// without the record (flag) carries no bit offset or width.  A record on a field of
// another reftype (notbitfld) gets a full-width member, which the ctype's own
// compare, print and read walks pass over.  The correctly paired field (good) and
// the plain Val fields draw no report.
void atf_comp::comptest_amc_BadBitfldReftype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.flag  arg:bool  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.good  arg:bool  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.FDb.good  offset:0  width:1  srcfield:algo_lib.FDb.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.notbitfld  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.FDb.notbitfld  offset:8  width:8  srcfield:algo_lib.FDb.word  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a bitset over a 9-byte (72-bit) Smallstr element, and check that amc
// reports amc.bitset_elem.
// The element is no unsigned integer builtin, so it has no exact bit index/shift
// split.
void atf_comp::comptest_amc_BadBitsetElem() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Str9  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Str9.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Str9.ch  length:9  strtype:rightpad  pad:\"' '\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:algo_lib.Str9  size:9  alignment:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestBitset  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.bits  arg:algo_lib.Str9  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.bits  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a bitset over an i8 element, and check that amc reports amc.bitset_elem.
// A signed element compiles and sign-extends on read, so bit queries such as Sup
// return wrong values at run time.
void atf_comp::comptest_amc_BadBitsetSigned() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestBitset  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.bits  arg:i8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.bits  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a bitset over a u32 element in a universe without the u32 csize row, and
// check that amc.bitset_elem_size names the missing csize row.
// The u32 element type is valid.  amc lacks only its width.  A diagnostic that
// prescribed a different element type would send the user to replace a valid type.
void atf_comp::comptest_amc_BadBitsetNosize() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestBitset  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.bits  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.bits  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc five bitsets, three of them over elements whose csize row states a width
// other than the width in the type's name.  Check that amc reports
// amc.bitset_elem_width for each of the three, and that the two controls draw no
// report.
// Each type name passes the unsigned-builtin check, while amc generates the
// accessors from the csize row.  A u32 of 3 bytes is 24 bits, which the FloorLog2
// index split truncates.  A u128 of 32 bytes is 256 bits, past the widest bit-scan
// helper.  A u16 of 8 bytes is 64 bits.  The accessors index that width well.  The
// struct declares a 16-bit member, though, so the shifts run off its end.  The u64
// and u8 elements are the controls, because their csize rows agree with their names.
void atf_comp::comptest_amc_BadBitsetWidth() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:8  alignment:8  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:3  alignment:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:32  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestBitset  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.bits  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.bits  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.wide  arg:u128  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.wide  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.mid  arg:u16  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.mid  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.ok  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.ok  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.small  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.small  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc two fixed-inlary field defaults that name the record by the Init
// parameter name, in shapes outside what the copy-constructor inliner rewrites.
// Check that amc reports amc.copyctor_init for both and exits nonzero.
// One default names the record bare (sizeof parent).  Inlining would leak that name
// into the member copy constructor as an undeclared identifier.  The other reaches
// the name through a member operator (gcfg.parent.n), where the name belongs to
// another record.  The member-access rewrite would take the operator with it, and
// turn the default into a different expression that still compiles.
void atf_comp::comptest_amc_BadCopyctorInit() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.grow  arg:u32  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.grow  min:1  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.fix  arg:u32  reftype:Inlary  dflt:\"u32(sizeof parent)\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.fix  min:4  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.mem  arg:u32  reftype:Inlary  dflt:\"u32(gcfg.parent.n)\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.mem  min:4  max:4  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc inlary fields in each shape that the copy-constructor inliner covers,
// print algo_lib.Rec..CopyCtor, and check that the constructor body retargets each
// reference to the record to *this and copies the rest of each default verbatim.
// A variable inlary's Init macro carries the two shapes that the generators emit:
// member access on the record, and the record passed to a function by reference.
// The inliner retargets both to *this.  A fixed inlary's default is arbitrary user
// text.  A default that names the record as *this (size) reaches the inliner
// spelled as the Init parameter, and returns to *this.  The inliner copies verbatim
// a default that names no record (lit).  It also copies gparent.n verbatim (fix),
// because a longer identifier that merely ends with the parameter name is a
// different name.
// The str, note and raw defaults carry the parameter name inside a string literal,
// a block comment, and a raw string literal whose payload holds the quote that ends
// an ordinary one.  Each is content, so the inliner copies each verbatim and amc
// reports none.  The chr default puts a reference to the record after a character
// literal that holds a quote.  A scan that read that quote as opening a string
// literal would lose the reference, and leave the undeclared parameter name in the
// constructor.  With the literal last, the same misreading would run out of text
// and emit the same body, so the test puts the reference behind it.
void atf_comp::comptest_amc_CopyctorInit() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N algo_lib.Rec..CopyCtor < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.grow  arg:u32  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.grow  min:1  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.fix  arg:u32  reftype:Inlary  dflt:\"u32(gparent.n)\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.fix  min:4  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.size  arg:u32  reftype:Inlary  dflt:\"u32(ssizeof(*this))\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.size  min:4  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.lit  arg:u32  reftype:Inlary  dflt:\"7\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.lit  min:4  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.str  arg:u32  reftype:Inlary  dflt:'u32(sizeof(\"parent\"))'  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.str  min:4  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.note  arg:u32  reftype:Inlary  dflt:\"u32(1 /* parent */)\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.note  min:4  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.chr  arg:u32  reftype:Inlary  dflt:\"u32('\\\"') + u32(sizeof(parent))\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.chr  min:4  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.raw  arg:u32  reftype:Inlary  dflt:'u32(sizeof(R\"(a \"parent\" b)\"))'  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.raw  min:4  max:4  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc one Bitfld field per combination of arg type and width, print each
// field's ReadStrptrMaybe, and check which readers bound the value by the field's
// store.
// A read parses into a temporary of the field's arg type, and the Bitfld setter
// masks the store.  So the store truncates a value too wide for the bits, unless
// the reader refuses it first.  The reader of a field with a symbolic constant
// tries the symbol and falls back to the arg type.  amc emits the refusal there,
// with a term for each end of the arg type's range that the field's bits fall
// short of.
// At the high end narrow (u32 in 3 bits) and qos (a u32 wrapped in one field, in 4
// bits) draw a bound.  full (u8 in 8 bits), signed_full (i8 in 8 bits) and flag
// (bool in 1 bit) span their whole arg type and draw none.
// At the low end a store that drops the arg type's sign bit reads a negative back
// as a positive number.  sig (an i32 wrapped in one field, in 4 bits) refuses a
// negative on top of its high-end bound.  The bits of sig_edge (an i8 wrapper in 7
// bits) hold every positive value of its arg type, so it refuses a negative with
// no high-end bound.  sig_full (the same wrapper in 8 bits) keeps the sign and
// draws neither term.
// plain is narrow again with no symbolic constant, and its reader carries no term.
// Its three bits take a "9" as 1, and the read reports success.
// An fdec field carries its bound inside the loops that its reader parses with.
// dec scales two decimal places into 12 bits.  So the digit loop takes a digit
// only while the value it builds still fits 4095, the bound of the field's own
// store.  The u64 that the digits accumulate into is wider.  The loop that fills
// the decimal places a short text leaves out tests the same bound.  The reader
// leaves the finished magnitude untested, because it bounded every digit that went
// into it.
// amc rejects one combination at generation time, a width past the arg type, so
// it never reaches a reader.  amc also rejects a narrower width over a signed arg
// type, in a universe that carries the amcdb.bltin table.  This fixture carries
// none.  Signedness has two sources.  The range machinery reads it from the type's
// name, so i8 and i32 are signed here.  The size pass reads the type's amcdb.bltin
// row and finds none, so its amc.bitfield_signed_narrow never fires.  That is why
// sig and sig_edge reach a reader in this golden.  With the table, amc reports
// both of them and generates no code.  The name-based source is deliberate.  A
// universe without a signed integer's amcdb.bltin row still gives it signed range
// checks, and this fixture is that universe.
void atf_comp::comptest_amc_BitfldReadRange() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.Rec.(narrow|full|signed_full|flag|qos|sig|sig_edge|sig_full|dec|plain).ReadStrptrMaybe' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Qos  comment:\"value wrapped in a single field\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Qos.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Qos.String  printfmt:Raw  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Rec.String  printfmt:Tuple  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.value  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.narrow  arg:u32  reftype:Bitfld  dflt:''  comment:\"arg range exceeds the field's bits\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.narrow  offset:0  width:3  srcfield:algo_lib.Rec.value  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.narrow/zero  value:0  comment:\"a symbol puts the read on the path that bounds the value\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.full  arg:u8  reftype:Bitfld  dflt:''  comment:\"field spans the whole arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.full  offset:3  width:8  srcfield:algo_lib.Rec.value  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.full/zero  value:0  comment:\"same path, and the bits reach every value the arg type holds\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.signed_full  arg:i8  reftype:Bitfld  dflt:''  comment:\"signed arg, whole width\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.signed_full  offset:11  width:8  srcfield:algo_lib.Rec.value  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.signed_full/zero  value:0  comment:\"same path, and the bits keep the sign\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.flag  arg:bool  reftype:Bitfld  dflt:''  comment:\"bool holds no value past one bit\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.flag  offset:19  width:1  srcfield:algo_lib.Rec.value  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.flag/zero  value:0  comment:\"same path, and one bit is the whole of a bool\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.qos  arg:algo_lib.Qos  reftype:Bitfld  dflt:''  comment:\"wrapped value, arg range exceeds the bits\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.qos  offset:20  width:4  srcfield:algo_lib.Rec.value  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.qos/zero  value:0  comment:\"same path, through the field the wrapper stores into\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sig  comment:\"signed value wrapped in a single field\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sig.value  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Sig.String  printfmt:Raw  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.sig  arg:algo_lib.Sig  reftype:Bitfld  dflt:''  comment:\"wrapped signed value, narrowed by the bits\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.sig  offset:24  width:4  srcfield:algo_lib.Rec.value  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.sig/zero  value:0  comment:\"same path, and the bits drop both ends of the wrapped range\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.dec  arg:u32  reftype:Bitfld  dflt:''  comment:\"decimal value scaled into the field's bits\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.dec  offset:28  width:12  srcfield:algo_lib.Rec.value  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.Rec.dec  nplace:2  fixedfmt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sig8  comment:\"signed 8-bit value wrapped in a single field\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sig8.value  arg:i8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Sig8.String  printfmt:Raw  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.sig_edge  arg:algo_lib.Sig8  reftype:Bitfld  dflt:''  comment:\"wrapped signed value, bits hold its whole positive half\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.sig_edge  offset:40  width:7  srcfield:algo_lib.Rec.value  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.sig_edge/zero  value:0  comment:\"same path, and the bits drop the sign alone\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.sig_full  arg:algo_lib.Sig8  reftype:Bitfld  dflt:''  comment:\"wrapped signed value, bits span the whole arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.sig_full  offset:47  width:8  srcfield:algo_lib.Rec.value  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.sig_full/zero  value:0  comment:\"same path, and the bits span the wrapped arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.plain  arg:u32  reftype:Bitfld  dflt:''  comment:\"narrow again, with no symbol to read first\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.plain  offset:55  width:3  srcfield:algo_lib.Rec.value  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a cascdel Ptrary whose row ctype has no instance pool, and check that amc
// reports amc.nopool on the xref and then the cascdel pool vrfy error.
// With no pool to delete a row from, DeleteExpr would be empty.  The Cascdel loop
// would then spin forever.  The test pins the existing rejection, so the loop stays
// unreachable.
void atf_comp::comptest_amc_BadCascdelNopool() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.c_row  arg:algo_lib.FRow  reftype:Ptrary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ptrary  field:algo_lib.FDb.c_row  unique:Y  heaplike:N");
    atf_comp::ProcWrite(proc, "dmmeta.cascdel  field:algo_lib.FDb.c_row  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Ptrary.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Ptrary.oncecurs  dflt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.c_row  inscond:false  via:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a cascdel unique:Y Ptrary whose field has no dmmeta.xref record, and
// check that amc reports amc.cascdel_xref.
// The generated Cascdel deletes the last row until the array is empty.  Only an
// xref makes the row's delete unlink it, so without one the loop would never end.
void atf_comp::comptest_amc_BadCascdelXref() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.c_row  arg:algo_lib.FRow  reftype:Ptrary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ptrary  field:algo_lib.FDb.c_row  unique:Y  heaplike:N");
    atf_comp::ProcWrite(proc, "dmmeta.cascdel  field:algo_lib.FDb.c_row  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Ptrary.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Ptrary.oncecurs  dflt:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a buffer condition on a global (FDb) fbuf, and check that amc reports
// amc.fcond_arg.
// The record that enters the list is the buffer's parent row.  A global has no row
// to enter it with.
void atf_comp::comptest_amc_BadFbufCondGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:cd  circular:Y  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuftype  fbuftype:Bytebuf  skipbytes:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbufdir  fbufdir:in  read:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbufdir  fbufdir:out  read:N  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.delcurs  dflt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FIohook  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FIohook.fildes  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.cd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.cd_ready  havetail:N  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.cd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fcond  fcond:algo_lib.FDb.in/ready  ins:algo_lib.FDb.cd_ready  via:''  rem:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc cfmts across the strfmt and printfmt pairs, and check that amc reports
// amc.badcfmt, naming both attributes, for each print:Y pair with no print path.
// Such a cfmt would ship a Print function with an empty body.  amc rejects one
// ctype per axis.  Sample has the Tuple strfmt, which has no print path under any
// printfmt.  Compact has the CompactSep printfmt under the String strfmt, and the
// String arm leaves that printfmt unhandled.  The rest of the table are controls
// that draw no diagnostic: String with Tuple, Auto, Raw, Sep and Bitset; Argv and
// Json, which take any printfmt because neither reads one; the Extern printfmt,
// which hands the body to the user even under the Tuple strfmt; and a print:N cfmt
// (Quiet) that carries the rejected printfmt and generates no print function.
void atf_comp::comptest_amc_BadCfmtPrint() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sample  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sample.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Sample.Tuple  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Compact  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Compact.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Compact.b  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Compact.String  printfmt:CompactSep  read:N  print:Y  sep:,  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Quiet  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Quiet.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Quiet.String  printfmt:CompactSep  read:N  print:N  sep:,  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CTuple  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CTuple.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CTuple.String  printfmt:Tuple  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CAuto  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CAuto.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CAuto.String  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CRaw  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CRaw.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CRaw.String  printfmt:Raw  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CSep  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CSep.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CSep.b  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CSep.String  printfmt:Sep  read:N  print:Y  sep::  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CBitset  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CBitset.a  arg:bool  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CBitset.b  arg:bool  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CBitset.String  printfmt:Bitset  read:N  print:Y  sep:,  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CArgv  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CArgv.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CArgv.Argv  printfmt:CompactSep  read:N  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CJson  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CJson.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CJson.b  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:u32.Json  printfmt:Extern  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CJson.Json  printfmt:CompactSep  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CExtern  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CExtern.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CExtern.Tuple  printfmt:Extern  read:N  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc read:Y cfmts across the whole table of strfmt and printfmt pairs that
// the read emitter decides, and check that amc reports amc.badcfmt, naming both
// attributes, for the one pair with no read path.
// Such a cfmt would ship a ReadStrptrMaybe with an empty body.  That reader returns
// success for every input, and leaves the destination holding its old value.  A
// caller then has no way to tell a parsed value from an unparsed one.  The rejected
// pair is the CompactSep printfmt under the String strfmt (RCompact), the one
// printfmt that the String arm has no reader for.  The rest of the table are
// controls that draw no diagnostic: String with Tuple, Auto, Raw, Sep, Bitset and
// Extern, where Auto reads a tuple the way the print side prints one; the Tuple
// strfmt, whose reader consumes an already-parsed tuple and ignores the printfmt,
// so it accepts CompactSep and still hands Extern to the user; the Json strfmt,
// which the read emitter passes over; and a read:N cfmt (RQuiet) that carries the
// rejected printfmt and generates no read function.
void atf_comp::comptest_amc_BadCfmtRead() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RCompact  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RCompact.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RCompact.b  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RCompact.String  printfmt:CompactSep  read:Y  print:N  sep:,  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RQuiet  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RQuiet.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RQuiet.String  printfmt:CompactSep  read:N  print:N  sep:,  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RTuple  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RTuple.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RTuple.String  printfmt:Tuple  read:Y  print:N  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RAuto  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RAuto.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RAuto.String  printfmt:Auto  read:Y  print:N  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RRaw  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RRaw.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RRaw.String  printfmt:Raw  read:Y  print:N  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RSep  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RSep.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RSep.b  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RSep.String  printfmt:Sep  read:Y  print:N  sep::  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RBitset  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RBitset.a  arg:bool  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RBitset.b  arg:bool  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RBitset.String  printfmt:Bitset  read:Y  print:N  sep:,  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RExtern  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RExtern.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RExtern.String  printfmt:Extern  read:Y  print:N  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RTupfmt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RTupfmt.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RTupfmt.Tuple  printfmt:CompactSep  read:Y  print:N  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RTupxtern  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RTupxtern.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RTupxtern.Tuple  printfmt:Extern  read:Y  print:N  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RJson  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RJson.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RJson.Json  printfmt:CompactSep  read:Y  print:N  sep:''  genop:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a String cfmt with printfmt:CompactSep, and check that the print and the
// read emitter each report amc.badcfmt and that the run reaches its exit with a
// nonzero code.
// The printfmt table declares the value.  amc generates neither a printer nor a
// reader for it, so the request would ship an empty print body and an empty read
// body.  Each diagnostic names the cfmt, the strfmt and the offending printfmt.
// This golden pins the two diagnostics of one cfmt.  BadCfmtPrint and BadCfmtRead
// pin the wider table that each emitter decides.  The Sep ctype (Ok) beside it is
// the control, and draws no diagnostic on either side because amc generates its
// printfmt.
void atf_comp::comptest_amc_BadCompactSep() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Ok  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ok.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ok.b  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Ok.String  printfmt:Sep  read:Y  print:Y  sep:\":\"  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Compact  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Compact.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Compact.b  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Compact.String  printfmt:CompactSep  read:Y  print:Y  sep:\":\"  genop:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc the table of command-line shapes that the reader's gates decide, and
// check that amc reports amc.badargvread for each rejected shape, naming the reader
// and the companion that it lacks.
// amc assembles the field-aware command-line reader from companion functions, and
// emits each companion under its own gate.  A read cfmt with strfmt:Argv yields the
// reader and NArgs.  A read cfmt whose printfmt is neither Raw nor Extern yields
// ReadFieldMaybe and GetAnon.  The namespace-level ReadArgv calls the cmdline
// ctype's reader.
// These shapes draw no diagnostic: command.good (Argv read:Y printfmt Auto over a
// base that supplies both companions); command.goodnobase (no basecmdline, so the
// reader calls no base companion); command.goodraw (its Argv cfmt is Raw, and a
// second read cfmt supplies ReadFieldMaybe and GetAnon for its anonfld); and
// namespace okapp over command.good.
// amc rejects the rest: command.raw and command.xtern, whose only read cfmt is Raw
// or Extern, so they lack the store and the positional lookup; command.basenoread
// and command.baseraw, whose base lacks NArgs or the store; algo_lib.CmdlineRaw,
// which is a base and a broken reader at once; and namespaces noreadapp and gnuapp,
// whose cmdline ctype yields no reader.  The Argv cfmt behind noreadapp is read:N.
// ArgvGnu, behind gnuapp, generates no reader however it is spelled.
void atf_comp::comptest_amc_BadArgvRead() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:command  nstype:lib  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:command  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:okapp  nstype:lib  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:okapp  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:noreadapp  nstype:lib  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:noreadapp  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:gnuapp  nstype:lib  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:gnuapp  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Cmdline  comment:\"base command line that generates both companions the reader calls\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmdline.help  arg:bool  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmdline.verbose  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Cmdline.Argv  printfmt:Auto  read:Y  print:N  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CmdlineNoread  comment:\"base command line whose Argv cfmt is read:N: no NArgs\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CmdlineNoread.help  arg:bool  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CmdlineNoread.Argv  printfmt:Auto  read:N  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CmdlineRaw  comment:\"base command line whose only read cfmt is Raw: no ReadFieldMaybe\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CmdlineRaw.help  arg:bool  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CmdlineRaw.Argv  printfmt:Raw  read:Y  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.cmdline  arg:algo_lib.Cmdline  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.cmdline_noread  arg:algo_lib.CmdlineNoread  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.cmdline_raw  arg:algo_lib.CmdlineRaw  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.good  comment:\"accept: Argv read:Y, printfmt Auto, base generates both companions\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.good.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.good.Argv  printfmt:Auto  read:Y  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.good  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.goodnobase  comment:\"accept: no basecmdline, so the reader calls no base companion\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.goodnobase.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.goodnobase.Argv  printfmt:Auto  read:Y  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.goodnobase  read:Y  basecmdline:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.goodraw  comment:\"accept: the Argv cfmt is Raw but the String cfmt supplies ReadFieldMaybe and GetAnon\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.goodraw.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.goodraw.name  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.anonfld  field:command.goodraw.name  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.goodraw.Argv  printfmt:Raw  read:Y  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.goodraw.String  printfmt:Tuple  read:Y  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.goodraw  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.raw  comment:\"reject: the only read cfmt is Raw, so neither ReadFieldMaybe nor GetAnon\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.raw.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.raw.name  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.anonfld  field:command.raw.name  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.raw.Argv  printfmt:Raw  read:Y  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.raw  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.xtern  comment:\"reject: the only read cfmt is Extern, so no ReadFieldMaybe\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.xtern.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.xtern.Argv  printfmt:Extern  read:Y  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.xtern  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.basenoread  comment:\"reject: the base's Argv cfmt is read:N, so no base NArgs\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.basenoread.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.basenoread.Argv  printfmt:Auto  read:Y  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.basenoread  read:Y  basecmdline:algo_lib.FDb.cmdline_noread  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.baseraw  comment:\"reject: the base's only read cfmt is Raw, so no base ReadFieldMaybe\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.baseraw.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.baseraw.Argv  printfmt:Auto  read:Y  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.baseraw  read:Y  basecmdline:algo_lib.FDb.cmdline_raw  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.noread  comment:\"reject at the namespace: Argv cfmt is read:N, so no command.noread_ReadArgv\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.noread.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.noread.Argv  printfmt:Auto  read:N  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.noread  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.gnu  comment:\"reject at the namespace: only an ArgvGnu cfmt, which generates no reader\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.gnu.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.gnu.ArgvGnu  printfmt:Auto  read:Y  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.gnu  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:okapp.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:okapp.FDb._db  arg:okapp.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:okapp.FDb.cmdline  arg:command.good  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:okapp  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:noreadapp.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:noreadapp.FDb._db  arg:noreadapp.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:noreadapp.FDb.cmdline  arg:command.noread  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:noreadapp  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:gnuapp.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:gnuapp.FDb._db  arg:gnuapp.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:gnuapp.FDb.cmdline  arg:command.gnu  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:gnuapp  ismodule:N");
    atf_comp::ProcWriteEof(proc);
}

// Give amc Bytebuf, Linebuf and Msgbuf fields over elements of several widths and
// comparison sources, and check that amc reports amc.fbuf_elem for each buffer
// whose element breaks the way the buffer measures or scans a message.
// A Bytebuf or Linebuf measures its message in bytes and hands it back as an
// element count.  A wider element makes the returned message run past the data.
// amc takes the element's width from the declared csize of a bltin, and from the
// emitted struct body of a generated ctype.  The fixture crosses both buffer types
// with both width sources.  amc rejects a byte buffer of u16, of u64, of an
// eight-byte generated struct and of an extern struct of unknown width, and a line
// buffer of u32.  The accepted controls are both buffer types over a one-byte bltin
// and over a one-byte generated struct, and a Msgbuf over the eight-byte struct.  A
// Msgbuf carries its length in the message header.
// A Linebuf needs more than a one-byte element.  Its generated ScanMsg finds the
// end of the message by comparing each element with the delimiter.  So the same
// fixture crosses the sources of an operator == with the shapes that emit no
// comparison:
//
//   Linebuf element                    | ScanMsg      | accepted
//
//   one-byte bltin (char)              | generated    | yes
//   struct, ccmp genop:Y               | generated    | yes
//   struct, ccmp extrn:Y               | generated    | yes
//   struct, ccmp order:Y genop:N       | generated    | no
//   struct, no ccmp                    | generated    | no
//   struct, no ccmp                    | ffunc extrn:Y| yes
//   struct, no ccmp, out direction     | not emitted  | yes
//
// The Bytebuf over the same ccmp-less struct stays accepted throughout, because it
// returns every available byte and compares no element.
void atf_comp::comptest_amc_BadFbufElem() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i8  likeu64:Y  bigendok:N  issigned:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u8  likeu64:Y  bigendok:N  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u8  extrn:N  genop:N  order:Y  minmax:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i16  likeu64:Y  bigendok:Y  issigned:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i64  likeu64:Y  bigendok:Y  issigned:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:N  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:bool  likeu64:Y  bigendok:N  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:char  likeu64:Y  bigendok:N  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:cd  circular:Y  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuftype  fbuftype:Bytebuf  skipbytes:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbufdir  fbufdir:in  read:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbufdir  fbufdir:out  read:N  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.delcurs  dflt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuftype  fbuftype:Linebuf  skipbytes:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuftype  fbuftype:Msgbuf  skipbytes:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FIohook  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FIohook.fildes  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Elem  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Elem.value  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Eqelem  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Eqelem.value  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo_lib.Eqelem  extrn:N  genop:Y  order:N  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Extrnelem  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Extrnelem.value  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo_lib.Extrnelem  extrn:Y  genop:N  order:N  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Cmpelem  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmpelem.value  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo_lib.Cmpelem  extrn:N  genop:N  order:Y  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Opaque  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cextern  ctype:algo_lib.Opaque  initmemset:N  isstruct:Y  plaindata:N");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_byte  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_byte  max:64  fbuftype:Bytebuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_char  arg:char  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_char  max:64  fbuftype:Bytebuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_line  arg:char  reftype:Fbuf  dflt:\"'\\\\n'\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_line  max:64  fbuftype:Linebuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.out_msg  arg:algo_lib.FRow  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.out_msg  max:64  fbuftype:Msgbuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_wide  arg:u16  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_wide  max:64  fbuftype:Bytebuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_word  arg:u64  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_word  max:64  fbuftype:Bytebuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_rec  arg:algo_lib.FRow  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_rec  max:64  fbuftype:Bytebuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_lwide  arg:u32  reftype:Fbuf  dflt:\"32\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_lwide  max:64  fbuftype:Linebuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_elem  arg:algo_lib.Elem  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_elem  max:64  fbuftype:Bytebuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_eline  arg:algo_lib.Elem  reftype:Fbuf  dflt:\"algo_lib::Elem()\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_eline  max:64  fbuftype:Linebuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_eqline  arg:algo_lib.Eqelem  reftype:Fbuf  dflt:\"algo_lib::Eqelem()\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_eqline  max:64  fbuftype:Linebuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_xline  arg:algo_lib.Extrnelem  reftype:Fbuf  dflt:\"algo_lib::Extrnelem()\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_xline  max:64  fbuftype:Linebuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_cmpline  arg:algo_lib.Cmpelem  reftype:Fbuf  dflt:\"algo_lib::Cmpelem()\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_cmpline  max:64  fbuftype:Linebuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_scanline  arg:algo_lib.Elem  reftype:Fbuf  dflt:\"algo_lib::Elem()\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_scanline  max:64  fbuftype:Linebuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ffunc  ffunc:algo_lib.FDb.in_scanline.ScanMsg  extrn:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.out_eline  arg:algo_lib.Elem  reftype:Fbuf  dflt:\"algo_lib::Elem()\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.out_eline  max:64  fbuftype:Linebuf  iotype:nofd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_opaque  arg:algo_lib.Opaque  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_opaque  max:64  fbuftype:Bytebuf  iotype:nofd  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc Varlen, Opt and Val fields over an element type with a destructor and
// over a plain one, and check that amc reports amc.opt_dtor for the Varlen and the
// Opt field over the element with a destructor.
// A Varlen or an Opt field addresses its element in place inside the message that
// carries it.  The message allocates no storage for the element, and destroying
// the message ends no lifetime of the element's own.  The program would construct
// an element whose type owns a destructor and never destroy it, which leaks
// whatever the element holds.  So amc refuses the schema at generation time.
// The ctype decides whether an element type owns a destructor.  The pass that also
// decides whether a value is plain data computes this flag for the whole ctype
// graph, so this check reads a flag that a different pass fills in.  Run before
// that pass, the check would read the flag's initial value, find every element
// type destructor-free, and pass a schema that it exists to refuse.  So the
// fixture carries the accepted rows next to the refused ones.  A check that has
// gone quiet treats both kinds alike.
//
//   field reftype | element type       | accepted
//
//   Varlen        | has destructor     | no
//   Opt           | has destructor     | no
//   Varlen        | plain data         | yes
//   Opt           | plain data         | yes
//   Val           | has destructor     | yes
//
// The element that owns a destructor is an extern ctype declared plaindata:N, the
// shortest way to say that destroying it does something.  The plain element is a
// packed ctype of one integer field.  The Val row is the control on the rule's
// other edge.  That reftype gives its member a lifetime of its own, so the member's
// destructor runs and the element type is unconstrained.
void atf_comp::comptest_amc_BadOptDtor() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i8  likeu64:Y  bigendok:N  issigned:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u8  likeu64:Y  bigendok:N  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u8  extrn:N  genop:N  order:Y  minmax:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i16  likeu64:Y  bigendok:Y  issigned:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i64  likeu64:Y  bigendok:Y  issigned:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:N  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:bool  likeu64:Y  bigendok:N  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:char  likeu64:Y  bigendok:N  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Opaque  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cextern  ctype:algo_lib.Opaque  initmemset:N  isstruct:Y  plaindata:N");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:algo_lib.Opaque  size:8  alignment:8  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.PlainElem  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.PlainElem  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PlainElem.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgVarlenDtor  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgVarlenDtor  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgVarlenDtor.head  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgVarlenDtor.tail  arg:algo_lib.Opaque  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgOptDtor  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgOptDtor  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOptDtor.head  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOptDtor.tail  arg:algo_lib.Opaque  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgVarlenPlain  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgVarlenPlain  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgVarlenPlain.head  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgVarlenPlain.tail  arg:algo_lib.PlainElem  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgOptPlain  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgOptPlain  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOptPlain.head  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOptPlain.tail  arg:algo_lib.PlainElem  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgValDtor  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgValDtor  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgValDtor.body  arg:algo_lib.Opaque  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a universe with an FDb index of each reftype and a condition whose ins
// index is a hash, and check that amc reports amc.fcond_ins for the hash.
// A condition inserts into its ins index by name, one row at a time.  So the index
// must let a single row in and out.  A hash is the tempting mistake, because it
// holds the same rows and its name reads like a list's.  It offers InsertMaybe in
// place of Insert, so the emitted call fails to compile.  The rule states the
// requirement positively, as an Llist or a Bheap.  That one rule refuses every
// other shape a field can have: a pool and a pointer array, which lack a
// single-row remove; a count, which keeps a tally and hands no row back; and a
// tree or a sorted list, whose insert takes more than the row.
// The fcond rules are vrfy checks, so the first violation ends the run.  Each run
// therefore names one rejected row, and a fixture crossing every reftype would
// report the same single line.  The accepted control is a condition on the
// sibling buffer that names an Llist of the watched ctype, and it draws no
// diagnostic.
void atf_comp::comptest_amc_BadFcondIns() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u64  extrn:N  genop:N  order:Y  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:cd  circular:Y  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuftype  fbuftype:Bytebuf  skipbytes:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbufdir  fbufdir:in  read:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbufdir  fbufdir:out  read:N  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.delcurs  dflt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FIohook  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FIohook.fildes  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.cd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.cd_ready  havetail:N  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.cd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FConn  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.connid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.conn  arg:algo_lib.FConn  reftype:Lary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.cd_conn_read  arg:algo_lib.FConn  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.cd_conn_read  havetail:N  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.cd_conn_read  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.c_conn_read  arg:algo_lib.FConn  reftype:Ptrary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ptrary  field:algo_lib.FDb.c_conn_read  unique:Y  heaplike:N");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.c_conn_read  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.ind_conn  arg:algo_lib.FConn  reftype:Thash  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.thash  field:algo_lib.FDb.ind_conn  hashfld:algo_lib.FConn.connid  unique:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.ind_conn  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.n_conn  arg:algo_lib.FConn  reftype:Count  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.bh_conn  arg:algo_lib.FConn  reftype:Bheap  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.sortfld  field:algo_lib.FDb.bh_conn  sortfld:algo_lib.FConn.connid");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.bh_conn  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.tr_conn  arg:algo_lib.FConn  reftype:Atree  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.sortfld  field:algo_lib.FDb.tr_conn  sortfld:algo_lib.FConn.connid");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.tr_conn  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zs_conn  arg:algo_lib.FConn  reftype:ZSListMT  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zs_conn  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.p_conn  arg:algo_lib.FConn  reftype:Upptr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.cd_sub  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FConn.cd_sub  havetail:N  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FConn.cd_sub  inscond:true  via:algo_lib.FRow.p_conn");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:lib_other  nstype:lib  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:lib_other  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:lib_other.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:lib_other.FDb._db  arg:lib_other.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:lib_other.FConn  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:lib_other.FConn.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:lib_other.FDb.conn  arg:lib_other.FConn  reftype:Lary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:lib_other.FDb.cd_conn_read  arg:lib_other.FConn  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:lib_other.FDb.cd_conn_read  havetail:N  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:lib_other.FDb.cd_conn_read  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:lib_other.FDb.cd_conn_eof  arg:lib_other.FConn  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:lib_other.FDb.cd_conn_eof  havetail:N  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:lib_other.FDb.cd_conn_eof  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.out  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.out  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_ns  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_ns  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_eof  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_eof  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_elem  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_elem  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_par  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_par  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_pary  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_pary  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_pool  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_pool  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_hash  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_hash  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_count  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_count  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_bheap  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_bheap  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_atree  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_atree  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_zslist  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_zslist  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_self  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_self  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_eofself  arg:u8  reftype:Fbuf  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_eofself  max:64  fbuftype:Bytebuf  iotype:standard  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fcond  fcond:algo_lib.FConn.in/ready  ins:algo_lib.FDb.cd_conn_read  via:''  rem:Y  comment:\"accepted control: an Llist of the watched ctype\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcond  fcond:algo_lib.FConn.in_hash/ready  ins:algo_lib.FDb.ind_conn  via:''  rem:Y  comment:\"rejected: a hash offers InsertMaybe, not Insert\"");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a TimeHookOnce fstep on an Llist field, and check that amc reports
// amc.fstep_bheap.
// The step reads the expiration time from the first row's sort field, and only a
// Bheap step field provides one.
void atf_comp::comptest_amc_BadFstepBheap() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.delcurs  dflt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookOnce  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_row  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_row  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_row  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.zd_row  steptype:TimeHookOnce  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a TimeHookRecur fstep on an Upptr field, and check that amc reports
// amc.fstep_first.
// The generated code arms the time hook from the index's first row, and only an
// Llist or Bheap step field provides one.
void atf_comp::comptest_amc_BadFstepFirst() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookRecur  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.p_row  arg:algo_lib.FRow  reftype:Upptr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.p_row  steptype:TimeHookRecur  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a TimeHookRecur fstep on an Atree field, and check that amc reports
// amc.fstep_first.
// FirstChanged calls arm the hook, and only the Llist and Bheap generators emit
// them.  An Atree step would compile and never fire.
void atf_comp::comptest_amc_BadFstepAtree() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u64  extrn:N  genop:N  order:Y  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookRecur  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.tr_row  arg:algo_lib.FRow  reftype:Atree  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.sortfld  field:algo_lib.FDb.tr_row  sortfld:algo_lib.FRow.rowid");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.tr_row  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.tr_row  steptype:TimeHookRecur  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc an fstep on a fixed Inlary field, and check that amc reports
// amc.fstep_reftype.
// The step's loop condition tests the index with EmptyQ, which amc generates for a
// variable Inlary alone.  A fixed Inlary always holds max elements and offers no
// emptiness test.
void atf_comp::comptest_amc_BadFstepInlary() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.fixary  arg:u32  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.fixary  min:3  max:3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.fixary  steptype:Inline  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc an InlineRecur fstep with fdelay scale:Y on a Val field, and check that
// amc reports amc.fstep_scale.
// The scaled delay divides by the step index's row count, and a Val field has no
// row count.
void atf_comp::comptest_amc_BadFstepScale() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:InlineRecur  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.rowcheck  arg:bool  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.rowcheck  steptype:InlineRecur  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdelay  fstep:algo_lib.FDb.rowcheck  delay:1.000000000  scale:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a Callback fstep on a ZSListMT field, and check that amc reports
// amc.fstep_zslistmt.
// The list is concurrent and has no EmptyQ, so the step's loop condition tests
// DestructiveFirst.  Only the Inline and InlineRecur call shapes embed that test.
void atf_comp::comptest_amc_BadFstepZslistmt() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Callback  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zs_row  arg:algo_lib.FRow  reftype:ZSListMT  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zs_row  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.zs_row  steptype:Callback  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc an fdelay row on an Inline fstep, and check that amc reports
// amc.fstep_fdelay.
// Only the InlineRecur and TimeHookRecur call shapes read the delay between
// invocations.  On any other steptype the row would go unused.
void atf_comp::comptest_amc_BadFstepFdelay() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.rowcheck  arg:bool  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.rowcheck  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdelay  fstep:algo_lib.FDb.rowcheck  delay:1.000000000  scale:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc an fstep on an Lpool field, and check that amc reports
// amc.fstep_reftype.
// The step's loop condition tests the field for pending work.  A memory pool
// offers no emptiness test, so the generated condition would call an EmptyQ that
// does not exist.
void atf_comp::comptest_amc_BadFstepReftype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.sbrk  arg:u8  reftype:Sbrk  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.lpool  arg:u8  reftype:Lpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.basepool  field:algo_lib.FDb.lpool  base:algo_lib.FDb.sbrk");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.lpool  steptype:Inline  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc InlineRecur fsteps with fdelay scale:Y on a Blkhash and on a Lary field,
// and check that amc generates the schema without a diagnostic.
// Both reftypes generate the N function that the scaled delay divides by.
void atf_comp::comptest_amc_FstepScaleBlkhash() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u32  extrn:N  genop:N  order:N  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RowKey  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RowKey.id  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RowKey.seq  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.chash  ctype:algo_lib.RowKey  hashtype:CRC32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo_lib.RowKey  extrn:N  genop:Y  order:N  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowkey  arg:algo_lib.RowKey  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.ind_row  arg:algo_lib.FRow  reftype:Blkhash  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.blkhash  field:algo_lib.FDb.ind_row  hashfld:algo_lib.FRow.rowkey  linfld:algo_lib.RowKey.seq  linbits:12  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.ind_row  inscond:true  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:InlineRecur  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.ind_row  steptype:InlineRecur  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdelay  fstep:algo_lib.FDb.ind_row  delay:1.000000000  scale:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FLrow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FLrow.value  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.lrow  arg:algo_lib.FLrow  reftype:Lary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.lrow  steptype:InlineRecur  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdelay  fstep:algo_lib.FDb.lrow  delay:1.000000000  scale:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc an fstep on a field of a non-global ctype, and check that amc reports
// amc.fstep_global.
// The generated step would read the field through the namespace global, and the
// FDb has no such member.
void atf_comp::comptest_amc_BadFstepGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FRow.rowid  steptype:Inline  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an executable an Alias field on its FDb that names a library's list, with an
// fstep on the alias.  Print the executable's Steps(), and check that amc generates
// the alias step in the executable and calls it there.
void atf_comp::comptest_amc_FstepAlias() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N yyy.FDb._db.Steps < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookOnce  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_ready  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:yyy  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:yyy  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb._db  arg:yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_ready  arg:algo_lib.FRow  reftype:Alias  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.falias  field:yyy.FDb.zd_ready  srcfield:algo_lib.FDb.zd_ready  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_ready  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dev.target  target:algo_lib");
    atf_comp::ProcWrite(proc, "dev.target  target:yyy");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:yyy.algo_lib  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an executable an alias step on a library list that has a step of its own.
// Print the executable's Steps(), and check that it calls the alias step in the
// library step's slot and never the library's step.
void atf_comp::comptest_amc_FstepAliasOverride() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N yyy.FDb._db.Steps < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookOnce  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_ready  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:yyy  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:yyy  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb._db  arg:yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_ready  arg:algo_lib.FRow  reftype:Alias  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.falias  field:yyy.FDb.zd_ready  srcfield:algo_lib.FDb.zd_ready  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_ready  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.zd_ready  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dev.target  target:algo_lib");
    atf_comp::ProcWrite(proc, "dev.target  target:yyy");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:yyy.algo_lib  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an executable an alias step whose band (idle) differs from the band of the
// step it overrides, and check that amc reports amc.fstep_alias_band.
// The override runs in the library step's slot.  A band of its own would have no
// say in its position, and would still change how amc generates its wrapper.
void atf_comp::comptest_amc_BadFstepAliasBand() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N yyy.FDb._db.Steps < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookOnce  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_ready  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:yyy  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:yyy  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb._db  arg:yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_ready  arg:algo_lib.FRow  reftype:Alias  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.falias  field:yyy.FDb.zd_ready  srcfield:algo_lib.FDb.zd_ready  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_ready  steptype:Inline  stepband:idle  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.zd_ready  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dev.target  target:algo_lib");
    atf_comp::ProcWrite(proc, "dev.target  target:yyy");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:yyy.algo_lib  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an executable an alias step of steptype TimeHookRecur, and check that amc
// reports amc.fstep_alias_steptype.
// The list's FirstChanged arms the time hook, and amc generates FirstChanged in the
// list's own namespace.
void atf_comp::comptest_amc_BadFstepAliasSteptype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookOnce  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_ready  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:yyy  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:yyy  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb._db  arg:yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookRecur  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_ready  arg:algo_lib.FRow  reftype:Alias  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.falias  field:yyy.FDb.zd_ready  srcfield:algo_lib.FDb.zd_ready  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_ready  steptype:TimeHookRecur  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give one executable's closure two alias steps on one list, one in the executable
// and one in a library it links, and check that amc reports amc.fstep_alias_rival.
// The two steps share the process's main loop and would race for the list's rows.
void atf_comp::comptest_amc_BadFstepAliasRival() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookOnce  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_ready  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:yyy  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:yyy  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb._db  arg:yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_ready  arg:algo_lib.FRow  reftype:Alias  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.falias  field:yyy.FDb.zd_ready  srcfield:algo_lib.FDb.zd_ready  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_ready  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:lib_yyy  nstype:lib  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:lib_yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:lib_yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:lib_yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:lib_yyy.FDb._db  arg:lib_yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:lib_yyy.FDb.zd_ready  arg:algo_lib.FRow  reftype:Alias  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.falias  field:lib_yyy.FDb.zd_ready  srcfield:algo_lib.FDb.zd_ready  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:lib_yyy.FDb.zd_ready  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dev.target  target:algo_lib");
    atf_comp::ProcWrite(proc, "dev.target  target:lib_yyy");
    atf_comp::ProcWrite(proc, "dev.target  target:yyy");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:lib_yyy.algo_lib  comment:''");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:yyy.lib_yyy  comment:''");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:yyy.algo_lib  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an executable an alias step on a list whose own step is a TimeHookRecur,
// and check that amc reports amc.fstep_alias_timehook.
// The list's own step runs from the time hook and never from Steps(), so the alias
// step would find no slot in Steps() to take over.
void atf_comp::comptest_amc_BadFstepAliasTimehook() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookOnce  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_ready  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:yyy  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:yyy  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb._db  arg:yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookRecur  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.zd_ready  steptype:TimeHookRecur  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_ready  arg:algo_lib.FRow  reftype:Alias  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.falias  field:yyy.FDb.zd_ready  srcfield:algo_lib.FDb.zd_ready  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_ready  steptype:Inline  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an executable an alias step that names a field of a non-global ctype, and
// check that amc reports amc.fstep_alias_list.
// Such a field is no list of another namespace's FDb.
void atf_comp::comptest_amc_BadFstepAliasList() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookOnce  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_ready  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:yyy  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:yyy  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb._db  arg:yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.rowid  arg:u64  reftype:Alias  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.falias  field:yyy.FDb.rowid  srcfield:algo_lib.FRow.rowid  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.rowid  steptype:Inline  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an executable steps in the input, work, output and idle bands, and a library
// step in the work band.  Print the executable's Steps(), and check that it calls
// the steps band by band: the input step, the work band, the output step and then
// the idle step.
// Namespaces run in dependency order within a band, so the executable's own work
// step precedes the library's.
void atf_comp::comptest_amc_FstepBand() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N yyy.FDb._db.Steps < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_ready  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:yyy  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:yyy  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb._db  arg:yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FJob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FJob.id  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.job  arg:yyy.FJob  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_in  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_in  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_in  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_out  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_out  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_out  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_bg  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_bg  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_bg  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_work  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_work  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_work  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dev.target  target:algo_lib");
    atf_comp::ProcWrite(proc, "dev.target  target:yyy");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:yyy.algo_lib  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_out  steptype:Inline  stepband:output  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_in  steptype:Inline  stepband:input  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.zd_ready  steptype:Inline  stepband:work  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_work  steptype:Inline  stepband:work  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_bg  steptype:Inline  stepband:idle  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fsteprate  fstep:yyy.FDb.zd_bg  ncall:1  npass:10  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an executable a TimeHookRecur step in the idle band, print the list's
// FirstChanged, and check that it arms the hook on the idle heap of algo_lib.
void atf_comp::comptest_amc_FstepTimehookIdle() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N yyy.FDb.zd_bg.FirstChanged < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookRecur  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_ready  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:yyy  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:yyy  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb._db  arg:yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FJob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FJob.id  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.job  arg:yyy.FJob  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_bg  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_bg  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_bg  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dev.target  target:algo_lib");
    atf_comp::ProcWrite(proc, "dev.target  target:yyy");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:yyy.algo_lib  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_bg  steptype:TimeHookRecur  stepband:idle  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an executable a TimeHookRecur step in the output band, which has no algo_lib
// heap, and check that amc refuses it with amc.fstep_timehook_band.
void atf_comp::comptest_amc_BadFstepTimehookBand() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookRecur  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_ready  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:yyy  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:yyy  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb._db  arg:yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FJob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FJob.id  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.job  arg:yyy.FJob  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_bg  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_bg  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_bg  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dev.target  target:algo_lib");
    atf_comp::ProcWrite(proc, "dev.target  target:yyy");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:yyy.algo_lib  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_bg  steptype:TimeHookRecur  stepband:output  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an executable an idle step with fsteprate 2/3, print the step's Call, and
// check that each pass earns two thirds of a call, that the credit caps at 4, and
// that the step also runs in a pass that has time to give up.
// The cap at 4 loses none of the credit.
void atf_comp::comptest_amc_FstepRate() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N yyy.FDb.zd_bg.Call < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_ready  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:yyy  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:yyy  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb._db  arg:yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FJob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FJob.id  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.job  arg:yyy.FJob  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_in  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_in  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_in  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_out  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_out  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_out  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_bg  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_bg  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_bg  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dev.target  target:algo_lib");
    atf_comp::ProcWrite(proc, "dev.target  target:yyy");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:yyy.algo_lib  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_out  steptype:Inline  stepband:output  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_in  steptype:Inline  stepband:input  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.zd_ready  steptype:Inline  stepband:work  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_bg  steptype:Inline  stepband:idle  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fsteprate  fstep:yyy.FDb.zd_bg  ncall:2  npass:3  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an executable an idle step with no fsteprate row, print the step's Call, and
// check that the step runs at the idle band's default rate, 1/8.
void atf_comp::comptest_amc_FstepIdleRate() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N yyy.FDb.zd_bg.Call < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_ready  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:yyy  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:yyy  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb._db  arg:yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FJob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FJob.id  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.job  arg:yyy.FJob  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_in  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_in  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_in  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_out  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_out  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_out  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_bg  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_bg  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_bg  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dev.target  target:algo_lib");
    atf_comp::ProcWrite(proc, "dev.target  target:yyy");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:yyy.algo_lib  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_out  steptype:Inline  stepband:output  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_in  steptype:Inline  stepband:input  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.zd_ready  steptype:Inline  stepband:work  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_bg  steptype:Inline  stepband:idle  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an executable an InlineRecur step with an fsteprate row, and check that amc
// reports amc.fstep_rate_steptype.
// An InlineRecur step keeps its own clock.
void atf_comp::comptest_amc_BadFstepRateSteptype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_ready  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:yyy  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:yyy  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb._db  arg:yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FJob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FJob.id  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.job  arg:yyy.FJob  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_in  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_in  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_in  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_out  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_out  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_out  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_bg  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_bg  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_bg  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dev.target  target:algo_lib");
    atf_comp::ProcWrite(proc, "dev.target  target:yyy");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:yyy.algo_lib  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:InlineRecur  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_bg  steptype:InlineRecur  stepband:work  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fsteprate  fstep:yyy.FDb.zd_bg  ncall:3  npass:2  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an executable a step with an fsteprate of zero calls, and check that amc
// reports amc.fstep_rate_zero.
// A rate of zero calls would never run the step.
void atf_comp::comptest_amc_BadFstepRateZero() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_ready  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_ready  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:yyy  nstype:exe  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:yyy  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:yyy  genthrow:Y  pool:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:yyy  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb._db  arg:yyy.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:yyy.FJob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FJob.id  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.job  arg:yyy.FJob  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_in  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_in  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_in  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_out  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_out  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_out  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:yyy.FDb.zd_bg  arg:yyy.FJob  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:yyy.FDb.zd_bg  havetail:Y  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:yyy.FDb.zd_bg  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dev.target  target:algo_lib");
    atf_comp::ProcWrite(proc, "dev.target  target:yyy");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:yyy.algo_lib  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:yyy.FDb.zd_bg  steptype:Inline  stepband:work  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fsteprate  fstep:yyy.FDb.zd_bg  ncall:0  npass:2  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a dispatch filter over a message field that has its own Print function
// (an fconst field), print the filter's MatchEv, and check that the regx match
// prints the field through that function with the message as the parent argument.
// The golden pins the whole emitted Match body, including the well-formed Print
// call.
void atf_comp::comptest_amc_DispfilterFieldPrint() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N algo_lib.DispFilter..MatchEv < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Regx  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Regx.expr  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Ev  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ev.state  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Ev.state/idle  value:0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Ev.state/busy  value:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DispFilter  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DispFilter.state_regx  arg:algo_lib.Regx  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.dispatch  dispatch:algo_lib.Disp  unk:N  read:N  print:N  haslen:N  call:N  strict:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.dispatch_msg  dispatch_msg:algo_lib.Disp/algo_lib.Ev  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.dispfilter  dispatch:algo_lib.Disp  match_all:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a dispatch filter declared match_all:Y over two messages that between
// them cover every shape the rule has to place.  Print the filter's Match
// functions, and check how each one reads or refuses its message.
// Ev carries both filter fields, so its Match body reads the message.  The
// equality and the range tests take the big-endian field through its Get function.
// The regx match prints the message's own field through its fconst Print function
// before matching.  Ev2 carries neither field.  So each of Ev's filter fields turns
// into a present test that refuses the message, which is what match_all means.
// Ev2's own field is the control on the other side.  Ev's present tests for it are
// the same refusal in reverse.
void atf_comp::comptest_amc_DispfilterMatchAll() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.DispFilter..Match%' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Regx  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Regx.expr  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Ev  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ev.state  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Ev.state/idle  value:0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Ev.state/busy  value:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ev.seqno  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.Ev.seqno  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Ev2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ev2.other  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DispFilter  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DispFilter.state_regx  arg:algo_lib.Regx  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.dispatch  dispatch:algo_lib.Disp  unk:N  read:N  print:N  haslen:N  call:N  strict:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.dispatch_msg  dispatch_msg:algo_lib.Disp/algo_lib.Ev  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.dispatch_msg  dispatch_msg:algo_lib.Disp/algo_lib.Ev2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.dispfilter  dispatch:algo_lib.Disp  match_all:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Run amc over test/amc/bad_gconst_badline, whose gconst value table has a fourth
// and a fifth line that are no tuples, and print algo_lib.ColorId.  Check that amc
// reports amc.bad_gconst for both lines with the file, the line number and the
// line's text.
// Each bad line carries an unterminated quoted value.  The reader keeps what it
// read before the quote ran out and jumps to end of line.  The truncated value
// would then read back as a constant name the table never wrote, and every
// attribute past the bad quote would drop, the comment among them.  The tuple
// reader itself supplies none of the file, the line number or the line's text.
// The printed enum pins the controls too: a blank line and a comment line draw no
// diagnostic; the two well-formed rows still emit; and the auto-numbering counts
// only the lines that produced a constant.
void atf_comp::comptest_amc_BadGconstBadline() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/bad_gconst_badline -out_dir: -report:N algo_lib.ColorId");
}

// Run amc over test/amc/bad_gconst_dup, whose gconst value table holds one name
// twice, and check that amc reports amc.gconst_dup.
// The second row would silently vanish from the enum and still take a numbering
// index.  The numbering would then have a gap on a clean exit.
void atf_comp::comptest_amc_BadGconstDup() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/bad_gconst_dup -out_dir: -report:N zzz");
}

// Run amc over test/amc/bad_gconst_headonly, whose gconst value table has a line
// with a head and no attribute, and check that amc reports amc.gconst_headonly.
// The line creates no constant, yet auto-numbering would take an index for it.
// That would silently renumber every constant that follows.
void atf_comp::comptest_amc_BadGconstHeadonly() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/bad_gconst_headonly -out_dir: -report:N zzz");
}

// Run amc over test/amc/bad_gconst_ctype, whose gconst idfld names an existing
// field of a ctype other than the namefld's, and check that amc reports
// amc.gconst_idfld.
// The value table's rows lack that field, so every fconst value would come out
// empty.
void atf_comp::comptest_amc_BadGconstCtype() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/bad_gconst_ctype -out_dir: -report:N zzz");
}

// Run amc over test/amc/bad_gconst_idfld, whose gconst idfld names a field that
// does not exist, and check that amc reports amc.gconst_idfld.
// The report keeps the id column from silently falling back to the name column.
void atf_comp::comptest_amc_BadGconstIdfld() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/bad_gconst_idfld -out_dir: -report:N zzz");
}

// Run amc over test/amc/bad_gconst_ctype_char, with two gconsts whose idfld is bad,
// the first on a char-typed field.  Check that amc reports amc.gconst_idfld for
// both and exits.
// A run that went on to process the known-bad row would abort the scan on a
// char-parse failure that names no context.
void atf_comp::comptest_amc_BadGconstCtypeChar() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/bad_gconst_ctype_char -out_dir: -report:N zzz");
}

// Run amc over test/amc/bad_gconst_char, whose char-typed gconst has a value table
// with multi-character values, and print algo_lib.ColorId.  Check that amc reports
// amc.gconst_char for each offending row with its gconst, name and value, and
// scans on to the end of the table.
// The printed enum pins the table's one valid row too.  The row survives its
// skipped neighbors and emits as a single-quoted character constant.
void atf_comp::comptest_amc_BadGconstChar() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/bad_gconst_char -out_dir: -report:N algo_lib.ColorId");
}

// Run amc over test/amc/gconst_indir, whose gconst value table exists only inside
// that directory.  Check that amc side-loads the values from the -in_dir root and
// prints no amc.sideload notice for a directory universe.
// data/ under the CWD lacks the table, so only the -in_dir root can supply it.
void atf_comp::comptest_amc_GconstIndir() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/gconst_indir -out_dir: -report:N zzz");
}

// Run amc over test/amc/gconst_missing, whose -in_dir root lacks the gconst's value
// table, and check that amc reports amc.load with the file and exits nonzero.
// The table's rows are the enum's constants.  Loading the table as zero rows would
// compile an empty enum and exit 0.
void atf_comp::comptest_amc_GconstLoadFail() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/gconst_missing -out_dir: -report:N zzz");
}

// Give amc a gconst and a gstatic over each of two ctypes that reach no ssimfile,
// and check that amc reports amc.gconst_nossimfile and amc.gstatic_nossimfile for
// all four rows in one run.
// The ctype that the rows would come from reaches no ssimfile, so amc has no file
// to look for.  This is the other way a table can be absent.  amc reports it apart
// from a named file that fails to read, because a reader that was never given a
// file has no file to report.  amc.GconstLoadFail and amc.GstaticLoadFail cover
// the named file.  Their ctypes reach a ssimfile, one of its own and one through a
// base, which makes them the controls for this test.
// A ctype reaches no ssimfile in two ways, and the fixture holds both for each
// consumer.  Free declares no base, and Derived has a base that owns no ssimfile
// either.  Each report names the offending gconst or gstatic field and lets the
// run continue, so one run reports all four rows.
void atf_comp::comptest_amc_SideloadNossimfile() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i8  likeu64:Y  bigendok:N  issigned:Y  comment:\"8-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u8  likeu64:Y  bigendok:N  issigned:N  comment:\"8-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i16  likeu64:Y  bigendok:Y  issigned:Y  comment:\"16-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:Y  bigendok:Y  issigned:N  comment:\"16-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:\"32-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"32-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i64  likeu64:Y  bigendok:Y  issigned:Y  comment:\"64-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:\"64-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:Y  issigned:N  comment:\"128-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:bool  likeu64:Y  bigendok:N  issigned:N  comment:\"dflt is a c++ expr: real dflt is 'N'\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pad_byte  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:pad_byte  size:1  alignment:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:pad_byte.String  printfmt:Extern  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:pad_byte  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:char  likeu64:Y  bigendok:N  issigned:N  comment:char");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Free  comment:\"reaches no ssimfile and declares no base\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Free.name  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Derived  comment:\"reaches no ssimfile through a base that has none either\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Derived.base  arg:algo_lib.Free  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Derived.dname  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FreeId  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FreeId.value  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.gconst  field:algo_lib.FreeId.value  namefld:algo_lib.Free.name  idfld:''  wantenum:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DerivedId  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DerivedId.value  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.gconst  field:algo_lib.DerivedId.value  namefld:algo_lib.Derived.dname  idfld:''  wantenum:Y");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.free  arg:algo_lib.Free  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.free  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.gstatic  field:algo_lib.FDb.free  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.derived  arg:algo_lib.Derived  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.derived  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.gstatic  field:algo_lib.FDb.derived  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Run amc over test/amc/gstatic_missing, whose -in_dir root lacks the table file of
// a gstatic field, and check that amc reports amc.load with the file and exits
// nonzero.
// The rows would become the compiled-in registry.  Loading the table as zero rows
// would silently drop every entry.
void atf_comp::comptest_amc_GstaticLoadFail() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/gstatic_missing -out_dir: -report:N zzz");
}

// Run amc over test/amc/gstatic_badline, whose gstatic table has a second line with
// an unterminated quoted value, and print algo_lib.FDb.color.LoadStatic.  Check
// that amc reports amc.bad_gstatic with the field that was reading, the file, the
// line number and the line's text, and exits nonzero.
// The bad line would silently drop out of the compiled-in registry.  The tuple
// reader itself supplies none of the file, the line number or the line's text.
// The printed loader pins the two well-formed lines around the bad one as well.
// They come back as the loader's own rows, and a run that abandoned the table at
// the first bad line would show there.  The universe also carries an fconst whose
// value is a character literal.  So an earlier pass leaves a bad-number tag that
// names that valid value in algo_lib's shared error buffer.  A message taken from
// that buffer would name the fconst, and miss the file that is malformed.
void atf_comp::comptest_amc_GstaticBadLine() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/gstatic_badline -out_dir: -report:N algo_lib.FDb.color.LoadStatic");
}

// Run amc over test/amc/gsymbol_badline, whose gsymbol table has a fourth and a
// fifth line that are no tuples, each with an unterminated quoted value.  Check
// that amc reports amc.bad_gsymbol for both lines with the file, the line number
// and the line's text, and exits nonzero.
// The truncated text would become a symbol name and value the table never wrote.
// Every reference to the intended symbol would then fail to link.
void atf_comp::comptest_amc_GsymbolBadline() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/gsymbol_badline -out_dir: -report:N zzz");
}

// Run amc over test/amc/gsymbol_missing, whose -in_dir root lacks a gsymbol's
// table, and check that amc reports amc.load with the file and exits nonzero.
// The table's rows are the symbols that the namespace exports.  An empty symbol
// block and exit 0 would leave every reference to them unresolved at link time.
void atf_comp::comptest_amc_GsymbolLoadFail() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/gsymbol_missing -out_dir: -report:N zzz");
}

// Give amc a gsymbol in a single-file universe, and check that the amc.sideload
// notice names the fallback root and the file that amc reads from it.
// With no directory root, the symbol table side-loads from the default data/ root
// of the current directory.  That root may belong to a different universe than
// the -in_dir schema.
void atf_comp::comptest_amc_GsymbolSideload() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:ssimdb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:dmmeta  nstype:ssimdb  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:dmmeta.Nstype  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.Nstype.nstype  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ssimfile  ssimfile:dmmeta.nstype  ctype:dmmeta.Nstype");
    atf_comp::ProcWrite(proc, "dmmeta.gsymbol  gsymbol:algo_lib/dmmeta.nstype  inc:%  symboltype:''  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc an Llist field without its dmmeta.llist record, and check that amc
// reports amc.missing_record for the field and then stops at the reftype gate.
// Later gen phases dereference the per-reftype records that this phase found
// missing.
void atf_comp::comptest_amc_MissingLlist() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_row  arg:algo_lib.FRow  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a ccmp on a global ctype, and check that amc reports amc.ccmp_global.
// A global ctype has exactly one instance.  A global field's Get accessor takes no
// parent argument, so the generated Cmp/Lt bodies would silently compare the
// singleton with itself.
void atf_comp::comptest_amc_BadCcmpGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u32  extrn:N  genop:N  order:N  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u64  extrn:N  genop:N  order:N  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.seqno  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.FDb.seqno  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo_lib.FDb  extrn:N  genop:N  order:Y  minmax:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a chash on a global ctype, and check that amc reports amc.chash_global.
// The rule is the one for ccmp.  The generated Hash body would silently hash the
// singleton's own fields, whatever argument the caller passes.
void atf_comp::comptest_amc_BadChashGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.seqno  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.FDb.seqno  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.hashtype  hashtype:CRC32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.chash  ctype:algo_lib.FDb  hashtype:CRC32  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc an fcmp on a global ctype's field, and check that amc reports
// amc.fcmp_global.
// The rule is the one for ccmp, one level down.  Both operands of the generated
// Cmp/Lt body would collapse to the singleton's value, and the function would
// compare the field with itself.
void atf_comp::comptest_amc_BadFcmpGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u32  extrn:N  genop:N  order:N  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u64  extrn:N  genop:N  order:N  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.seqno  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.FDb.seqno  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fcmp  field:algo_lib.FDb.seqno  versionsort:N  casesens:Y  extrn:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a ctype with both a Tpool and a Global instance, hashed by an FDb Thash,
// and check that amc reports amc.global_inst.
// The pooled rows would reach field accessors that take no parent and read the
// global.  The hash would then compare the singleton with itself.  So the Global
// instance must be the ctype's only instance.
void atf_comp::comptest_amc_BadGlobalInst() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u32  extrn:N  genop:N  order:N  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.seqno  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.FRow.seqno  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.grow  arg:algo_lib.FRow  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.ind_row  arg:algo_lib.FRow  reftype:Thash  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.thash  field:algo_lib.FDb.ind_row  hashfld:algo_lib.FRow.seqno  unique:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.ind_row  inscond:true  via:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Thash.curs  dflt:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a minmax:Y ccmp on a non-builtin ctype without genop:Y order:Y, and
// check that amc reports amc.ccmp_minmax.
// The Min/Max functions compare with a raw <, and neither a generated nor a native
// operator provides one.
void atf_comp::comptest_amc_BadMinmax() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u32  extrn:N  genop:N  order:N  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestMinmax  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestMinmax.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo_lib.TestMinmax  extrn:N  genop:N  order:N  minmax:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a minmax:Y ccmp on a builtin and on an extrn:Y ctype, neither with
// order:Y genop:Y, and check that amc generates the schema without a diagnostic.
// The builtin has a native operator <, and the extrn:Y ctype has a user-defined
// comparison.  Both provide the operator < that the Min/Max bodies compare with.
void atf_comp::comptest_amc_MinmaxNative() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u32  extrn:N  genop:N  order:N  minmax:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestExtrn  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestExtrn.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo_lib.TestExtrn  extrn:Y  genop:N  order:N  minmax:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a ctype with one fdec field and a ctype with two, print each field's
// GetScale, and check that the one-fdec ctype keeps the ctype-named GetScale and
// the two-fdec ctype has none.
// amc emits the ctype-named GetScale convenience for a ctype with a single fdec
// field, because two fdec fields would give colliding definitions.
void atf_comp::comptest_amc_FdecGetScale() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.Price.value.GetScale|algo_lib.Quote.bid.GetScale|algo_lib.Quote.ask.GetScale' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Price  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Price.value  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.Price.value  nplace:2  fixedfmt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Quote  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Quote.bid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.Quote.bid  nplace:2  fixedfmt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Quote.ask  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.Quote.ask  nplace:4  fixedfmt:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Query amc for records in namespaces that have no dmmeta.nscpp row, and check that
// each such match reports amc.query_nocpp and counts toward the run's exit code:
// 1. Query two ctypes at once, algo_lib.CppWidget in a namespace with the row and
//    ncns.Widget in a namespace without it.  Check that amc reports the ncns match,
//    still prints the algo_lib struct, and exits 1 for the one report.
// 2. Query testdb.%, an ssimdb namespace projected to TypeScript alone, under the
//    default key, so both query walkers run.  Check that the two ctypes and the
//    seven generated funcs each draw a report and that the run exits 9.
// 3. Repeat the testdb.% query with stderr discarded, and check that stdout is
//    empty.
// 4. Query testdb.% and algo_lib.trace..Ctor together with stderr discarded, and
//    check that stdout holds the generated function and none of the nine reports
//    that the same run writes to stderr.
// A namespace without the row emits no C++, so the query has no code to print for
// its record.  Query mode's stdout carries the generated C++ alone, so a consumer
// that wants code alone drops stderr.  The fourth query spans a nocpp namespace and
// a code-generating one, the case that tells the two streams apart.
void atf_comp::comptest_amc_QueryNocpp() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'ctype:algo_lib.CppWidget|ncns.Widget' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:none  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:ssimdb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ncns  nstype:none  license:GPL  comment:\"Namespace with no dmmeta.nscpp row: emits no C++\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:testdb  nstype:ssimdb  license:GPL  comment:\"ssimdb namespace projected to TypeScript alone: emits no C++\"");
    atf_comp::ProcWrite(proc, "dmmeta.nslang  nslang:testdb/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CppWidget  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CppWidget.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:ncns.Widget  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ncns.Widget.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:testdb.App  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nossimfile  ctype:testdb.App  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:testdb.App.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc);
    atf_comp::FProc &proc2 = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N testdb.% < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc2, "dmmeta.nstype  nstype:none  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.nstype  nstype:ssimdb  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.ns  ns:ncns  nstype:none  license:GPL  comment:\"Namespace with no dmmeta.nscpp row: emits no C++\"");
    atf_comp::ProcWrite(proc2, "dmmeta.ns  ns:testdb  nstype:ssimdb  license:GPL  comment:\"ssimdb namespace projected to TypeScript alone: emits no C++\"");
    atf_comp::ProcWrite(proc2, "dmmeta.nslang  nslang:testdb/ts  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.ctype  ctype:algo_lib.CppWidget  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.CppWidget.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.ctype  ctype:ncns.Widget  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:ncns.Widget.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.ctype  ctype:testdb.App  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.nossimfile  ctype:testdb.App  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:testdb.App.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc2);
    atf_comp::FProc &proc3 = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N testdb.% 2>/dev/null < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc3, "dmmeta.nstype  nstype:none  comment:''");
    atf_comp::ProcWrite(proc3, "dmmeta.nstype  nstype:ssimdb  comment:''");
    atf_comp::ProcWrite(proc3, "dmmeta.ns  ns:ncns  nstype:none  license:GPL  comment:\"Namespace with no dmmeta.nscpp row: emits no C++\"");
    atf_comp::ProcWrite(proc3, "dmmeta.ns  ns:testdb  nstype:ssimdb  license:GPL  comment:\"ssimdb namespace projected to TypeScript alone: emits no C++\"");
    atf_comp::ProcWrite(proc3, "dmmeta.nslang  nslang:testdb/ts  comment:''");
    atf_comp::ProcWrite(proc3, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc3, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc3, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc3, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc3, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc3, "dmmeta.ctype  ctype:algo_lib.CppWidget  comment:''");
    atf_comp::ProcWrite(proc3, "dmmeta.field  field:algo_lib.CppWidget.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc3, "dmmeta.ctype  ctype:ncns.Widget  comment:''");
    atf_comp::ProcWrite(proc3, "dmmeta.field  field:ncns.Widget.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc3, "dmmeta.ctype  ctype:testdb.App  comment:''");
    atf_comp::ProcWrite(proc3, "dmmeta.nossimfile  ctype:testdb.App  comment:''");
    atf_comp::ProcWrite(proc3, "dmmeta.field  field:testdb.App.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc3);
    atf_comp::FProc &proc4 = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N '(testdb.%|algo_lib.trace..Ctor)' 2>/dev/null < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc4, "dmmeta.nstype  nstype:none  comment:''");
    atf_comp::ProcWrite(proc4, "dmmeta.nstype  nstype:ssimdb  comment:''");
    atf_comp::ProcWrite(proc4, "dmmeta.ns  ns:ncns  nstype:none  license:GPL  comment:\"Namespace with no dmmeta.nscpp row: emits no C++\"");
    atf_comp::ProcWrite(proc4, "dmmeta.ns  ns:testdb  nstype:ssimdb  license:GPL  comment:\"ssimdb namespace projected to TypeScript alone: emits no C++\"");
    atf_comp::ProcWrite(proc4, "dmmeta.nslang  nslang:testdb/ts  comment:''");
    atf_comp::ProcWrite(proc4, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc4, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc4, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc4, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc4, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc4, "dmmeta.ctype  ctype:algo_lib.CppWidget  comment:''");
    atf_comp::ProcWrite(proc4, "dmmeta.field  field:algo_lib.CppWidget.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc4, "dmmeta.ctype  ctype:ncns.Widget  comment:''");
    atf_comp::ProcWrite(proc4, "dmmeta.field  field:ncns.Widget.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc4, "dmmeta.ctype  ctype:testdb.App  comment:''");
    atf_comp::ProcWrite(proc4, "dmmeta.nossimfile  ctype:testdb.App  comment:''");
    atf_comp::ProcWrite(proc4, "dmmeta.field  field:testdb.App.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc4);
}

// Give amc an fdec on two bitfld fields of a global ctype.  Print decfrac's
// GetDouble and qSetDouble and decnarrow's SetDoubleMaybe and ReadStrptrMaybe, and
// check that each body reaches the value through the field's accessors and bounds
// it by the field's width.
// The Get/Set accessors of a field with a global parent take no parent argument.
// decnarrow is ten bits wide inside a u32 word, and pins the range that the
// emitters bound it by.  A bitfld setter masks the store to the declared width.  So
// the field holds at most 1023, below the 4294967295 that its arg would suggest.
// SetDoubleMaybe compares against 1023, and the decimal reader carries that
// magnitude split by ten, 102 and 3.
// Each of the three stores casts the value to the field's own type on the way in,
// so the setter receives exactly the type it declares.  The double that the double
// setters compute is a different type.  So is the u64 magnitude that the reader
// accumulates, and so is an arg that names a ctype standing for an integer builtin.
void atf_comp::comptest_amc_FdecGlobalBitfld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb.decfrac.GetDouble|algo_lib.FDb.decfrac.qSetDouble|algo_lib.FDb.decnarrow.SetDoubleMaybe|algo_lib.FDb.decnarrow.ReadStrptrMaybe' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:Y  print:N  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.decbits  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.decfrac  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.FDb.decfrac  offset:0  width:32  srcfield:algo_lib.FDb.decbits  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.FDb.decfrac  nplace:2  fixedfmt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.decnarrowbits  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.decnarrow  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.FDb.decnarrow  offset:0  width:10  srcfield:algo_lib.FDb.decnarrowbits  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.FDb.decnarrow  nplace:2  fixedfmt:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc one fdec field per decimal-place count, arg and bitfld width that the
// rule decides, and print algo_lib.DecPlaceCast.value.%.  Check that amc reports
// each count, arg and conversion that it has no consistent arithmetic for, and
// that the controls draw no diagnostic.
// Zero places is the nplace default, which a forgotten attr leaves behind.  It
// emits a reader that accumulates no digit.  A count of -1 draws the same report.
// Eighteen places outrun the powers of ten that the double accessors substitute,
// so they and the string accessors would disagree about the scale.  Thirty-one
// places outrun the printer's scratch buffer.  Twelve places on an i32 field
// outrun the field's own arg, which the emitted GetScale returns the scale as.
// That diagnostic states the narrower range that the arg allows.  A u8 arg is the
// narrowest range that the rule admits from the arg side.  Two places fit its 255,
// three exceed it, and the diagnostic states that bound.
// The fixture also holds one field per arg with no numeric range: bool, char,
// u128, float, double, pad_byte and a plain struct ctype.  amc reports each as an
// unusable arg.  The char field's count is zero as well, and amc still checks it
// against the powers-of-ten limit, so that field draws both diagnostics in one run.
// The remaining fields cover the other way to fix a field's range.  A Bitfld field
// stores into a slice of another field's word, and its setter masks the store.  So
// the width bounds the count, and the arg plays no part.  A ten-bit slice of a u32
// takes three places and rejects four.  A forty-bit slice of a u64 takes twelve and
// rejects thirteen.  A three-bit slice holds no decimal place, and the diagnostic
// states exactly that.
// amc refuses a slice of a signed arg narrower than its own type, because such a
// field does not sign-extend on read.  The ten-bit i32 field draws the bitfield
// diagnostic that refuses it.  The refusal follows the type that the arg stands
// for, so a ten-bit slice of a ctype whose single Val field is an i32 draws it too.
// A seven-bit slice of an i8 draws it at the narrowest signed arg there is.  A
// slice whose arg has no numeric range takes no bound from its width.  amc reports
// the four-bit char field as an unusable arg, and then holds its count against the
// powers-of-ten limit alone.
// The controls draw no diagnostic: the one-place and seventeen-place i64 fields
// sit at the bottom and the top of the supported range; each of the eight integer
// builtins carries a count its own range allows, which covers every width and both
// signs; and a slice as wide as its arg takes what the arg takes at both signs.
// u32 and i32 at thirty-two bits each take nine places; a u64 at sixty-four bits
// takes seventeen; and an i8 at eight bits takes the two places its 127 allows.
// An arg that stands for the builtin through another ctype stores that ctype.
// Every quantity the emitted functions compute is a number of the builtin, so the
// value crosses between the two at each end.  It leaves the wrapper through the
// operator that dmmeta.fcast generates.  It enters the wrapper through the
// fieldwise constructor that a dmmeta.cpptype row asking for a constructor
// provides.  The fixture carries one arg per combination of the two.
// DecPlaceCast carries both, and amc accepts it.  The query pins its emitted
// functions, so the golden compares what an accepted wrapper compiles to.
// DecPlaceFcast carries the operator alone, DecPlaceCtor the constructor alone,
// and DecPlaceWrap neither.  amc reports each of the three, naming what is
// missing.  DecPlaceChain stands for the builtin two ctypes deep, where the
// constructor takes the inner ctype in place of the builtin.  amc reports it as
// standing for the builtin only through another ctype.  The same rule reaches a
// Bitfld arg.  The ten-bit slice of a wrapped u32 carries both conversions and
// draws its count diagnostic alone.  The wrapped i32 slice carries neither, and
// draws the conversion diagnostic beside its own.
void atf_comp::comptest_amc_BadFdecNplace() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.DecPlaceCast.value.%' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i8  likeu64:Y  bigendok:N  issigned:Y  comment:\"8-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u8  likeu64:Y  bigendok:N  issigned:N  comment:\"8-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i16  likeu64:Y  bigendok:Y  issigned:Y  comment:\"16-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:Y  bigendok:Y  issigned:N  comment:\"16-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:\"32-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"32-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i64  likeu64:Y  bigendok:Y  issigned:Y  comment:\"64-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:\"64-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:Y  issigned:N  comment:\"128-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:bool  likeu64:Y  bigendok:N  issigned:N  comment:\"dflt is a c++ expr: real dflt is 'N'\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pad_byte  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:pad_byte  size:1  alignment:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:pad_byte.String  printfmt:Extern  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:pad_byte  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:char  likeu64:Y  bigendok:N  issigned:N  comment:char");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:float  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:float  likeu64:N  bigendok:N  issigned:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:float  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:float.String  printfmt:Extern  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:float  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:double  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:double  likeu64:N  bigendok:N  issigned:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:double  size:8  alignment:8  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:double.String  printfmt:Extern  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:double  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlace0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlace0.value  arg:i64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlace0.value  nplace:0  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceNeg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceNeg.value  arg:i64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceNeg.value  nplace:-1  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlace18  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlace18.value  arg:i64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlace18.value  nplace:18  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlace31  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlace31.value  arg:i64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlace31.value  nplace:31  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlace17  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlace17.value  arg:i64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlace17.value  nplace:17  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlace12I32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlace12I32.value  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlace12I32.value  nplace:12  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlace1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlace1.value  arg:i64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlace1.value  nplace:1  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceBool  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceBool.value  arg:bool  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceBool.value  nplace:1  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceChar  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceChar.value  arg:char  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceChar.value  nplace:0  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceU128  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceU128.value  arg:u128  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceU128.value  nplace:1  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceFloat  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceFloat.value  arg:float  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceFloat.value  nplace:1  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceDouble  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceDouble.value  arg:double  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceDouble.value  nplace:1  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlacePadbyte  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlacePadbyte.value  arg:pad_byte  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlacePadbyte.value  nplace:1  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceStruct  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceStruct.value  arg:algo_lib.FDb  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceStruct.value  nplace:1  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.I64Val  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.I64Val.value  arg:i64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceWrap  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceWrap.value  arg:algo_lib.I64Val  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceWrap.value  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.I64Cast  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.I64Cast.value  arg:i64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo_lib.I64Cast.value  expr:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.I64Cast  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceCast  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceCast.value  arg:algo_lib.I64Cast  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceCast.value  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.DecPlaceCast.String  printfmt:Raw  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.I64Fcast  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.I64Fcast.value  arg:i64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo_lib.I64Fcast.value  expr:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceFcast  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceFcast.value  arg:algo_lib.I64Fcast  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceFcast.value  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.I64Ctor  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.I64Ctor.value  arg:i64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.I64Ctor  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceCtor  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceCtor.value  arg:algo_lib.I64Ctor  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceCtor.value  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.I64Cast2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.I64Cast2.value  arg:algo_lib.I64Cast  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo_lib.I64Cast2.value  expr:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.I64Cast2  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceChain  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceChain.value  arg:algo_lib.I64Cast2  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceChain.value  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceI8  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceI8.value  arg:i8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceI8.value  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceI16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceI16.value  arg:i16  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceI16.value  nplace:4  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceU16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceU16.value  arg:u16  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceU16.value  nplace:4  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceI32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceI32.value  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceI32.value  nplace:9  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceU32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceU32.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceU32.value  nplace:9  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceU64  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceU64.value  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceU64.value  nplace:17  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceU8  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceU8.value  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceU8.value  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceU8Over  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceU8Over.value  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceU8Over.value  nplace:3  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitU32W32P9  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W32P9.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W32P9.value  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitU32W32P9.value  offset:0  width:32  srcfield:algo_lib.DecBitU32W32P9.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitU32W32P9.value  nplace:9  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitU32W10P3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W10P3.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W10P3.value  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitU32W10P3.value  offset:0  width:10  srcfield:algo_lib.DecBitU32W10P3.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitU32W10P3.value  nplace:3  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitU32W10P4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W10P4.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W10P4.value  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitU32W10P4.value  offset:0  width:10  srcfield:algo_lib.DecBitU32W10P4.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitU32W10P4.value  nplace:4  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitU32W3P1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W3P1.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W3P1.value  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitU32W3P1.value  offset:0  width:3  srcfield:algo_lib.DecBitU32W3P1.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitU32W3P1.value  nplace:1  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitU64W64P17  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU64W64P17.word  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU64W64P17.value  arg:u64  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitU64W64P17.value  offset:0  width:64  srcfield:algo_lib.DecBitU64W64P17.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitU64W64P17.value  nplace:17  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitU64W40P13  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU64W40P13.word  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU64W40P13.value  arg:u64  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitU64W40P13.value  offset:0  width:40  srcfield:algo_lib.DecBitU64W40P13.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitU64W40P13.value  nplace:13  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitI32W32P9  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI32W32P9.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI32W32P9.value  arg:i32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitI32W32P9.value  offset:0  width:32  srcfield:algo_lib.DecBitI32W32P9.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitI32W32P9.value  nplace:9  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitI32W10P2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI32W10P2.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI32W10P2.value  arg:i32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitI32W10P2.value  offset:0  width:10  srcfield:algo_lib.DecBitI32W10P2.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitI32W10P2.value  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.I32Val  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.I32Val.value  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.U32Val  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.U32Val.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo_lib.U32Val.value  expr:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.U32Val  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitWrapI32W10P2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitWrapI32W10P2.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitWrapI32W10P2.value  arg:algo_lib.I32Val  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitWrapI32W10P2.value  offset:0  width:10  srcfield:algo_lib.DecBitWrapI32W10P2.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitWrapI32W10P2.value  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitWrapU32W10P4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitWrapU32W10P4.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitWrapU32W10P4.value  arg:algo_lib.U32Val  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitWrapU32W10P4.value  offset:0  width:10  srcfield:algo_lib.DecBitWrapU32W10P4.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitWrapU32W10P4.value  nplace:4  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitI8W8P2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI8W8P2.word  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI8W8P2.value  arg:i8  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitI8W8P2.value  offset:0  width:8  srcfield:algo_lib.DecBitI8W8P2.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitI8W8P2.value  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitI8W7P2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI8W7P2.word  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI8W7P2.value  arg:i8  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitI8W7P2.value  offset:0  width:7  srcfield:algo_lib.DecBitI8W7P2.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitI8W7P2.value  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitCharW4P2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitCharW4P2.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitCharW4P2.value  arg:char  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitCharW4P2.value  offset:0  width:4  srcfield:algo_lib.DecBitCharW4P2.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitCharW4P2.value  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a Bitfld field with a declared width of zero and a decimal count, and
// check that amc reports the width as out of bounds and that the count draws no
// diagnostic of its own.
// The decimal range takes no bound from such a width.  A bound computed from it
// would state that the field holds no decimal place.  The bitfield accessors
// refuse the width once generation reaches the field, and the run ends there.
// That is why the width error follows the bounds report.  It is also why a fixture
// reaches the decimal rule with one bad width at most.  BadFdecBitwidthNeg covers
// the negative width.
void atf_comp::comptest_amc_BadFdecBitwidth() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i8  likeu64:Y  bigendok:N  issigned:Y  comment:\"8-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u8  likeu64:Y  bigendok:N  issigned:N  comment:\"8-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i16  likeu64:Y  bigendok:Y  issigned:Y  comment:\"16-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:Y  bigendok:Y  issigned:N  comment:\"16-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:\"32-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"32-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i64  likeu64:Y  bigendok:Y  issigned:Y  comment:\"64-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:\"64-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:Y  issigned:N  comment:\"128-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:bool  likeu64:Y  bigendok:N  issigned:N  comment:\"dflt is a c++ expr: real dflt is 'N'\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pad_byte  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:pad_byte  size:1  alignment:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:pad_byte.String  printfmt:Extern  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:pad_byte  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:char  likeu64:Y  bigendok:N  issigned:N  comment:char");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitW0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitW0.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitW0.value  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitW0.value  offset:0  width:0  srcfield:algo_lib.DecBitW0.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitW0.value  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a Bitfld field with a declared width of -64 and a decimal count, and
// check that amc reports the width as out of bounds and that the count draws no
// diagnostic of its own.
// This is the zero-width fixture's counterpart on the other side of the same rule.
// The decimal range takes no bound from a negative width.  The magnitude is
// sixty-four, because the test checks the rule through its output.  A bound built
// from a negative width shifts by a negative count.  A compiler that takes the
// count modulo sixty-four then computes a bound of zero from this width, and
// reports the field as holding no decimal place.  A run that keeps the width out
// of the shift prints two rows.  A run that lets it in prints three.
void atf_comp::comptest_amc_BadFdecBitwidthNeg() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i8  likeu64:Y  bigendok:N  issigned:Y  comment:\"8-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u8  likeu64:Y  bigendok:N  issigned:N  comment:\"8-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i16  likeu64:Y  bigendok:Y  issigned:Y  comment:\"16-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:Y  bigendok:Y  issigned:N  comment:\"16-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:\"32-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"32-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i64  likeu64:Y  bigendok:Y  issigned:Y  comment:\"64-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:\"64-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:Y  issigned:N  comment:\"128-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:bool  likeu64:Y  bigendok:N  issigned:N  comment:\"dflt is a c++ expr: real dflt is 'N'\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pad_byte  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:pad_byte  size:1  alignment:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:pad_byte.String  printfmt:Extern  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:pad_byte  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:char  likeu64:Y  bigendok:N  issigned:N  comment:char");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitWNeg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitWNeg.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitWNeg.value  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitWNeg.value  offset:0  width:-64  srcfield:algo_lib.DecBitWNeg.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitWNeg.value  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give a global ctype a String cfmt with printfmt Tuple, print the emitted Print
// body, and check that it reaches field values through the row argument, including
// the cursor walk of an array field.
void atf_comp::comptest_amc_PrintGlobalTuple() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N algo_lib.FDb.%Print% < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.val  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.buf  arg:u32  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.buf  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give a global ctype a read:Y Tuple cfmt, and print its ReadFieldMaybe, its
// ReadStrptrMaybe and the maxjobs field reader.  Check that the ReadFieldMaybe
// dispatcher keeps the ctype-name prefix its callers link against and passes no
// parent to the field readers, and that the Set-backed reader assigns through the
// collapsed Set accessor.
// Like every field-level function of a global, the field readers take no parent
// argument.
void atf_comp::comptest_amc_ReadGlobalTuple() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb.maxjobs.ReadStrptrMaybe|algo_lib.FDb..ReadFieldMaybe|algo_lib.FDb..ReadStrptrMaybe' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.pmask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.FDb.pmask  filter_print:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.maxjobs  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.timeout  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc an ArgvGnu cfmt, print its PrintArgv and ToArgv, and check that they
// emit the gnu form of the matrix that ArgvGlobal pins for Argv: a one-character
// field name takes a single dash and a longer name takes two; PrintArgv separates
// the name from its value with a space where Argv writes a colon; ToArgv appends
// the value as its own argv token; an array field repeats the name once per
// element; and args[0] is the bare command name, with no bin/ prefix.
// An ArgvGnu command is external and resolves on PATH.  atf_unit ExecToArgvSyntax
// asserts that against the real bash command.
void atf_comp::comptest_amc_ArgvGnu() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.Gnu..PrintArgv|algo_lib.Gnu..ToArgv' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Gnu  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Gnu.c  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Gnu.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Gnu.tag  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.Gnu.tag  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Gnu.ArgvGnu  printfmt:Auto  read:N  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give a global ctype and a regular one an Argv cfmt, print the PrintArgv and
// ToArgv bodies of both, and check that the value of a field with its own Print
// function (an fconst field) prints through that function.
// That function's parent argument collapses for the global.  For the regular ctype
// the argument is the row itself, with no const_cast.
void atf_comp::comptest_amc_ArgvGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb..PrintArgv|algo_lib.FDb..ToArgv|algo_lib.Cmd..PrintArgv|algo_lib.Cmd..ToArgv' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.state  arg:u8  reftype:Val  dflt:0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.FDb.state/idle  value:0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.FDb.state/busy  value:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.buf  arg:u32  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.buf  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.Argv  printfmt:Auto  read:N  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Cmd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.state  arg:u8  reftype:Val  dflt:0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Cmd.state/idle  value:0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Cmd.state/busy  value:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Cmd.Argv  printfmt:Auto  read:N  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc command ctypes with fields of many shapes, and print the PrintArgv
// bodies of FDb, Cmd and Flags and the ToArgv bodies of Cmd and Flags.  Check which
// fields become command-line tokens and how each token reaches its value.
// These fields become tokens: a plain Val; a Val cloned in from a Base; an fconst
// Val and a Smallstr, through their own Print functions; a Tary, through a cursor
// walk; and a big-endian Val and a bitfield, through their Get accessors.
// The rest become no token: the Global anchor and the Base anchor, which name no
// member of their own; the source word that a bitfield lives in, whose bits print
// individually outside Raw form; and a Ptr, whose address has no command-line
// spelling that ReadArgv could take back.
void atf_comp::comptest_amc_ArgvField() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb..PrintArgv|algo_lib.Cmd..PrintArgv|algo_lib.Cmd..ToArgv|algo_lib.Flags..PrintArgv|algo_lib.Flags..ToArgv' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u32  extrn:N  genop:N  order:N  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.Argv  printfmt:Auto  read:N  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.id  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Base  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Base.count  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Cmd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.base  arg:algo_lib.Base  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.plain  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.state  arg:u8  reftype:Val  dflt:0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Cmd.state/idle  value:0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Cmd.state/busy  value:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.big  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.Cmd.big  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.pmask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.Cmd.pmask  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.mode  arg:u8  reftype:Val  dflt:0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Cmd.mode/idle  value:0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Cmd.mode/busy  value:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.name  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Cmd.name  length:20  strtype:rpascal  pad:''  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.tag  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.Cmd.tag  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.p_row  arg:algo_lib.FRow  reftype:Ptr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Cmd.Argv  printfmt:Auto  read:N  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Ary  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ary.buf  arg:u32  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Ary.buf  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Ary.Argv  printfmt:Auto  read:N  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Flags  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Flags.seqno  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.Flags.seqno  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Flags.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Flags.flag  arg:bool  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Flags.flag  offset:0  width:1  srcfield:algo_lib.Flags.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Flags.Argv  printfmt:Auto  read:N  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give fconsts to two fields of a global ctype, print state's ToCstr, Print and
// SetStrptrMaybe and the bitfld mode's GetEnum, and check that the calls between
// the fconst accessors pass no parent.
// The fconst accessors of a global take no parent argument.  The golden pins
// ToCstr's GetEnum switch, Print's ToCstr call, SetStrptrMaybe's SetEnum calls for
// short names and for names beyond 8 bytes, and, through the bitfld member,
// GetEnum's Get call.
void atf_comp::comptest_amc_FconstGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb.state.ToCstr|algo_lib.FDb.state.Print|algo_lib.FDb.state.SetStrptrMaybe|algo_lib.FDb.mode.GetEnum' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.state  arg:u8  reftype:Val  dflt:0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.FDb.state/idle  value:0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.FDb.state/busy  value:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.FDb.state/quiescing  value:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.modebits  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.mode  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.FDb.mode  offset:0  width:32  srcfield:algo_lib.FDb.modebits  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.FDb.mode/off  value:0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.FDb.mode/on  value:1  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Pipe a universe to amc on stdin (-in_dir:-), query a function that only the piped
// rows define, and check that amc prints it.
// A caller composes a universe this way, as an acr_in dump plus extra rows, without
// a temp file.  "-" names no directory, so side table data would load from the
// default root.  This universe has no side table to load, so the run prints no
// amc.sideload notice.
void atf_comp::comptest_amc_StdinUniverse() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N algo_lib.Price.value.GetScale < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Price  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Price.value  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.Price.value  nplace:2  fixedfmt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Quote  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Quote.bid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.Quote.bid  nplace:2  fixedfmt:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Quote.ask  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.Quote.ask  nplace:4  fixedfmt:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give a global ctype an unsigned and a signed numstr field, print each field's
// Geti64 and GetnumDflt, and check that they call Getnum bare.
// The numstr accessors of a global take no parent argument.  The golden pins both
// Geti64 arms, the overflow-checked arm of the u64 numtype and the plain arm, and
// GetnumDflt's body.
void atf_comp::comptest_amc_NumstrGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb.gnumstr.Geti64|algo_lib.FDb.gnumstr.GetnumDflt|algo_lib.FDb.gnumsigned.Geti64|algo_lib.FDb.gnumsigned.GetnumDflt' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.gnumstr  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.FDb.gnumstr  length:20  strtype:leftpad  pad:\"'0'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.FDb.gnumstr  numtype:u64  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.gnumsigned  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.FDb.gnumsigned  length:20  strtype:leftpad  pad:\"' '\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.FDb.gnumsigned  numtype:i64  base:10  min_len:1");
    atf_comp::ProcWriteEof(proc);
}

// Give a global ctype a pmask, print one member's accessors and the filtered Print,
// and check that the member accessors and the print filter call the pmask's bitset
// accessors bare.
// The pmask's bitset accessors take no parent argument.  The golden pins the
// member's PresentQ, SetPresent and Set bodies and the filtered Print.
void atf_comp::comptest_amc_PmaskGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb.maxjobs.%|algo_lib.FDb..Print' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.pmask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.FDb.pmask  filter_print:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.maxjobs  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.timeout  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give a kafka codec a Bitfld field, print its KafkaEncode and KafkaDecode, and
// check that encode reads the field through the Get accessor and decode stores a
// temporary through the Set accessor.
// The field has no direct member, so raw parent.<name> access fails to compile.
// Encode takes the Get accessor from FieldvalExpr.  Decode picks the Set accessor
// by NoDirectMemberQ, which is deliberately narrower than AssignViaSetQ.  This
// codec's presence bits follow the wire's null flag, and a pmask Set would mark a
// null field present.
void atf_comp::comptest_amc_KafkaBitfld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'ktst.Rec..KafkaEncode|ktst.Rec..KafkaDecode' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ktst  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:ktst  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:ktst.Rec  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ktst.Rec.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ktst.Rec.val  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:ktst.Rec.val  offset:0  width:16  srcfield:ktst.Rec.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ckafka  ctype:ktst.Rec  kind:field  root:Rec  valid_versions:0  flexible_versions:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fkafka  field:ktst.Rec.val  versions:0+  nullable_versions:''  tagged_versions:''  tag:0  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give a kafka nullable array to a ctype whose pmask field is named field_mask, and
// print KafkaEncode, KafkaDecode and the presence accessors.  Check that the
// encoder's presence checks use the same derived name as the decoder's set side.
// The presence accessors derive their names from the pmask field, here
// val_FieldMaskQ and val_SetFieldMask.  The golden pins the KafkaEncode presence
// checks and the KafkaDecode present-mark.
void atf_comp::comptest_amc_KafkaPmaskName() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'ktst.Rec..KafkaEncode|ktst.Rec..KafkaDecode|ktst.Rec.val.FieldMaskQ|ktst.Rec.val.SetFieldMask' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ktst  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:ktst  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:ktst.Rec  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ktst.Rec.field_mask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:ktst.Rec.field_mask  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ktst.Rec.val  arg:i32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:ktst.Rec.val  aliased:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ckafka  ctype:ktst.Rec  kind:field  root:Rec  valid_versions:0  flexible_versions:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fkafka  field:ktst.Rec.val  versions:0+  nullable_versions:0+  tagged_versions:''  tag:0  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an optional fpbuf field to a ctype whose pmask field is named field_mask,
// and print PbufEncode, PbufDecode, the field's Set and the presence accessors.
// Check that both sides of the protobuf codec spell the name that the pmask field
// derives.
// The derived-name rule is the one of KafkaPmaskName.  The encoder's presence guard
// reads val_FieldMaskQ.  The decoder stores the plain Val field into its own member
// and marks presence with val_SetFieldMask, the same update that the field's own
// Set carries.
void atf_comp::comptest_amc_PbufPmaskName() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'pbt.Msg..PbufEncode|pbt.Msg..PbufDecode|pbt.Msg.val.Set|pbt.Msg.val.FieldMaskQ|pbt.Msg.val.SetFieldMask' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.field_mask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Msg.field_mask  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.val  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.val  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give a bitfld an fpbuf on a ctype with a pmask, print PbufEncode, PbufDecode and
// the field's Set, and check that the encoder reads the field through the Get
// accessor and the decoder stores a temporary through the Set accessor.
// The field's value lives inside its source field, with no member of its own.  The
// temporary starts empty, because the varint decoder replaces whatever it
// receives.  That Set writes the slice of the source word and leaves the pmask
// alone, so the decoder marks presence itself.
void atf_comp::comptest_amc_PbufBitfld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'pbt.Msg..PbufEncode|pbt.Msg..PbufDecode|pbt.Msg.val.Set' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.pmask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Msg.pmask  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.flags  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.val  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:pbt.Msg.val  offset:0  width:8  srcfield:pbt.Msg.flags  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.val  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give a bitfld an fpbuf on a proto3 ctype with no pmask, print PbufEncode,
// PbufDecode and the field's Set, and check that the encode guard reads the field
// through its Get accessor.
// Presence is implicit, so the encode guard is the pbtype's default-value check.
// A bitfield has no member of its own to compare against zero.  The golden also
// pins the Set body, which stores straight into the source word because no
// presence bit follows.
void atf_comp::comptest_amc_PbufBitfldNondflt() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'pbt.Msg..PbufEncode|pbt.Msg..PbufDecode|pbt.Msg.val.Set' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.flags  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.val  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:pbt.Msg.val  offset:0  width:8  srcfield:pbt.Msg.flags  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.val  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a repeated fpbuf field that is a pmask member, print PbufDecode and the
// filtered Print, and check that the decoder calls SetPresent in both the packed
// and the one-element-per-tag decode branch.
// The print filter consults the same bit.  A decoder that left the bit clear when
// elements arrive would make the filter drop the decoded elements.
void atf_comp::comptest_amc_PbufRepeatedPmask() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'pbt.Msg..PbufDecode|pbt.Msg..Print' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.pmask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Msg.pmask  filter_print:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.tag  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Msg.tag  aliased:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:pbt.Msg.String  printfmt:Tuple  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.tag  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc one message with each singular decode store shape and one with the
// repeated controls, print both PbufDecode bodies, and check the store path that
// each field takes.
// A singular field decodes into its own member.  Under pb_type message that member
// already holds the earlier occurrence, so the nested decoder merges into it
// (pbt.Msg.bare and .sub).  A pmask member decodes in place too, and the decoder
// marks it present afterwards (.opt, .sub).  Its own Set would mark it as well.
// Storing through that Set would copy the value out and back on every occurrence
// of the field number.
// The shapes with no member to decode into pay for a temporary: .slice through the
// bitfld accessors, and .netval through the byte-swapping pair.  So does .cond,
// whose Set moves the record between fcond lists.  Of the eight, only .computed
// seeds its temporary from the field's current value, because it alone is both
// accessor-routed and a message.  Get is then the merge's only way to see what the
// earlier occurrence set.  pbt.Rep appends through Alloc in every branch.
void atf_comp::comptest_amc_PbufStore() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'pbt.Msg..PbufDecode|pbt.Rep..PbufDecode' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:cd  circular:Y  haveprev:Y  instail:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.delcurs  dflt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:lib  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Sub  comment:\"embedded message, with a repeated member so a merge is visible\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Sub.item  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Sub.item  aliased:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Sub  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Sub.item  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:\"every singular store shape of the pbuf decoder in one message\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.pmask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Msg.pmask  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.plain  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.opt  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:pbt.Msg.pmask/pbt.Msg.opt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.bare  arg:pbt.Sub  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.sub  arg:pbt.Sub  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:pbt.Msg.pmask/pbt.Msg.sub  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.flags  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.slice  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:pbt.Msg.slice  offset:0  width:8  srcfield:pbt.Msg.flags  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:pbt.Msg.pmask/pbt.Msg.slice  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.netval  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:pbt.Msg.netval  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.computed  arg:pbt.Sub  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cppfunc  field:pbt.Msg.computed  expr:\"parent.bare\"  print:N  set:Y");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.cond  arg:bool  reftype:Val  dflt:false  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.FDb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.FDb._db  arg:pbt.FDb  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.FDb.msg  arg:pbt.Msg  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.FDb.cd_msg_on  arg:pbt.Msg  reftype:Llist  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:pbt.FDb.cd_msg_on  havetail:N  havecount:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:pbt.FDb.cd_msg_on  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.fcond  fcond:pbt.Msg.cond/true  ins:pbt.FDb.cd_msg_on  via:''  rem:Y  comment:\"mirror: on the list iff the wire said true\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.plain  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.opt  field_number:2  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.bare  field_number:3  pb_type:message  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.sub  field_number:4  pb_type:message  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.slice  field_number:5  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.netval  field_number:6  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.computed  field_number:7  pb_type:message  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.cond  field_number:8  pb_type:bool  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Rep  comment:\"the repeated controls: every occurrence appends through Alloc\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Rep.pmask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Rep.pmask  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Rep.tag  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Rep.tag  aliased:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:pbt.Rep.pmask/pbt.Rep.tag  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Rep.item  arg:pbt.Sub  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Rep.item  aliased:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Rep  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Rep.tag  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Rep.item  field_number:2  pb_type:message  packed:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give a global ctype an Opt field, print the field's Print, and check that amc
// rejects the field as amc.infinity_pool and that the Print body emitted beside
// the error holds no bare (void); statement for the absent parent argument.
void atf_comp::comptest_amc_OptGlobalPrint() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N algo_lib.FDb.tail.Print < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sub  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sub.n  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Sub.String  printfmt:Extern  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.tail  arg:algo_lib.Sub  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a base-1 numstr and a base-37 one, and check that amc reports
// amc.bad_numstr for both fields, naming each.
// Neither base has a digit encoding.  Unary needs one digit per unit of value, and
// bases 37..94 have no character map.
void atf_comp::comptest_amc_BadNumstrBase() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr5_U32_Base1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr5_U32_Base1.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr5_U32_Base1.ch  length:5  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr5_U32_Base1.ch  numtype:u32  base:1  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr5_U32_Base37  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr5_U32_Base37.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr5_U32_Base37.ch  length:5  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr5_U32_Base37.ch  numtype:u32  base:37  min_len:1");
    atf_comp::ProcWriteEof(proc);
}

// Give amc three numstr fields whose min_len breaks a width contract, and check
// that amc reports amc.bad_numstr for all three.
// LnumStr9_U32's min_len is wider than the string itself.  LspaceStr100_U64's is
// wider than SetnumMaybe's 64-character pad budget.  The signed LspaceStr6_I32's
// equals the string length, which leaves the sign no slot, so every negative value
// would fail at run time.  amc has no way to honor any of these width contracts.
void atf_comp::comptest_amc_BadNumstrMinlen() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr9_U32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr9_U32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr9_U32.ch  length:9  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr9_U32.ch  numtype:u32  base:10  min_len:12");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LspaceStr100_U64  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LspaceStr100_U64.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LspaceStr100_U64.ch  length:100  strtype:leftpad  pad:\"' '\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LspaceStr100_U64.ch  numtype:u64  base:10  min_len:80");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LspaceStr6_I32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LspaceStr6_I32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LspaceStr6_I32.ch  length:6  strtype:leftpad  pad:\"' '\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LspaceStr6_I32.ch  numtype:i32  base:10  min_len:6");
    atf_comp::ProcWriteEof(proc);
}

// Give amc numstr fields over numtypes that stand for no integer bltin, over names
// that disagree with their numtype, and over wrappers with each combination of
// conversions, and print LnumStr6_U32's Getnum and Geti64.  Check that amc reports
// each bad numtype, each lying name and each missing conversion, and that the
// controls draw no diagnostic.
// The fixture holds one field per numtype that stands for no integer bltin: double
// and float carry no integer range; u128 is wider than the range amc represents;
// pad_byte is no number; and a plain struct ctype stands for no bltin.  Each falls
// back on the widest signed bounds, which admit every value and make the sign
// tests fire even for an unsigned numtype.  A range comparison built from those
// bounds would never reject.  One fdec field trips the same check from the other
// side.
// The next fields exercise the agreement between a ctype name and its numtype.  amc
// enforces it so that a name never lies about the range its string holds.
// LnumStr6_I32 says I32 over a u32 numtype, and LnumStr9_U64 says U64 over the
// same one.  amc rejects a name that lies about the sign and one that lies about
// the width alike.  LnumStr4_I16 says I16 over algo_lib.U32Cast, a ctype whose
// single Val field holds a u32.  amc judges the numtype by the u32 it stands for,
// so it catches the lie through the wrapper.  The controls draw no diagnostic from
// the same run: LnumStr7_U32 states a truthful token over a bare u32; LnumStr6_U32
// states one over the wrapper; LnumStrPlain carries no token to judge; and
// LnumStr7_I8 carries a false one under strict:N, which asks for no check.
// A numtype that stands for the builtin through another ctype moves a value both
// ways.  Getnum constructs the numtype from the digits it parsed, and Geti64 reads
// that result as an i64.  The constructor comes from a dmmeta.cpptype row asking
// for one.  The read comes from the operator that dmmeta.fcast generates.  The
// fixture carries one numtype per combination of the two.  LnumStr6_U32 names a
// wrapper with both, and amc accepts it.  The query pins its Getnum and Geti64, the
// two statements that the conversions have to satisfy.  LnumStr8_U32 names a
// wrapper with the operator alone, LnumStr10_U32 one with the constructor alone,
// and LnumStr5_U32 one with neither.  amc reports each, naming what is missing.
// LnumStr11_U32 names a wrapper two ctypes above the builtin, whose constructor
// takes the inner wrapper in place of the builtin.  amc reports it as standing for
// the builtin only through another ctype.  The bare builtins are the accepted edge
// of the same rule.  All eight are present with a truthful name token, so the test
// covers both signs and every width.
void atf_comp::comptest_amc_BadNumstrNumtype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.LnumStr6_U32.ch.Getnum|algo_lib.LnumStr6_U32.ch.Geti64' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pad_byte  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:pad_byte  size:1  alignment:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:pad_byte.String  printfmt:Extern  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:pad_byte  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:float  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:float  likeu64:N  bigendok:N  issigned:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:float  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:float.String  printfmt:Extern  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:float  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:double  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:double  likeu64:N  bigendok:N  issigned:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:double  size:8  alignment:8  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:double.String  printfmt:Extern  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:double  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr8_F64  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr8_F64.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr8_F64.ch  length:8  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr8_F64.ch  numtype:double  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr5_Fdb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr5_Fdb.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr5_Fdb.ch  length:5  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr5_Fdb.ch  numtype:algo_lib.FDb  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr7_U32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr7_U32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr7_U32.ch  length:7  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr7_U32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr6_I32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr6_I32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr6_I32.ch  length:6  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr6_I32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr4_U128  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr4_U128.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr4_U128.ch  length:4  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr4_U128.ch  numtype:u128  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr4_Float  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr4_Float.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr4_Float.ch  length:4  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr4_Float.ch  numtype:float  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr4_Padbyte  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr4_Padbyte.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr4_Padbyte.ch  length:4  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr4_Padbyte.ch  numtype:pad_byte  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.U32Cast  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.U32Cast.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo_lib.U32Cast.value  expr:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.U32Cast  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr6_U32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr6_U32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr6_U32.ch  length:6  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr6_U32.ch  numtype:algo_lib.U32Cast  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr4_I16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr4_I16.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr4_I16.ch  length:4  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr4_I16.ch  numtype:algo_lib.U32Cast  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.U32Val  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.U32Val.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr5_U32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr5_U32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr5_U32.ch  length:5  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr5_U32.ch  numtype:algo_lib.U32Val  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.U32Fcast  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.U32Fcast.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo_lib.U32Fcast.value  expr:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr8_U32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr8_U32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr8_U32.ch  length:8  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr8_U32.ch  numtype:algo_lib.U32Fcast  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.U32Ctor  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.U32Ctor.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.U32Ctor  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr10_U32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr10_U32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr10_U32.ch  length:10  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr10_U32.ch  numtype:algo_lib.U32Ctor  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.U32Cast2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.U32Cast2.value  arg:algo_lib.U32Cast  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo_lib.U32Cast2.value  expr:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.U32Cast2  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr11_U32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr11_U32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr11_U32.ch  length:11  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr11_U32.ch  numtype:algo_lib.U32Cast2  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr3_I8  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr3_I8.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr3_I8.ch  length:3  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr3_I8.ch  numtype:i8  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr3_U8  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr3_U8.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr3_U8.ch  length:3  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr3_U8.ch  numtype:u8  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr6_I16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr6_I16.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr6_I16.ch  length:6  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr6_I16.ch  numtype:i16  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr5_U16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr5_U16.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr5_U16.ch  length:5  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr5_U16.ch  numtype:u16  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr10_I32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr10_I32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr10_I32.ch  length:10  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr10_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr19_I64  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr19_I64.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr19_I64.ch  length:19  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr19_I64.ch  numtype:i64  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr20_U64  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr20_U64.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr20_U64.ch  length:20  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr20_U64.ch  numtype:u64  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr9_U64  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr9_U64.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr9_U64.ch  length:9  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr9_U64.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStrPlain  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStrPlain.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStrPlain.ch  length:4  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStrPlain.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr7_I8  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr7_I8.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr7_I8.ch  length:7  strtype:leftpad  pad:\"'0'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr7_I8.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.BadDec  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.BadDec.price  arg:double  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.BadDec.price  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc numstr and smallstr fields whose pads cover the digits of each base, the
// sign character and the NUL pad, and check that amc rejects each pad that reads
// back as part of a number and passes each control.
// A left-pad that is a nonzero digit of the base strips leading value digits on
// read: '!' is digit 1 in base 95; 'c' is a digit in base 36; and '5' is a digit
// in base 10.  A right-pad that is any digit of the base divides the stored value
// by a power of the base: NUL in base 256, ' ' in base 95, 'A' in base 36 and '7'
// in base 10.  The pad is a C++ character literal, so amc judges an escaped
// spelling by the byte it denotes.  A base-16 right-pad '\x30' is the digit zero
// and draws a rejection.  A base-95 left-pad '\0' is no digit of that base and
// passes.  Only the zero digit is a safe left-pad, and no digit is a safe
// right-pad, so amc rejects eight of these nine numstr fields.
// Four more fields carry no numstr row and meet the plain smallstr right-pad check
// (amc.invalidpad).  That check refuses any pad character that a reader could take
// for part of a number, and it too reads an escaped pad by its denoted byte.  It
// refuses the digit '3' and the escaped digit '\x30'.  The escaped 'A' and the
// escaped NUL belong to no number and pass.
// The sign character is the other pad that a signed numstr is unable to read back.
// The reader strips a left pad from the end that holds the sign.  So a signed
// base-10 field padded with '-' loses the sign of every negative value, and amc
// rejects it.  Its three neighbors are controls that draw no diagnostic: the same
// '-' pad on an unsigned field, which writes no sign; the same '-' pad on the
// right, stripped from the end away from the sign; and '+' on the left of a signed
// field, a character the reader never takes for a sign.  A signed field
// left-padded with the zero digit is the fourth control, because leading zeros
// carry no value and strip safely.
// A signed numstr may not use the NUL pad either, and that case shows again that
// amc judges a pad by the byte it denotes.  amc rejects all three spellings of
// that one pad on a signed field: the bare 0, the empty pad value that amc fills
// in as 0, and the escaped '\0'.  It rejects them on the left and on the right
// alike.  The same empty pad on an unsigned field passes at either end.  The two
// rpascal fields are the controls for the strtype dimension.  Such a string
// carries its own length and strips no pad, so neither the NUL pad on a signed
// field nor a digit pad draws a diagnostic.
void atf_comp::comptest_amc_BadNumstrPad() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LbangStr5_U32_Base95  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LbangStr5_U32_Base95.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LbangStr5_U32_Base95.ch  length:5  strtype:leftpad  pad:\"'!'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LbangStr5_U32_Base95.ch  numtype:u32  base:95  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LceeStr5_U32_Base36  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LceeStr5_U32_Base36.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LceeStr5_U32_Base36.ch  length:5  strtype:leftpad  pad:\"'c'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LceeStr5_U32_Base36.ch  numtype:u32  base:36  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LdashStr5_I32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LdashStr5_I32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LdashStr5_I32.ch  length:5  strtype:leftpad  pad:\"'-'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LdashStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LdashStr5_U32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LdashStr5_U32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LdashStr5_U32.ch  length:5  strtype:leftpad  pad:\"'-'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LdashStr5_U32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LemptyStr5_I32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LemptyStr5_I32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LemptyStr5_I32.ch  length:5  strtype:leftpad  pad:''  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LemptyStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LemptyStr5_U32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LemptyStr5_U32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LemptyStr5_U32.ch  length:5  strtype:leftpad  pad:''  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LemptyStr5_U32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LescnulStr5_I32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LescnulStr5_I32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LescnulStr5_I32.ch  length:5  strtype:leftpad  pad:\"'\\\\0'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LescnulStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LescnulStr5_U32_Base95  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LescnulStr5_U32_Base95.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LescnulStr5_U32_Base95.ch  length:5  strtype:leftpad  pad:\"'\\\\0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LescnulStr5_U32_Base95.ch  numtype:u32  base:95  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LfiveStr6_U32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LfiveStr6_U32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LfiveStr6_U32.ch  length:6  strtype:leftpad  pad:\"'5'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LfiveStr6_U32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnullStr5_I32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnullStr5_I32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnullStr5_I32.ch  length:5  strtype:leftpad  pad:0  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnullStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LplusStr5_I32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LplusStr5_I32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LplusStr5_I32.ch  length:5  strtype:leftpad  pad:\"'+'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LplusStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LzeroStr5_I32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LzeroStr5_I32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LzeroStr5_I32.ch  length:5  strtype:leftpad  pad:\"'0'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LzeroStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.PpasStr5_I32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PpasStr5_I32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.PpasStr5_I32.ch  length:5  strtype:rpascal  pad:''  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.PpasStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.PpasdigStr5_U32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PpasdigStr5_U32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.PpasdigStr5_U32.ch  length:5  strtype:rpascal  pad:\"'7'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.PpasdigStr5_U32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RcapaStr5_U32_Base36  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RcapaStr5_U32_Base36.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RcapaStr5_U32_Base36.ch  length:5  strtype:rightpad  pad:\"'A'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RcapaStr5_U32_Base36.ch  numtype:u32  base:36  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RdashStr5_I32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RdashStr5_I32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RdashStr5_I32.ch  length:5  strtype:rightpad  pad:\"'-'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RdashStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RemptyStr5_I32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RemptyStr5_I32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RemptyStr5_I32.ch  length:5  strtype:rightpad  pad:''  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RemptyStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RemptyStr5_U32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RemptyStr5_U32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RemptyStr5_U32.ch  length:5  strtype:rightpad  pad:''  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RemptyStr5_U32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RescdigStr5  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RescdigStr5.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RescdigStr5.ch  length:5  strtype:rightpad  pad:\"'\\\\x30'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RescletStr5  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RescletStr5.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RescletStr5.ch  length:5  strtype:rightpad  pad:\"'\\\\x41'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RescnulStr5  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RescnulStr5.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RescnulStr5.ch  length:5  strtype:rightpad  pad:\"'\\\\0'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RescnumStr5_U32_Base16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RescnumStr5_U32_Base16.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RescnumStr5_U32_Base16.ch  length:5  strtype:rightpad  pad:\"'\\\\x30'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RescnumStr5_U32_Base16.ch  numtype:u32  base:16  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RnullStr4_U32_Base256  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RnullStr4_U32_Base256.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RnullStr4_U32_Base256.ch  length:4  strtype:rightpad  pad:0  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RnullStr4_U32_Base256.ch  numtype:u32  base:256  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RsevenStr5_U32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RsevenStr5_U32.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RsevenStr5_U32.ch  length:5  strtype:rightpad  pad:\"'7'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RsevenStr5_U32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RspaceStr5_U32_Base95  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RspaceStr5_U32_Base95.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RspaceStr5_U32_Base95.ch  length:5  strtype:rightpad  pad:\"' '\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RspaceStr5_U32_Base95.ch  numtype:u32  base:95  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RthreeStr5  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RthreeStr5.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RthreeStr5.ch  length:5  strtype:rightpad  pad:\"'3'\"  strict:N");
    atf_comp::ProcWriteEof(proc);
}

// Give amc rpascal strings of 255, 256 and 300 characters and a 300-character
// right-padded string, and check that amc reports smallstr.toobig for the two long
// rpascal strings and exits with 2.
// An rpascal string stores its length in one byte, so 255 is the longest one whose
// length that byte can hold.  Each diagnostic raises the error count, so a later
// one never discards what an earlier one reported.  The controls draw no
// diagnostic.  A 255-character rpascal string is the longest accepted one.  A
// 300-character right-padded string carries no length byte, so the bound belongs
// to the rpascal strtype alone.
void atf_comp::comptest_amc_BadSmallstrToobig() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Smallstr255  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Smallstr255.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Smallstr255.ch  length:255  strtype:rpascal  pad:''  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Smallstr256  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Smallstr256.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Smallstr256.ch  length:256  strtype:rpascal  pad:''  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Smallstr300  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Smallstr300.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Smallstr300.ch  length:300  strtype:rpascal  pad:''  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RspaceStr300  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RspaceStr300.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RspaceStr300.ch  length:300  strtype:rightpad  pad:\"' '\"  strict:Y");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a signed base-95 numstr and a signed base-256 one, and check that amc
// rejects both fields with amc.bad_numstr.
// '-' is a digit in both bases, so a stored digit string looks the same as a
// signed one.  Both fields left-pad with ' ', which is digit 0 in base 95 and
// digit 32 in base 256.  So the base-256 field draws a second rejection from the
// pad check, because a left-pad that is a nonzero digit of the base strips leading
// value digits on read.
void atf_comp::comptest_amc_BadNumstrSignedBase() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LspaceStr5_I32_Base95  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LspaceStr5_I32_Base95.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LspaceStr5_I32_Base95.ch  length:5  strtype:leftpad  pad:\"' '\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LspaceStr5_I32_Base95.ch  numtype:i32  base:95  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LspaceStr4_I32_Base256  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LspaceStr4_I32_Base256.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LspaceStr4_I32_Base256.ch  length:4  strtype:leftpad  pad:\"' '\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LspaceStr4_I32_Base256.ch  numtype:i32  base:256  min_len:1");
    atf_comp::ProcWriteEof(proc);
}

// Give one cpbuf ctype every illegal field number, and check that amc reports
// amc.pbuf_field_number for each in one run.
// A duplicate within the ctype gives two identical decoder case labels, which fail
// to compile.  Number 0 is wire tag 0, which the protobuf format reserves as
// illegal.  2^29 overflows the (number<<3)|wire_type tag and aliases another
// field.  Protobuf reserves 19000..19999 for its own implementation, so amc
// rejects both ends of that range and accepts 18999 and 20000 beside it.
void atf_comp::comptest_amc_BadPbufFieldNumber() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.a  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.b  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.c  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.d  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.e  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.f  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.g  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.h  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.a  field_number:5  pb_type:int32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.b  field_number:5  pb_type:int32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.c  field_number:0  pb_type:int32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.d  field_number:536870912  pb_type:int32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.e  field_number:19000  pb_type:int32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.f  field_number:19999  pb_type:int32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.g  field_number:18999  pb_type:int32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.h  field_number:20000  pb_type:int32  packed:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give one cpbuf ctype every arg and reftype shape that the pbuf codec fails to
// compile against, and check that amc rejects each in one run.
// The lib_pb codec accepts only the pbtype's argtype, so pb_type string on a u32
// fails.  pb_type message needs a cpbuf arg.  An Inlary field has no wire mapping.
// pb_type enum needs an arg with the fconst-carrying i32 value shape.  One enum
// arg has no value field, and the other's value field is u8.  The lib_pb varint
// codec binds i32.
void atf_comp::comptest_amc_BadPbufArg() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Color  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Color.value  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:pbt.Color.value/red  value:0  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:pbt.Color.value/green  value:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.b  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.c  arg:u32  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:pbt.Msg.c  min:4  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.d  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.e  arg:pbt.Color  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.a  field_number:1  pb_type:string  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.b  field_number:2  pb_type:message  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.c  field_number:3  pb_type:int32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.d  field_number:4  pb_type:enum  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.e  field_number:5  pb_type:enum  packed:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc three fpbuf rows that ask for packed encoding, and check that amc
// reports amc.pbuf_packed for the two where the protobuf format has none.
// One of the two is on a singular field, and the other is on a repeated field of a
// length-delimited pb_type.  The emitter reads packed only in the repeated
// non-length-delimited branch.  It would silently ignore both rows, and the author
// would get the ordinary encoding.  The third row, packed on a repeated varint
// field, is the legal shape and draws no report.
void atf_comp::comptest_amc_BadPbufPacked() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Sub  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Sub.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Sub  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Sub.a  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.b  arg:pbt.Sub  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Msg.b  aliased:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.c  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Msg.c  aliased:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.a  field_number:1  pb_type:uint32  packed:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.b  field_number:2  pb_type:message  packed:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.c  field_number:3  pb_type:uint32  packed:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc two fpbuf rows that would generate no wire code and a cpbuf ctype that
// inherits a base, and check that amc reports each of the three.
// One fpbuf row is on a field of a ctype with no cpbuf row, and amc generates no
// codec without one (amc.pbuf_ctype).  The other is on a Base field, and emission
// never writes a base's fields to the wire (amc.pbuf_reftype).  pbt.Msg2 is a
// cpbuf ctype with a Base field, so the base's fields would be absent from its
// message (amc.pbuf_base).  Each report keeps a row from sitting silently dead.
void atf_comp::comptest_amc_BadPbufNoCodec() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Sub  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Sub.val  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Sub  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Sub.val  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Orphan  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Orphan.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Orphan.a  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.base  arg:pbt.Sub  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.base  field_number:1  pb_type:message  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg2.base  arg:pbt.Sub  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg2.own  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg2  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg2.own  field_number:2  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc four cpbuf ctypes, two with a valid syntax, one misspelled and one empty,
// and print their PbufEncode.  Check that amc reports amc.pbuf_syntax for the
// misspelling and the empty value, and that the two controls show what each
// accepted syntax emits.
// cpbuf.syntax names a row of dmmeta.pbsyntax, the table that says whether the
// syntax has implicit presence.  acr -check catches the spelling when the row goes
// through the database.  amc also reads a schema out of -in_dir, so it rejects a
// value with no row itself.  proto2 writes the scalar unconditionally, and proto3
// guards it with the default-value test.
void atf_comp::comptest_amc_BadPbufSyntax() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'pbt.Proto%..PbufEncode' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Proto2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Proto2.val  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Proto2  syntax:proto2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Proto2.val  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Proto3.val  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Proto3  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Proto3.val  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Misspelled  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Misspelled.val  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Misspelled  syntax:prot3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Misspelled.val  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Unset  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Unset.val  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Unset  syntax:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Unset.val  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc four ctypes that each declare a oneof, and check that amc reports
// amc.pbuf_oneof for the three whose variants share no single pmask.
// A oneof is a union.  The encoder writes at most one variant only because a store
// into one variant clears the presence bits of its siblings on the same pmask.
// Three shapes break that.  NoMask's variants have no pmask, so the proto3
// default-value test guards each, and every nonzero variant goes on the wire.
// Split's variants sit on two pmasks, and a store clears siblings only on the mask
// it writes.  Half's pmask has an explicit member list that covers one variant and
// leaves out the other.  The first ctype, pbt.Msg, is the control, with both
// variants on one pmask, and it draws no diagnostic.
void atf_comp::comptest_amc_BadPbufOneof() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.pmask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Msg.pmask  filter_print:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.b  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.plain  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.a  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.b  field_number:2  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.plain  field_number:3  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.Msg.a/payload  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.Msg.b/payload  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.NoMask  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.NoMask.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.NoMask.b  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.NoMask  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.NoMask.a  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.NoMask.b  field_number:2  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.NoMask.a/payload  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.NoMask.b/payload  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Split  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Split.pmask1  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Split.pmask1  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Split.pmask2  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Split.pmask2  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Split.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Split.b  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:pbt.Split.pmask1/pbt.Split.a  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:pbt.Split.pmask2/pbt.Split.b  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Split  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Split.a  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Split.b  field_number:2  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.Split.a/payload  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.Split.b/payload  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Half  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Half.pmask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Half.pmask  filter_print:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Half.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Half.b  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:pbt.Half.pmask/pbt.Half.a  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Half  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Half.a  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Half.b  field_number:2  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.Half.a/payload  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.Half.b/payload  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give one cpbuf ctype each singular store shape and another the repeated
// controls, and check that amc reports amc.pbuf_store for the three shapes with a
// Get and no Set.
// The decoder of a singular field with no member of its own stores through the
// field's generated Set.  A shape that has a Get and no Set would emit a call to a
// function amc never writes.  The three rejected rows are a cppfunc with set:N, a
// substr and a falias.  The shapes that have a Set stand beside them as controls
// that draw no diagnostic: an fbigend field, a bitfld, a cppfunc with set:Y, and a
// plain Val field stored in place.  The second ctype carries the repeated
// controls.  There the decoder appends through Alloc, and a fldfunc element type
// needs no Set.
void atf_comp::comptest_amc_BadPbufStore() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:algo  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:algo  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo.Smallstr20  comment:\"inline string with length field\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo.Smallstr20  extrn:N  genop:Y  order:Y  minmax:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.chash  ctype:algo.Smallstr20  hashtype:CRC32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cstr  ctype:algo.Smallstr20  strequiv:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo.Smallstr20  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo.Smallstr20.String  printfmt:Raw  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo.Smallstr20.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo.Smallstr20.ch  expr:algo::strptr  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo.Smallstr20.ch  length:20  strtype:rpascal  pad:''  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.plain  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.netval  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:pbt.Msg.netval  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.flags  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.slice  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:pbt.Msg.slice  offset:0  width:8  srcfield:pbt.Msg.flags  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.computed  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cppfunc  field:pbt.Msg.computed  expr:\"parent.plain\"  print:N  set:Y");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.readonly  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cppfunc  field:pbt.Msg.readonly  expr:\"parent.plain\"  print:N  set:N");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.addr  arg:algo.Smallstr20  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.port  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.substr  field:pbt.Msg.port  expr:/RR  srcfield:pbt.Msg.addr");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.alias  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.falias  field:pbt.Msg.alias  srcfield:pbt.Msg.plain  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.plain  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.netval  field_number:2  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.slice  field_number:3  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.computed  field_number:4  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.readonly  field_number:5  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.port  field_number:6  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.alias  field_number:7  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Rep  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Rep.plain  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Rep.plain  aliased:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Rep.readonly  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Rep.readonly  aliased:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cppfunc  field:pbt.Rep.readonly  expr:''  print:N  set:N");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Rep  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Rep.plain  field_number:1  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Rep.readonly  field_number:2  pb_type:uint32  packed:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give an fpbuf field a pb_type with no amcdb.pbtype row, and check that amc
// reports amc.pbuf_type.
// The table supplies the wire type and the codec binding.  Without them the
// encoder would write the field tag with no payload, and the decoder case would
// consume the tag and leave the value unread.
void atf_comp::comptest_amc_BadPbufType() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.val  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.val  field_number:1  pb_type:int33  packed:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give a u8 pmask nine member fields, and check that amc reports amc.pmask_width.
// Nine presence bits overflow the pmask field's eight bits.
void atf_comp::comptest_amc_BadPmaskWidth() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.pmask  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.Rec.pmask  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f0  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f1  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f2  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f3  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f4  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f5  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f6  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f7  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f8  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc explicit pmaskfld_member rows, one per field kind that the membership
// rule excludes, and check that amc reports amc.pmask_member for each row by name.
// The excluded fields are a field of another ctype, the Global anchor, the pmask
// word itself, a length field, a type field and a Base field.  None of them has a
// presence bit that the pmask's parent struct could back.  Auto-populated members
// satisfy the rule by construction.  An explicit row must pass the same
// validation, and one row per exclusion cause keeps every arm of the rule
// load-bearing.  A type field and a Base field never sit on one ctype.  The base
// declares the type field, and the deriving ctype declares the Base field as its
// first field.  So Hdr and Msg carry those two, each with its own pmask.
// The width check counts a rejected row's bit like any other, because the
// membership rule reports a row and leaves it on the pmask.  This fixture declares
// the pmask a u32 and puts five member rows on it.  The count then stays far inside
// the width, and the golden carries the membership diagnostics alone.
void atf_comp::comptest_amc_BadPmaskMember() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.pmask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.FDb.pmask  filter_print:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.maxjobs  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.timeout  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.FDb.pmask/algo_lib.FDb._db  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.FDb.pmask/algo_lib.FDb.maxjobs  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sub  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sub.val  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.FDb.pmask/algo_lib.Sub.val  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msglen  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.FDb.msglen  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.FDb.pmask/algo_lib.FDb.pmask  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.FDb.pmask/algo_lib.FDb.msglen  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Hdr  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Hdr.mtype  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.typefld  field:algo_lib.Hdr.mtype  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Hdr.hdrmask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.Hdr.hdrmask  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.Hdr.hdrmask/algo_lib.Hdr.mtype  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.hdr  arg:algo_lib.Hdr  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.Msg  type:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.msgmask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.Msg.msgmask  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.Msg.msgmask/algo_lib.Msg.hdr  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc two ctypes that contain each other by value, one that contains itself,
// and two bitfields over those cycles.  Check that amc reports each decimal
// field's arg as amc.bad_numtype and ends the run at the circular definition.
// No finite layout exists for such a cycle, so no struct in it has a size.
// Each cycle also carries a decimal field, which asks for the arg's numeric range
// before the size computation runs.  amc resolves the range by walking the chain of
// ctypes whose single Val field names the next one.  A cycle would send that walk
// around forever.  So the fixture holds the two shapes a cycle can take, a ctype
// naming itself and a pair naming each other.  amc reports each decimal field as
// an arg with no numeric range, then reaches the size computation and ends at the
// circular definition.
// A bitfield asks the same walk two further questions, and the fixture puts a
// cycle in front of each.  One is the type that its accessors hand back, which the
// bitfield's own arg names.  The other is the type that its mask operations run
// on, which its source field's arg names.  Neither question turns the cycle into
// an internal error.  The run still ends at the circular definition.
void atf_comp::comptest_amc_BadSizeCycle() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CycA  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CycB  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CycA.b  arg:algo_lib.CycB  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CycB.a  arg:algo_lib.CycA  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.CycA.b  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CycSelf  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CycSelf.self  arg:algo_lib.CycSelf  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.CycSelf.self  nplace:2  fixedfmt:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CycBit  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CycBit.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CycBit.slice  arg:algo_lib.CycSelf  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.CycBit.slice  offset:0  width:4  srcfield:algo_lib.CycBit.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CycBitSrc  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CycBitSrc.word  arg:algo_lib.CycSelf  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CycBitSrc.slice  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.CycBitSrc.slice  offset:0  width:4  srcfield:algo_lib.CycBitSrc.word  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc an Inlary of 65537 elements, each a 65536-byte fixed Inlary, and check
// that amc reports amc.size_overflow.
// The total byte size exceeds the i32 range that amc computes sizes in.
void atf_comp::comptest_amc_BadSizeOverflow() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Big  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Big.buf  arg:u64  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Big.buf  min:8192  max:8192  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Huge  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Huge.big  arg:algo_lib.Big  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Huge.big  min:65537  max:65537  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc one fixed field of each shape after a Varlen field, and check that amc
// reports amc.varlen_last for each field that declares a member of its own,
// attributes it to the varlen field it follows, and exits nonzero.
// The varlen data begins at the end of the struct.  Writes through the varlen
// accessors would clobber a trailing fixed member.
//
// Only a field that declares a member of its own has storage to clobber, so the
// golden is an accept/reject table.  amc reports a Val and a Ptr, which declare
// members directly, and a ZSListMT, whose read-side and write-side list heads are
// two more members of the parent.  amc passes over the rest: a Bitfld, which
// re-slices the word of a field declared before the varlen data; the three
// fldfunc kinds, a cppfunc, a falias and a substr, whose value amc computes and
// never stores; Malloc, Global and Cppstack, whose generators put no member in the
// struct; and an Opt, which draws a comment line where its optional trailing
// element would sit.  Count and Exec are the intermediate case.  The declared
// field draws no report, and amc reports the Val children it generates from it in
// their own right.  That is why cnt_n and the child_* rows appear, and cnt and
// child do not.
//
// The Opt is the one field here that a second rule answers as well.  It claims the
// end of the fixed portion that the varlen data already claims, so the golden
// carries its amc.trail_overlay row.  The absence of an amc.varlen_last row beside
// it pins the Opt as declaring no storage.
void atf_comp::comptest_amc_BadVarlenLast() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sub  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sub.val  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Sub.Argv  printfmt:Auto  read:Y  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Blob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.n  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.data  arg:u8  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.blob  arg:algo_lib.Blob  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.crc  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.ptr  arg:algo_lib.Blob  reftype:Ptr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.cnt  arg:algo_lib.Sub  reftype:Count  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.child  arg:algo_lib.Sub  reftype:Exec  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.flag  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Blob.flag  offset:0  width:8  srcfield:algo_lib.Blob.n  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.mem  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.glob  arg:u32  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.stack  arg:u32  reftype:Cppstack  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.zs_mt  arg:algo_lib.Sub  reftype:ZSListMT  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.calc  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cppfunc  field:algo_lib.Blob.calc  expr:\"parent.n + 1\"  print:N  set:N");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.nn  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.falias  field:algo_lib.Blob.nn  srcfield:algo_lib.Blob.n  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.half  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.substr  field:algo_lib.Blob.half  expr:/RL  srcfield:algo_lib.Blob.n");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.opt  arg:algo_lib.Sub  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc ctypes that pair Varlen and Opt fields in each way a ctype can declare
// them, and check that amc reports amc.trail_overlay once per rejected ctype and
// exits nonzero.
// Varlen data and an optional trailing element both begin at the end of the fixed
// portion, and the generated accessors would overlay one on the other.  Only
// varlen fields carry the end offset that lets one begin where another ends.  So
// a ctype may claim that address once, and amc rejects each further claim.
// amc rejects Varlen with Opt in both declaration orders, one ctype each.  It also
// rejects a ctype with two Opt fields, whose getters would return the same address
// twice.  A fourth ctype declares an Opt ahead of two varlen fields and draws one
// row.  The first varlen field begins at the disputed address, and the second
// begins where the first ends.  So amc reports a single overlay once, however many
// varlen fields trail it.
// Each rejected ctype draws exactly one diagnostic.  The order of a Varlen and an
// Opt matters to the separate rule that fixed fields may not follow a varlen
// field.  Reporting the Opt there would state the same overlay twice.  The
// controls draw no diagnostic: a ctype with one Opt; a ctype with one Varlen; a
// ctype with two Varlen fields, the accepting arm of the rule, where the second
// begins where the first ends; and a ctype with neither.  The two-Varlen control
// carries no pool instance.  The pool generator refuses a pool arg with more than
// one varlen field, and its diagnostic would arrive beside the ones this test is
// about.
void atf_comp::comptest_amc_BadVarlenOpt() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Blob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.n  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Blob.n  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Tail  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Tail.t  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.tail  arg:algo_lib.Tail  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.data  arg:u8  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.blob  arg:algo_lib.Blob  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rblob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rblob.n  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Rblob.n  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rblob.data  arg:u8  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rblob.tail  arg:algo_lib.Tail  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.rblob  arg:algo_lib.Rblob  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Oblob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Oblob.n  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Oblob.tail  arg:algo_lib.Tail  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Oblob.tail2  arg:algo_lib.Tail  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.oblob  arg:algo_lib.Oblob  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sblob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sblob.n  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sblob.tail  arg:algo_lib.Tail  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.sblob  arg:algo_lib.Sblob  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Vblob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Vblob.n  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Vblob.n  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Vblob.data  arg:u8  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.vblob  arg:algo_lib.Vblob  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Dblob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Dblob.n  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Dblob.n  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Dblob.tail  arg:algo_lib.Tail  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Dblob.data  arg:u8  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Dblob.data2  arg:u8  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Wblob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Wblob.n  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Wblob.n  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Wblob.data  arg:u8  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Wblob.data2  arg:u8  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give a varlen ctype a non-varlen pool instance, and check that amc reports the
// instance as amc.infinity_pool and exits nonzero.
// The pool allocates fixed-size records, so the trailing varlen data has nowhere
// to live.
void atf_comp::comptest_amc_BadInfinityPool() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Blob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.n  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.data  arg:u8  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.blob  arg:algo_lib.Blob  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a pool arg with a varlen field and no lenfld, print the pool's Delete,
// and check that its length statement is well-formed, with no stray semicolon
// inside the length expression.
// The element count is an extern user function, and Delete computes the total
// length from it.
void atf_comp::comptest_amc_PoolVarlenExtern() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N algo_lib.FDb.blob.Delete < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Blob  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.n  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.data  arg:u8  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.blob  arg:algo_lib.Blob  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a pooled ctype whose fixed size plus lenfld extra is no multiple of the
// lenfld scale, and check that amc reports amc.lenfld_scale.
// Pool alloc stores len = (size + extra) / scale, which truncates.  The reader then
// reconstructs less than the pool allocated.
void atf_comp::comptest_amc_BadLenfldScale() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.len  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.pad  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg.len  extra:0  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msg  arg:algo_lib.Msg  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a pool-less message ctype whose fixed size is no multiple of the lenfld
// scale, and check that amc reports amc.lenfld_scale.
// The message constructor and the field default store the same truncating length
// word that pool alloc would.  So the divisibility check must cover message ctypes
// as well as pooled ones.
void atf_comp::comptest_amc_BadLenfldMsgtype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.len  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.pad  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg.len  extra:0  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.Msg  type:1");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a fixed-only ctype whose only length store is its Fmt* constructor (a
// pnew row, no msgtype, no pool instance), and check that amc reports
// amc.lenfld_scale.
// The constructor's run-time total is always the fixed size.  So an indivisible
// size + extra stores the same truncating length word that pool alloc would, and
// the divisibility check must cover pnew carriers too.
void atf_comp::comptest_amc_BadLenfldPnew() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.len  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.pad  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg.len  extra:0  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.pnew  pnew:algo_lib/algo_lib.Msg.ByteAry  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc three lenflds with scale:0 on ctypes whose Opt fields consume the length
// expression, and check that amc reports amc.lenfld_scale for every offender in
// one run.
// The reports go through the accumulate path, so the run continues past the first
// length-expression consumer, where a throw would stop it.  MsgC adds the pooled
// shape with a u64 length word, the one that reaches the pool InsertMaybe raw-word
// bound.  That bound divides by the scale, and amc emits it a whole gen step before
// gen_check_lenfld reports the scale.  Without its own scale test the run would
// die on SIGFPE with no diagnostic.  The cloned base field (FMsgC.len) reports the
// row a second time.
void atf_comp::comptest_amc_BadLenfldZeroScale() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Byte  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Byte.val  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgA  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgA.len  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgA.len  extra:0  scale:0");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgA.tail  arg:algo_lib.Byte  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgB  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgB.len  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgB.len  extra:0  scale:0");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgB.tail  arg:algo_lib.Byte  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgC  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgC.len  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgC.len  extra:0  scale:0");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgC.tail  arg:algo_lib.Byte  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgC  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgC.base  arg:algo_lib.MsgC  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgc  arg:algo_lib.FMsgC  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc four message ctypes whose length field is u128, bool, char and u16, and
// check that amc reports amc.lenfld_type for the first three in one run.
// A length word needs a numeric range.  GetMinMax resolves one for the fixed-width
// integers u8 through i64 alone.  Every range check built from the widest-signed
// fallback runs vacuously, and the TS encoder omits its range guard outright.  So
// amc refuses u128, which has no arithmetic amc can do; bool, which holds no
// magnitude; and char, whose signedness is the compiler's choice.  MsgOk carries
// the same shape over a u16 word and draws no report.  That control shows that amc
// rejects the other three for their type and accepts the shape.
void atf_comp::comptest_amc_BadLenfldType() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.len  arg:u128  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.pad  arg:u128  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.Msg  type:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBool  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBool.len  arg:bool  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBool.pad  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBool.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.MsgBool  type:2");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgChar  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgChar.len  arg:char  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgChar.pad  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgChar.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.MsgChar  type:3");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgOk  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.len  arg:u16  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.pad  arg:u16  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgOk.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.MsgOk  type:4");
    atf_comp::ProcWriteEof(proc);
}

// Give a pooled pnew ctype a u8 lenfld with scale 4, and a second ctype a u32
// lenfld with scale 1.  Print the pool's AllocExtraMaybe and both Fmt*
// constructors, and check that every store of a run-time total guards the length
// field's storable range as well as scale divisibility.
// A total beyond the range would wrap mod 2^N through the word's own type, and
// frame a shorter message than the writer wrote.  The pool's AllocExtraMaybe
// bounds the caller's addon by the range minus the fixed portion, as a
// compile-time ssizeof expression.  The Fmt* constructor refuses the total with
// NULL, as the TS encoder's RangeError guard does.
// MsgW is the same shape with a wide length field (u32, scale 1).  Its range
// covers every frame length, so amc emits no lenfld test, and the frame length
// domain itself remains.  Its buffer is an Alloc, whose size argument is an int.
// So the emitted bound is the i32 maximum, and the totals it accepts are exactly
// those that a caller can ask the allocator for.
void atf_comp::comptest_amc_LenfldNarrow() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N '%msgn.AllocExtraMaybe|%MsgN_FmtByteAry|%MsgW_FmtAlloc' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgN  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgN.len  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgN.len  extra:0  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgN.p1  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgN.p2  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgN.p3  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgN.data  arg:u32  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgn  arg:algo_lib.MsgN  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pnew  pnew:algo_lib/algo_lib.MsgN.ByteAry  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgW  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgW  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgW.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgW.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgW.data  arg:u8  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pnew  pnew:algo_lib/algo_lib.MsgW.Alloc  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Generate TypeScript for two ctypes that share one u32 lenfld shape and differ in
// whether the frame length is a compile-time constant, and check that only the
// variable one's encoder carries a range guard:
// 1. Make the output directories under the test's tempdir.
// 2. Run amc from the checkout root with -out_dir under the tempdir, so the
//    derived-table acr child writes into that tempdir too.
// 3. Print both emitted TypeScript encoders from ts/gen/algo_lib_gen.ts.
// MsgFix is fixed-size, so the emitted `length` is a literal, and gen_check_lenfld
// has already proven it storable.  Its encoder carries no range guard.  MsgVar
// owns a varlen tail, so its length is a run-time sum and the guard stays.
void atf_comp::comptest_amc_JsFixedFrame() {
    atf_comp::FProc &proc = atf_comp::ProcStart("bash -c 'mkdir -p $tempdir/o/ts/gen $tempdir/o/include/gen $tempdir/o/cpp/gen $tempdir/o/data"
                                                " && $bindir/amc -in_dir:- -out_dir:$tempdir/o -report:N < <(cat test/amc/bootstrap.ssim -)"
                                                " && sed -n \"/_Encode/,/^}/p\" $tempdir/o/ts/gen/algo_lib_gen.ts'");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgFix  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgFix/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgFix  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgFix.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgFix.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgFix.val  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgVar  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgVar/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgVar  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgVar.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgVar.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgVar.data  arg:u8  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc one varlen pool with InsertMaybe per cell of the addon-count rule,
// print each pool's InsertMaybe and AllocExtraMaybe, and check that InsertMaybe
// computes the addon count of an untrusted source struct in the i64 domain and
// bounds it.
// A multiply in the length word's own type would wrap a huge corrupt stored value
// mod 2^N into a small positive count.  That count would pass the negative-addon
// guard and read past the source message.  The widened arithmetic keeps the true
// total visible, and totals outside [fixed size, i32 max] return NULL.  The
// widening itself stays exact only while the word's range fits u32.  Beyond that
// the i64 expansion can overflow, with scale > 1 in the multiply, and even at
// scale 1 in the extra adjustment after the wrap.  The overflow is undefined
// behavior, which an optimizer may exploit to elide the range test.  So amc first
// bounds a wider word by the largest value whose expanded total fits the i32
// frame-length domain, a generation-time constant.
//
// The fixture is the rule's table, one pool ctype per cell.  The arm that applies
// turns on the word's largest storable value, so the accepted edges sit either
// side of u32's max.  MsgU32 (u32, scale 4), MsgI32 (i32) and MsgBit32 (a u64 word
// masked to 32 bits by its Bitfld width) draw no bound.  MsgBit33 (masked to 33
// bits), MsgU64 (u64, scale 2) and MsgI64 (i64, scale 8) draw one.  A signed word
// is the cell where the bound, through the u64 conversion in the emitted test,
// rejects a negative stored length ahead of the addon test.  The bound's constant
// follows scale and extra.  MsgNeg carries extra -2 and MsgPos extra 4, and the
// reader formula shifts the constant for each.
//
// InsertMaybe reaches the allocation through the die-on-fail AllocExtra.  So a
// count that AllocExtraMaybe would refuse kills the process, where AllocExtraMaybe
// would return NULL.  The two functions accept exactly the same counts.  The test
// therefore prints both for every pool, and a term emitted into one and missing
// from the other shows up as a diff.  Two cells of the table make that agreement
// worth pinning.  The bound's constant comes from the row type, and msg6 and msg5
// show it as block pools whose row carries a buffer member that the base lacks.  A
// word that stores totals past i32 max leaves the length field with no bound to
// give.  So every such pool bounds the count by the i32 that a frame length is
// read back through.
void atf_comp::comptest_amc_PoolInsertScale() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb.msg%.(InsertMaybe|AllocExtraMaybe)' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sub  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sub.val  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgU32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgU32.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgU32.len  extra:0  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgU32.sub  arg:algo_lib.Sub  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgU32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgU32.base  arg:algo_lib.MsgU32  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgu32  arg:algo_lib.FMsgU32  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgI32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgI32.len  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgI32.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgI32.sub  arg:algo_lib.Sub  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgI32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgI32.base  arg:algo_lib.MsgI32  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgi32  arg:algo_lib.FMsgI32  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBit32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBit32.word  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBit32.len  arg:u64  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.MsgBit32.len  offset:0  width:32  srcfield:algo_lib.MsgBit32.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBit32.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBit32.sub  arg:algo_lib.Sub  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgBit32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgBit32.base  arg:algo_lib.MsgBit32  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgbit32  arg:algo_lib.FMsgBit32  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBit33  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBit33.word  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBit33.len  arg:u64  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.MsgBit33.len  offset:0  width:33  srcfield:algo_lib.MsgBit33.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBit33.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBit33.sub  arg:algo_lib.Sub  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgBit33  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgBit33.base  arg:algo_lib.MsgBit33  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgbit33  arg:algo_lib.FMsgBit33  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgU64  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgU64.len  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgU64.len  extra:0  scale:2");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgU64.sub  arg:algo_lib.Sub  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgU64  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgU64.base  arg:algo_lib.MsgU64  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgu64  arg:algo_lib.FMsgU64  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgI64  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgI64.len  arg:i64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgI64.len  extra:0  scale:8");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgI64.sub  arg:algo_lib.Sub  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgI64  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgI64.base  arg:algo_lib.MsgI64  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgi64  arg:algo_lib.FMsgI64  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgNeg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNeg.len  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgNeg.len  extra:-2  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNeg.sub  arg:algo_lib.Sub  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgNeg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgNeg.base  arg:algo_lib.MsgNeg  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgneg  arg:algo_lib.FMsgNeg  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgPos  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgPos.len  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgPos.len  extra:4  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgPos.sub  arg:algo_lib.Sub  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgPos  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgPos.base  arg:algo_lib.MsgPos  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgpos  arg:algo_lib.FMsgPos  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg3.len  arg:i32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg3.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg3.sub  arg:algo_lib.Sub  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsg3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsg3.base  arg:algo_lib.Msg3  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msg3  arg:algo_lib.FMsg3  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg6  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg6.len  arg:u16  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg6.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg6.sub  arg:algo_lib.Sub  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsg6  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsg6.base  arg:algo_lib.Msg6  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msg6  arg:algo_lib.FMsg6  reftype:Blkpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.basepool  field:algo_lib.FDb.msg6  base:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg5  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg5.len  arg:u16  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg5.len  extra:0  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg5.pad  arg:u16  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg5.sub  arg:algo_lib.Sub  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsg5  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsg5.base  arg:algo_lib.Msg5  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msg5  arg:algo_lib.FMsg5  reftype:Blkpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.basepool  field:algo_lib.FDb.msg5  base:algo_lib.FDb.malloc");
    atf_comp::ProcWriteEof(proc);
}

// Give amc two Bitfld lenflds whose stores mask to the declared width, and check
// that amc reports amc.lenfld_extra for both in one run, bounded by the
// width-limited max.
// MsgWidth (width 4, u32 source word, msgtype store, fixed size 20) stores at most
// 15, so the minimum frame's length word is unstorable.  MsgExtra (width 6,
// extra:252) stores at most 63, so no frame length is representable.  A check that
// bounded by the source word's full range would pass both.
void atf_comp::comptest_amc_BadLenfldBitfld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgWidth  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgWidth.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgWidth.len  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.MsgWidth.len  offset:0  width:4  srcfield:algo_lib.MsgWidth.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgWidth.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgWidth.p1  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgWidth.p2  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgWidth.p3  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgWidth.p4  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.MsgWidth  type:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgExtra  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgExtra.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgExtra.len  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.MsgExtra.len  offset:0  width:6  srcfield:algo_lib.MsgExtra.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgExtra.len  extra:252  scale:1");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a u8 lenfld with extra:256, and check that amc refuses the schema at
// generation time with amc.lenfld_extra.
// Every value the type can hold maps to a negative frame length, so no frame is
// representable.  Left unrejected, the TS encoder's range-guard bound
// maxval*scale - extra wraps in u64, and the emitted guard never fires.
void atf_comp::comptest_amc_BadLenfldExtra() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.len  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.pad  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg.len  extra:256  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msg  arg:algo_lib.Msg  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc two packed TypeScript ctypes whose fixed size plus lenfld extra falls
// outside the length field's range, with no msgtype and no C++ alloc instance, and
// check that amc rejects both in one run.
// The generated TypeScript Encode is their only store site.  MsgLow (u32 lenfld,
// extra:-8, fixed size 4) would wrap the minimal frame's stored length through
// setUint32.  MsgHigh (u8 lenfld, extra:255, fixed size 4) would get a range guard
// that throws on every encode.
void atf_comp::comptest_amc_BadLenfldTs() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgLow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgLow/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgLow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgLow.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgLow.len  extra:-8  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgHigh  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgHigh/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgHigh  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.len  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.pad1  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.pad2  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.pad3  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgHigh.len  extra:255  scale:1");
    atf_comp::ProcWriteEof(proc);
}

// Give amc packed TypeScript ctypes with fields of each wire form, run it with
// -out_dir:$tempdir, and check that amc names in one run every field that the
// TypeScript codec is unable to encode or decode, before it writes any file.
// MsgBad carries three such fields, which shows that the walk goes on past the
// first: an inlary of char and an inlary of a cstr ctype, both of which the class
// projects as `string` with no byte-copy form; and a Smallstr, a storage reftype
// that the codec maps nowhere.  MsgBadOpt adds the fourth, an Opt of the same cstr
// ctype, which reads the same byte-copy rule on the other reftype that uses it.
// MsgBadInlary adds the fifth, an inlary of u8 with min:0 max:8.  Its class
// projects as a byte array.  A variable inline array, though, stores a live
// element count after its elements.  So its width differs from max elements, and
// the count has no member to carry it.  MsgBadVal adds the sixth, a Val of a ctype
// that declares a Varlen.  The nested codec appends a tail past the width that the
// outer walk steps over.  MsgBadValOpt adds the seventh, the same rule read on a
// nested Opt tail.  MsgBadVarlen adds the eighth, a Varlen of the cstr ctype.  Its
// member projects as a string, while its tail is a run of records.  MsgTagged
// closes the list with the ninth and tenth, two inline arrays of char on a ctype
// whose only other field is a length word.
//
// MsgNolen and MsgNolenOpt read the other dimension, the ctype's own.  MsgNolen
// declares two Varlen fields and MsgNolenOpt an Opt, each with no length field to
// frame the tail.  Each draws amc.ts_lenfld.  MsgFixed is that rule's control, a
// packed TypeScript ctype with no tail and no length field, and it draws no
// report.
//
// The rest of the fixture is the accepted side of that table and draws no report.
// MsgOk carries a Base, a lenfld, a plain Val, a Bitfld, a Val of the cstr ctype (a
// fixed-length string does have a wire form), a Val of a tail-free ctype, an
// inlary of u8, and the three accepted Varlen args: char for the text tail, and u8
// and a plain ctype for the byte tail.  MsgOpt carries an Opt whose arg maps to a
// byte array.
// The two remaining ctypes fix the check's scope.  MsgUnpacked and MsgNojs both
// carry the offending inlary of char, and neither draws a wire refusal.
// MsgUnpacked has no pack row, so amc projects it as a type alone, where the
// inlary of char is a string.  MsgNojs has no ctypelang row, and amc projects it
// nowhere.
//
// $tempdir holds none of the gen directories that amc writes to.  So a generated
// file would report amc.outfile_write, naming the path that amc failed to create.
// The absence of that line is the assertion this run makes about ordering.
// check_ts sits ahead of every emitting gen.  So amc refuses the schema before the
// first write, and emits no decoder for the fields it accepted either.
void atf_comp::comptest_amc_BadTsWire() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir:$tempdir -report:N < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Str4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Str4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cstr  ctype:algo_lib.Str4  strequiv:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Str4.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Str4.ch  length:4  strtype:rpascal  pad:''  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Hdr  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Hdr  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Hdr.magic  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Tail  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Tail  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Tail.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Tail.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Tail.text  arg:char  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgOk  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgOk/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgOk  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.base  arg:algo_lib.Hdr  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgOk.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.flags  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.lo  arg:u8  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.MsgOk.lo  offset:0  width:4  srcfield:algo_lib.MsgOk.flags  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.str  arg:algo_lib.Str4  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.hdr  arg:algo_lib.Hdr  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.data  arg:u8  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgOk.data  min:4  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.text  arg:char  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.blob  arg:u8  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.rec  arg:algo_lib.Hdr  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgOpt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgOpt/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgOpt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOpt.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgOpt.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOpt.body  arg:algo_lib.Hdr  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBad  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgBad/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgBad  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBad.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBad.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBad.name  arg:char  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgBad.name  min:8  max:8  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBad.tag  arg:algo_lib.Str4  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgBad.tag  min:2  max:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBad.label  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.MsgBad.label  length:4  strtype:rpascal  pad:''  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBad.seq  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBadOpt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgBadOpt/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgBadOpt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadOpt.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBadOpt.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadOpt.body  arg:algo_lib.Str4  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBadInlary  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgBadInlary/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgBadInlary  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadInlary.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBadInlary.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadInlary.data  arg:u8  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgBadInlary.data  min:0  max:8  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadInlary.seq  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBadVal  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgBadVal/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgBadVal  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadVal.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBadVal.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadVal.inner  arg:algo_lib.Tail  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadVal.seq  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBadValOpt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgBadValOpt/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgBadValOpt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadValOpt.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBadValOpt.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadValOpt.inner  arg:algo_lib.MsgOpt  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadValOpt.seq  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBadVarlen  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgBadVarlen/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgBadVarlen  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadVarlen.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBadVarlen.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadVarlen.body  arg:algo_lib.Str4  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgNolen  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgNolen/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgNolen  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNolen.magic  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNolen.text  arg:char  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNolen.more  arg:char  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgNolenOpt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgNolenOpt/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgNolenOpt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNolenOpt.magic  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNolenOpt.body  arg:algo_lib.Hdr  reftype:Opt  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgFixed  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgFixed/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgFixed  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgFixed.magic  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgUnpacked  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgUnpacked/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgUnpacked.name  arg:char  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgUnpacked.name  min:8  max:8  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgNojs  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgNojs  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNojs.name  arg:char  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgNojs.name  min:8  max:8  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgTagged  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgTagged/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgTagged  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgTagged.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgTagged.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgTagged.tag  arg:char  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgTagged.tag  min:4  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgTagged.name  arg:char  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgTagged.name  min:8  max:8  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// -----------------------------------------------------------------------------

// Give amc three projected ctypes that no codec can write correctly, and check
// that amc refuses each by name and exits with one error per refusal.
//
// Msg, packed and projected into Rust, Go and Python, has a pointer member with no
// wire form, and draws amc.proj_wire.  The language emitters run after the checks
// and walk the same fields.  The Python emitter, handed the pointer, takes the
// process down with SIGSEGV.  The exit code is the assertion that no emitter ran.
// Three refusals exit 3, and a crash in any emitter exits 139.
//
// Lenmsg is a message whose length field is a bitfield of a header word, as in
// amc.LenfldBitfld.  Every codec stores the frame length at the length field's own
// slot.  A bitfield owns none, so a codec would write the length over the next
// member, and amc reports amc.proj_lenfld.
//
// Dflt carries two nested members with defaults of their own.  Wrap wraps one u64,
// so its -1 lands on that scalar and amc admits it.  Pair holds two scalars, so its
// default has no single scalar to land on, and amc reports amc.proj_dflt.
void atf_comp::comptest_amc_BadProj() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir:$tempdir -report:N < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.Msg/go  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.Msg/py  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.Msg/rs  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.magic  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.p  arg:algo_lib.Msg  reftype:Ptr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Hdr  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Hdr  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Hdr.type  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Hdr.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Hdr.len  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Hdr.len  offset:0  width:24  srcfield:algo_lib.Hdr.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.typefld  field:algo_lib.Hdr.type  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Hdr.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Lenmsg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.Lenmsg/rs  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Lenmsg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Lenmsg.base  arg:algo_lib.Hdr  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Lenmsg.data  arg:u8  reftype:Varlen  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.Lenmsg  type:5");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Pair  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Pair  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Pair.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Pair.b  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Wrap  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Wrap  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Wrap.value  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Dflt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.Dflt/go  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Dflt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Dflt.pair  arg:algo_lib.Pair  reftype:Val  dflt:7  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Dflt.wrap  arg:algo_lib.Wrap  reftype:Val  dflt:-1  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc two store-carrying ctypes whose fixed size plus lenfld extra falls
// outside the length field's range, and check that amc rejects both in one run.
// MsgLow (u32 lenfld, extra:-8, fixed size 4) stores a negative length for the
// minimum frame.  The value wraps through the unsigned store and frames gigabytes
// from a 4-byte message.  MsgHigh (u8 lenfld, extra:255, fixed size 4) stores 259,
// past the type's max.  The emitted range guard bound then falls below the fixed
// size, and every encode throws at first use.
void atf_comp::comptest_amc_BadLenfldMinFrame() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgLow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgLow.len  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgLow.len  extra:-8  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msglow  arg:algo_lib.MsgLow  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgHigh  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.len  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.pad1  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.pad2  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.pad3  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgHigh.len  extra:255  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msghigh  arg:algo_lib.MsgHigh  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc two packed TypeScript ctypes whose lenfld sits on a Bitfld field over a
// raw header word, with no msgtype and no C++ alloc instance, and check that amc
// accepts both with exit code 0.
// This is header-like framing for an external payload.  The TS fixed walk writes
// the source word raw from the parent, and emits no store for a Bitfld lenfld.  So
// under the no-store-site doctrine the length words arrive from outside, and the
// schemas stay legal.  MsgIndiv (size 4, scale:8) is indivisible, and MsgRange (u8
// lenfld, extra:252, size 4) has an unstorable minimum frame.  amc still accepts
// both.
void atf_comp::comptest_amc_LenfldBitfld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgIndiv  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgIndiv/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgIndiv  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgIndiv.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgIndiv.len  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.MsgIndiv.len  offset:8  width:24  srcfield:algo_lib.MsgIndiv.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgIndiv.len  extra:0  scale:8");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgRange  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelang  ctypelang:algo_lib.MsgRange/ts  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgRange  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgRange.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgRange.len  arg:u8  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.MsgRange.len  offset:0  width:8  srcfield:algo_lib.MsgRange.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgRange.len  extra:252  scale:1");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a packed message based on a private header ctype with no dmmeta.pack
// row, and check that amc reports amc.back_pack and amc.what_the_pack, each
// followed by the bare dmmeta.pack row that the message tells the author to
// insert.
// An author lands in this state after repointing a message's base from
// ams.MsgHeader to a freshly created header.  The namespace is itself pack:Y, so
// the unpacked header also draws the namespace-level error.
void atf_comp::comptest_amc_MissingPack() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ttt  nstype:protocol  license:GPL  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:ttt  genthrow:N  correct_getorcreate:N  pool:algo_lib.FDb.malloc  sortxref:N  pack:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:ttt  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:ttt.EventHeader  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ttt.EventHeader.type  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ttt.EventHeader.length  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.typefld  field:ttt.EventHeader.type  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:ttt.EventHeader.length  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:ttt.Event  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ttt.Event.base  arg:ttt.EventHeader  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:ttt.Event  type:3001");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:ttt.Event  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a pmask field of 536870913 bytes, and check that amc generates the
// schema without a diagnostic.
// The field's bit count exceeds i32, and the struct size cap (i32 with alignment
// headroom) permits that.  The schema is legal, so the bit-width bookkeeping must
// hold the count without wrapping.
void atf_comp::comptest_amc_PmaskGiantField() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.pmask  arg:u8  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.pmask  min:536870913  max:536870913  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.Rec.pmask  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f0  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f1  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f2  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f3  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f4  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f5  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f6  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f7  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f8  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc csize rows with alignment 32 and alignment 6, and check that amc
// reports amc.invalid_size for both.
// Alignment 32 exceeds the headroom that the struct size cap reserves for RoundUp.
// Alignment 6 is no power of two, so the rounding mask miscomputes.  Silent
// acceptance of either would ship a wrong layout.
void atf_comp::comptest_amc_BadCsizeAlignment() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Avx  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:algo_lib.Avx  size:64  alignment:32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Odd  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:algo_lib.Odd  size:12  alignment:6  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a variable char inlary and a variable u8 inlary with min>0, and check
// that amc reports amc.bad_inlary_min for both.
// The string read copies the input and sets the count to its length, and a short
// input puts that count below min.  amc rejects the shape until someone defines
// floor semantics for it.
void atf_comp::comptest_amc_BadInlaryMin() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestInlaryMin  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestInlaryMin.str  arg:char  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.TestInlaryMin.str  min:2  max:8  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestInlaryMin.buf  arg:u8  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.TestInlaryMin.buf  min:1  max:4  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc an inlary with min>max, and check that amc reports
// amc.bad_inlary_minmax.
// Init preallocates min elements against a capacity of max, so the first record
// constructed would die at run time.  The rejection keeps that trap out of the
// generated code.
void atf_comp::comptest_amc_BadInlaryMinmax() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestInlaryMinmax  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestInlaryMinmax.val  arg:u32  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.TestInlaryMinmax.val  min:5  max:3  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a variable inlary with fnoremove, and check that amc reports
// amc.bad_inlary_fnoremove.
// Setary, the separated-string read and Uninit all empty the array through
// RemoveAll, which fnoremove suppresses.  The generated code would then call a
// function that amc never emits.
void atf_comp::comptest_amc_BadInlaryFnoremove() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestInlary  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestInlary.val  arg:u32  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.TestInlary.val  min:0  max:3  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fnoremove  field:algo_lib.TestInlary.val  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc Json cfmts over fields that no FmtJson branch covers, and check that amc
// reports amc.badjsonfld for every offender in one run, through the accumulate
// path.
// The first cause is a reftype that the walk has no branch for, a raw Smallstr
// field.  The fixture shows it both as a ctype's single datafld and among several.
// The second cause is a field type with neither a Json cfmt nor a String print,
// which leaves no expression to produce its node.  The fixture shows it singular
// and repeated.
void atf_comp::comptest_amc_BadJsonFld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Name  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Name.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Name.ch  length:20  strtype:rpascal  pad:''  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Name.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.name  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Rec.name  length:20  strtype:rpascal  pad:''  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Rec.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Opaque  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Opaque.id  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RecVal  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecVal.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecVal.opaque  arg:algo_lib.Opaque  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RecVal.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RecAry  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecAry.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecAry.opaque  arg:algo_lib.Opaque  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.RecAry.opaque  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RecAry.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.InlOpaque  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.InlOpaque.v  arg:algo_lib.Opaque  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.InlOpaque.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.InlAryOpaque  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.InlAryOpaque.v  arg:algo_lib.Opaque  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.InlAryOpaque.v  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.InlAryOpaque.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc Json cfmts over fields of each kind, print every algo_lib FmtJson, and
// check how each field reaches its node.
// A field whose type carries its own Json cfmt gets that type's FmtJson.  A field
// whose type carries only a String cfmt prints into a string node.  A repeated
// field becomes an array node holding one element node per element, by the same
// two rules.  A ctype with one data field prints as that field's node, with no
// object wrapper.  The same rules pick the node there: the field type's FmtJson,
// or a string node that its Print writes into, and for a repeated field an array
// node holding one element node per element.
// Computed is the case where the two field selectors could disagree.  Its only
// data field is a Val, so amc elides the object wrapper and no object node exists.
// Its other field is a repeated fldfunc, which the array branch would emit an
// array node into.
// The remaining ctypes carry a print filter, which prints a field only when its
// bit is set.  PmFilter shows the three member forms, each inside a test of its
// bit.  Its presence mask is a data field itself, which alone keeps the ctype from
// inlining.  In PmInlJson and PmInlPrint the mask is a bitfield of the one data
// field.  That field carries a bit, so amc keeps it from inlining and prints it as
// an object member under its test, the first through its type's FmtJson and the
// second through its type's Print.  PmInlJson carries a second bitfield on the
// mask.  A bitfield prints no node, so amc opens no test around it.  PmInlFree
// keeps the same shape and names its member explicitly, which leaves the data
// field off the mask.  That field inlines as it would with no filter.
void atf_comp::comptest_amc_JsonFld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N algo_lib.%.FmtJson < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:u32.Json  printfmt:Extern  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Str  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Str.id  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Str.String  printfmt:Raw  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Multi  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Multi.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Multi.s  arg:algo_lib.Str  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Multi.ja  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.Multi.ja  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Multi.sa  arg:algo_lib.Str  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.Multi.sa  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Multi.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Computed  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Computed.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Computed.derived  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.Computed.derived  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cppfunc  field:algo_lib.Computed.derived  expr:''  print:N  set:N");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Computed.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.InlJson  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.InlJson.v  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.InlJson.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.InlPrint  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.InlPrint.v  arg:algo_lib.Str  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.InlPrint.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.InlAryJson  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.InlAryJson.v  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.InlAryJson.v  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.InlAryJson.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.InlAryPrint  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.InlAryPrint.v  arg:algo_lib.Str  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.InlAryPrint.v  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.InlAryPrint.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.PmFilter  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmFilter.a  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmFilter.s  arg:algo_lib.Str  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmFilter.ja  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.PmFilter.ja  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmFilter.pmask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.PmFilter.pmask  filter_print:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.PmFilter.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.PmInlJson  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlJson.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlJson.pm  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.PmInlJson.pm  offset:0  width:8  srcfield:algo_lib.PmInlJson.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.PmInlJson.pm  filter_print:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlJson.flag  arg:bool  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.PmInlJson.flag  offset:8  width:1  srcfield:algo_lib.PmInlJson.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.PmInlJson.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.PmInlPrint  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlPrint.word  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlPrint.pm  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.PmInlPrint.pm  offset:0  width:8  srcfield:algo_lib.PmInlPrint.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.PmInlPrint.pm  filter_print:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.PmInlPrint.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.PmInlFree  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlFree.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlFree.pm  arg:u32  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.PmInlFree.pm  offset:0  width:8  srcfield:algo_lib.PmInlFree.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.PmInlFree.pm  filter_print:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlFree.flag  arg:bool  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.PmInlFree.flag  offset:8  width:1  srcfield:algo_lib.PmInlFree.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.PmInlFree.pm/algo_lib.PmInlFree.flag  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.PmInlFree.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc a Json cfmt on a base-derived ctype and on a bitfield-carrying ctype,
// print both FmtJson bodies, and check that amc generates both schemas without a
// diagnostic.
// Neither the Base field nor the Bitfld field prints.  The base's cloned fields
// print as members of the object, and the bitfield's bits travel inside the
// printed source word.
void atf_comp::comptest_amc_JsonBaseBitfld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N %.FmtJson < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.BaseType  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.BaseType.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.BaseType.count  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Derived  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Derived.base  arg:algo_lib.BaseType  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Derived.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Flags  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Flags.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Flags.bit0  arg:bool  reftype:Bitfld  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Flags.bit0  offset:0  width:1  srcfield:algo_lib.Flags.value  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Flags.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc Json cfmts over the whole rendering table, print every FmtJson, and
// check how each field renders and that no field name collides with a local of
// the walk.
// A field renders through the Json cfmt of its type when the type has one.
// Otherwise it renders as a string that holds what its Print writes.  An array
// field renders as a JSON array whose elements render the same two ways.  A record
// that stands on a single data field renders as that field's own node.  So an
// array there is the record's array, and a value there is the record's value.
// AryPlain, AryElem, ValPlain and ValElem are the four single-field cells.  Rec
// holds the two array cells as members of an object beside a plain value.
// RecName is the naming control, and each of its fields bears the name of a local
// that the walk declares.  A local named after a field takes that name through one
// role suffix: "_node" for the node that stands for the field, and "_ary" for its
// element span.  The record's own node is named objnode, which no suffix
// produces.  So the field named "objnode" reaches for the record's node, the field
// named "x_field" for the node of the field named "x", and the field named "y_ary"
// for the element span of the field named "y".  The emitted body declares each of
// them once.  The field named "object" pins the same rule from the other side.
// The record's node goes by objnode, so a field may take the name "object".
void atf_comp::comptest_amc_JsonAry() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N %.FmtJson < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Elem  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Elem.n  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Elem.m  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Elem.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.AryPlain  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.AryPlain.buf  arg:u32  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.AryPlain.buf  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.AryPlain.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.AryElem  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.AryElem.rec  arg:algo_lib.Elem  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.AryElem.rec  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.AryElem.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.n  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.buf  arg:u32  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.buf  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.rec  arg:algo_lib.Elem  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.rec  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Rec.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.ValPlain  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.ValPlain.n  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.ValPlain.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.ValElem  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.ValElem.rec  arg:algo_lib.Elem  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.ValElem.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RecName  comment:\"field names that could collide with the walk's own locals\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecName.object  arg:u32  reftype:Inlary  dflt:''  comment:\"the object node's name, before it moved out of reach\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.RecName.object  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecName.objnode  arg:u32  reftype:Inlary  dflt:''  comment:\"the object node's name\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.RecName.objnode  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecName.x  arg:algo_lib.Elem  reftype:Val  dflt:''  comment:\"prints through its own node, named for this field\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecName.x_field  arg:u32  reftype:Inlary  dflt:''  comment:\"the name the field above once gave its node\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.RecName.x_field  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecName.y  arg:u32  reftype:Inlary  dflt:''  comment:\"its element span carries the name of the field below\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.RecName.y  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecName.y_ary  arg:u32  reftype:Inlary  dflt:''  comment:\"the name of the element span above\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.RecName.y_ary  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RecName.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc Json cfmts on global ctypes and on ctypes whose one data field carries
// no data, print every FmtJson, and check that each body holds the ctype's data
// fields and calls a global's accessors with no parent.
// The Global anchor field is the singleton instance itself and carries no data.
// Like Base and Bitfld, it passes the field check without printing, and the object
// holds the ctype's data fields.  Anchor, Derived and Presence carry that to the
// degenerate end.  Their one data field carries no data of its own: a Global, a
// Base and a presence mask.  So the walk emits no member, and each must still
// return an empty object and never a null node.  One ctype per cause keeps every
// arm of that rule load-bearing.
// The FDb ctype also carries the repeated fields, which reach their elements
// through the field's own Getary.  A global's accessors take no parent argument,
// so amc emits those calls with none.  The repeated fields are a Tary whose
// element type prints as a string, a Tary whose element type has a Json cfmt of
// its own, and an Inlary.  GlobAry is a second global ctype whose only data beside
// the anchor is one Tary, which keeps the collapse pinned apart from FDb's other
// members.  The golden pins every emitted FmtJson body in the fixture.
void atf_comp::comptest_amc_JsonGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N %.FmtJson < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.val  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Anchor  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Anchor._anchor  arg:algo_lib.Anchor  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Anchor.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Empty  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Derived  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Derived.base  arg:algo_lib.Empty  reftype:Base  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Derived.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Presence  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Presence.pmask  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.Presence.pmask  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Presence.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.ja  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.FDb.ja  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Elem  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Elem.v  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Elem.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.jb  arg:algo_lib.Elem  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.FDb.jb  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.GlobAry  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.GlobAry._globary  arg:algo_lib.GlobAry  reftype:Global  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.GlobAry.v  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.GlobAry.v  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.GlobAry.Json  printfmt:Auto  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.jc  arg:u32  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.jc  min:2  max:4  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give a single-file universe a Ptrary, an fcurs request for its cursor and no
// amcdb.tcurs rows, and check that amc reports amc.bad_fcurs and then
// amc.missing_tcurs and exits with their count.
// The request makes the cursor generator run.  The generator reports the missing
// tcurs row as a generation error and keeps generating.  check_fcurs flags the
// same request first, so the run accumulates both errors.
void atf_comp::comptest_amc_BadMissingTcurs() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.c_row  arg:algo_lib.FRow  reftype:Ptrary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ptrary  field:algo_lib.FDb.c_row  unique:Y  heaplike:N");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.c_row  inscond:false  via:''");
    atf_comp::ProcWrite(proc, "dmmeta.fcurs  fcurs:algo_lib.FDb.c_row/curs  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give a single-file universe a bitset field and no amcdb.tcurs rows, print the
// bitset's NBits and bitcurs functions, and check that amc emits NBits and no
// bitcurs cursor.
// The tcurs row is the only carrier of the dflt flag, so amc has no way to know
// that the bitcurs cursor is request-only.  Without a dmmeta.fcurs request the
// cursor stays unemitted.  The NBits witness proves that generation itself ran.
void atf_comp::comptest_amc_CursUnrequested() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.TestBitset.bits.NBits|algo_lib.TestBitset.bits_bitcurs.%' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestBitset  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.bits  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.bits  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc field defaults that name the containing instance, print the Init of
// Rec, Msg and Ptr, and check that each reference retargets to the emitting
// context's instance name, in both spellings that C++ gives that instance.
// The object spelling *this becomes the instance itself, so Msg Init assigns
// ssizeof(parent).  The pointer spelling this becomes its address, parenthesized,
// so that the member access in this->pad binds to the whole address expression.
// Ptr Init assigns (&parent)->pad.  An identifier that merely starts with the same
// characters (*thisvalue) refers to no instance and survives verbatim.  Rec's
// default carries the object spelling next to that longer identifier, and Ptr's
// carries the pointer spelling next to it.  So each pins that one expression keeps
// the longer identifier while amc retargets its real reference.
void atf_comp::comptest_amc_DfltRetarget() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N '(algo_lib.Rec..Init|algo_lib.Msg..Init|algo_lib.Ptr..Init)' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.val  arg:u32  reftype:Val  dflt:\"*thisvalue + ssizeof(*this)\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Ptr  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ptr.pad  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ptr.val  arg:u32  reftype:Val  dflt:\"u32(thisvalue + this->pad)\"  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.len  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.Msg  type:1");
    atf_comp::ProcWriteEof(proc);
}

// Run amc from a directory with no bin/acr, as outside a checkout, and check that
// amc reports amc.table_write and fails.
// The acr subprocess that rewrites the ctypelen, dispsig, tracefld, tracerec and
// userfunc tables fails to start.  A run that exited 0 would leave those tables
// silently unwritten.  The command cds into the test's tempdir, so the relative
// bin/acr path resolves nowhere.  It creates out's gen directories up front, so
// the output-file writes succeed and only the acr failure is under test.
void atf_comp::comptest_amc_TableWriteAcrFail() {
    atf_comp::FProc &proc = atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir -p out/cpp/gen out/include/gen && $$OLDPWD/$bindir/amc -in_dir:- -out_dir:out -report:N < <(cat $$OLDPWD/test/amc/bootstrap.ssim -)'");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.val  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.buf  arg:u32  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.buf  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Point amc at an -out_dir that is a regular file, and check that amc reports
// amc.outfile_write naming each path and fails.
// amc is unable to create the gen directories under a file, so every output-file
// write fails.  A run that exited 0 would leave the generated code silently
// missing.
void atf_comp::comptest_amc_OutfileWriteFail() {
    atf_comp::FProc &proc = atf_comp::ProcStart("bash -c 'cd $tempdir && touch out && $$OLDPWD/$bindir/amc -in_dir:- -out_dir:out -report:N < <(cat $$OLDPWD/test/amc/bootstrap.ssim -)'");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.val  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.buf  arg:u32  reftype:Inlary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.buf  min:0  max:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:N  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Run amc -e with EDITOR=false, so that its acr -e child exits 1, and check that
// amc reports amc.edit and fails loudly before regenerating anything.
// A run that read the nonzero exit as an edit that modified files would regenerate
// on top of a schema that never changed.  amc skips the phases that would read the
// input set, and the run ends through its own exit path.  The command cds into the
// test's tempdir and symlinks bin and data from the checkout.  amc's relative
// bin/acr spawn and acr's data load then both resolve, while every write stays in
// the tempdir.
void atf_comp::comptest_amc_EditFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && ln -s $$OLDPWD/bin bin && ln -s $$OLDPWD/data data && mkdir temp"
                        " && EDITOR=false $$OLDPWD/$bindir/amc ctype:amc.FGen -e'");
}

// Give a FAST template a presence mask named nullable, and check that the codec
// calls the presence accessors that the pmask rows generate:
// 1. Print Msg's FastEncode and FastDecode.  Check that the message's own presence
//    reads through Value_NullableQ and writes through nullable_qSetBitVal, and
//    that the state record's two masks, named assigned and present, keep the
//    accessors those names generate.
// 2. Rename the FastState presence mask to seen, and add a second template and a
//    namespace zz with a type-encoded cfast.  Check that amc reports
//    amc.fast_state once and exits 1.
// amc names the presence accessors after the pmask row.  The row's field name
// prefixes the whole-word bitset accessors, and its funcname spells the per-field
// ones.  A codec that spelled those names itself would work only for a mask named
// pmask.  The FAST codec qualifies each field's previous value through two pmasks
// of the namespace's FastState record, which it tells apart by name.  With the
// presence mask renamed, the codec has no accessor to call, and the diagnostic
// and the nonzero exit replace a silently unnamed accessor.  The missing row
// belongs to the namespace, so step 2 also pins how often amc charges it.  Two
// templates of the namespace need it, each through an encoder and a decoder, and
// amc reports it once.  The universe's other two namespaces draw no report.  One
// holds no cfast ctype.  The other (zz) holds one encoded as a type, which its
// holder codes inline and which needs no state pmask of its own.
void atf_comp::comptest_amc_FastPmaskName() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.Msg..Fast%' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FastState  comment:\"Value the peer last saw for each field, and the two pmasks qualifying it\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FastState.assigned  arg:u64  reftype:Val  dflt:''  comment:\"A value has been seen for the field\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.FastState.assigned  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FastState.present  arg:u64  reftype:Val  dflt:''  comment:\"The last value seen was present\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.FastState.present  filter_print:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FastState.tid  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FastState.Value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:\"FAST template whose presence mask is named nullable, not pmask\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Msg  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.id  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.nullable  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.Msg.nullable  filter_print:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.Value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfast  ctype:algo_lib.Msg  id:1  encoding:template  reset:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ffast  ffast:algo_lib.Msg.Value.field  name:Value  id:1  pos:1  optional:Y  encoding:unsigned  op:copy  value:''  comment:''");
    atf_comp::ProcWriteEof(proc);
    atf_comp::FProc &proc2 = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc2, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:''  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.ctype  ctype:algo_lib.FastState  comment:\"Value the peer last saw for each field, and the two pmasks qualifying it\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.FastState.assigned  arg:u64  reftype:Val  dflt:''  comment:\"A value has been seen for the field\"");
    atf_comp::ProcWrite(proc2, "dmmeta.pmaskfld  field:algo_lib.FastState.assigned  filter_print:N  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.FastState.seen  arg:u64  reftype:Val  dflt:''  comment:\"The last value seen was present\"");
    atf_comp::ProcWrite(proc2, "dmmeta.pmaskfld  field:algo_lib.FastState.seen  filter_print:Y  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.FastState.tid  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.FastState.Value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.ctype  ctype:algo_lib.Msg  comment:\"FAST template whose presence mask is named nullable, not pmask\"");
    atf_comp::ProcWrite(proc2, "dmmeta.pack  ctype:algo_lib.Msg  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.Msg.id  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.Msg.nullable  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.pmaskfld  field:algo_lib.Msg.nullable  filter_print:Y  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.Msg.Value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.cfast  ctype:algo_lib.Msg  id:1  encoding:template  reset:N  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.ffast  ffast:algo_lib.Msg.Value.field  name:Value  id:1  pos:1  optional:Y  encoding:unsigned  op:copy  value:''  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.ctype  ctype:algo_lib.Msg2  comment:\"Second FAST template of the same namespace, coded by the same pmask row\"");
    atf_comp::ProcWrite(proc2, "dmmeta.pack  ctype:algo_lib.Msg2  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.Msg2.id  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.Msg2.Value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.cfast  ctype:algo_lib.Msg2  id:2  encoding:template  reset:N  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.ffast  ffast:algo_lib.Msg2.Value.field  name:Value  id:1  pos:1  optional:N  encoding:unsigned  op:copy  value:''  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.ns  ns:zz  nstype:lib  license:GPL  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.nscpp  ns:zz  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.ctype  ctype:zz.Qty  comment:\"cfast type encoding: coded inline by its holder, so zz gets no codec\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:zz.Qty.value  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc2, "dmmeta.cfast  ctype:zz.Qty  id:0  encoding:type  reset:N  comment:''");
    atf_comp::ProcWriteEof(proc2);
}

// Give amc one ctype per argv strfmt over the same three fields, print the
// prototypes of their argv accessors, and check which accessors each strfmt
// generates.
// Both cfmts ask for print and read alike, so the strfmt in the cfmt key is the
// only difference between them.  Argv gets the whole surface: the two printers;
// the ToCmdline convenience, which prefixes bin/ and inherits the verbosity flags;
// the NArgs token-count table; and the ReadArgv parser that the namespace ReadArgv
// delegates to.  ArgvGnu gets the two printers alone, so its read:Y buys no
// command-line reader.  The test prints the prototypes alone, because what is
// under test is which functions exist.
void atf_comp::comptest_amc_ArgvAccessor() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N -proto -showcomment:N 'algo_lib.Amc..%Argv%|algo_lib.Amc..ToCmdline|algo_lib.Amc..NArgs|algo_lib.Gnu..%Argv%|algo_lib.Gnu..ToCmdline|algo_lib.Gnu..NArgs' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Gnu  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Gnu.c  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Gnu.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Gnu.tag  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.Gnu.tag  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Gnu.ArgvGnu  printfmt:Auto  read:Y  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Amc  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Amc.c  arg:u8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Amc.limit  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Amc.tag  arg:u32  reftype:Tary  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.Amc.tag  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Amc.Argv  printfmt:Auto  read:Y  print:Y  sep:''  genop:N  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc fconsts across stores of each width and sign, spelled in each form of a
// C++ integer literal, and check that amc reports amc.fconst_range by name for
// each value that the field's store fails to hold, stating the range that the
// store holds.
// The symbol reaches the field through the generated Set, which for a bitfield
// masks the store.  So the field would end up holding a value other than the one
// the schema names, and no accessor could map it back to the symbol.  The rejected
// values walk the stores: a value past the bits (2000 in ten bits); past the arg
// type (300 in a u8); past a signed arg type's maximum (2147483648 in an i32); a
// negative on an unsigned store; and a negative on bits that drop a signed arg
// type's sign.
// The value is a C++ integer literal, and amc reads it the way the compiler will.
// 0377 is octal 255 and fits a u8 store, while 0400 is 256 and exceeds it.  A
// u/U/l/L suffix names the literal's type and leaves its number unchanged.  A
// leading plus or minus is the number's sign.  amc carries sign and magnitude
// apart, so an unsigned 64-bit store holds 0xffffffffffffffff, while an i32 store
// rejects -0x80000001, one past its minimum.  A magnitude that outruns 64 bits,
// 0x10000000000000001 or twenty-three decimal digits, fits no store, and amc
// reports it against the widest one.
// The controls draw no report.  They are the values at each store's own edges: the
// u32, u8 and u64 maxima; the i32 minimum in decimal and in hex; 1023 in ten bits;
// 127 in seven; and -128 in eight, in decimal and in hex.  They also include every
// spelling that is no whole integer literal and so carries no number to compare: a
// shift expression, an arithmetic one, digits with trailing text, a character
// literal, a name only the compiler resolves, and a binary literal, a spelling
// that amc leaves unread.  A string field's value is a string however it is
// spelled.  So the string field carries one control reading csv and one reading
// 99999, and amc bounds neither.  The field's arg is a character array, which
// resolves to no integer type and has no range to compare with.  The mask of a
// flag on the top bit of a signed word is unbounded as well, because it is a
// bitfield's mask and no value of the word.
void atf_comp::comptest_amc_BadFconstRange() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sig8  comment:\"signed 8-bit value wrapped in a single field\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sig8.value  arg:i8  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Sig8.String  printfmt:Raw  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Str4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Str4.ch  arg:char  reftype:Smallstr  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Str4.ch  length:4  strtype:rpascal  pad:''  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.cstr  ctype:algo_lib.Str4  strequiv:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Str4.String  printfmt:Raw  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Str4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Rec.String  printfmt:Tuple  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.word  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.wide  arg:u32  reftype:Val  dflt:''  comment:\"store is the whole arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.wide/argmax  value:4294967295  comment:\"the arg type's own maximum fits\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.wide/shifted  value:\"1<<3\"  comment:\"not a plain integer: nothing to compare\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.wide/negative  value:-1  comment:\"an unsigned store holds no negative\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.byte  arg:u8  reftype:Val  dflt:''  comment:\"store is the whole arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.byte/argmax  value:255  comment:\"the arg type's own maximum fits\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.byte/past  value:300  comment:\"past the arg type's maximum\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.byte/plus  value:+255  comment:\"a leading plus belongs to the number\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.byte/suffix  value:255u  comment:\"a type suffix is not part of the number\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.byte/suffixpast  value:300ull  comment:\"past the maximum, suffix and all\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.byte/octal  value:0377  comment:\"a leading zero is octal: 255 fits\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.byte/octalpast  value:0400  comment:\"octal 400 is 256, past the maximum\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.byte/arith  value:\"1000-999\"  comment:\"an expression: nothing to compare\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.byte/shift  value:\"1<<40\"  comment:\"a shift expression: nothing to compare\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.byte/trail  value:300ms  comment:\"digits with trailing text: no number to compare\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.byte/char  value:\"'A'\"  comment:\"a character literal: nothing to compare\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.byte/name  value:UCHAR_MAX  comment:\"a name only the compiler resolves\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.byte/binary  value:0b100000000  comment:\"a binary literal is a spelling amc does not read\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.snum  arg:i32  reftype:Val  dflt:''  comment:\"store is the whole arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.snum/argmin  value:-2147483648  comment:\"the arg type's own minimum fits\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.snum/past  value:2147483648  comment:\"past the signed arg type's maximum\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.snum/hexmin  value:-0x80000000  comment:\"the minimum, spelled as a signed hex literal\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.snum/hexpast  value:-0x80000001  comment:\"one past the minimum, spelled in hex\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.full  arg:u64  reftype:Val  dflt:''  comment:\"store is the whole 64-bit arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.full/argmax  value:18446744073709551615  comment:\"the arg type's own maximum fits\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.full/highbit  value:0x8000000000000000  comment:\"past the signed maximum, and an unsigned store holds it\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.full/hexmax  value:0xffffffffffffffff  comment:\"the arg type's maximum, spelled in hex\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.full/hexwide  value:0x10000000000000001  comment:\"a magnitude past 64 bits fits no store\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.full/decwide  value:99999999999999999999999  comment:\"a decimal magnitude past 64 bits fits no store\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.narrow  arg:u32  reftype:Bitfld  dflt:''  comment:\"bits hold less than the arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.narrow  offset:0  width:10  srcfield:algo_lib.Rec.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.narrow/maxstore  value:1023  comment:\"the widest value the bits hold\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.narrow/past  value:2000  comment:\"past the bits, would be stored masked\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.sig_edge  arg:algo_lib.Sig8  reftype:Bitfld  dflt:''  comment:\"bits drop the arg type's sign\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.sig_edge  offset:10  width:7  srcfield:algo_lib.Rec.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.sig_edge/maxstore  value:127  comment:\"the widest value the bits hold\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.sig_edge/negative  value:-1  comment:\"the bits drop the sign, so no negative fits\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.sig_full  arg:algo_lib.Sig8  reftype:Bitfld  dflt:''  comment:\"bits span the whole arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.sig_full  offset:17  width:8  srcfield:algo_lib.Rec.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.sig_full/argmin  value:-128  comment:\"the bits keep the sign, so the arg minimum fits\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.sig_full/hexmin  value:-0x80  comment:\"the arg minimum, spelled as a signed hex literal\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.name  arg:algo_lib.Str4  reftype:Val  dflt:''  comment:\"a string field: its fconst value is a string\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.name/csv  value:csv  comment:\"a string, not a number to bound\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.name/digits  value:99999  comment:\"a string field's value is a string however it is spelled\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Flags  comment:\"a signed word carrying single-bit flags\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Flags.String  printfmt:Bitset  read:Y  print:Y  sep:,  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Flags.value  arg:i32  reftype:Val  dflt:''  comment:\"the word the flags source\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Flags.top  arg:bool  reftype:Bitfld  dflt:''  comment:\"a flag on the word's top bit\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Flags.top  offset:31  width:1  srcfield:algo_lib.Flags.value  comment:''");
    atf_comp::ProcWriteEof(proc);
}

// Give amc one bitfield per declared width, each with fconsts at and past its
// edge, and check that amc bounds each fconst by its field's width, reports the
// widths out of bounds, and ends the run at the first width the accessors refuse.
// A bitfield's Set masks the store to the declared width.  So amc builds the range
// that it checks an fconst value against by shifting a u64 by that width.  The
// width is a signed schema argument, and a run reaches this check before the
// accessors refuse the width.  So the width arrives holding whatever the schema
// wrote.  The widths from one to sixty-three name bits inside the word.  Each of
// those fields carries the widest value its bits hold, which draws no report,
// next to a value past it, which draws one.  A width of sixty-four spans the arg
// type and masks no bit, so the arg type's own maximum draws no report.  amc
// reports the three widths outside the range, negative, zero, and sixty-five bits
// of a u128 source field, as out of bounds, naming the field.  The value 1 on each
// of them draws no range complaint of its own, because a width that names no bits
// bounds no value.  A run that computed a bound from one of them would report 1 as
// a value the field fails to hold, though the field's arg type holds it well.  The
// bitfield accessors end the run at the first width they refuse, which is the last
// line of the golden.
void atf_comp::comptest_amc_FconstBitfldWidth() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i8  likeu64:Y  bigendok:N  issigned:Y  comment:\"8-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u8  likeu64:Y  bigendok:N  issigned:N  comment:\"8-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i16  likeu64:Y  bigendok:Y  issigned:Y  comment:\"16-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:Y  bigendok:Y  issigned:N  comment:\"16-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:\"32-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"32-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i64  likeu64:Y  bigendok:Y  issigned:Y  comment:\"64-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:\"64-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:Y  issigned:N  comment:\"128-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:bool  likeu64:Y  bigendok:N  issigned:N  comment:\"dflt is a c++ expr: real dflt is 'N'\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pad_byte  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:pad_byte  size:1  alignment:1  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:pad_byte.String  printfmt:Extern  read:Y  print:Y  sep:''  genop:Y  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:pad_byte  comment:''");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:char  likeu64:Y  bigendok:N  issigned:N  comment:char");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.word  arg:u32  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.dword  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.dword2  arg:u64  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.oword  arg:u128  reftype:Val  dflt:''  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.negw  arg:u32  reftype:Bitfld  dflt:''  comment:\"width below the range: names no bits\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.negw  offset:0  width:-64  srcfield:algo_lib.Rec.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.negw/one  value:1  comment:\"a bad width bounds nothing, so the value draws no range complaint\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.zerow  arg:u32  reftype:Bitfld  dflt:''  comment:\"width below the range: names no bits\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.zerow  offset:1  width:0  srcfield:algo_lib.Rec.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.zerow/one  value:1  comment:\"a bad width bounds nothing, so the value draws no range complaint\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.widew  arg:u128  reftype:Bitfld  dflt:''  comment:\"width above the range: more bits than a u64 mask holds\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.widew  offset:0  width:65  srcfield:algo_lib.Rec.oword  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.widew/one  value:1  comment:\"a bad width bounds nothing, so the value draws no range complaint\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.w1  arg:u32  reftype:Bitfld  dflt:''  comment:\"the narrowest width the range admits\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.w1  offset:2  width:1  srcfield:algo_lib.Rec.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.w1/maxstore  value:1  comment:\"the widest value one bit holds\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.w1/past  value:2  comment:\"past one bit, would be stored masked\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.w10  arg:u32  reftype:Bitfld  dflt:''  comment:\"bits hold less than the arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.w10  offset:3  width:10  srcfield:algo_lib.Rec.word  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.w10/maxstore  value:1023  comment:\"the widest value ten bits hold\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.w10/past  value:2000  comment:\"past ten bits, would be stored masked\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.w63  arg:u64  reftype:Bitfld  dflt:''  comment:\"the widest width the mask shift builds\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.w63  offset:0  width:63  srcfield:algo_lib.Rec.dword  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.w63/maxstore  value:0x7fffffffffffffff  comment:\"the widest value sixty-three bits hold\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.w63/past  value:0x8000000000000000  comment:\"past sixty-three bits, would be stored masked\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.w64  arg:u64  reftype:Bitfld  dflt:''  comment:\"width spans the whole arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.w64  offset:0  width:64  srcfield:algo_lib.Rec.dword2  comment:''");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.w64/argmax  value:0xffffffffffffffff  comment:\"the arg type's maximum, which sixty-four bits hold\"");
    atf_comp::ProcWriteEof(proc);
}
