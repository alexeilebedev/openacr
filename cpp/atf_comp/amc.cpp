// Copyright (C) 2026 AlgoRND
//
// License: GPL
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

// A field of reftype Ptrary without its dmmeta.ptrary record is an input
// error: amc reports amc.missing_ptrary and exits nonzero without writing
// any output.
void atf_comp::comptest_amc_MissingPtrary() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.c_row  arg:algo_lib.FRow  reftype:Ptrary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Ptrary.curs  dflt:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Ptrary.oncecurs  dflt:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// The universe is the schema closure the live tree declares as amc's own
// input, dumped by acr_in and read from stdin. Every other test in this file
// gives amc a fixture holding a handful of ctypes, which reaches only the
// generator modules those shapes need; this one generates every namespace the
// tree defines, and the empty -out_dir: keeps the run from writing any of it.
void atf_comp::comptest_amc_SchemaClosure() {
    atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N < <($bindir/acr_in amc -data)");
}

// A big-endian u16 field in a universe lacking the u16 csize row: the
// type's width is unknown, not unsupported, so the diagnostic names the
// missing dmmeta.csize row instead of prescribing a width change for a
// type whose width is already right.
void atf_comp::comptest_amc_BadBigendNosize() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:N  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.big  arg:u16  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.FDb.big  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A big-endian field of reftype Inlary on a ctype with a fieldwise
// constructor: the rejection reports and generation continues to the end
// of the run, accumulating any further schema errors into the exit code.
void atf_comp::comptest_amc_BadBigendReftype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:u16  ctor:N");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:N  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.Rec  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.big  arg:u16  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.big  min:4  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.Rec.big  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A big-endian u32 field whose bltin row withholds bigendok: the width has
// a byteswap primitive, but the type is not one the schema allows stored
// byteswapped, and the Val accessors would swap it anyway.  The check that
// reads the flag is separate from the width check, so it needs its own pin.
void atf_comp::comptest_amc_BadBigendDeadend() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:N  bigendok:N  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.big  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.FDb.big  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A big-endian u128 field whose bltin row claims bigendok: byteswap
// primitives exist for 16, 32, and 64 bits only, so the Val accessors would
// call a nonexistent be128toh/htobe128. The width check rejects the field
// independently of the bltin claim.
void atf_comp::comptest_amc_BadBigendU128() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.big  arg:u128  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.FDb.big  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Reftype Bitfld and the dmmeta.bitfld record imply each other, and the
// universe holds one field for each of the four combinations.  A Bitfld
// field without the record carries no bit offset or width, and a record on
// a field of another reftype gets a full-width member the ctype's own
// compare, print and read walks pass over.  Both are reported in one run;
// the correctly paired field and the plain Val fields draw nothing.
void atf_comp::comptest_amc_BadBitfldReftype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.flag  arg:bool  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.good  arg:bool  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.FDb.good  offset:0  width:1  srcfield:algo_lib.FDb.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.notbitfld  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.FDb.notbitfld  offset:8  width:8  srcfield:algo_lib.FDb.word  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A bitset over a 9-byte (72-bit) Smallstr element: not an unsigned integer
// builtin, so no exact bit index/shift split exists.
void atf_comp::comptest_amc_BadBitsetElem() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Str9  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Str9.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Str9.ch  length:9  strtype:rightpad  pad:\"' '\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:algo_lib.Str9  size:9  alignment:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestBitset  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.bits  arg:algo_lib.Str9  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.bits  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A bitset over an i8 element: a signed element compiles but sign-extends on
// read, so bit queries such as Sup return wrong values at runtime.
void atf_comp::comptest_amc_BadBitsetSigned() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestBitset  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.bits  arg:i8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.bits  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A bitset over a u32 element whose csize row is missing from the universe:
// the element type is fine; only its width is unknown. The diagnostic names
// the missing csize row rather than prescribing a different element type.
void atf_comp::comptest_amc_BadBitsetNosize() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestBitset  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.bits  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.bits  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Bitset elements whose csize row states a width other than the one the
// element type's own name states: the name passes the unsigned-builtin check
// in each case, while the accessors are generated from the csize row. A u32
// of 3 bytes is 24 bits, which the FloorLog2 index split truncates; a u128 of
// 32 bytes is 256 bits, past the widest bit-scan helper; a u16 of 8 bytes is
// 64 bits, a width the accessors can index perfectly well but not the width
// of the member the struct declares, so the shifts run off its end. The u64
// and u8 elements of the same universe, whose csize rows agree with their
// names, draw nothing and stand as the controls.
void atf_comp::comptest_amc_BadBitsetWidth() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:8  alignment:8  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:3  alignment:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:32  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestBitset  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.bits  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.bits  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.wide  arg:u128  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.wide  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.mid  arg:u16  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.mid  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.ok  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.ok  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.small  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.small  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Fixed-inlary field defaults that name the record by the Init parameter
// name in a shape the copy-constructor inliner's rewrite does not cover.
// One field names it bare (sizeof parent), which inlining would leak into
// the member copy constructor as an undeclared identifier. The other reaches
// it through a member operator (gcfg.parent.n), where the name belongs to
// another record and the member-access rewrite would take the operator with
// it, turning the default into a different expression that still compiles.
// Both are reported and the run exits nonzero.
void atf_comp::comptest_amc_BadCopyctorInit() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.grow  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.grow  min:1  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.fix  arg:u32  reftype:Inlary  dflt:\"u32(sizeof parent)\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.fix  min:4  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.mem  arg:u32  reftype:Inlary  dflt:\"u32(gcfg.parent.n)\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.mem  min:4  max:4  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// The shapes the copy-constructor inliner does cover, pinned through the
// emitted constructor body. A variable inlary's Init macro carries the two
// shapes the generators emit -- member access on the record, and the record
// passed to a function by reference -- and both are retargeted to *this. A
// fixed inlary's default is arbitrary user text: a default naming the record
// as *this arrives at the inliner spelled as the Init parameter and returns
// to *this, a default naming no record at all is copied verbatim, and so is
// one naming a longer identifier that merely ends with the parameter name
// (gparent.n), which is a different name and not a reference to the record.
// The remaining defaults carry the parameter name where it is not an
// identifier at all: inside a string literal, inside a block comment, and
// inside a raw string literal whose payload carries the quote that ends an
// ordinary one. Each is content rather than code, so each is copied into the
// constructor verbatim and none is reported. A character literal holding a
// quote is followed by a reference to the record, since a default whose
// literal is its last content pins nothing: the scan would read the quote as
// opening a string literal, run out of text and emit the same body. With the
// reference behind it, that misreading loses the reference and leaves the
// parameter name in the constructor, where it is undeclared.
void atf_comp::comptest_amc_CopyctorInit() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N algo_lib.Rec..CopyCtor < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.grow  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.grow  min:1  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.fix  arg:u32  reftype:Inlary  dflt:\"u32(gparent.n)\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.fix  min:4  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.size  arg:u32  reftype:Inlary  dflt:\"u32(ssizeof(*this))\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.size  min:4  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.lit  arg:u32  reftype:Inlary  dflt:\"7\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.lit  min:4  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.str  arg:u32  reftype:Inlary  dflt:'u32(sizeof(\"parent\"))'  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.str  min:4  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.note  arg:u32  reftype:Inlary  dflt:\"u32(1 /* parent */)\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.note  min:4  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.chr  arg:u32  reftype:Inlary  dflt:\"u32('\\\"') + u32(sizeof(parent))\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.chr  min:4  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.raw  arg:u32  reftype:Inlary  dflt:'u32(sizeof(R\"(a \"parent\" b)\"))'  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.raw  min:4  max:4  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Which bitfield reads bound the value by the field's store, over one universe
// holding a field per combination of arg type and width. A read parses into a
// temporary of the field's arg type and the Bitfld setter masks the store, so a
// value the bits cannot hold is stored truncated unless the reader refuses it
// first. The refusal is emitted for a field that carries a symbolic constant,
// whose reader tries the symbol and falls back to the arg type, and each end of
// the arg type's range draws a term there exactly when the field's bits do not
// reach that end. At the high end narrow (u32 in 3 bits) and qos (a u32 wrapped
// in one field, in 4 bits) draw a bound, while full (u8 in 8 bits), signed_full
// (i8 in 8 bits) and flag (bool in 1 bit) span their whole arg type and draw
// none. At the low end a store that drops the arg type's sign bit reads a
// negative back as a positive number: sig (an i32 wrapped in one field, in 4
// bits) refuses a negative on top of its high-end bound, and sig_edge (an i8
// wrapper in 7 bits), whose bits hold every positive value its arg type holds,
// refuses one with no high-end bound at all, while sig_full (the same wrapper in
// 8 bits) keeps the sign and draws neither term. plain is narrow again with no
// symbolic constant on it, and its reader carries no term at all: three bits
// take a "9" as 1 and the read reports success. An fdec field carries its bound
// inside the loops its reader parses with: dec scales two decimal places into 12
// bits, so the digit loop takes a digit only while the value it is building still
// fits 4095 -- the field's own store, not the u64 the digits accumulate into --
// and the loop filling the decimal places a short text leaves out tests the same
// bound. The finished magnitude is tested against nothing, every digit that went
// into it having been bounded already. One combination never reaches a reader, rejected at generation
// time: a width past the arg type. A narrower width over a signed arg type is
// rejected too, but only in a universe that carries the amcdb.bltin table, and
// this fixture carries none. Signedness has two sources: the range machinery
// reads it from the type's name, so i8 and i32 are signed here, while the size
// pass reads the type's amcdb.bltin row and finds none, so its
// amc.bitfield_signed_narrow never fires. That is why sig and sig_edge reach a
// reader in this golden -- adding the table makes amc report both of them and
// generate nothing at all. The name-based source is the deliberate one: a
// signed integer named by a universe that lacks the type's amcdb.bltin row
// still gets signed range checks, and this fixture is that universe.
void atf_comp::comptest_amc_BitfldReadRange() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.Rec.(narrow|full|signed_full|flag|qos|sig|sig_edge|sig_full|dec|plain).ReadStrptrMaybe' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Qos  comment:\"value wrapped in a single field\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Qos.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Qos.String  printfmt:Raw  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Rec.String  printfmt:Tuple  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.value  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.narrow  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"arg range exceeds the field's bits\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.narrow  offset:0  width:3  srcfield:algo_lib.Rec.value  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.narrow/zero  value:0  comment:\"a symbol puts the read on the path that bounds the value\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.full  arg:u8  reftype:Bitfld  dflt:\"\"  comment:\"field spans the whole arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.full  offset:3  width:8  srcfield:algo_lib.Rec.value  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.full/zero  value:0  comment:\"same path, and the bits reach every value the arg type holds\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.signed_full  arg:i8  reftype:Bitfld  dflt:\"\"  comment:\"signed arg, whole width\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.signed_full  offset:11  width:8  srcfield:algo_lib.Rec.value  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.signed_full/zero  value:0  comment:\"same path, and the bits keep the sign\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.flag  arg:bool  reftype:Bitfld  dflt:\"\"  comment:\"bool holds no value past one bit\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.flag  offset:19  width:1  srcfield:algo_lib.Rec.value  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.flag/zero  value:0  comment:\"same path, and one bit is the whole of a bool\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.qos  arg:algo_lib.Qos  reftype:Bitfld  dflt:\"\"  comment:\"wrapped value, arg range exceeds the bits\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.qos  offset:20  width:4  srcfield:algo_lib.Rec.value  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.qos/zero  value:0  comment:\"same path, through the field the wrapper stores into\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sig  comment:\"signed value wrapped in a single field\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sig.value  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Sig.String  printfmt:Raw  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.sig  arg:algo_lib.Sig  reftype:Bitfld  dflt:\"\"  comment:\"wrapped signed value, narrowed by the bits\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.sig  offset:24  width:4  srcfield:algo_lib.Rec.value  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.sig/zero  value:0  comment:\"same path, and the bits drop both ends of the wrapped range\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.dec  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"decimal value scaled into the field's bits\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.dec  offset:28  width:12  srcfield:algo_lib.Rec.value  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.Rec.dec  nplace:2  fixedfmt:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sig8  comment:\"signed 8-bit value wrapped in a single field\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sig8.value  arg:i8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Sig8.String  printfmt:Raw  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.sig_edge  arg:algo_lib.Sig8  reftype:Bitfld  dflt:\"\"  comment:\"wrapped signed value, bits hold its whole positive half\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.sig_edge  offset:40  width:7  srcfield:algo_lib.Rec.value  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.sig_edge/zero  value:0  comment:\"same path, and the bits drop the sign alone\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.sig_full  arg:algo_lib.Sig8  reftype:Bitfld  dflt:\"\"  comment:\"wrapped signed value, bits span the whole arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.sig_full  offset:47  width:8  srcfield:algo_lib.Rec.value  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.sig_full/zero  value:0  comment:\"same path, and the bits span the wrapped arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.plain  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"narrow again, with no symbol to read first\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.plain  offset:55  width:3  srcfield:algo_lib.Rec.value  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A cascdel Ptrary whose row ctype has no instance pool: with nothing to
// delete a row from, DeleteExpr would be empty and the Cascdel loop would
// spin forever. Pins the existing rejection (amc.nopool on the xref, then
// the cascdel pool vrfy) so the loop stays unreachable.
void atf_comp::comptest_amc_BadCascdelNopool() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.c_row  arg:algo_lib.FRow  reftype:Ptrary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ptrary  field:algo_lib.FDb.c_row  unique:Y  heaplike:N");
    atf_comp::ProcWrite(proc, "dmmeta.cascdel  field:algo_lib.FDb.c_row  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Ptrary.curs  dflt:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Ptrary.oncecurs  dflt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.c_row  inscond:false  via:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A cascdel unique:Y Ptrary whose field has no dmmeta.xref record: the
// generated Cascdel deletes the last row until the array is empty, and only
// an xref makes the row's delete unlink it, so the loop would never
// terminate.
void atf_comp::comptest_amc_BadCascdelXref() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.c_row  arg:algo_lib.FRow  reftype:Ptrary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ptrary  field:algo_lib.FDb.c_row  unique:Y  heaplike:N");
    atf_comp::ProcWrite(proc, "dmmeta.cascdel  field:algo_lib.FDb.c_row  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Ptrary.curs  dflt:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Ptrary.oncecurs  dflt:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A buffer condition on a global (FDb) fbuf: the record that enters the list
// is the buffer's parent row, and a global has no row to enter it with.
void atf_comp::comptest_amc_BadFbufCondGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:cd  circular:Y  haveprev:Y  instail:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuftype  fbuftype:Bytebuf  skipbytes:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbufdir  fbufdir:in  read:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbufdir  fbufdir:out  read:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.delcurs  dflt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FIohook  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FIohook.fildes  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.cd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.cd_ready  havetail:N  havecount:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.cd_ready  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcond  fcond:algo_lib.FDb.in/ready  ins:algo_lib.FDb.cd_ready  via:\"\"  rem:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A print:Y cfmt whose (strfmt, printfmt) pair has no print path would ship a
// Print function with an empty body, so it is rejected and the diagnostic
// names both attributes. Two ctypes are rejected, one per axis: the Tuple
// strfmt, for which no print path exists under any printfmt, and the
// CompactSep printfmt under the String strfmt, which the String arm does not
// handle. The controls are the whole rest of the table and draw no
// diagnostic: String with Tuple, Auto, Raw, Sep and Bitset; Argv and Json,
// which take any printfmt because neither reads one; the Extern printfmt,
// which hands the body to the user even under the Tuple strfmt; and a print:N
// cfmt carrying the rejected printfmt, which generates no print function at
// all.
void atf_comp::comptest_amc_BadCfmtPrint() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sample  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sample.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Sample.Tuple  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Compact  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Compact.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Compact.b  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Compact.String  printfmt:CompactSep  read:N  print:Y  sep:,  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Quiet  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Quiet.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Quiet.String  printfmt:CompactSep  read:N  print:N  sep:,  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CTuple  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CTuple.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CTuple.String  printfmt:Tuple  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CAuto  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CAuto.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CAuto.String  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CRaw  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CRaw.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CRaw.String  printfmt:Raw  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CSep  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CSep.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CSep.b  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CSep.String  printfmt:Sep  read:N  print:Y  sep::  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CBitset  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CBitset.a  arg:bool  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CBitset.b  arg:bool  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CBitset.String  printfmt:Bitset  read:N  print:Y  sep:,  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CArgv  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CArgv.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CArgv.Argv  printfmt:CompactSep  read:N  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CJson  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CJson.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CJson.b  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:u32.Json  printfmt:Extern  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CJson.Json  printfmt:CompactSep  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CExtern  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CExtern.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CExtern.Tuple  printfmt:Extern  read:N  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A read:Y cfmt whose (strfmt, printfmt) pair has no read path would ship a
// ReadStrptrMaybe with an empty body. Such a reader returns success for every
// input and leaves the destination holding whatever it held before the call,
// so no caller can tell a parsed value from an unparsed one. The read emitter
// rejects the cfmt instead and names both attributes. The fixture is the whole
// table the emitter decides. Rejected on one axis: the CompactSep printfmt
// under the String strfmt, the one printfmt the String arm has no reader for.
// The controls are the rest of the table and draw no diagnostic: String with
// Tuple, Auto, Raw, Sep, Bitset and Extern, Auto reading as a tuple the way
// the print side prints one; the Tuple strfmt, whose reader consumes an
// already-parsed tuple and so is chosen without consulting the printfmt, which
// is why it accepts CompactSep and still hands Extern to the user; the Json
// strfmt, for which the read emitter draws nothing at all; and a read:N cfmt
// carrying the rejected printfmt, which generates no read function to be empty.
void atf_comp::comptest_amc_BadCfmtRead() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RCompact  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RCompact.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RCompact.b  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RCompact.String  printfmt:CompactSep  read:Y  print:N  sep:,  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RQuiet  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RQuiet.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RQuiet.String  printfmt:CompactSep  read:N  print:N  sep:,  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RTuple  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RTuple.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RTuple.String  printfmt:Tuple  read:Y  print:N  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RAuto  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RAuto.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RAuto.String  printfmt:Auto  read:Y  print:N  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RRaw  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RRaw.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RRaw.String  printfmt:Raw  read:Y  print:N  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RSep  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RSep.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RSep.b  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RSep.String  printfmt:Sep  read:Y  print:N  sep::  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RBitset  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RBitset.a  arg:bool  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RBitset.b  arg:bool  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RBitset.String  printfmt:Bitset  read:Y  print:N  sep:,  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RExtern  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RExtern.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RExtern.String  printfmt:Extern  read:Y  print:N  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RTupfmt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RTupfmt.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RTupfmt.Tuple  printfmt:CompactSep  read:Y  print:N  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RTupxtern  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RTupxtern.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RTupxtern.Tuple  printfmt:Extern  read:Y  print:N  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RJson  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RJson.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RJson.Json  printfmt:CompactSep  read:Y  print:N  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A String cfmt asking for printfmt:CompactSep: the printfmt table declares
// the value, but amc generates neither a printer nor a reader for it, so the
// request would ship an empty print body and an empty read body. Both emitters
// reject it, each naming the cfmt, the strfmt and the offending printfmt, and
// the run still reaches its exit with a nonzero code. The two diagnostics of
// one cfmt are what this golden pins; the wider table each emitter decides is
// pinned by BadCfmtPrint and BadCfmtRead. The Sep ctype beside it is the
// control: the printfmt amc does generate draws no diagnostic on either side.
void atf_comp::comptest_amc_BadCompactSep() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Ok  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ok.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ok.b  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Ok.String  printfmt:Sep  read:Y  print:Y  sep:\":\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Compact  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Compact.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Compact.b  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Compact.String  printfmt:CompactSep  read:Y  print:Y  sep:\":\"  genop:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// The field-aware command-line reader is assembled from companion functions,
// and each companion is emitted under a gate of its own: the reader and NArgs
// come from a read cfmt with strfmt:Argv, ReadFieldMaybe and GetAnon from a read
// cfmt whose printfmt is neither Raw nor Extern, and the namespace-level ReadArgv
// is a call to the cmdline ctype's reader. The fixture is the table of shapes
// those gates decide. Accepted, drawing no diagnostic: command.good (Argv read:Y
// printfmt Auto over a base that supplies both companions), command.goodnobase
// (no basecmdline, so no base companion is called), command.goodraw (its Argv
// cfmt is Raw, but a second read cfmt supplies ReadFieldMaybe and GetAnon for its
// anonfld), and namespace okapp over command.good. Rejected: command.raw and
// command.xtern, whose only read cfmt is Raw or Extern, so the store and the
// positional lookup are missing; command.basenoread and command.baseraw, whose
// base is missing NArgs or the store; algo_lib.CmdlineRaw, which is a base and a
// broken reader at once; and namespaces noreadapp and gnuapp, whose cmdline ctype
// yields no reader at all -- one because its Argv cfmt is read:N, the other
// because ArgvGnu generates no reader however it is spelled.
void atf_comp::comptest_amc_BadArgvRead() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:command  nstype:lib  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:command  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:okapp  nstype:lib  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:okapp  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:noreadapp  nstype:lib  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:noreadapp  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:gnuapp  nstype:lib  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:gnuapp  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Cmdline  comment:\"base command line that generates both companions the reader calls\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmdline.help  arg:bool  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmdline.verbose  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Cmdline.Argv  printfmt:Auto  read:Y  print:N  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CmdlineNoread  comment:\"base command line whose Argv cfmt is read:N: no NArgs\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CmdlineNoread.help  arg:bool  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CmdlineNoread.Argv  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CmdlineRaw  comment:\"base command line whose only read cfmt is Raw: no ReadFieldMaybe\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CmdlineRaw.help  arg:bool  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.CmdlineRaw.Argv  printfmt:Raw  read:Y  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.cmdline  arg:algo_lib.Cmdline  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.cmdline_noread  arg:algo_lib.CmdlineNoread  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.cmdline_raw  arg:algo_lib.CmdlineRaw  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.good  comment:\"accept: Argv read:Y, printfmt Auto, base generates both companions\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.good.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.good.Argv  printfmt:Auto  read:Y  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.good  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.goodnobase  comment:\"accept: no basecmdline, so the reader calls no base companion\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.goodnobase.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.goodnobase.Argv  printfmt:Auto  read:Y  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.goodnobase  read:Y  basecmdline:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.goodraw  comment:\"accept: the Argv cfmt is Raw but the String cfmt supplies ReadFieldMaybe and GetAnon\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.goodraw.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.goodraw.name  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.anonfld  field:command.goodraw.name  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.goodraw.Argv  printfmt:Raw  read:Y  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.goodraw.String  printfmt:Tuple  read:Y  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.goodraw  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.raw  comment:\"reject: the only read cfmt is Raw, so neither ReadFieldMaybe nor GetAnon\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.raw.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.raw.name  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.anonfld  field:command.raw.name  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.raw.Argv  printfmt:Raw  read:Y  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.raw  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.xtern  comment:\"reject: the only read cfmt is Extern, so no ReadFieldMaybe\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.xtern.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.xtern.Argv  printfmt:Extern  read:Y  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.xtern  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.basenoread  comment:\"reject: the base's Argv cfmt is read:N, so no base NArgs\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.basenoread.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.basenoread.Argv  printfmt:Auto  read:Y  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.basenoread  read:Y  basecmdline:algo_lib.FDb.cmdline_noread  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.baseraw  comment:\"reject: the base's only read cfmt is Raw, so no base ReadFieldMaybe\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.baseraw.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.baseraw.Argv  printfmt:Auto  read:Y  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.baseraw  read:Y  basecmdline:algo_lib.FDb.cmdline_raw  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.noread  comment:\"reject at the namespace: Argv cfmt is read:N, so no command.noread_ReadArgv\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.noread.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.noread.Argv  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.noread  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.gnu  comment:\"reject at the namespace: only an ArgvGnu cfmt, which generates no reader\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.gnu.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:command.gnu.ArgvGnu  printfmt:Auto  read:Y  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.gnu  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:okapp.FDb  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:okapp.FDb._db  arg:okapp.FDb  reftype:Global  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:okapp.FDb.cmdline  arg:command.good  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:okapp  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:noreadapp.FDb  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:noreadapp.FDb._db  arg:noreadapp.FDb  reftype:Global  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:noreadapp.FDb.cmdline  arg:command.noread  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:noreadapp  ismodule:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:gnuapp.FDb  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:gnuapp.FDb._db  arg:gnuapp.FDb  reftype:Global  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:gnuapp.FDb.cmdline  arg:command.gnu  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.main  ns:gnuapp  ismodule:N");
    atf_comp::ProcWriteEof(proc);
}

// A Bytebuf or Linebuf measures its message in bytes and hands it back as an
// element count, so a wider element makes the returned message run past the
// data. The element's width comes from the declared csize of a bltin and from
// the emitted struct body of a generated ctype, so the fixture crosses both
// buffer types with both width sources: the rejected rows are a byte buffer of
// u16, of u64, of an eight-byte generated struct and of an extern struct of
// unknown width, and a line buffer of u32; the accepted controls are both
// buffer types over a one-byte bltin and over a one-byte generated struct, and
// a Msgbuf, which carries its length in the message header, over the
// eight-byte struct.
// A one-byte element is not enough for a Linebuf, whose generated ScanMsg finds
// the end of the message by comparing each element with the delimiter, so the
// same fixture crosses the sources of an operator == with the shapes that emit
// no comparison at all:
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
// The Bytebuf over the same ccmp-less struct stays accepted throughout: it
// returns every available byte and compares nothing.
void atf_comp::comptest_amc_BadFbufElem() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i8  likeu64:Y  bigendok:N  issigned:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u8  likeu64:Y  bigendok:N  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u8  extrn:N  genop:N  order:Y  minmax:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i16  likeu64:Y  bigendok:Y  issigned:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i64  likeu64:Y  bigendok:Y  issigned:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:N  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:bool  likeu64:Y  bigendok:N  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:char  likeu64:Y  bigendok:N  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:cd  circular:Y  haveprev:Y  instail:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuftype  fbuftype:Bytebuf  skipbytes:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbufdir  fbufdir:in  read:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbufdir  fbufdir:out  read:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.delcurs  dflt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuftype  fbuftype:Linebuf  skipbytes:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuftype  fbuftype:Msgbuf  skipbytes:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FIohook  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FIohook.fildes  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Elem  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Elem.value  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Eqelem  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Eqelem.value  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo_lib.Eqelem  extrn:N  genop:Y  order:N  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Extrnelem  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Extrnelem.value  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo_lib.Extrnelem  extrn:Y  genop:N  order:N  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Cmpelem  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmpelem.value  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo_lib.Cmpelem  extrn:N  genop:N  order:Y  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Opaque  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cextern  ctype:algo_lib.Opaque  initmemset:N  isstruct:Y  plaindata:N");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_byte  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_byte  max:64  fbuftype:Bytebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_char  arg:char  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_char  max:64  fbuftype:Bytebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_line  arg:char  reftype:Fbuf  dflt:\"'\\\\n'\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_line  max:64  fbuftype:Linebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.out_msg  arg:algo_lib.FRow  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.out_msg  max:64  fbuftype:Msgbuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_wide  arg:u16  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_wide  max:64  fbuftype:Bytebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_word  arg:u64  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_word  max:64  fbuftype:Bytebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_rec  arg:algo_lib.FRow  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_rec  max:64  fbuftype:Bytebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_lwide  arg:u32  reftype:Fbuf  dflt:\"32\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_lwide  max:64  fbuftype:Linebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_elem  arg:algo_lib.Elem  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_elem  max:64  fbuftype:Bytebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_eline  arg:algo_lib.Elem  reftype:Fbuf  dflt:\"algo_lib::Elem()\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_eline  max:64  fbuftype:Linebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_eqline  arg:algo_lib.Eqelem  reftype:Fbuf  dflt:\"algo_lib::Eqelem()\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_eqline  max:64  fbuftype:Linebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_xline  arg:algo_lib.Extrnelem  reftype:Fbuf  dflt:\"algo_lib::Extrnelem()\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_xline  max:64  fbuftype:Linebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_cmpline  arg:algo_lib.Cmpelem  reftype:Fbuf  dflt:\"algo_lib::Cmpelem()\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_cmpline  max:64  fbuftype:Linebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_scanline  arg:algo_lib.Elem  reftype:Fbuf  dflt:\"algo_lib::Elem()\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_scanline  max:64  fbuftype:Linebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ffunc  ffunc:algo_lib.FDb.in_scanline.ScanMsg  extrn:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.out_eline  arg:algo_lib.Elem  reftype:Fbuf  dflt:\"algo_lib::Elem()\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.out_eline  max:64  fbuftype:Linebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.in_opaque  arg:algo_lib.Opaque  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FDb.in_opaque  max:64  fbuftype:Bytebuf  iotype:nofd  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// The element of a Varlen or an Opt field is addressed in place inside the
// message that carries it: the message allocates nothing for it, and destroying
// the message ends no lifetime of its own. An element type that owns a
// destructor would therefore be constructed and never destroyed, which leaks
// whatever the element holds, so the schema is refused at generation time.
// Whether an element type owns a destructor is not a property of the field. It
// is a property of the ctype, computed for the whole ctype graph by the pass
// that also decides whether a value is plain data, so this check reads a flag a
// different pass fills in. Run before that pass, it reads the flag's initial
// value, finds every element type destructor-free, and passes a schema it was
// written to refuse. That is why the fixture carries the accepted rows next to
// the refused ones: a check that has gone quiet reports nothing on either.
//
//   field reftype | element type       | accepted
//
//   Varlen        | has destructor     | no
//   Opt           | has destructor     | no
//   Varlen        | plain data         | yes
//   Opt           | plain data         | yes
//   Val           | has destructor     | yes
//
// The element that owns a destructor is an extern ctype declared plaindata:N,
// which is the shortest way to say that destroying it does something; the plain
// element is a packed ctype of one integer field. The Val row is the control on
// the rule's other edge: that reftype gives its member a lifetime of its own, so
// the destructor is called and the element type is unconstrained.
void atf_comp::comptest_amc_BadOptDtor() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i8  likeu64:Y  bigendok:N  issigned:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u8  likeu64:Y  bigendok:N  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u8  extrn:N  genop:N  order:Y  minmax:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i16  likeu64:Y  bigendok:Y  issigned:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i64  likeu64:Y  bigendok:Y  issigned:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:N  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:bool  likeu64:Y  bigendok:N  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:char  likeu64:Y  bigendok:N  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Opaque  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cextern  ctype:algo_lib.Opaque  initmemset:N  isstruct:Y  plaindata:N");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:algo_lib.Opaque  size:8  alignment:8  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.PlainElem  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.PlainElem  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PlainElem.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgVarlenDtor  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgVarlenDtor  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgVarlenDtor.head  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgVarlenDtor.tail  arg:algo_lib.Opaque  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgOptDtor  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgOptDtor  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOptDtor.head  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOptDtor.tail  arg:algo_lib.Opaque  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgVarlenPlain  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgVarlenPlain  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgVarlenPlain.head  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgVarlenPlain.tail  arg:algo_lib.PlainElem  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgOptPlain  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgOptPlain  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOptPlain.head  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOptPlain.tail  arg:algo_lib.PlainElem  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgValDtor  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgValDtor  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgValDtor.body  arg:algo_lib.Opaque  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A condition's ins index is inserted into by name, one row at a time, so the
// index has to be one a single row can be put into and taken out of. A hash is
// the tempting mistake: it holds the same rows and its name reads like a list's,
// but it offers InsertMaybe rather than Insert, so the emitted call does not
// compile. The rule states the requirement positively -- an Llist or a Bheap --
// which refuses in one test every other shape a field can have: a pool and a
// pointer array, which have no single-row remove; a count, which keeps a tally
// and hands no row back; and a tree or a sorted list, whose insert takes more
// than the row.
// The fixture's universe therefore carries an FDb index of each of those
// reftypes, and its rejected row names the hash. Only that one is named per
// run: the fcond rules are vrfy checks, so the first violation ends the run, and
// a fixture crossing every reftype would report the same single line. The
// accepted control is a condition on the sibling buffer naming an Llist of the
// watched ctype, which draws no diagnostic.
void atf_comp::comptest_amc_BadFcondIns() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u64  extrn:N  genop:N  order:Y  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:cd  circular:Y  haveprev:Y  instail:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuftype  fbuftype:Bytebuf  skipbytes:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbufdir  fbufdir:in  read:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbufdir  fbufdir:out  read:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.delcurs  dflt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FIohook  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FIohook.fildes  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.cd_ready  arg:algo_lib.FRow  reftype:Llist  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.cd_ready  havetail:N  havecount:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.cd_ready  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FConn  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.connid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.conn  arg:algo_lib.FConn  reftype:Lary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.cd_conn_read  arg:algo_lib.FConn  reftype:Llist  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.cd_conn_read  havetail:N  havecount:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.cd_conn_read  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.c_conn_read  arg:algo_lib.FConn  reftype:Ptrary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ptrary  field:algo_lib.FDb.c_conn_read  unique:Y  heaplike:N");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.c_conn_read  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.ind_conn  arg:algo_lib.FConn  reftype:Thash  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.thash  field:algo_lib.FDb.ind_conn  hashfld:algo_lib.FConn.connid  unique:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.ind_conn  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.n_conn  arg:algo_lib.FConn  reftype:Count  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.bh_conn  arg:algo_lib.FConn  reftype:Bheap  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.sortfld  field:algo_lib.FDb.bh_conn  sortfld:algo_lib.FConn.connid");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.bh_conn  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.tr_conn  arg:algo_lib.FConn  reftype:Atree  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.sortfld  field:algo_lib.FDb.tr_conn  sortfld:algo_lib.FConn.connid");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.tr_conn  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zs_conn  arg:algo_lib.FConn  reftype:ZSListMT  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zs_conn  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.p_conn  arg:algo_lib.FConn  reftype:Upptr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.cd_sub  arg:algo_lib.FRow  reftype:Llist  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FConn.cd_sub  havetail:N  havecount:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FConn.cd_sub  inscond:true  via:algo_lib.FRow.p_conn");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:lib_other  nstype:lib  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:lib_other  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:lib_other.FDb  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:lib_other.FDb._db  arg:lib_other.FDb  reftype:Global  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:lib_other.FConn  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:lib_other.FConn.rowid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:lib_other.FDb.conn  arg:lib_other.FConn  reftype:Lary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:lib_other.FDb.cd_conn_read  arg:lib_other.FConn  reftype:Llist  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:lib_other.FDb.cd_conn_read  havetail:N  havecount:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:lib_other.FDb.cd_conn_read  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:lib_other.FDb.cd_conn_eof  arg:lib_other.FConn  reftype:Llist  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:lib_other.FDb.cd_conn_eof  havetail:N  havecount:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:lib_other.FDb.cd_conn_eof  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.out  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.out  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_ns  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_ns  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_eof  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_eof  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_elem  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_elem  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_par  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_par  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_pary  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_pary  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_pool  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_pool  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_hash  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_hash  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_count  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_count  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_bheap  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_bheap  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_atree  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_atree  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_zslist  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_zslist  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_self  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_self  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FConn.in_eofself  arg:u8  reftype:Fbuf  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbuf  field:algo_lib.FConn.in_eofself  max:64  fbuftype:Bytebuf  iotype:standard  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcond  fcond:algo_lib.FConn.in/ready  ins:algo_lib.FDb.cd_conn_read  via:\"\"  rem:Y  comment:\"accepted control: an Llist of the watched ctype\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcond  fcond:algo_lib.FConn.in_hash/ready  ins:algo_lib.FDb.ind_conn  via:\"\"  rem:Y  comment:\"rejected: a hash offers InsertMaybe, not Insert\"");
    atf_comp::ProcWriteEof(proc);
}

// A TimeHookOnce fstep on an Llist field: the step reads the expiration time
// from the first row's sort field, which only a Bheap step field provides.
void atf_comp::comptest_amc_BadFstepBheap() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.delcurs  dflt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:zd  circular:N  haveprev:Y  instail:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookOnce  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_row  arg:algo_lib.FRow  reftype:Llist  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:algo_lib.FDb.zd_row  havetail:Y  havecount:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zd_row  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.zd_row  steptype:TimeHookOnce  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A TimeHookRecur fstep on an Upptr field: the time hook is armed from the
// index's first row, which only an Llist or Bheap step field provides.
void atf_comp::comptest_amc_BadFstepFirst() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookRecur  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.p_row  arg:algo_lib.FRow  reftype:Upptr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.p_row  steptype:TimeHookRecur  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A TimeHookRecur fstep on an Atree field: the hook is armed by FirstChanged
// calls, which only the Llist and Bheap generators emit -- an Atree step
// would compile and never fire.
void atf_comp::comptest_amc_BadFstepAtree() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u64  extrn:N  genop:N  order:Y  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:TimeHookRecur  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.tr_row  arg:algo_lib.FRow  reftype:Atree  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.sortfld  field:algo_lib.FDb.tr_row  sortfld:algo_lib.FRow.rowid");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.tr_row  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.tr_row  steptype:TimeHookRecur  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An fstep on a fixed Inlary field: the step's loop condition tests the
// index with EmptyQ, which generates only for a variable Inlary -- a fixed
// one always holds max elements and offers no emptiness test.
void atf_comp::comptest_amc_BadFstepInlary() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.fixary  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.fixary  min:3  max:3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.fixary  steptype:Inline  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An InlineRecur fstep with fdelay scale:Y on a Val field: the scaled delay
// divides by the step index's row count, which a Val field does not have.
void atf_comp::comptest_amc_BadFstepScale() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:InlineRecur  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.rowcheck  arg:bool  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.rowcheck  steptype:InlineRecur  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdelay  fstep:algo_lib.FDb.rowcheck  delay:1.000000000  scale:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A Callback fstep on a ZSListMT field: the list is concurrent and has no
// EmptyQ, so the step's loop condition tests DestructiveFirst, which only
// the Inline and InlineRecur call shapes embed.
void atf_comp::comptest_amc_BadFstepZslistmt() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Callback  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zs_row  arg:algo_lib.FRow  reftype:ZSListMT  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.zs_row  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.zs_row  steptype:Callback  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An fdelay row on an Inline fstep: the delay between invocations is read
// only by the InlineRecur and TimeHookRecur call shapes, so on any other
// steptype the row configures nothing.
void atf_comp::comptest_amc_BadFstepFdelay() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.rowcheck  arg:bool  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.rowcheck  steptype:Inline  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdelay  fstep:algo_lib.FDb.rowcheck  delay:1.000000000  scale:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An fstep on an Lpool field: the step's loop condition tests the field
// for pending work, and a memory pool offers no emptiness test, so the
// generated condition would call an EmptyQ that does not exist.
void atf_comp::comptest_amc_BadFstepReftype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.sbrk  arg:u8  reftype:Sbrk  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.lpool  arg:u8  reftype:Lpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.basepool  field:algo_lib.FDb.lpool  base:algo_lib.FDb.sbrk");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.lpool  steptype:Inline  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// InlineRecur fsteps with fdelay scale:Y on a Blkhash and on a Lary field:
// both reftypes generate the N function the scaled delay divides by, so the
// schema generates cleanly.
void atf_comp::comptest_amc_FstepScaleBlkhash() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u32  extrn:N  genop:N  order:N  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RowKey  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RowKey.id  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RowKey.seq  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.chash  ctype:algo_lib.RowKey  hashtype:CRC32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo_lib.RowKey  extrn:N  genop:Y  order:N  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowkey  arg:algo_lib.RowKey  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.ind_row  arg:algo_lib.FRow  reftype:Blkhash  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.blkhash  field:algo_lib.FDb.ind_row  hashfld:algo_lib.FRow.rowkey  linfld:algo_lib.RowKey.seq  linbits:12  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.ind_row  inscond:true  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:InlineRecur  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.ind_row  steptype:InlineRecur  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdelay  fstep:algo_lib.FDb.ind_row  delay:1.000000000  scale:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FLrow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FLrow.value  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.lrow  arg:algo_lib.FLrow  reftype:Lary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FDb.lrow  steptype:InlineRecur  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdelay  fstep:algo_lib.FDb.lrow  delay:1.000000000  scale:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An fstep on a field of a non-global ctype: the generated step would read
// the field through the namespace global, a member FDb does not have.
void atf_comp::comptest_amc_BadFstepGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.steptype  steptype:Inline  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fstep  fstep:algo_lib.FRow.rowid  steptype:Inline  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A dispatch filter over a message field that has its own Print function
// (an fconst field): the filter's regx match prints the field through that
// function, passing the message as the parent argument. Pins the emitted
// Match body, including the well-formed Print call.
void atf_comp::comptest_amc_DispfilterFieldPrint() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N algo_lib.DispFilter..MatchEv < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Regx  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Regx.expr  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Ev  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ev.state  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Ev.state/idle  value:0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Ev.state/busy  value:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DispFilter  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DispFilter.state_regx  arg:algo_lib.Regx  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.dispatch  dispatch:algo_lib.Disp  unk:N  read:N  print:N  haslen:N  call:N  strict:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.dispatch_msg  dispatch_msg:algo_lib.Disp/algo_lib.Ev  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.dispfilter  dispatch:algo_lib.Disp  match_all:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A dispatch filter declared match_all:Y, whose two messages between them cover
// every shape the rule has to place. Ev carries both filter fields, so its Match
// body reads the message: the equality and the range tests take the big-endian
// field through its Get function, and the regx match prints the message's own
// field through its fconst Print function before matching. Ev2 carries neither,
// so every filter field of Ev's turns into a present test that refuses the
// message, which is what match_all means. Ev2's own field is the control on the
// other side, and Ev's present tests for it are the same refusal in reverse.
void atf_comp::comptest_amc_DispfilterMatchAll() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.DispFilter..Match%' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Regx  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Regx.expr  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Ev  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ev.state  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Ev.state/idle  value:0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Ev.state/busy  value:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ev.seqno  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.Ev.seqno  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Ev2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ev2.other  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DispFilter  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DispFilter.state_regx  arg:algo_lib.Regx  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.dispatch  dispatch:algo_lib.Disp  unk:N  read:N  print:N  haslen:N  call:N  strict:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.dispatch_msg  dispatch_msg:algo_lib.Disp/algo_lib.Ev  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.dispatch_msg  dispatch_msg:algo_lib.Disp/algo_lib.Ev2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.dispfilter  dispatch:algo_lib.Disp  match_all:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A gconst value table whose fourth and fifth lines are not tuples: each
// carries an unterminated quoted value. The reader keeps what it read before
// the quote ran out and jumps to end of line, so the truncated value would
// read back as a constant name the table never wrote, with every attribute
// past the bad quote -- the comment among them -- dropped. Both lines are
// reported with the file, the line number and the line's text, none of which
// the tuple reader itself supplies. The query prints the resulting enum, so
// the controls are pinned too: a blank line and a comment line draw no
// diagnostic, the two well-formed rows still emit, and the auto-numbering
// consumes no index for a line that produced no constant.
void atf_comp::comptest_amc_BadGconstBadline() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/bad_gconst_badline -out_dir: -report:N algo_lib.ColorId");
}

// A gconst value table in which the same name appears twice: the second row
// would silently vanish from the enum while still consuming a numbering
// index, gapping the numbering on a clean exit.
void atf_comp::comptest_amc_BadGconstDup() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/bad_gconst_dup -out_dir: -report:N zzz");
}

// A gconst value table containing a line with a head but no attribute: the
// line creates no constant, yet auto-numbering would consume an index for
// it, silently renumbering every constant that follows.
void atf_comp::comptest_amc_BadGconstHeadonly() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/bad_gconst_headonly -out_dir: -report:N zzz");
}

// A gconst whose idfld names an existing field of a ctype other than the
// namefld's: the field is absent from the value table's rows, so every
// fconst value would come out empty.
void atf_comp::comptest_amc_BadGconstCtype() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/bad_gconst_ctype -out_dir: -report:N zzz");
}

// A gconst whose idfld names no existing field: the id column must not
// silently fall back to the name column.
void atf_comp::comptest_amc_BadGconstIdfld() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/bad_gconst_idfld -out_dir: -report:N zzz");
}

// Two gconsts with a bad idfld, the first on a char-typed field: the run
// reports both and exits, instead of processing the known-bad row and
// aborting the scan on a context-free char-parse failure.
void atf_comp::comptest_amc_BadGconstCtypeChar() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/bad_gconst_ctype_char -out_dir: -report:N zzz");
}

// A char-typed gconst whose value table holds multi-character values: each
// offending row is reported with its gconst, name and value, and the scan
// continues to the end of the table.  The query prints the resulting enum,
// so the table's one valid row is pinned too: it survives the skipped
// neighbors and emits as a single-quoted character constant.
void atf_comp::comptest_amc_BadGconstChar() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/bad_gconst_char -out_dir: -report:N algo_lib.ColorId");
}

// A gconst over a value table that exists only inside the fixture universe:
// the values side-load from the -in_dir root, not from data/ under the CWD,
// and no amc.sideload notice appears for a directory universe.
void atf_comp::comptest_amc_GconstIndir() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/gconst_indir -out_dir: -report:N zzz");
}

// A gconst whose value table is absent from the -in_dir root: the table's rows
// are the enum's constants, so loading it as zero rows would compile an empty
// enum and exit 0. The run reports the file and exits nonzero.
void atf_comp::comptest_amc_GconstLoadFail() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/gconst_missing -out_dir: -report:N zzz");
}

// A gconst and a gstatic whose table has no name at all: the ctype the rows
// would come from reaches no ssimfile, so there is no file to look for. This is
// the other way a table can be absent, and it is reported apart from a named
// file that cannot be read, because the reader can say nothing about a file it
// was never given -- amc.GconstLoadFail and amc.GstaticLoadFail are that case,
// and their ctypes reach a ssimfile of their own and through a base
// respectively, which is what makes them the controls for this one.
// A ctype reaches no ssimfile two ways, and the fixture holds both for each
// consumer: it declares no base at all, or a base that owns no ssimfile either.
// Both name the offending gconst or gstatic field and let the run continue, so
// one run reports all four rows rather than ending at the first.
void atf_comp::comptest_amc_SideloadNossimfile() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i8  likeu64:Y  bigendok:N  issigned:Y  comment:\"8-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u8  likeu64:Y  bigendok:N  issigned:N  comment:\"8-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i16  likeu64:Y  bigendok:Y  issigned:Y  comment:\"16-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:Y  bigendok:Y  issigned:N  comment:\"16-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:\"32-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"32-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i64  likeu64:Y  bigendok:Y  issigned:Y  comment:\"64-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:\"64-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:Y  issigned:N  comment:\"128-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:bool  likeu64:Y  bigendok:N  issigned:N  comment:\"dflt is a c++ expr: real dflt is 'N'\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pad_byte  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:pad_byte  size:1  alignment:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:pad_byte.String  printfmt:Extern  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:pad_byte  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:char  likeu64:Y  bigendok:N  issigned:N  comment:char");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Free  comment:\"reaches no ssimfile and declares no base\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Free.name  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Derived  comment:\"reaches no ssimfile through a base that has none either\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Derived.base  arg:algo_lib.Free  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Derived.dname  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FreeId  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FreeId.value  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.gconst  field:algo_lib.FreeId.value  namefld:algo_lib.Free.name  idfld:\"\"  wantenum:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DerivedId  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DerivedId.value  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.gconst  field:algo_lib.DerivedId.value  namefld:algo_lib.Derived.dname  idfld:\"\"  wantenum:Y");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.free  arg:algo_lib.Free  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.free  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.gstatic  field:algo_lib.FDb.free  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.derived  arg:algo_lib.Derived  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.derived  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.gstatic  field:algo_lib.FDb.derived  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A gstatic field whose table file is absent from the -in_dir root: the
// rows would become the compiled-in registry, so loading the table as
// zero rows would silently drop every entry. The run reports the file
// and exits nonzero.
void atf_comp::comptest_amc_GstaticLoadFail() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/gstatic_missing -out_dir: -report:N zzz");
}

// A gstatic table whose second line is not a tuple: an unterminated quoted
// value. The line would silently drop out of the compiled-in registry, so the
// run reports it and exits nonzero. The diagnostic names the field that was
// reading, the file, the line number and the line's text, none of which the
// tuple reader itself supplies. The query prints the loader the table compiles
// into, so the two well-formed lines around the bad one are pinned as well: they
// come back as the loader's own rows, which is how a run that abandoned the
// table at the first bad line would show itself. The universe also carries an
// fconst whose value is a character literal, so an earlier pass has left a
// bad-number tag naming that valid value in algo_lib's shared error buffer: a
// message taken from there would name the fconst instead of the file that is
// actually malformed.
void atf_comp::comptest_amc_GstaticBadLine() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/gstatic_badline -out_dir: -report:N algo_lib.FDb.color.LoadStatic");
}

// A gsymbol table whose fourth and fifth lines are not tuples: each carries an
// unterminated quoted value. The truncated text would become a symbol name and
// value the table never wrote, so every reference to the intended symbol fails
// to link. Both lines are reported with the file, the line number and the
// line's text, and the run exits nonzero.
void atf_comp::comptest_amc_GsymbolBadline() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/gsymbol_badline -out_dir: -report:N zzz");
}

// A gsymbol whose table is absent from the -in_dir root: the table's rows are
// the symbols the namespace exports, so emitting an empty symbol block and
// exiting 0 would leave every reference to them unresolved at link time. The
// run reports the file and exits nonzero.
void atf_comp::comptest_amc_GsymbolLoadFail() {
    atf_comp::ProcStart("$bindir/amc -in_dir:test/amc/gsymbol_missing -out_dir: -report:N zzz");
}

// A gsymbol in a single-file universe: with no directory root the symbol
// table side-loads from the default data/ root of the current directory,
// which may belong to a different universe than the -in_dir schema; the
// amc.sideload notice names the fallback root and the file read from it.
void atf_comp::comptest_amc_GsymbolSideload() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:ssimdb  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:dmmeta  nstype:ssimdb  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:dmmeta.Nstype  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.Nstype.nstype  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ssimfile  ssimfile:dmmeta.nstype  ctype:dmmeta.Nstype");
    atf_comp::ProcWrite(proc, "dmmeta.gsymbol  gsymbol:algo_lib/dmmeta.nstype  inc:%  symboltype:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An Llist field without its dmmeta.llist record: reported per field, then
// the run stops at the reftype gate, since later gen phases dereference the
// per-reftype records this phase found missing.
void atf_comp::comptest_amc_MissingLlist() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.zd_row  arg:algo_lib.FRow  reftype:Llist  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A ccmp on a global ctype: there is exactly one instance, and the generated
// Cmp/Lt bodies would silently compare the singleton with itself (a global
// field's Get accessor takes no parent argument).
void atf_comp::comptest_amc_BadCcmpGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u32  extrn:N  genop:N  order:N  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u64  extrn:N  genop:N  order:N  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.seqno  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.FDb.seqno  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo_lib.FDb  extrn:N  genop:N  order:Y  minmax:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A chash on a global ctype: same rule as ccmp -- the generated Hash body
// would silently hash the singleton's own fields whatever argument is passed.
void atf_comp::comptest_amc_BadChashGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.seqno  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.FDb.seqno  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.hashtype  hashtype:CRC32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.chash  ctype:algo_lib.FDb  hashtype:CRC32  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An fcmp on a global ctype's field: same rule as ccmp, one level down --
// both operands of the generated Cmp/Lt body would collapse to the
// singleton's value and the function would compare the field with itself.
void atf_comp::comptest_amc_BadFcmpGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u32  extrn:N  genop:N  order:N  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u64  extrn:N  genop:N  order:N  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.seqno  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.FDb.seqno  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcmp  field:algo_lib.FDb.seqno  versionsort:N  casesens:Y  extrn:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A ctype with both a Tpool and a Global instance, hashed by an FDb Thash:
// the pooled rows would reach field accessors that take no parent and read
// the global, so the hash would compare the singleton with itself. The
// Global instance must be the ctype's only instance.
void atf_comp::comptest_amc_BadGlobalInst() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u32  extrn:N  genop:N  order:N  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.seqno  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.FRow.seqno  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.grow  arg:algo_lib.FRow  reftype:Global  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.ind_row  arg:algo_lib.FRow  reftype:Thash  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.thash  field:algo_lib.FDb.ind_row  hashfld:algo_lib.FRow.seqno  unique:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.ind_row  inscond:true  via:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Thash.curs  dflt:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A minmax:Y ccmp on a non-builtin ctype without genop:Y order:Y: the
// Min/Max functions compare with a raw <, which no generated or native
// operator provides.
void atf_comp::comptest_amc_BadMinmax() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u32  extrn:N  genop:N  order:N  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestMinmax  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestMinmax.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo_lib.TestMinmax  extrn:N  genop:N  order:N  minmax:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// minmax:Y ccmp on a builtin (native operator <) and on an extrn:Y ctype
// (user-defined comparison), neither with order:Y genop:Y: both provide the
// operator < the Min/Max bodies compare with, so the schema generates
// cleanly.
void atf_comp::comptest_amc_MinmaxNative() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u32  extrn:N  genop:N  order:N  minmax:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestExtrn  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestExtrn.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo_lib.TestExtrn  extrn:Y  genop:N  order:N  minmax:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// The ctype-named GetScale convenience exists only for a ctype with a
// single fdec field; with two fdec fields the definitions would collide.
// Pins that the one-fdec ctype keeps it and the two-fdec ctype has none.
void atf_comp::comptest_amc_FdecGetScale() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.Price.value.GetScale|algo_lib.Quote.bid.GetScale|algo_lib.Quote.ask.GetScale' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Price  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Price.value  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.Price.value  nplace:2  fixedfmt:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Quote  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Quote.bid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.Quote.bid  nplace:2  fixedfmt:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Quote.ask  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.Quote.ask  nplace:4  fixedfmt:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A query match in a namespace with no dmmeta.nscpp row reports
// amc.query_nocpp and counts toward the run's exit code, because the namespace
// emits no C++ and the query has nothing to print for that record.
// The first invocation matches two ctypes at once, one in a namespace that has
// the row (algo_lib.CppWidget) and one in a namespace that does not
// (ncns.Widget): the ncns match is reported, the algo_lib match still generates
// and prints its struct, and the run exits 1 for the one report.
// The second invocation queries testdb, an ssimdb namespace projected to
// TypeScript alone, under the default key, so both query walkers run: the two
// ctypes and the seven generated funcs each draw a report and the run exits 9.
// The third and fourth invocations discard stderr. Query mode's stdout carries
// the generated C++ and nothing else, so a consumer that wants code alone drops
// stderr, and for a query wholly inside a nocpp namespace its stdout is empty.
// The fourth query spans a nocpp namespace and a code-generating one, the case
// that tells the two streams apart: its stdout holds the generated function and
// none of the nine reports the same run writes to stderr.
void atf_comp::comptest_amc_QueryNocpp() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'ctype:algo_lib.CppWidget|ncns.Widget' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:none  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:ssimdb  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ncns  nstype:none  license:GPL  comment:\"Namespace with no dmmeta.nscpp row: emits no C++\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:testdb  nstype:ssimdb  license:GPL  comment:\"ssimdb namespace projected to TypeScript alone: emits no C++\"");
    atf_comp::ProcWrite(proc, "dmmeta.nsjs  ns:testdb  typescript:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CppWidget  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CppWidget.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:ncns.Widget  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ncns.Widget.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:testdb.App  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nossimfile  ctype:testdb.App  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:testdb.App.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
    atf_comp::FProc &proc2 = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N testdb.% < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc2, "dmmeta.nstype  nstype:none  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.nstype  nstype:ssimdb  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.ns  ns:ncns  nstype:none  license:GPL  comment:\"Namespace with no dmmeta.nscpp row: emits no C++\"");
    atf_comp::ProcWrite(proc2, "dmmeta.ns  ns:testdb  nstype:ssimdb  license:GPL  comment:\"ssimdb namespace projected to TypeScript alone: emits no C++\"");
    atf_comp::ProcWrite(proc2, "dmmeta.nsjs  ns:testdb  typescript:Y  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.ctype  ctype:algo_lib.CppWidget  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.CppWidget.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.ctype  ctype:ncns.Widget  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:ncns.Widget.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.ctype  ctype:testdb.App  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.nossimfile  ctype:testdb.App  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:testdb.App.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc2);
    atf_comp::FProc &proc3 = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N testdb.% 2>/dev/null < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc3, "dmmeta.nstype  nstype:none  comment:\"\"");
    atf_comp::ProcWrite(proc3, "dmmeta.nstype  nstype:ssimdb  comment:\"\"");
    atf_comp::ProcWrite(proc3, "dmmeta.ns  ns:ncns  nstype:none  license:GPL  comment:\"Namespace with no dmmeta.nscpp row: emits no C++\"");
    atf_comp::ProcWrite(proc3, "dmmeta.ns  ns:testdb  nstype:ssimdb  license:GPL  comment:\"ssimdb namespace projected to TypeScript alone: emits no C++\"");
    atf_comp::ProcWrite(proc3, "dmmeta.nsjs  ns:testdb  typescript:Y  comment:\"\"");
    atf_comp::ProcWrite(proc3, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc3, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc3, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc3, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc3, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc3, "dmmeta.ctype  ctype:algo_lib.CppWidget  comment:\"\"");
    atf_comp::ProcWrite(proc3, "dmmeta.field  field:algo_lib.CppWidget.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc3, "dmmeta.ctype  ctype:ncns.Widget  comment:\"\"");
    atf_comp::ProcWrite(proc3, "dmmeta.field  field:ncns.Widget.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc3, "dmmeta.ctype  ctype:testdb.App  comment:\"\"");
    atf_comp::ProcWrite(proc3, "dmmeta.nossimfile  ctype:testdb.App  comment:\"\"");
    atf_comp::ProcWrite(proc3, "dmmeta.field  field:testdb.App.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc3);
    atf_comp::FProc &proc4 = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N '(testdb.%|algo_lib.trace..Ctor)' 2>/dev/null < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc4, "dmmeta.nstype  nstype:none  comment:\"\"");
    atf_comp::ProcWrite(proc4, "dmmeta.nstype  nstype:ssimdb  comment:\"\"");
    atf_comp::ProcWrite(proc4, "dmmeta.ns  ns:ncns  nstype:none  license:GPL  comment:\"Namespace with no dmmeta.nscpp row: emits no C++\"");
    atf_comp::ProcWrite(proc4, "dmmeta.ns  ns:testdb  nstype:ssimdb  license:GPL  comment:\"ssimdb namespace projected to TypeScript alone: emits no C++\"");
    atf_comp::ProcWrite(proc4, "dmmeta.nsjs  ns:testdb  typescript:Y  comment:\"\"");
    atf_comp::ProcWrite(proc4, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc4, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc4, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc4, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc4, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc4, "dmmeta.ctype  ctype:algo_lib.CppWidget  comment:\"\"");
    atf_comp::ProcWrite(proc4, "dmmeta.field  field:algo_lib.CppWidget.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc4, "dmmeta.ctype  ctype:ncns.Widget  comment:\"\"");
    atf_comp::ProcWrite(proc4, "dmmeta.field  field:ncns.Widget.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc4, "dmmeta.ctype  ctype:testdb.App  comment:\"\"");
    atf_comp::ProcWrite(proc4, "dmmeta.nossimfile  ctype:testdb.App  comment:\"\"");
    atf_comp::ProcWrite(proc4, "dmmeta.field  field:testdb.App.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc4);
}

// An fdec on a bitfld field of a global ctype: the value is reached through
// the field's Get/Set accessors, which for a global parent take no parent
// argument. Pins the emitted GetDouble/qSetDouble bodies.
// The second field is ten bits wide inside a u32 word, and pins the range
// the emitters bound it by. A bitfld setter masks the store to the declared
// width, so the magnitude the field holds is 1023 and not the 4294967295 its
// arg would suggest: SetDoubleMaybe compares against 1023 and the decimal
// reader carries that magnitude split by ten, 102 and 3.
// Each of the three stores casts the value to the field's own type on the
// way in, so the setter is handed exactly the type it declares. The double
// the double setters compute and the u64 magnitude the reader accumulates
// are neither of them that type, and an arg naming a ctype that stands for
// an integer builtin is not that type either.
void atf_comp::comptest_amc_FdecGlobalBitfld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb.decfrac.GetDouble|algo_lib.FDb.decfrac.qSetDouble|algo_lib.FDb.decnarrow.SetDoubleMaybe|algo_lib.FDb.decnarrow.ReadStrptrMaybe' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:Y  print:N  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.decbits  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.decfrac  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.FDb.decfrac  offset:0  width:32  srcfield:algo_lib.FDb.decbits  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.FDb.decfrac  nplace:2  fixedfmt:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.decnarrowbits  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.decnarrow  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.FDb.decnarrow  offset:0  width:10  srcfield:algo_lib.FDb.decnarrowbits  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.FDb.decnarrow  nplace:2  fixedfmt:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A decimal scale amc cannot generate consistent arithmetic for. Zero places
// -- the nplace default, so what a forgotten attr leaves behind -- emits a
// reader that accumulates no digit at all; eighteen places outrun the powers
// of ten the double accessors substitute, so they and the string accessors
// would disagree about the scale; thirty-one places outrun the printer's
// scratch buffer; twelve places on an i32 field outrun the field's own arg,
// which the emitted GetScale returns the scale as, and the diagnostic states
// the narrower range that arg allows. A u8 arg is the narrowest range the
// rule admits from the arg side: two places fit its 255 and three do not, and
// the diagnostic states that bound.
// The fixture also holds one field per arg that carries no numeric range at
// all -- bool, char, u128, float, double, pad_byte, and a plain struct ctype
// -- and each is reported as an unusable arg. The char field's count is zero
// as well, and it is still examined against the powers-of-ten limit, so that
// field draws both diagnostics in one run.
// The remaining fields cover the other way a field's range is fixed. A
// Bitfld field stores into a slice of another field's word, and its setter
// masks the store, so the width and not the arg is what bounds the count: a
// ten-bit slice of a u32 takes three places and rejects four, a forty-bit
// slice of a u64 takes twelve and rejects thirteen, and a three-bit slice
// holds no decimal place at all, which the diagnostic states rather than
// naming an empty range. A signed arg cannot be sliced narrower than its own
// type at all: the ten-bit i32 field draws the bitfield diagnostic that
// refuses it, since such a field does not sign-extend on read. That refusal
// follows the type the arg stands for rather than the arg's own name, so a
// ten-bit slice of a ctype whose single Val field is an i32 draws it too, and
// a seven-bit slice of an i8 draws it at the narrowest signed arg there is.
// A slice whose arg carries no numeric range takes no bound from its width:
// the four-bit char field is reported as an unusable arg, and its count is
// then held against the powers-of-ten limit alone.
// The controls draw no diagnostic: the one-place and seventeen-place i64
// fields sit at the bottom and the top of the supported range, each of the
// eight integer builtins carries a count its own range allows, so no width
// and neither sign is left untested, and a slice as wide as its arg takes
// what the arg takes at both signs -- u32 and i32 at thirty-two bits each
// take nine places, a u64 at sixty-four takes seventeen, and an i8 at eight
// takes the two places its 127 allows.
// An arg standing for the builtin through another ctype stores that ctype,
// while every quantity the emitted functions compute is a number of the
// builtin, so the value crosses between the two at each end: out of the
// wrapper through the operator dmmeta.fcast generates, and into it through
// the fieldwise constructor a dmmeta.cpptype row asking for a constructor
// provides. The fixture carries one arg per combination of the two.
// DecPlaceCast carries both and is accepted, and the query pins its emitted
// functions, so what an accepted wrapper compiles to is part of what this
// test compares rather than something no golden looks at. DecPlaceFcast
// carries the operator alone, DecPlaceCtor the constructor alone, and
// DecPlaceWrap neither, and each is reported naming what is missing.
// DecPlaceChain stands for the builtin two ctypes deep, where the
// constructor takes the inner ctype rather than the builtin, and is reported
// as standing for the builtin only through another ctype. The same rule
// reaches a Bitfld arg: the ten-bit slice of a wrapped u32 carries both
// conversions and draws its count diagnostic alone, while the wrapped i32
// slice carries neither and draws the conversion diagnostic beside its own.
void atf_comp::comptest_amc_BadFdecNplace() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.DecPlaceCast.value.%' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i8  likeu64:Y  bigendok:N  issigned:Y  comment:\"8-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u8  likeu64:Y  bigendok:N  issigned:N  comment:\"8-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i16  likeu64:Y  bigendok:Y  issigned:Y  comment:\"16-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:Y  bigendok:Y  issigned:N  comment:\"16-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:\"32-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"32-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i64  likeu64:Y  bigendok:Y  issigned:Y  comment:\"64-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:\"64-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:Y  issigned:N  comment:\"128-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:bool  likeu64:Y  bigendok:N  issigned:N  comment:\"dflt is a c++ expr: real dflt is 'N'\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pad_byte  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:pad_byte  size:1  alignment:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:pad_byte.String  printfmt:Extern  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:pad_byte  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:char  likeu64:Y  bigendok:N  issigned:N  comment:char");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:float  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:float  likeu64:N  bigendok:N  issigned:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:float  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:float.String  printfmt:Extern  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:float  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:double  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:double  likeu64:N  bigendok:N  issigned:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:double  size:8  alignment:8  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:double.String  printfmt:Extern  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:double  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlace0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlace0.value  arg:i64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlace0.value  nplace:0  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceNeg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceNeg.value  arg:i64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceNeg.value  nplace:-1  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlace18  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlace18.value  arg:i64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlace18.value  nplace:18  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlace31  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlace31.value  arg:i64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlace31.value  nplace:31  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlace17  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlace17.value  arg:i64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlace17.value  nplace:17  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlace12I32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlace12I32.value  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlace12I32.value  nplace:12  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlace1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlace1.value  arg:i64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlace1.value  nplace:1  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceBool  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceBool.value  arg:bool  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceBool.value  nplace:1  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceChar  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceChar.value  arg:char  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceChar.value  nplace:0  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceU128  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceU128.value  arg:u128  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceU128.value  nplace:1  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceFloat  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceFloat.value  arg:float  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceFloat.value  nplace:1  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceDouble  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceDouble.value  arg:double  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceDouble.value  nplace:1  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlacePadbyte  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlacePadbyte.value  arg:pad_byte  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlacePadbyte.value  nplace:1  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceStruct  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceStruct.value  arg:algo_lib.FDb  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceStruct.value  nplace:1  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.I64Val  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.I64Val.value  arg:i64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceWrap  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceWrap.value  arg:algo_lib.I64Val  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceWrap.value  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.I64Cast  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.I64Cast.value  arg:i64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo_lib.I64Cast.value  expr:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.I64Cast  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceCast  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceCast.value  arg:algo_lib.I64Cast  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceCast.value  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.DecPlaceCast.String  printfmt:Raw  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.I64Fcast  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.I64Fcast.value  arg:i64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo_lib.I64Fcast.value  expr:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceFcast  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceFcast.value  arg:algo_lib.I64Fcast  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceFcast.value  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.I64Ctor  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.I64Ctor.value  arg:i64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.I64Ctor  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceCtor  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceCtor.value  arg:algo_lib.I64Ctor  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceCtor.value  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.I64Cast2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.I64Cast2.value  arg:algo_lib.I64Cast  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo_lib.I64Cast2.value  expr:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.I64Cast2  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceChain  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceChain.value  arg:algo_lib.I64Cast2  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceChain.value  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceI8  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceI8.value  arg:i8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceI8.value  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceI16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceI16.value  arg:i16  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceI16.value  nplace:4  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceU16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceU16.value  arg:u16  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceU16.value  nplace:4  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceI32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceI32.value  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceI32.value  nplace:9  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceU32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceU32.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceU32.value  nplace:9  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceU64  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceU64.value  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceU64.value  nplace:17  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceU8  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceU8.value  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceU8.value  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecPlaceU8Over  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecPlaceU8Over.value  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecPlaceU8Over.value  nplace:3  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitU32W32P9  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W32P9.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W32P9.value  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitU32W32P9.value  offset:0  width:32  srcfield:algo_lib.DecBitU32W32P9.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitU32W32P9.value  nplace:9  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitU32W10P3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W10P3.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W10P3.value  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitU32W10P3.value  offset:0  width:10  srcfield:algo_lib.DecBitU32W10P3.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitU32W10P3.value  nplace:3  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitU32W10P4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W10P4.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W10P4.value  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitU32W10P4.value  offset:0  width:10  srcfield:algo_lib.DecBitU32W10P4.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitU32W10P4.value  nplace:4  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitU32W3P1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W3P1.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU32W3P1.value  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitU32W3P1.value  offset:0  width:3  srcfield:algo_lib.DecBitU32W3P1.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitU32W3P1.value  nplace:1  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitU64W64P17  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU64W64P17.word  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU64W64P17.value  arg:u64  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitU64W64P17.value  offset:0  width:64  srcfield:algo_lib.DecBitU64W64P17.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitU64W64P17.value  nplace:17  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitU64W40P13  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU64W40P13.word  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitU64W40P13.value  arg:u64  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitU64W40P13.value  offset:0  width:40  srcfield:algo_lib.DecBitU64W40P13.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitU64W40P13.value  nplace:13  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitI32W32P9  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI32W32P9.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI32W32P9.value  arg:i32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitI32W32P9.value  offset:0  width:32  srcfield:algo_lib.DecBitI32W32P9.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitI32W32P9.value  nplace:9  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitI32W10P2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI32W10P2.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI32W10P2.value  arg:i32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitI32W10P2.value  offset:0  width:10  srcfield:algo_lib.DecBitI32W10P2.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitI32W10P2.value  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.I32Val  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.I32Val.value  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.U32Val  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.U32Val.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo_lib.U32Val.value  expr:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.U32Val  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitWrapI32W10P2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitWrapI32W10P2.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitWrapI32W10P2.value  arg:algo_lib.I32Val  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitWrapI32W10P2.value  offset:0  width:10  srcfield:algo_lib.DecBitWrapI32W10P2.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitWrapI32W10P2.value  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitWrapU32W10P4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitWrapU32W10P4.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitWrapU32W10P4.value  arg:algo_lib.U32Val  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitWrapU32W10P4.value  offset:0  width:10  srcfield:algo_lib.DecBitWrapU32W10P4.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitWrapU32W10P4.value  nplace:4  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitI8W8P2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI8W8P2.word  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI8W8P2.value  arg:i8  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitI8W8P2.value  offset:0  width:8  srcfield:algo_lib.DecBitI8W8P2.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitI8W8P2.value  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitI8W7P2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI8W7P2.word  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitI8W7P2.value  arg:i8  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitI8W7P2.value  offset:0  width:7  srcfield:algo_lib.DecBitI8W7P2.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitI8W7P2.value  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitCharW4P2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitCharW4P2.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitCharW4P2.value  arg:char  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitCharW4P2.value  offset:0  width:4  srcfield:algo_lib.DecBitCharW4P2.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitCharW4P2.value  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A Bitfld field whose declared width is zero, carrying a decimal count. The
// width is reported as out of bounds. The decimal range takes no bound from
// such a width: a bound computed from it would state that the field holds no
// decimal place at all, so the count draws no diagnostic of its own. The
// bitfield accessors refuse the width once generation reaches the field, and
// the run ends there -- which is why the width error follows the bounds
// report, and why a fixture reaches the decimal rule with one bad width at
// most. The negative width has a fixture of its own.
void atf_comp::comptest_amc_BadFdecBitwidth() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i8  likeu64:Y  bigendok:N  issigned:Y  comment:\"8-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u8  likeu64:Y  bigendok:N  issigned:N  comment:\"8-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i16  likeu64:Y  bigendok:Y  issigned:Y  comment:\"16-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:Y  bigendok:Y  issigned:N  comment:\"16-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:\"32-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"32-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i64  likeu64:Y  bigendok:Y  issigned:Y  comment:\"64-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:\"64-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:Y  issigned:N  comment:\"128-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:bool  likeu64:Y  bigendok:N  issigned:N  comment:\"dflt is a c++ expr: real dflt is 'N'\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pad_byte  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:pad_byte  size:1  alignment:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:pad_byte.String  printfmt:Extern  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:pad_byte  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:char  likeu64:Y  bigendok:N  issigned:N  comment:char");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitW0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitW0.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitW0.value  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitW0.value  offset:0  width:0  srcfield:algo_lib.DecBitW0.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitW0.value  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// The zero-width fixture's counterpart on the other side of the same rule: a
// Bitfld field whose declared width is negative, carrying a decimal count.
// The width is reported as out of bounds and the decimal range takes no bound
// from it, so the count draws no diagnostic of its own. The magnitude is
// sixty-four rather than one because the rule is checked through its output:
// a bound built from a negative width shifts by a negative count, and a
// compiler that takes the count modulo sixty-four then computes a bound of
// zero from this width and reports the field as holding no decimal place. A
// run that keeps the width out of the shift prints two rows; one that lets it
// in prints three.
void atf_comp::comptest_amc_BadFdecBitwidthNeg() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i8  likeu64:Y  bigendok:N  issigned:Y  comment:\"8-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u8  likeu64:Y  bigendok:N  issigned:N  comment:\"8-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i16  likeu64:Y  bigendok:Y  issigned:Y  comment:\"16-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:Y  bigendok:Y  issigned:N  comment:\"16-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:\"32-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"32-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i64  likeu64:Y  bigendok:Y  issigned:Y  comment:\"64-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:\"64-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:Y  issigned:N  comment:\"128-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:bool  likeu64:Y  bigendok:N  issigned:N  comment:\"dflt is a c++ expr: real dflt is 'N'\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pad_byte  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:pad_byte  size:1  alignment:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:pad_byte.String  printfmt:Extern  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:pad_byte  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:char  likeu64:Y  bigendok:N  issigned:N  comment:char");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.DecBitWNeg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitWNeg.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.DecBitWNeg.value  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.DecBitWNeg.value  offset:0  width:-64  srcfield:algo_lib.DecBitWNeg.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.DecBitWNeg.value  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A Tuple-printfmt String cfmt on a global ctype: field values are reached
// through the print function's row argument, including the cursor walk of an
// array field. Pins the emitted Print body.
void atf_comp::comptest_amc_PrintGlobalTuple() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N algo_lib.FDb.%Print% < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.val  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.buf  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.buf  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A read:Y Tuple cfmt on a global ctype: the ReadFieldMaybe dispatcher
// keeps the ctype-name prefix its callers link against and passes no
// parent to the field readers, which -- like every field-level function
// of a global -- take no parent argument; the Set-backed reader assigns
// through the collapsed Set accessor.
void atf_comp::comptest_amc_ReadGlobalTuple() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb.maxjobs.ReadStrptrMaybe|algo_lib.FDb..ReadFieldMaybe|algo_lib.FDb..ReadStrptrMaybe' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.pmask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.FDb.pmask  filter_print:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.maxjobs  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.timeout  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An ArgvGnu cfmt: the gnu form of the same emission matrix ArgvGlobal pins
// for Argv. A one-character field name takes a single dash, longer names take
// two, PrintArgv separates the name from its value with a space where Argv
// writes a colon, ToArgv appends the value as its own argv token, and an array
// field repeats the name once per element. args[0] is the bare command name,
// no bin/ prefix: an ArgvGnu command is external and resolves on PATH, which
// atf_unit ExecToArgvSyntax asserts against the real bash command.
void atf_comp::comptest_amc_ArgvGnu() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.Gnu..PrintArgv|algo_lib.Gnu..ToArgv' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Gnu  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Gnu.c  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Gnu.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Gnu.tag  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.Gnu.tag  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Gnu.ArgvGnu  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An Argv cfmt on a global ctype and on a regular one: the value of a field
// with its own Print function (an fconst field) prints through that
// function, whose parent argument collapses for the global and is the row
// itself -- no const_cast -- for the regular ctype. Pins the PrintArgv and
// ToArgv bodies of both.
void atf_comp::comptest_amc_ArgvGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb..PrintArgv|algo_lib.FDb..ToArgv|algo_lib.Cmd..PrintArgv|algo_lib.Cmd..ToArgv' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.state  arg:u8  reftype:Val  dflt:0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.FDb.state/idle  value:0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.FDb.state/busy  value:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.buf  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.buf  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.Argv  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Cmd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.state  arg:u8  reftype:Val  dflt:0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Cmd.state/idle  value:0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Cmd.state/busy  value:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Cmd.Argv  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Which field of a command ctype becomes a command-line token, and how its
// value is reached. Tokens: a plain Val, a Val cloned in from a Base, an
// fconst Val and a Smallstr through their own Print functions, a Tary through
// a cursor walk, a big-endian Val and a bitfield through their Get accessors.
// No token: the Global anchor and the Base anchor, which name no member of
// their own; the source word a bitfield lives in, whose bits print
// individually outside Raw form; and a Ptr, whose address has no command-line
// spelling ReadArgv could take back. Pins the PrintArgv bodies of all three
// ctypes and the ToArgv bodies of the two that carry the interesting fields.
void atf_comp::comptest_amc_ArgvField() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb..PrintArgv|algo_lib.Cmd..PrintArgv|algo_lib.Cmd..ToArgv|algo_lib.Flags..PrintArgv|algo_lib.Flags..ToArgv' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:u32  extrn:N  genop:N  order:N  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.Argv  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.id  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Base  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Base.count  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Cmd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.base  arg:algo_lib.Base  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.plain  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.state  arg:u8  reftype:Val  dflt:0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Cmd.state/idle  value:0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Cmd.state/busy  value:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.big  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.Cmd.big  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.pmask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.Cmd.pmask  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.mode  arg:u8  reftype:Val  dflt:0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Cmd.mode/idle  value:0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Cmd.mode/busy  value:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.name  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Cmd.name  length:20  strtype:rpascal  pad:\"\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.tag  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.Cmd.tag  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Cmd.p_row  arg:algo_lib.FRow  reftype:Ptr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Cmd.Argv  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Ary  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ary.buf  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Ary.buf  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Ary.Argv  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Flags  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Flags.seqno  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:algo_lib.Flags.seqno  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Flags.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Flags.flag  arg:bool  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Flags.flag  offset:0  width:1  srcfield:algo_lib.Flags.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Flags.Argv  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An fconst on a global ctype's field: the fconst accessors take no parent
// argument, so the calls between them pass none. Pins ToCstr's GetEnum
// switch, Print's ToCstr call, SetStrptrMaybe's SetEnum calls (short and
// beyond-8-byte names), and -- via a bitfld member -- GetEnum's Get call.
void atf_comp::comptest_amc_FconstGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb.state.ToCstr|algo_lib.FDb.state.Print|algo_lib.FDb.state.SetStrptrMaybe|algo_lib.FDb.mode.GetEnum' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.state  arg:u8  reftype:Val  dflt:0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.FDb.state/idle  value:0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.FDb.state/busy  value:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.FDb.state/quiescing  value:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.modebits  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.mode  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.FDb.mode  offset:0  width:32  srcfield:algo_lib.FDb.modebits  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.FDb.mode/off  value:0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.FDb.mode/on  value:1  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// The universe read from stdin (-in_dir:-), which is how a caller composes
// one -- an acr_in dump plus extra rows -- without a temp file.  The query
// prints a function that can only come from the piped rows, and "-" names no
// directory, so side table data loads from the default root and the run says
// so.
void atf_comp::comptest_amc_StdinUniverse() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N algo_lib.Price.value.GetScale < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Price  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Price.value  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.Price.value  nplace:2  fixedfmt:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Quote  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Quote.bid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.Quote.bid  nplace:2  fixedfmt:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Quote.ask  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.Quote.ask  nplace:4  fixedfmt:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A numstr on a global ctype's field: the numstr accessors take no parent
// argument, so Geti64 and GetnumDflt call Getnum bare. Pins both Geti64
// arms (the u64 numtype's overflow-checked arm and the plain arm, via an
// unsigned and a signed field) and GetnumDflt's body.
void atf_comp::comptest_amc_NumstrGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb.gnumstr.Geti64|algo_lib.FDb.gnumstr.GetnumDflt|algo_lib.FDb.gnumsigned.Geti64|algo_lib.FDb.gnumsigned.GetnumDflt' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.gnumstr  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.FDb.gnumstr  length:20  strtype:leftpad  pad:\"'0'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.FDb.gnumstr  numtype:u64  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.gnumsigned  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.FDb.gnumsigned  length:20  strtype:leftpad  pad:\"' '\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.FDb.gnumsigned  numtype:i64  base:10  min_len:1");
    atf_comp::ProcWriteEof(proc);
}

// A pmask on a global ctype: the pmask's bitset accessors take no parent
// argument, so the member accessors and the print filter call them bare.
// Pins one member's PresentQ/SetPresent/Set bodies and the filtered Print.
void atf_comp::comptest_amc_PmaskGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb.maxjobs.%|algo_lib.FDb..Print' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.pmask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.FDb.pmask  filter_print:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.maxjobs  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.timeout  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A Bitfld field in a kafka codec: the field has no direct member, so raw
// parent.<name> access is uncompilable. Encode reads through the Get
// accessor (FieldvalExpr); decode lands in a temporary stored through the
// Set accessor (NoDirectMemberQ -- deliberately narrower than
// AssignViaSetQ, since this codec's presence bits follow the wire's null
// flag and a pmask Set would mark a null field present).
void atf_comp::comptest_amc_KafkaBitfld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'ktst.Rec..KafkaEncode|ktst.Rec..KafkaDecode' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ktst  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:ktst  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:ktst.Rec  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ktst.Rec.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ktst.Rec.val  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:ktst.Rec.val  offset:0  width:16  srcfield:ktst.Rec.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ckafka  ctype:ktst.Rec  kind:field  root:Rec  valid_versions:0  flexible_versions:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fkafka  field:ktst.Rec.val  versions:0+  nullable_versions:\"\"  tagged_versions:\"\"  tag:0  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A kafka nullable array on a ctype whose pmask field is not named "pmask":
// the presence accessors derive their names from the pmask field
// (val_FieldMaskQ / val_SetFieldMask here), and the encoder's presence
// checks must use the same derived name the decoder's set side does.
// Pins the KafkaEncode presence checks and the KafkaDecode present-mark.
void atf_comp::comptest_amc_KafkaPmaskName() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'ktst.Rec..KafkaEncode|ktst.Rec..KafkaDecode|ktst.Rec.val.FieldMaskQ|ktst.Rec.val.SetFieldMask' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ktst  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:ktst  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:ktst.Rec  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ktst.Rec.field_mask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:ktst.Rec.field_mask  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ktst.Rec.val  arg:i32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:ktst.Rec.val  aliased:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ckafka  ctype:ktst.Rec  kind:field  root:Rec  valid_versions:0  flexible_versions:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fkafka  field:ktst.Rec.val  versions:0+  nullable_versions:0+  tagged_versions:\"\"  tag:0  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An optional fpbuf field on a ctype whose pmask field is not named "pmask":
// same derived-name rule as KafkaPmaskName, and both sides of the protobuf
// codec have to spell the name the pmask field derives. The encoder's presence
// guard reads val_FieldMaskQ; the decoder stores the plain Val field into its
// own member and marks presence with val_SetFieldMask, which is the same
// update the field's own Set carries.
// Pins the PbufEncode guard, the PbufDecode present-mark, the Set, and the
// accessors.
void atf_comp::comptest_amc_PbufPmaskName() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'pbt.Msg..PbufEncode|pbt.Msg..PbufDecode|pbt.Msg.val.Set|pbt.Msg.val.FieldMaskQ|pbt.Msg.val.SetFieldMask' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.field_mask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Msg.field_mask  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.val  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.val  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A bitfld carrying an fpbuf, on a ctype with a pmask: the field's value
// lives inside its source field, not in a member of its own, so the encoder
// reads it through the Get accessor and the decoder stores a temporary through
// the Set accessor. The temporary starts empty, the varint decoder replacing
// whatever it is handed. That Set writes the slice of the source word and
// leaves the pmask alone, so the decoder marks presence itself.
// Pins the accessor-routed PbufEncode and PbufDecode, and the Set body.
void atf_comp::comptest_amc_PbufBitfld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'pbt.Msg..PbufEncode|pbt.Msg..PbufDecode|pbt.Msg.val.Set' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.pmask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Msg.pmask  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.flags  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.val  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:pbt.Msg.val  offset:0  width:8  srcfield:pbt.Msg.flags  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.val  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A bitfld carrying an fpbuf on a proto3 ctype with no pmask: presence is
// implicit, so the encode guard is the pbtype's default-value check, and that
// check must read the field through its Get accessor -- a bitfield has no
// member of its own to compare against zero. Pins the guard, and the Set body
// storing straight into the source word since no presence bit follows.
void atf_comp::comptest_amc_PbufBitfldNondflt() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'pbt.Msg..PbufEncode|pbt.Msg..PbufDecode|pbt.Msg.val.Set' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.flags  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.val  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:pbt.Msg.val  offset:0  width:8  srcfield:pbt.Msg.flags  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.val  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A repeated fpbuf field that is a pmask member: the decoder must mark the
// field present when elements arrive, or the print filter that consults the
// same bit drops the decoded elements. Pins the SetPresent call in both the
// packed and the one-element-per-tag decode branch, and the filtered Print.
void atf_comp::comptest_amc_PbufRepeatedPmask() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'pbt.Msg..PbufDecode|pbt.Msg..Print' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.pmask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Msg.pmask  filter_print:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.tag  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Msg.tag  aliased:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:pbt.Msg.String  printfmt:Tuple  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.tag  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Every decode store path in one run: one message carrying each singular
// shape, one carrying the repeated controls. A singular field decodes into its
// own member, and under pb_type message that member already holds the earlier
// occurrence, so the nested decoder merges into it -- pbt.Msg.bare and .sub.
// A pmask member decodes in place too and is marked present afterwards (.opt,
// .sub): its own Set would have marked it, but storing through that Set would
// copy the value out and back on every occurrence of the field number. The
// shapes with no member to decode into do pay for a temporary -- .slice
// through the bitfld accessors, .netval through the byte-swapping pair -- and
// so does .cond, whose Set moves the record between fcond lists. Of the eight,
// only .computed seeds its temporary from the field's current value, being the
// one that is both accessor-routed and a message: Get is then the only way for
// the merge to see what the earlier occurrence set. pbt.Rep appends through
// Alloc in every branch.
void atf_comp::comptest_amc_PbufStore() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'pbt.Msg..PbufDecode|pbt.Rep..PbufDecode' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.listtype  listtype:cd  circular:Y  haveprev:Y  instail:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.curs  dflt:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.tcurs  tfunc:Llist.delcurs  dflt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:lib  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Sub  comment:\"embedded message, with a repeated member so a merge is visible\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Sub.item  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Sub.item  aliased:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Sub  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Sub.item  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:\"every singular store shape of the pbuf decoder in one message\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.pmask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Msg.pmask  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.plain  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.opt  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:pbt.Msg.pmask/pbt.Msg.opt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.bare  arg:pbt.Sub  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.sub  arg:pbt.Sub  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:pbt.Msg.pmask/pbt.Msg.sub  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.flags  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.slice  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:pbt.Msg.slice  offset:0  width:8  srcfield:pbt.Msg.flags  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:pbt.Msg.pmask/pbt.Msg.slice  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.netval  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:pbt.Msg.netval  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.computed  arg:pbt.Sub  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cppfunc  field:pbt.Msg.computed  expr:\"parent.bare\"  print:N  set:Y");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.cond  arg:bool  reftype:Val  dflt:false  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.FDb  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.FDb._db  arg:pbt.FDb  reftype:Global  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.FDb.msg  arg:pbt.Msg  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.FDb.cd_msg_on  arg:pbt.Msg  reftype:Llist  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.llist  field:pbt.FDb.cd_msg_on  havetail:N  havecount:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:pbt.FDb.cd_msg_on  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcond  fcond:pbt.Msg.cond/true  ins:pbt.FDb.cd_msg_on  via:\"\"  rem:Y  comment:\"mirror: on the list iff the wire said true\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.plain  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.opt  field_number:2  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.bare  field_number:3  pb_type:message  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.sub  field_number:4  pb_type:message  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.slice  field_number:5  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.netval  field_number:6  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.computed  field_number:7  pb_type:message  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.cond  field_number:8  pb_type:bool  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Rep  comment:\"the repeated controls: every occurrence appends through Alloc\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Rep.pmask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Rep.pmask  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Rep.tag  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Rep.tag  aliased:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:pbt.Rep.pmask/pbt.Rep.tag  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Rep.item  arg:pbt.Sub  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Rep.item  aliased:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Rep  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Rep.tag  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Rep.item  field_number:2  pb_type:message  packed:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An Opt field on a global ctype: rejected as amc.infinity_pool, and the
// Print body emitted alongside the error must not contain a bare (void);
// statement for the absent parent argument.
void atf_comp::comptest_amc_OptGlobalPrint() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N algo_lib.FDb.tail.Print < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sub  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sub.n  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Sub.String  printfmt:Extern  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.tail  arg:algo_lib.Sub  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A base-1 numstr and a base-37 one: neither base has a digit encoding
// (unary needs one digit per unit of value; 37..94 has no character map),
// so amc rejects both fields, naming each.
void atf_comp::comptest_amc_BadNumstrBase() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr5_U32_Base1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr5_U32_Base1.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr5_U32_Base1.ch  length:5  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr5_U32_Base1.ch  numtype:u32  base:1  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr5_U32_Base37  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr5_U32_Base37.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr5_U32_Base37.ch  length:5  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr5_U32_Base37.ch  numtype:u32  base:37  min_len:1");
    atf_comp::ProcWriteEof(proc);
}

// A numstr min_len wider than the string itself, one wider than
// SetnumMaybe's 64-character pad budget, and a signed one equal to the
// string length: the sign then has no slot, so every negative value would
// fail at runtime. None of these width contracts can be honored, so amc
// rejects all three fields.
void atf_comp::comptest_amc_BadNumstrMinlen() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr9_U32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr9_U32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr9_U32.ch  length:9  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr9_U32.ch  numtype:u32  base:10  min_len:12");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LspaceStr100_U64  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LspaceStr100_U64.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LspaceStr100_U64.ch  length:100  strtype:leftpad  pad:\"' '\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LspaceStr100_U64.ch  numtype:u64  base:10  min_len:80");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LspaceStr6_I32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LspaceStr6_I32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LspaceStr6_I32.ch  length:6  strtype:leftpad  pad:\"' '\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LspaceStr6_I32.ch  numtype:i32  base:10  min_len:6");
    atf_comp::ProcWriteEof(proc);
}

// A numtype that does not stand for an integer bltin. The fixture holds one
// field per such numtype: double and float carry no integer range, u128 is
// wider than the range amc represents, pad_byte is no number at all, and a
// plain struct ctype stands for no bltin whatever. Each falls back on the
// widest signed bounds, which admit every value and make the sign tests fire
// even for an unsigned numtype, so a range comparison built from one would
// never reject. One fdec field trips the same check from the other side.
// The rest of the fields exercise the agreement between a ctype name and its
// numtype, which amc enforces so that a name cannot lie about the range its
// string holds. LnumStr6_I32 says I32 over a u32 numtype and LnumStr9_U64
// says U64 over the same one: a name is rejected for lying about the sign
// and for lying about the width alike. LnumStr4_I16 says I16 over
// algo_lib.U32Val, a ctype whose single Val field holds a u32, and the
// numtype is judged by the u32 it stands for, so the lie is caught through
// the wrapper. The controls draw no diagnostic from the same run:
// LnumStr7_U32 states a truthful token over a bare u32, LnumStr6_U32 states
// one over the wrapper, LnumStrPlain carries no token to judge, and
// LnumStr7_I8 carries a false one under strict:N, which asks for no check.
// A numtype standing for the builtin through another ctype has to move a
// value both ways: Getnum constructs the numtype from the digits it parsed,
// and Geti64 reads that result as an i64. The constructor comes from a
// dmmeta.cpptype row asking for one and the read from the operator
// dmmeta.fcast generates, and the fixture carries one numtype per
// combination of the two. LnumStr6_U32 names a wrapper carrying both and is
// accepted, and the query pins its Getnum and Geti64, which are the two
// statements the conversions have to satisfy. LnumStr8_U32 names one with
// the operator alone, LnumStr10_U32 one with the constructor alone, and
// LnumStr5_U32 one with neither, and each is reported naming what is
// missing. LnumStr11_U32 names a wrapper two ctypes above the builtin, whose
// constructor takes the inner wrapper rather than the builtin, and is
// reported as standing for the builtin only through another ctype. The bare
// builtins are the accepted edge of the same rule, and all eight are present
// with a truthful name token, so neither sign nor any width is left
// untested.
void atf_comp::comptest_amc_BadNumstrNumtype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.LnumStr6_U32.ch.Getnum|algo_lib.LnumStr6_U32.ch.Geti64' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pad_byte  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:pad_byte  size:1  alignment:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:pad_byte.String  printfmt:Extern  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:pad_byte  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:float  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:float  likeu64:N  bigendok:N  issigned:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:float  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:float.String  printfmt:Extern  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:float  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:double  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:double  likeu64:N  bigendok:N  issigned:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:double  size:8  alignment:8  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:double.String  printfmt:Extern  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:double  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr8_F64  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr8_F64.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr8_F64.ch  length:8  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr8_F64.ch  numtype:double  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr5_Fdb  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr5_Fdb.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr5_Fdb.ch  length:5  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr5_Fdb.ch  numtype:algo_lib.FDb  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr7_U32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr7_U32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr7_U32.ch  length:7  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr7_U32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr6_I32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr6_I32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr6_I32.ch  length:6  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr6_I32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr4_U128  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr4_U128.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr4_U128.ch  length:4  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr4_U128.ch  numtype:u128  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr4_Float  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr4_Float.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr4_Float.ch  length:4  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr4_Float.ch  numtype:float  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr4_Padbyte  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr4_Padbyte.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr4_Padbyte.ch  length:4  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr4_Padbyte.ch  numtype:pad_byte  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.U32Cast  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.U32Cast.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo_lib.U32Cast.value  expr:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.U32Cast  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr6_U32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr6_U32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr6_U32.ch  length:6  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr6_U32.ch  numtype:algo_lib.U32Cast  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr4_I16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr4_I16.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr4_I16.ch  length:4  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr4_I16.ch  numtype:algo_lib.U32Cast  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.U32Val  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.U32Val.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr5_U32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr5_U32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr5_U32.ch  length:5  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr5_U32.ch  numtype:algo_lib.U32Val  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.U32Fcast  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.U32Fcast.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo_lib.U32Fcast.value  expr:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr8_U32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr8_U32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr8_U32.ch  length:8  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr8_U32.ch  numtype:algo_lib.U32Fcast  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.U32Ctor  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.U32Ctor.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.U32Ctor  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr10_U32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr10_U32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr10_U32.ch  length:10  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr10_U32.ch  numtype:algo_lib.U32Ctor  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.U32Cast2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.U32Cast2.value  arg:algo_lib.U32Cast  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo_lib.U32Cast2.value  expr:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpptype  ctype:algo_lib.U32Cast2  ctor:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr11_U32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr11_U32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr11_U32.ch  length:11  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr11_U32.ch  numtype:algo_lib.U32Cast2  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr3_I8  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr3_I8.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr3_I8.ch  length:3  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr3_I8.ch  numtype:i8  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr3_U8  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr3_U8.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr3_U8.ch  length:3  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr3_U8.ch  numtype:u8  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr6_I16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr6_I16.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr6_I16.ch  length:6  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr6_I16.ch  numtype:i16  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr5_U16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr5_U16.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr5_U16.ch  length:5  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr5_U16.ch  numtype:u16  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr10_I32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr10_I32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr10_I32.ch  length:10  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr10_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr19_I64  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr19_I64.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr19_I64.ch  length:19  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr19_I64.ch  numtype:i64  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr20_U64  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr20_U64.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr20_U64.ch  length:20  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr20_U64.ch  numtype:u64  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr9_U64  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr9_U64.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr9_U64.ch  length:9  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr9_U64.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStrPlain  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStrPlain.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStrPlain.ch  length:4  strtype:leftpad  pad:\"'0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStrPlain.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnumStr7_I8  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnumStr7_I8.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnumStr7_I8.ch  length:7  strtype:leftpad  pad:\"'0'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnumStr7_I8.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.BadDec  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.BadDec.price  arg:double  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.BadDec.price  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Pads that read back as digits. A left-pad that is a nonzero digit of the
// base strips leading value digits on read: '!' is digit 1 in base 95, 'c'
// is a digit in base 36, '5' is a digit in base 10. A right-pad that is any
// digit of the base divides the stored value by a power of the base: NUL in
// base 256, ' ' in base 95, 'A' in base 36, '7' in base 10. The pad is a C++
// character literal, so its escaped spellings are judged by the byte they
// denote: a base-16 right-pad '\x30' is the digit zero and is rejected,
// while a base-95 left-pad '\0' is no digit of that base and passes. Only
// the zero digit is a safe left-pad and no digit is a safe right-pad, so amc
// rejects eight of the nine numstr fields. Four more fields carry no numstr
// row at all and meet the plain smallstr right-pad check, which refuses any
// pad character that can be confused with a number and reads an escaped pad
// by its denoted byte just the same: the digit '3' and the escaped digit
// '\x30' are refused, while the escaped 'A' and the escaped NUL are no part
// of any number and pass.
// The sign character is the other pad a signed numstr cannot read back. A
// left pad is stripped from the end the sign is written to, so a signed
// base-10 field padded with '-' loses the sign of every negative value and
// is rejected. The three neighbours of that case are the controls and draw
// nothing: the same '-' pad on an unsigned field, which writes no sign; the
// same '-' pad on the right, stripped from the end the sign is not at; and
// '+' on the left of a signed field, which is not the character the reader
// takes for a sign. A signed field left-padded with the zero digit is the
// fourth control -- leading zeros carry no value, so they strip safely.
// The NUL pad, which a signed numstr may not use either, shows that a pad is
// judged by the byte it denotes and not by the spelling it was written as.
// The three spellings of that one pad -- the bare 0, the empty pad value amc
// fills in as 0, and the escaped '\0' -- are all rejected on a signed field,
// on the left and on the right alike, while the same empty pad on an unsigned
// field passes at either end. The two rpascal fields are the controls for the
// strtype dimension: such a string carries its own length and strips no pad,
// so neither the NUL pad on a signed field nor a digit pad draws anything.
void atf_comp::comptest_amc_BadNumstrPad() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LbangStr5_U32_Base95  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LbangStr5_U32_Base95.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LbangStr5_U32_Base95.ch  length:5  strtype:leftpad  pad:\"'!'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LbangStr5_U32_Base95.ch  numtype:u32  base:95  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LceeStr5_U32_Base36  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LceeStr5_U32_Base36.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LceeStr5_U32_Base36.ch  length:5  strtype:leftpad  pad:\"'c'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LceeStr5_U32_Base36.ch  numtype:u32  base:36  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LdashStr5_I32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LdashStr5_I32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LdashStr5_I32.ch  length:5  strtype:leftpad  pad:\"'-'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LdashStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LdashStr5_U32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LdashStr5_U32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LdashStr5_U32.ch  length:5  strtype:leftpad  pad:\"'-'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LdashStr5_U32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LemptyStr5_I32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LemptyStr5_I32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LemptyStr5_I32.ch  length:5  strtype:leftpad  pad:\"\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LemptyStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LemptyStr5_U32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LemptyStr5_U32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LemptyStr5_U32.ch  length:5  strtype:leftpad  pad:\"\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LemptyStr5_U32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LescnulStr5_I32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LescnulStr5_I32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LescnulStr5_I32.ch  length:5  strtype:leftpad  pad:\"'\\\\0'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LescnulStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LescnulStr5_U32_Base95  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LescnulStr5_U32_Base95.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LescnulStr5_U32_Base95.ch  length:5  strtype:leftpad  pad:\"'\\\\0'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LescnulStr5_U32_Base95.ch  numtype:u32  base:95  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LfiveStr6_U32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LfiveStr6_U32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LfiveStr6_U32.ch  length:6  strtype:leftpad  pad:\"'5'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LfiveStr6_U32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LnullStr5_I32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LnullStr5_I32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LnullStr5_I32.ch  length:5  strtype:leftpad  pad:0  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LnullStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LplusStr5_I32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LplusStr5_I32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LplusStr5_I32.ch  length:5  strtype:leftpad  pad:\"'+'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LplusStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LzeroStr5_I32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LzeroStr5_I32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LzeroStr5_I32.ch  length:5  strtype:leftpad  pad:\"'0'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LzeroStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.PpasStr5_I32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PpasStr5_I32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.PpasStr5_I32.ch  length:5  strtype:rpascal  pad:\"\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.PpasStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.PpasdigStr5_U32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PpasdigStr5_U32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.PpasdigStr5_U32.ch  length:5  strtype:rpascal  pad:\"'7'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.PpasdigStr5_U32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RcapaStr5_U32_Base36  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RcapaStr5_U32_Base36.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RcapaStr5_U32_Base36.ch  length:5  strtype:rightpad  pad:\"'A'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RcapaStr5_U32_Base36.ch  numtype:u32  base:36  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RdashStr5_I32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RdashStr5_I32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RdashStr5_I32.ch  length:5  strtype:rightpad  pad:\"'-'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RdashStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RemptyStr5_I32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RemptyStr5_I32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RemptyStr5_I32.ch  length:5  strtype:rightpad  pad:\"\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RemptyStr5_I32.ch  numtype:i32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RemptyStr5_U32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RemptyStr5_U32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RemptyStr5_U32.ch  length:5  strtype:rightpad  pad:\"\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RemptyStr5_U32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RescdigStr5  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RescdigStr5.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RescdigStr5.ch  length:5  strtype:rightpad  pad:\"'\\\\x30'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RescletStr5  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RescletStr5.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RescletStr5.ch  length:5  strtype:rightpad  pad:\"'\\\\x41'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RescnulStr5  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RescnulStr5.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RescnulStr5.ch  length:5  strtype:rightpad  pad:\"'\\\\0'\"  strict:N");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RescnumStr5_U32_Base16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RescnumStr5_U32_Base16.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RescnumStr5_U32_Base16.ch  length:5  strtype:rightpad  pad:\"'\\\\x30'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RescnumStr5_U32_Base16.ch  numtype:u32  base:16  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RnullStr4_U32_Base256  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RnullStr4_U32_Base256.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RnullStr4_U32_Base256.ch  length:4  strtype:rightpad  pad:0  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RnullStr4_U32_Base256.ch  numtype:u32  base:256  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RsevenStr5_U32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RsevenStr5_U32.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RsevenStr5_U32.ch  length:5  strtype:rightpad  pad:\"'7'\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RsevenStr5_U32.ch  numtype:u32  base:10  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RspaceStr5_U32_Base95  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RspaceStr5_U32_Base95.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RspaceStr5_U32_Base95.ch  length:5  strtype:rightpad  pad:\"' '\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.RspaceStr5_U32_Base95.ch  numtype:u32  base:95  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RthreeStr5  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RthreeStr5.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RthreeStr5.ch  length:5  strtype:rightpad  pad:\"'3'\"  strict:N");
    atf_comp::ProcWriteEof(proc);
}

// An rpascal string longer than its length byte can count. The length of an
// rpascal string is stored in one byte, so 255 is the longest one whose
// length is representable, and amc rejects a longer one. Two such fields
// appear in the fixture and the run exits with 2: each diagnostic raises the
// error count, so a later one cannot discard what an earlier one reported.
// The controls draw no diagnostic: a 255-character rpascal string is the
// longest accepted one, and a 300-character right-padded string carries no
// length byte, so the bound is the rpascal strtype's alone.
void atf_comp::comptest_amc_BadSmallstrToobig() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Smallstr255  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Smallstr255.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Smallstr255.ch  length:255  strtype:rpascal  pad:\"\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Smallstr256  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Smallstr256.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Smallstr256.ch  length:256  strtype:rpascal  pad:\"\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Smallstr300  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Smallstr300.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Smallstr300.ch  length:300  strtype:rpascal  pad:\"\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RspaceStr300  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RspaceStr300.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.RspaceStr300.ch  length:300  strtype:rightpad  pad:\"' '\"  strict:Y");
    atf_comp::ProcWriteEof(proc);
}

// A signed base-95 numstr and a signed base-256 one: '-' is a digit in
// both bases, so a stored digit string is indistinguishable from a signed
// one, and amc rejects both fields. Both fields left-pad with ' ', which is
// digit 0 in base 95 and digit 32 in base 256, so the base-256 field draws
// a second rejection from the pad check: a left-pad that is a nonzero digit
// of the base strips leading value digits on read.
void atf_comp::comptest_amc_BadNumstrSignedBase() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LspaceStr5_I32_Base95  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LspaceStr5_I32_Base95.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LspaceStr5_I32_Base95.ch  length:5  strtype:leftpad  pad:\"' '\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LspaceStr5_I32_Base95.ch  numtype:i32  base:95  min_len:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.LspaceStr4_I32_Base256  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.LspaceStr4_I32_Base256.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.LspaceStr4_I32_Base256.ch  length:4  strtype:leftpad  pad:\"' '\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.numstr  field:algo_lib.LspaceStr4_I32_Base256.ch  numtype:i32  base:256  min_len:1");
    atf_comp::ProcWriteEof(proc);
}

// One cpbuf ctype with every illegal field number in one run: a duplicate
// within the ctype (two identical decoder case labels: uncompilable),
// number 0 (wire tag 0, reserved as illegal by the protobuf format), and
// 2^29 (overflows the (number<<3)|wire_type tag, aliasing another field).
void atf_comp::comptest_amc_BadPbufFieldNumber() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.a  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.b  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.c  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.d  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.e  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.f  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.g  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.h  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.a  field_number:5  pb_type:int32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.b  field_number:5  pb_type:int32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.c  field_number:0  pb_type:int32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.d  field_number:536870912  pb_type:int32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.e  field_number:19000  pb_type:int32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.f  field_number:19999  pb_type:int32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.g  field_number:18999  pb_type:int32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.h  field_number:20000  pb_type:int32  packed:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// One cpbuf ctype with every arg/reftype shape the pbuf codec cannot
// compile against, rejected in one run: pb_type string on a u32 (the
// lib_pb codec accepts only the pbtype's argtype), pb_type message on a
// non-cpbuf arg, an Inlary field (no wire mapping), and pb_type enum on
// args without the fconst-carrying i32 value shape (one with no value
// field at all, one whose value field is u8 -- the lib_pb varint codec
// binds i32).
void atf_comp::comptest_amc_BadPbufArg() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Color  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Color.value  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:pbt.Color.value/red  value:0  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:pbt.Color.value/green  value:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.b  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.c  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:pbt.Msg.c  min:4  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.d  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.e  arg:pbt.Color  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.a  field_number:1  pb_type:string  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.b  field_number:2  pb_type:message  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.c  field_number:3  pb_type:int32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.d  field_number:4  pb_type:enum  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.e  field_number:5  pb_type:enum  packed:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Two fpbuf rows asking for packed encoding where the protobuf format has
// none: on a singular field, and on a repeated field of a length-delimited
// pb_type. The emitter reads packed only in the repeated non-length-delimited
// branch, so both rows would be silently ignored and the author would get the
// ordinary encoding. The third row, packed on a repeated varint field, is the
// legal shape and is not reported.
void atf_comp::comptest_amc_BadPbufPacked() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Sub  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Sub.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Sub  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Sub.a  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.b  arg:pbt.Sub  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Msg.b  aliased:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.c  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Msg.c  aliased:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.a  field_number:1  pb_type:uint32  packed:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.b  field_number:2  pb_type:message  packed:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.c  field_number:3  pb_type:uint32  packed:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Two fpbuf rows that would generate no wire code at all, rejected rather
// than left silently dead: one on a field of a ctype with no cpbuf row (no
// codec is generated without one), one on a Base field (emission never
// writes a base's fields to the wire).
void atf_comp::comptest_amc_BadPbufNoCodec() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Sub  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Sub.val  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Sub  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Sub.val  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Orphan  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Orphan.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Orphan.a  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.base  arg:pbt.Sub  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.base  field_number:1  pb_type:message  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg2.base  arg:pbt.Sub  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg2.own  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg2  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg2.own  field_number:2  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// The syntax dimension of cpbuf, over four ctypes in one run. cpbuf.syntax
// names a row of dmmeta.pbsyntax, the table that says whether the syntax has
// implicit presence, and a value with no row is rejected: acr -check catches
// the spelling when the row goes through the database, but amc reads a schema
// out of -in_dir too. A misspelling and an empty value are both reported. The
// two accepted values stand as controls, and the report shows what each one
// emits: proto2 writes the scalar unconditionally, proto3 guards it with the
// default-value test.
void atf_comp::comptest_amc_BadPbufSyntax() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'pbt.Proto%..PbufEncode' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Proto2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Proto2.val  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Proto2  syntax:proto2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Proto2.val  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Proto3.val  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Proto3  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Proto3.val  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Misspelled  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Misspelled.val  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Misspelled  syntax:prot3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Misspelled.val  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Unset  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Unset.val  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Unset  syntax:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Unset.val  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// The presence dimension of a oneof, over four ctypes in one run. A oneof is
// a union, and the encoder writes at most one variant only because a store
// into one variant clears the presence bits of its siblings on the same
// pmask. Three shapes break that and are rejected: variants with no pmask at
// all (each guarded by the proto3 default-value test, so every nonzero
// variant goes on the wire), variants split across two pmasks (a store
// clears siblings only on the mask it writes), and a pmask whose explicit
// member list covers one variant and not the other. The first ctype is the
// control, with both variants on one pmask, and it draws no diagnostic.
void atf_comp::comptest_amc_BadPbufOneof() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.pmask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Msg.pmask  filter_print:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.b  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.plain  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.a  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.b  field_number:2  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.plain  field_number:3  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.Msg.a/payload  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.Msg.b/payload  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.NoMask  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.NoMask.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.NoMask.b  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.NoMask  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.NoMask.a  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.NoMask.b  field_number:2  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.NoMask.a/payload  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.NoMask.b/payload  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Split  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Split.pmask1  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Split.pmask1  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Split.pmask2  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Split.pmask2  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Split.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Split.b  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:pbt.Split.pmask1/pbt.Split.a  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:pbt.Split.pmask2/pbt.Split.b  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Split  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Split.a  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Split.b  field_number:2  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.Split.a/payload  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.Split.b/payload  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Half  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Half.pmask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:pbt.Half.pmask  filter_print:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Half.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Half.b  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:pbt.Half.pmask/pbt.Half.a  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Half  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Half.a  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Half.b  field_number:2  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.Half.a/payload  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbufcase  fpbufcase:pbt.Half.b/payload  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// The whole store dimension of one cpbuf ctype in one run: the decoder of a
// singular field with no member of its own stores through the field's
// generated Set, so a shape that has a Get and no Set emits a call to a
// function amc never writes. Three such rows are rejected -- a cppfunc with
// set:N, a substr, a falias -- and the shapes that do have a Set stand
// alongside them as controls drawing no diagnostic: an fbigend field, a
// bitfld, a cppfunc with set:Y, and a plain Val field stored in place. The
// second ctype carries the repeated controls, where the decoder appends
// through Alloc and a fldfunc element type needs no Set at all.
void atf_comp::comptest_amc_BadPbufStore() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:algo  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:algo  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo.Smallstr20  comment:\"inline string with length field\"");
    atf_comp::ProcWrite(proc, "dmmeta.ccmp  ctype:algo.Smallstr20  extrn:N  genop:Y  order:Y  minmax:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.chash  ctype:algo.Smallstr20  hashtype:CRC32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cstr  ctype:algo.Smallstr20  strequiv:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo.Smallstr20  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo.Smallstr20.String  printfmt:Raw  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo.Smallstr20.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcast  field:algo.Smallstr20.ch  expr:algo::strptr  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo.Smallstr20.ch  length:20  strtype:rpascal  pad:\"\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.plain  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.netval  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbigend  field:pbt.Msg.netval  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.flags  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.slice  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:pbt.Msg.slice  offset:0  width:8  srcfield:pbt.Msg.flags  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.computed  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cppfunc  field:pbt.Msg.computed  expr:\"parent.plain\"  print:N  set:Y");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.readonly  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cppfunc  field:pbt.Msg.readonly  expr:\"parent.plain\"  print:N  set:N");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.addr  arg:algo.Smallstr20  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.port  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.substr  field:pbt.Msg.port  expr:/RR  srcfield:pbt.Msg.addr");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.alias  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.falias  field:pbt.Msg.alias  srcfield:pbt.Msg.plain  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.plain  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.netval  field_number:2  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.slice  field_number:3  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.computed  field_number:4  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.readonly  field_number:5  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.port  field_number:6  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.alias  field_number:7  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Rep  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Rep.plain  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Rep.plain  aliased:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Rep.readonly  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:pbt.Rep.readonly  aliased:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cppfunc  field:pbt.Rep.readonly  expr:\"\"  print:N  set:N");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Rep  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Rep.plain  field_number:1  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Rep.readonly  field_number:2  pb_type:uint32  packed:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An fpbuf field whose pb_type has no amcdb.pbtype row: without the table's
// wire type and codec binding the encoder would write the field tag with no
// payload and the decoder case would consume the tag but not the value, so
// amc rejects the field.
void atf_comp::comptest_amc_BadPbufType() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:pbt  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:pbt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pbt.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:pbt.Msg.val  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cpbuf  ctype:pbt.Msg  syntax:proto3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fpbuf  field:pbt.Msg.val  field_number:1  pb_type:int33  packed:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A u8 pmask covering nine member fields: nine presence bits cannot fit in
// the pmask field's eight bits.
void atf_comp::comptest_amc_BadPmaskWidth() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.pmask  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.Rec.pmask  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f0  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f1  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f2  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f3  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f4  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f5  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f6  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f7  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f8  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Explicit pmaskfld_member rows naming, one per row, every field the
// membership rule excludes: a field of another ctype, the Global anchor, the
// pmask word itself, a length field, a type field, and a Base field. None of
// them has a presence bit the pmask's parent struct could back, so amc
// rejects each row by name. Auto-populated members satisfy the rule by
// construction; an explicit row must pass the same validation, and one row
// per exclusion cause keeps every arm of the rule load-bearing. The type
// field and the Base field cannot both sit on one ctype -- a type field is
// declared on the base, a Base field is the deriving ctype's first field --
// so Hdr and Msg carry those two, each with its own pmask.
// The width check counts a rejected row's bit like any other: the membership
// rule reports a row and leaves it on the pmask. This fixture declares the
// pmask a u32 and puts five member rows on it, so the count stays far inside
// the width and the golden carries the membership diagnostics alone.
void atf_comp::comptest_amc_BadPmaskMember() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.pmask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.FDb.pmask  filter_print:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.maxjobs  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.timeout  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.FDb.pmask/algo_lib.FDb._db  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.FDb.pmask/algo_lib.FDb.maxjobs  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sub  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sub.val  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.FDb.pmask/algo_lib.Sub.val  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msglen  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.FDb.msglen  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.FDb.pmask/algo_lib.FDb.pmask  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.FDb.pmask/algo_lib.FDb.msglen  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Hdr  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Hdr.mtype  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.typefld  field:algo_lib.Hdr.mtype  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Hdr.hdrmask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.Hdr.hdrmask  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.Hdr.hdrmask/algo_lib.Hdr.mtype  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.hdr  arg:algo_lib.Hdr  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.Msg  type:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.msgmask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.Msg.msgmask  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.Msg.msgmask/algo_lib.Msg.hdr  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Two ctypes containing each other by value, and one containing itself: no
// finite layout exists, so no struct's size can be computed.
// Each cycle also carries a decimal field, which asks for the arg's numeric
// range before the size computation runs. The range is resolved by walking
// the chain of ctypes whose single Val field names the next one, and that
// walk is what a cycle would send around forever, so the fixture holds the
// two shapes a cycle can take -- a ctype naming itself and a pair naming each
// other. Each field is reported as an arg carrying no numeric range, and the
// run reaches the size computation and ends at the circular definition.
// A bitfield asks the same walk two further questions, and the fixture puts a
// cycle in front of each: the type its accessors hand back, which the
// bitfield's own arg names, and the type its mask operations run on, which
// its source field's arg names. Neither question turns the cycle into an
// internal error; the run still ends at the circular definition.
void atf_comp::comptest_amc_BadSizeCycle() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CycA  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CycB  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CycA.b  arg:algo_lib.CycB  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CycB.a  arg:algo_lib.CycA  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.CycA.b  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CycSelf  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CycSelf.self  arg:algo_lib.CycSelf  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fdec  field:algo_lib.CycSelf.self  nplace:2  fixedfmt:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CycBit  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CycBit.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CycBit.slice  arg:algo_lib.CycSelf  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.CycBit.slice  offset:0  width:4  srcfield:algo_lib.CycBit.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.CycBitSrc  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CycBitSrc.word  arg:algo_lib.CycSelf  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.CycBitSrc.slice  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.CycBitSrc.slice  offset:0  width:4  srcfield:algo_lib.CycBitSrc.word  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An Inlary of 65537 elements, each itself a 65536-byte fixed Inlary: the
// total byte size exceeds the i32 range amc computes sizes in.
void atf_comp::comptest_amc_BadSizeOverflow() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Big  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Big.buf  arg:u64  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Big.buf  min:8192  max:8192  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Huge  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Huge.big  arg:algo_lib.Big  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Huge.big  min:65537  max:65537  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A fixed field after a Varlen field: the varlen data begins at the end of
// the struct, so writes through the varlen accessors would clobber the
// trailing fixed member. The schema is rejected with a nonzero exit, the
// offender attributed to the varlen field it follows.
//
// Only a field that declares a member of its own can be clobbered, so the
// fixture places one field of each shape after the varlen field and the
// golden is the resulting accept/reject table. Reported: a Val and a Ptr,
// which declare members directly, and a ZSListMT, whose read-side and
// write-side list heads are two more members of the parent. Not reported:
// a Bitfld, which re-slices the word of a field declared before the varlen
// data; the three fldfunc kinds, whose value is computed rather than stored
// -- a cppfunc, a falias and a substr; Malloc, Global and Cppstack, whose
// generators put no member in the struct at all; and an Opt, which draws a
// comment line where its optional trailing element would sit. Count and Exec are
// the intermediate case -- the declared field draws nothing, while the Val
// children amc generates from it are reported in their own right, which is
// why cnt_n and the child_* rows appear and cnt and child do not.
//
// The Opt is the one field here that a second rule answers as well: it
// claims the end of the fixed portion that the varlen data already claims,
// so the golden carries its amc.trail_overlay row, and the absence of an
// amc.varlen_last row beside it is what pins the Opt as declaring no
// storage.
void atf_comp::comptest_amc_BadVarlenLast() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sub  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sub.val  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Sub.Argv  printfmt:Auto  read:Y  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Blob  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.n  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.data  arg:u8  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.blob  arg:algo_lib.Blob  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.crc  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.ptr  arg:algo_lib.Blob  reftype:Ptr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.cnt  arg:algo_lib.Sub  reftype:Count  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.child  arg:algo_lib.Sub  reftype:Exec  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.flag  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Blob.flag  offset:0  width:8  srcfield:algo_lib.Blob.n  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.mem  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.glob  arg:u32  reftype:Global  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.stack  arg:u32  reftype:Cppstack  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.zs_mt  arg:algo_lib.Sub  reftype:ZSListMT  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.calc  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cppfunc  field:algo_lib.Blob.calc  expr:\"parent.n + 1\"  print:N  set:N");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.nn  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.falias  field:algo_lib.Blob.nn  srcfield:algo_lib.Blob.n  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.half  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.substr  field:algo_lib.Blob.half  expr:/RL  srcfield:algo_lib.Blob.n");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.opt  arg:algo_lib.Sub  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Fields that begin at the end of the fixed portion: varlen data and an
// optional trailing element both start there, and the generated accessors
// would overlay one on the other. Only varlen fields carry the end offset
// that lets one begin where another ends, so a ctype may claim that address
// once, and each further claim is rejected with a nonzero exit.
// The fixture pins the rule over the pairs a ctype can declare. Rejected:
// Varlen with Opt in both declaration orders, one ctype each, and a ctype
// with two Opt fields, whose getters would otherwise return the same address
// twice. A fourth ctype declares an Opt ahead of two varlen fields and draws
// one row, not two: the first varlen field is the one that begins at the
// disputed address, and the second begins where the first ends, so a single
// overlay is reported once however many varlen fields trail it.
// Each rejected ctype draws exactly one diagnostic -- the order of a
// Varlen and an Opt matters to the separate rule that fixed fields may not
// follow a varlen field, and reporting the Opt there would state the same
// overlay twice. The controls draw none: a ctype with one Opt, a ctype with
// one Varlen, a ctype with two Varlen fields -- the accepting arm of the
// rule, the second beginning where the first ends -- and a ctype with
// neither. The two-Varlen control carries no pool instance, since a pool arg
// with more than one varlen field is refused by the pool generator, whose
// diagnostic would then arrive alongside the ones this test is about.
void atf_comp::comptest_amc_BadVarlenOpt() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Blob  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.n  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Blob.n  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Tail  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Tail.t  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.tail  arg:algo_lib.Tail  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.data  arg:u8  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.blob  arg:algo_lib.Blob  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rblob  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rblob.n  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Rblob.n  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rblob.data  arg:u8  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rblob.tail  arg:algo_lib.Tail  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.rblob  arg:algo_lib.Rblob  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Oblob  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Oblob.n  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Oblob.tail  arg:algo_lib.Tail  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Oblob.tail2  arg:algo_lib.Tail  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.oblob  arg:algo_lib.Oblob  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sblob  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sblob.n  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sblob.tail  arg:algo_lib.Tail  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.sblob  arg:algo_lib.Sblob  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Vblob  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Vblob.n  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Vblob.n  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Vblob.data  arg:u8  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.vblob  arg:algo_lib.Vblob  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Dblob  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Dblob.n  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Dblob.n  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Dblob.tail  arg:algo_lib.Tail  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Dblob.data  arg:u8  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Dblob.data2  arg:u8  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Wblob  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Wblob.n  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Wblob.n  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Wblob.data  arg:u8  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Wblob.data2  arg:u8  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A non-varlen pool instance of a varlen ctype: the pool allocates fixed-size
// records, so the trailing varlen data has nowhere to live. The offending
// instance is reported and the schema is rejected with a nonzero exit.
void atf_comp::comptest_amc_BadInfinityPool() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Blob  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.n  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.data  arg:u8  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.blob  arg:algo_lib.Blob  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A pool arg with a varlen field but no lenfld: the element count is an
// extern user function, and Delete computes the total length from it. The
// query prints the generated Delete; the length statement must be
// well-formed (no stray semicolon inside the length expression).
void atf_comp::comptest_amc_PoolVarlenExtern() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N algo_lib.FDb.blob.Delete < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Blob  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.n  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Blob.data  arg:u8  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.blob  arg:algo_lib.Blob  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A pooled ctype whose fixed size plus lenfld extra is not a multiple of the
// lenfld scale: pool alloc stores len = (size + extra) / scale, which
// truncates, and the reader reconstructs less than was allocated.
void atf_comp::comptest_amc_BadLenfldScale() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.len  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.pad  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg.len  extra:0  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msg  arg:algo_lib.Msg  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A pool-less message ctype whose fixed size is not a multiple of the lenfld
// scale: the message constructor and the field default store the same
// truncating length word pool alloc would, so the divisibility check must
// cover message ctypes, not only pooled ones.
void atf_comp::comptest_amc_BadLenfldMsgtype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.len  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.pad  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg.len  extra:0  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.Msg  type:1");
    atf_comp::ProcWriteEof(proc);
}

// A fixed-only ctype whose only length store is its Fmt* constructor (a pnew
// row, no msgtype, no pool instance): the constructor's runtime total is
// always the fixed size, so an indivisible size + extra stores the same
// truncating length word pool alloc would, and the divisibility check must
// cover pnew carriers too.
void atf_comp::comptest_amc_BadLenfldPnew() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.len  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.pad  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg.len  extra:0  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.pnew  pnew:algo_lib/algo_lib.Msg.ByteAry  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Three lenflds with scale:0 on ctypes whose Opt fields consume the length
// expression: every offender is reported in one run through the accumulate
// path, not a single throw at the first length-expression consumer. MsgC adds
// the pooled shape with a u64 length word, the one that reaches the pool
// InsertMaybe raw-word bound -- that bound divides by the scale, and it is
// emitted a whole gen step before gen_check_lenfld reports the scale, so
// without its own scale test the run would die on SIGFPE with no diagnostic
// at all. The cloned base field (FMsgC.len) reports the row a second time.
void atf_comp::comptest_amc_BadLenfldZeroScale() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Byte  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Byte.val  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgA  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgA.len  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgA.len  extra:0  scale:0");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgA.tail  arg:algo_lib.Byte  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgB  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgB.len  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgB.len  extra:0  scale:0");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgB.tail  arg:algo_lib.Byte  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgC  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgC.len  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgC.len  extra:0  scale:0");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgC.tail  arg:algo_lib.Byte  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgC  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgC.base  arg:algo_lib.MsgC  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgc  arg:algo_lib.FMsgC  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// The declared-type dimension of the length field, one ctype per case. A
// length word needs a numeric range: GetMinMax resolves one for the
// fixed-width integers u8 through i64 and for nothing else, and every range
// check built from the widest-signed fallback runs vacuously (the TS encoder's
// range guard is omitted outright). So u128 is refused for having no
// arithmetic amc can do, bool for holding no magnitude, and char for having
// the compiler's choice of signedness -- three rejections in one run. MsgOk
// carries the same shape over a u16 word and draws nothing, which is what
// makes the three above a rejection of the type rather than of the shape.
void atf_comp::comptest_amc_BadLenfldType() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.len  arg:u128  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.pad  arg:u128  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.Msg  type:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBool  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBool.len  arg:bool  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBool.pad  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBool.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.MsgBool  type:2");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgChar  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgChar.len  arg:char  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgChar.pad  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgChar.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.MsgChar  type:3");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgOk  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.len  arg:u16  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.pad  arg:u16  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgOk.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.MsgOk  type:4");
    atf_comp::ProcWriteEof(proc);
}

// A u8 lenfld (scale 4) on a pooled + pnew ctype: every store of a runtime
// total guards the length field's storable range, not just scale
// divisibility -- a total beyond the range would wrap mod 2^N through the
// word's own type, framing a shorter message than was written. The pool's
// AllocExtraMaybe bounds the caller's addon (range minus the fixed portion,
// as a compile-time ssizeof expression) and the Fmt* constructor refuses
// the total (NULL), mirroring the TS encoder's RangeError guard.
// MsgW is the same shape with a wide length field (u32, scale 1): its range
// covers every frame length, so no lenfld test is emitted, and what remains
// is the frame length domain itself. Its buffer is an Alloc, whose size
// argument is an int, so the emitted bound is the i32 maximum and the totals
// it accepts are exactly those the allocator can be asked for.
void atf_comp::comptest_amc_LenfldNarrow() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N '%msgn.AllocExtraMaybe|%MsgN_FmtByteAry|%MsgW_FmtAlloc' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgN  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgN.len  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgN.len  extra:0  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgN.p1  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgN.p2  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgN.p3  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgN.data  arg:u32  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgn  arg:algo_lib.MsgN  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pnew  pnew:algo_lib/algo_lib.MsgN.ByteAry  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgW  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgW  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgW.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgW.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgW.data  arg:u8  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pnew  pnew:algo_lib/algo_lib.MsgW.Alloc  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Two jstype ctypes sharing one u32 lenfld shape, differing only in whether
// the frame length is a compile-time constant. MsgFix is fixed-size, so the
// emitted `length` is a literal and gen_check_lenfld has already proven it
// storable: the encoder carries no range guard at all. MsgVar owns a varlen
// tail, so its length is a runtime sum and the guard stays. The test writes
// real output (amc runs from the checkout root with -out_dir under the test's
// tempdir, so the derived-table acr child writes into that tempdir too) and
// pins both emitted TypeScript encoders.
void atf_comp::comptest_amc_JsFixedFrame() {
    atf_comp::FProc &proc = atf_comp::ProcStart("bash -c 'mkdir -p $tempdir/o/ts/gen $tempdir/o/include/gen $tempdir/o/cpp/gen $tempdir/o/data"
                                                " && $bindir/amc -in_dir:- -out_dir:$tempdir/o -report:N < <(cat test/amc/bootstrap.ssim -)"
                                                " && sed -n \"/_Encode/,/^}/p\" $tempdir/o/ts/gen/algo_lib_gen.ts'");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nsjs  ns:algo_lib  typescript:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgFix  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgFix  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgFix  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgFix.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgFix.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgFix.val  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgVar  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgVar  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgVar  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgVar.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgVar.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgVar.data  arg:u8  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Varlen pools with InsertMaybe: the addon count of an untrusted source
// struct is computed in i64 domain. A multiply in the length word's own
// type would wrap a huge corrupt stored value mod 2^N into a small
// positive count that passes the negative-addon guard and reads past the
// source message; the widened arithmetic keeps the true total visible,
// and totals outside [fixed size, i32 max] return NULL. That widening is
// itself exact only while the word's range fits u32. Beyond it the i64
// expansion can overflow -- with scale > 1 in the multiply, and even at
// scale 1 in the extra adjustment after the wrap -- undefined behavior an
// optimizer may exploit to elide the range test -- so a wider word is
// first bounded by the largest value whose expanded total fits the i32
// frame-length domain, a generation-time constant.
//
// The fixture is the rule's table, one pool ctype per cell. Which arm
// applies turns on the word's largest storable value, so the accepted
// edges sit either side of u32's max: MsgU32 (u32, scale 4), MsgI32
// (i32) and MsgBit32 (u64 word masked to 32 bits by its Bitfld width)
// draw no bound, while MsgBit33 (masked to 33 bits), MsgU64 (u64,
// scale 2) and MsgI64 (i64, scale 8) do. A signed word is the cell
// where the bound rather than the addon test rejects a negative stored
// length, through the u64 conversion in the emitted test. The bound's
// constant follows scale and extra: MsgNeg carries extra -2 and MsgPos
// extra 4, each shifting the constant by the reader formula.
//
// InsertMaybe reaches the allocation through the die-on-fail AllocExtra,
// so a count AllocExtraMaybe would refuse kills the process instead of
// returning NULL, and the two accept exactly the same counts. Both
// functions are therefore printed for every pool in the fixture, and a
// term emitted into one and not the other shows up as a diff here. Two
// cells of the table make that agreement worth pinning: the bound's
// constant is the row type's, not the base type's, which msg6 and msg5
// show as block pools whose row carries a buffer member the base does
// not; and a word storing totals past i32 max leaves the length field
// with no bound to give, so every such pool bounds the count by the i32
// a frame length is read back through instead.
void atf_comp::comptest_amc_PoolInsertScale() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.FDb.msg%.(InsertMaybe|AllocExtraMaybe)' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sub  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sub.val  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgU32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgU32.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgU32.len  extra:0  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgU32.sub  arg:algo_lib.Sub  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgU32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgU32.base  arg:algo_lib.MsgU32  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgu32  arg:algo_lib.FMsgU32  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgI32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgI32.len  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgI32.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgI32.sub  arg:algo_lib.Sub  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgI32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgI32.base  arg:algo_lib.MsgI32  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgi32  arg:algo_lib.FMsgI32  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBit32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBit32.word  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBit32.len  arg:u64  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.MsgBit32.len  offset:0  width:32  srcfield:algo_lib.MsgBit32.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBit32.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBit32.sub  arg:algo_lib.Sub  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgBit32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgBit32.base  arg:algo_lib.MsgBit32  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgbit32  arg:algo_lib.FMsgBit32  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBit33  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBit33.word  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBit33.len  arg:u64  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.MsgBit33.len  offset:0  width:33  srcfield:algo_lib.MsgBit33.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBit33.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBit33.sub  arg:algo_lib.Sub  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgBit33  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgBit33.base  arg:algo_lib.MsgBit33  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgbit33  arg:algo_lib.FMsgBit33  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgU64  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgU64.len  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgU64.len  extra:0  scale:2");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgU64.sub  arg:algo_lib.Sub  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgU64  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgU64.base  arg:algo_lib.MsgU64  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgu64  arg:algo_lib.FMsgU64  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgI64  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgI64.len  arg:i64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgI64.len  extra:0  scale:8");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgI64.sub  arg:algo_lib.Sub  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgI64  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgI64.base  arg:algo_lib.MsgI64  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgi64  arg:algo_lib.FMsgI64  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgNeg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNeg.len  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgNeg.len  extra:-2  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNeg.sub  arg:algo_lib.Sub  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgNeg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgNeg.base  arg:algo_lib.MsgNeg  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgneg  arg:algo_lib.FMsgNeg  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgPos  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgPos.len  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgPos.len  extra:4  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgPos.sub  arg:algo_lib.Sub  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsgPos  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsgPos.base  arg:algo_lib.MsgPos  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msgpos  arg:algo_lib.FMsgPos  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg3.len  arg:i32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg3.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg3.sub  arg:algo_lib.Sub  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsg3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsg3.base  arg:algo_lib.Msg3  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msg3  arg:algo_lib.FMsg3  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg6  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg6.len  arg:u16  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg6.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg6.sub  arg:algo_lib.Sub  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsg6  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsg6.base  arg:algo_lib.Msg6  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msg6  arg:algo_lib.FMsg6  reftype:Blkpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.basepool  field:algo_lib.FDb.msg6  base:algo_lib.FDb.malloc");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg5  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg5.len  arg:u16  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg5.len  extra:0  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg5.pad  arg:u16  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg5.sub  arg:algo_lib.Sub  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FMsg5  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FMsg5.base  arg:algo_lib.Msg5  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msg5  arg:algo_lib.FMsg5  reftype:Blkpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.basepool  field:algo_lib.FDb.msg5  base:algo_lib.FDb.malloc");
    atf_comp::ProcWriteEof(proc);
}

// Two Bitfld lenflds whose stores mask to the declared width: MsgWidth
// (width 4, u32 source word, msgtype store, fixed size 20) can store at
// most 15, so the minimum frame's length word is unstorable; MsgExtra
// (width 6, extra:252) can store at most 63, so no frame length is
// representable at all. Both range checks must bound by the width-limited
// max, not the source word's full range, and report in one run.
void atf_comp::comptest_amc_BadLenfldBitfld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgWidth  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgWidth.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgWidth.len  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.MsgWidth.len  offset:0  width:4  srcfield:algo_lib.MsgWidth.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgWidth.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgWidth.p1  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgWidth.p2  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgWidth.p3  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgWidth.p4  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.MsgWidth  type:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgExtra  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgExtra.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgExtra.len  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.MsgExtra.len  offset:0  width:6  srcfield:algo_lib.MsgExtra.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgExtra.len  extra:252  scale:1");
    atf_comp::ProcWriteEof(proc);
}

// A u8 lenfld with extra:256: every value the type can hold maps to a
// negative frame length, so no frame at all is representable. Left
// unrejected, the TS encoder's range-guard bound maxval*scale - extra
// wraps in u64 and the emitted guard never fires; the schema must be
// refused at generation time instead.
void atf_comp::comptest_amc_BadLenfldExtra() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.len  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.pad  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg.len  extra:256  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msg  arg:algo_lib.Msg  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Two packed jstype ctypes whose fixed size plus lenfld extra falls outside
// the length field's range, with no msgtype and no C++ alloc instance: the
// generated TypeScript Encode is their only store site. MsgLow (u32 lenfld,
// extra:-8, fixed size 4) would wrap the minimal frame's stored length
// through setUint32; MsgHigh (u8 lenfld, extra:255, fixed size 4) would get
// a range guard that throws on every encode. Both offenders are rejected in
// one run.
void atf_comp::comptest_amc_BadLenfldJstype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nsjs  ns:algo_lib  typescript:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgLow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgLow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgLow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgLow.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgLow.len  extra:-8  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgHigh  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgHigh  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgHigh  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.len  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.pad1  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.pad2  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.pad3  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgHigh.len  extra:255  scale:1");
    atf_comp::ProcWriteEof(proc);
}

// The wire-form dimension of a packed jstype ctype's fields: a field the
// TypeScript codec can neither encode nor decode is refused, and every
// offender is named in one run. MsgBad carries three -- an inlary of char and
// an inlary of a cstr ctype, both of which the class projects as `string`
// with no byte-copy form, and a Smallstr, a storage reftype the codec maps
// nowhere -- so the walk cannot be stopping at the first. MsgBadOpt adds the
// fourth, an Opt of the same cstr ctype, which is the same byte-copy rule
// read on the other reftype that uses it. MsgBadInlary adds the fifth, an
// inlary of u8 with min:0 max:8: the class projects as a byte array, but a
// variable inline array stores a live element count after its elements, so
// its width is not max elements and the count has no member to carry it.
// MsgBadVal adds the sixth, a Val of a ctype that declares a Varlen: the
// nested codec appends a tail past the width the outer walk steps over.
// MsgBadValOpt adds the seventh, the same rule read on a nested Opt tail.
// MsgBadVarlen adds the eighth, a Varlen of the cstr ctype, whose member
// projects as a string while its tail is a run of records rather than text.
// MsgTagged closes the list with the ninth and tenth, two inline arrays of
// char on a ctype that carries a length word and nothing else.
//
// MsgNolen and MsgNolenOpt read the other dimension, the ctype's own: each
// declares a tail -- two Varlen fields, and an Opt -- with no length field to
// frame it, and each draws amc.jstype_lenfld. MsgFixed is that rule's control,
// a packed jstype ctype with no tail and no length field, and it draws
// nothing.
//
// The rest of the fixture is the accepted side of that table and draws
// nothing: MsgOk carries a Base, a lenfld, a plain Val, a Bitfld, a Val of the
// cstr ctype (a fixed-length string does have a wire form), a Val of a
// tail-free ctype, an inlary of u8, and the three accepted Varlen args -- char
// for the text tail, u8 and a plain ctype for the byte tail; MsgOpt carries an
// Opt whose arg maps to a byte array.
// The two remaining ctypes fix the check's scope: MsgUnpacked and MsgNojs both
// carry the offending inlary of char, and neither is checked, because a ctype
// with no pack row and a ctype with no jstype row get no codec to be wrong.
//
// The run writes into $tempdir, which holds none of the gen directories amc
// writes to, so a generated file would report amc.outfile_write naming the
// path it could not create. No such line appears, and that absence is the
// assertion this run makes about ordering: check_jstype sits ahead of every
// emitting gen, so a refused schema is refused before the first write and no
// decoder is emitted for the fields it accepted either.
void atf_comp::comptest_amc_BadJstypeWire() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir:$tempdir -report:N < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nsjs  ns:algo_lib  typescript:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Str4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Str4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cstr  ctype:algo_lib.Str4  strequiv:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Str4.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Str4.ch  length:4  strtype:rpascal  pad:\"\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Hdr  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Hdr  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Hdr.magic  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Tail  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Tail  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Tail.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Tail.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Tail.text  arg:char  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgOk  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgOk  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgOk  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.base  arg:algo_lib.Hdr  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgOk.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.flags  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.lo  arg:u8  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.MsgOk.lo  offset:0  width:4  srcfield:algo_lib.MsgOk.flags  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.str  arg:algo_lib.Str4  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.hdr  arg:algo_lib.Hdr  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.data  arg:u8  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgOk.data  min:4  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.text  arg:char  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.blob  arg:u8  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOk.rec  arg:algo_lib.Hdr  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgOpt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgOpt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgOpt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOpt.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgOpt.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgOpt.body  arg:algo_lib.Hdr  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBad  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgBad  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgBad  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBad.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBad.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBad.name  arg:char  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgBad.name  min:8  max:8  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBad.tag  arg:algo_lib.Str4  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgBad.tag  min:2  max:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBad.label  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.MsgBad.label  length:4  strtype:rpascal  pad:\"\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBad.seq  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBadOpt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgBadOpt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgBadOpt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadOpt.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBadOpt.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadOpt.body  arg:algo_lib.Str4  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBadInlary  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgBadInlary  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgBadInlary  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadInlary.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBadInlary.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadInlary.data  arg:u8  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgBadInlary.data  min:0  max:8  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadInlary.seq  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBadVal  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgBadVal  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgBadVal  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadVal.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBadVal.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadVal.inner  arg:algo_lib.Tail  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadVal.seq  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBadValOpt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgBadValOpt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgBadValOpt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadValOpt.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBadValOpt.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadValOpt.inner  arg:algo_lib.MsgOpt  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadValOpt.seq  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgBadVarlen  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgBadVarlen  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgBadVarlen  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadVarlen.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgBadVarlen.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgBadVarlen.body  arg:algo_lib.Str4  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgNolen  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgNolen  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgNolen  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNolen.magic  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNolen.text  arg:char  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNolen.more  arg:char  reftype:Varlen  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgNolenOpt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgNolenOpt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgNolenOpt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNolenOpt.magic  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNolenOpt.body  arg:algo_lib.Hdr  reftype:Opt  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgFixed  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgFixed  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgFixed  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgFixed.magic  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgUnpacked  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgUnpacked  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgUnpacked.name  arg:char  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgUnpacked.name  min:8  max:8  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgNojs  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgNojs  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgNojs.name  arg:char  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgNojs.name  min:8  max:8  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgTagged  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgTagged  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgTagged  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgTagged.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgTagged.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgTagged.tag  arg:char  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgTagged.tag  min:4  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgTagged.name  arg:char  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.MsgTagged.name  min:8  max:8  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Two store-carrying ctypes whose fixed size plus lenfld extra falls outside
// the length field's range. MsgLow (u32 lenfld, extra:-8, fixed size 4): the
// minimum frame's stored length goes negative and wraps through the unsigned
// store, framing gigabytes from a 4-byte message. MsgHigh (u8 lenfld,
// extra:255, fixed size 4): the stored value 259 exceeds the type's max, so
// the emitted range guard bound falls below the fixed size and every encode
// throws at first use. Both offenders are rejected in one run.
void atf_comp::comptest_amc_BadLenfldMinFrame() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgLow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgLow.len  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgLow.len  extra:-8  scale:4");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msglow  arg:algo_lib.MsgLow  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgHigh  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.len  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.pad1  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.pad2  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgHigh.pad3  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgHigh.len  extra:255  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.msghigh  arg:algo_lib.MsgHigh  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// Two packed jstype ctypes whose lenfld sits on a Bitfld field over a raw
// header word, with no msgtype and no C++ alloc instance -- header-like
// framing for an external payload. The TS fixed walk writes the source word
// raw from the parent and emits no store for a Bitfld lenfld, so per the
// no-store-site doctrine the length words arrive from outside and the
// schemas stay legal: MsgIndiv (size 4, scale:8) is indivisible and MsgRange
// (u8 lenfld, extra:252, size 4) has an unstorable minimum frame, yet amc
// accepts both with exit code 0.
void atf_comp::comptest_amc_LenfldBitfld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nsjs  ns:algo_lib  typescript:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgIndiv  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgIndiv  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgIndiv  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgIndiv.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgIndiv.len  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.MsgIndiv.len  offset:8  width:24  srcfield:algo_lib.MsgIndiv.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgIndiv.len  extra:0  scale:8");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.MsgRange  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.jstype  ctype:algo_lib.MsgRange  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.MsgRange  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgRange.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.MsgRange.len  arg:u8  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.MsgRange.len  offset:0  width:8  srcfield:algo_lib.MsgRange.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.MsgRange.len  extra:252  scale:1");
    atf_comp::ProcWriteEof(proc);
}

// A packed message based on a private header ctype that has no dmmeta.pack
// row -- the state an author lands in after repointing a message's base from
// ams.MsgHeader to a freshly created header; the namespace is itself pack:Y,
// so the unpacked header also draws the namespace-level error. Pins both
// rejections (amc.back_pack, amc.what_the_pack), each followed by the bare
// dmmeta.pack row the message tells the author to insert.
void atf_comp::comptest_amc_MissingPack() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ttt  nstype:protocol  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nsx  ns:ttt  genthrow:N  correct_getorcreate:N  pool:algo_lib.FDb.malloc  sortxref:N  pack:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nscpp  ns:ttt  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:ttt.EventHeader  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ttt.EventHeader.type  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ttt.EventHeader.length  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.typefld  field:ttt.EventHeader.type  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:ttt.EventHeader.length  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:ttt.Event  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ttt.Event.base  arg:ttt.EventHeader  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:ttt.Event  type:3001");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:ttt.Event  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A pmask field of 536870913 bytes: its bit count exceeds i32, which the
// struct size cap (i32 with alignment headroom) permits. The bit-width
// bookkeeping must not wrap; the schema is legal and generates cleanly.
void atf_comp::comptest_amc_PmaskGiantField() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.pmask  arg:u8  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.pmask  min:536870913  max:536870913  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.Rec.pmask  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f0  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f1  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f2  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f3  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f4  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f5  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f6  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f7  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.f8  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// csize rows with alignment 32 (beyond the headroom the struct size cap
// reserves for RoundUp) and alignment 6 (not a power of two, so the rounding
// mask miscomputes): both must be rejected, not silently accepted.
void atf_comp::comptest_amc_BadCsizeAlignment() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Avx  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:algo_lib.Avx  size:64  alignment:32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Odd  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:algo_lib.Odd  size:12  alignment:6  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A variable char/u8 inlary with min>0: the string read copies the input
// and sets the count to its length, which a short input puts below min.
// The shape is rejected until someone defines floor semantics for it.
void atf_comp::comptest_amc_BadInlaryMin() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestInlaryMin  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestInlaryMin.str  arg:char  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.TestInlaryMin.str  min:2  max:8  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestInlaryMin.buf  arg:u8  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.TestInlaryMin.buf  min:1  max:4  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An inlary with min>max: Init preallocates min elements against a
// capacity of max, so the first record constructed would die at runtime.
// amc rejects the shape instead of shipping the trap.
void atf_comp::comptest_amc_BadInlaryMinmax() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestInlaryMinmax  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestInlaryMinmax.val  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.TestInlaryMinmax.val  min:5  max:3  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A variable inlary with fnoremove: Setary, the separated-string read, and
// Uninit all empty the array through RemoveAll, which fnoremove suppresses,
// so the generated code would call a function that is never emitted.
void atf_comp::comptest_amc_BadInlaryFnoremove() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestInlary  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestInlary.val  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.TestInlary.val  min:0  max:3  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fnoremove  field:algo_lib.TestInlary.val  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A Json cfmt over fields no FmtJson branch covers. Two causes: a reftype the
// walk has no branch for -- a raw Smallstr field, shown both as a ctype's
// single datafld and among several -- and a field type carrying neither a Json
// cfmt nor a String print, so no expression exists to produce its node, shown
// singular and repeated. Every offender is rejected in one run through the
// accumulate path.
void atf_comp::comptest_amc_BadJsonFld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Name  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Name.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Name.ch  length:20  strtype:rpascal  pad:\"\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Name.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.name  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Rec.name  length:20  strtype:rpascal  pad:\"\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Rec.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Opaque  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Opaque.id  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RecVal  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecVal.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecVal.opaque  arg:algo_lib.Opaque  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RecVal.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RecAry  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecAry.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecAry.opaque  arg:algo_lib.Opaque  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.RecAry.opaque  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RecAry.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.InlOpaque  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.InlOpaque.v  arg:algo_lib.Opaque  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.InlOpaque.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.InlAryOpaque  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.InlAryOpaque.v  arg:algo_lib.Opaque  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.InlAryOpaque.v  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.InlAryOpaque.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// How each field of a Json cfmt reaches its node. A field whose type carries
// its own Json cfmt gets that type's FmtJson; a field whose type carries only
// a String cfmt prints into a string node instead. A repeated field becomes an
// array node holding one element node per element, by the same two rules.
// A ctype with one data field prints as that field's node, with no object
// wrapper, and the same rules pick the node there: the field type's FmtJson,
// or a string node its Print writes into, and for a repeated field an array
// node holding one element node per element.
// One ctype is the case where the two field selectors used to disagree: its
// only data field is a Val, so the object wrapper is elided and no object node
// exists, and its other field is a repeated fldfunc, which the array branch
// would have emitted an array node into.
// The remaining ctypes carry a print filter, which prints a field only when
// its bit is set. PmFilter shows the three member forms each inside a test of
// their bit, and its presence mask is a data field itself, which by itself
// keeps the ctype from inlining. PmInlJson and PmInlPrint are the shapes where
// the mask is a bitfield of the one data field: that field carries a bit, so
// it is not inlined and prints as an object member under its test -- the first
// through its type's FmtJson, the second through its type's Print. PmInlJson
// carries a second bitfield on the mask, and a bitfield prints nothing, so no
// test is opened around it. PmInlFree keeps the same shape but names its
// member explicitly, leaving the data field off the mask, and that field
// inlines as it would with no filter at all.
void atf_comp::comptest_amc_JsonFld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N algo_lib.%.FmtJson < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:u32.Json  printfmt:Extern  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Str  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Str.id  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Str.String  printfmt:Raw  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Multi  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Multi.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Multi.s  arg:algo_lib.Str  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Multi.ja  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.Multi.ja  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Multi.sa  arg:algo_lib.Str  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.Multi.sa  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Multi.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Computed  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Computed.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Computed.derived  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.Computed.derived  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cppfunc  field:algo_lib.Computed.derived  expr:\"\"  print:N  set:N");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Computed.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.InlJson  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.InlJson.v  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.InlJson.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.InlPrint  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.InlPrint.v  arg:algo_lib.Str  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.InlPrint.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.InlAryJson  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.InlAryJson.v  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.InlAryJson.v  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.InlAryJson.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.InlAryPrint  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.InlAryPrint.v  arg:algo_lib.Str  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.InlAryPrint.v  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.InlAryPrint.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.PmFilter  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmFilter.a  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmFilter.s  arg:algo_lib.Str  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmFilter.ja  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.PmFilter.ja  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmFilter.pmask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.PmFilter.pmask  filter_print:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.PmFilter.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.PmInlJson  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlJson.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlJson.pm  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.PmInlJson.pm  offset:0  width:8  srcfield:algo_lib.PmInlJson.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.PmInlJson.pm  filter_print:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlJson.flag  arg:bool  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.PmInlJson.flag  offset:8  width:1  srcfield:algo_lib.PmInlJson.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.PmInlJson.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.PmInlPrint  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlPrint.word  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlPrint.pm  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.PmInlPrint.pm  offset:0  width:8  srcfield:algo_lib.PmInlPrint.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.PmInlPrint.pm  filter_print:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.PmInlPrint.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.PmInlFree  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlFree.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlFree.pm  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.PmInlFree.pm  offset:0  width:8  srcfield:algo_lib.PmInlFree.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.PmInlFree.pm  filter_print:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.PmInlFree.flag  arg:bool  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.PmInlFree.flag  offset:8  width:1  srcfield:algo_lib.PmInlFree.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld_member  pmaskfld_member:algo_lib.PmInlFree.pm/algo_lib.PmInlFree.flag  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.PmInlFree.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A Json cfmt on a base-derived ctype and on a bitfield-carrying ctype:
// neither the Base field nor the Bitfld field prints — the base's cloned
// fields print as members of the object, and the bitfield's bits travel
// inside the printed source word — so both schemas generate cleanly.
// Pins both emitted FmtJson bodies.
void atf_comp::comptest_amc_JsonBaseBitfld() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N %.FmtJson < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.BaseType  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.BaseType.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.BaseType.count  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Derived  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Derived.base  arg:algo_lib.BaseType  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Derived.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Flags  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Flags.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Flags.bit0  arg:bool  reftype:Bitfld  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Flags.bit0  offset:0  width:1  srcfield:algo_lib.Flags.value  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Flags.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A Json cfmt over the whole rendering table: a field renders through the
// Json cfmt of its type when the type has one, and otherwise as a string
// holding what its Print writes; an array field renders as a JSON array
// whose elements render the same two ways; and a record standing on a single
// data field renders as that field's own node, so an array there is the
// record's array and a value there is the record's value. AryPlain, AryElem,
// ValPlain and ValElem are the four single-field cells, Rec the two array
// cells as members of an object beside a plain value.
// RecName is the naming control, and every one of its fields is named for a
// local the walk declares. A local named after a field takes that name through
// one role suffix -- "_node" for the node standing for the field, "_ary" for
// its element span -- and the record's own node is named objnode, which no
// suffix produces. So the field named "objnode" reaches for the record's node,
// the field named "x_field" reaches for the node of the field named "x", and
// the field named "y_ary" reaches for the element span of the field named "y",
// and the emitted body declares each of them once. The field named "object"
// pins the same rule from the other side: the record's node no longer answers
// to that name, so a field may.
void atf_comp::comptest_amc_JsonAry() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N %.FmtJson < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Elem  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Elem.n  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Elem.m  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Elem.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.AryPlain  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.AryPlain.buf  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.AryPlain.buf  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.AryPlain.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.AryElem  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.AryElem.rec  arg:algo_lib.Elem  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.AryElem.rec  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.AryElem.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.n  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.buf  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.buf  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.rec  arg:algo_lib.Elem  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.Rec.rec  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Rec.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.ValPlain  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.ValPlain.n  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.ValPlain.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.ValElem  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.ValElem.rec  arg:algo_lib.Elem  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.ValElem.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.RecName  comment:\"field names that could collide with the walk's own locals\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecName.object  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"the object node's name, before it moved out of reach\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.RecName.object  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecName.objnode  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"the object node's name\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.RecName.objnode  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecName.x  arg:algo_lib.Elem  reftype:Val  dflt:\"\"  comment:\"prints through its own node, named for this field\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecName.x_field  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"the name the field above once gave its node\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.RecName.x_field  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecName.y  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"its element span carries the name of the field below\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.RecName.y  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.RecName.y_ary  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"the name of the element span above\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.RecName.y_ary  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.RecName.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A Json cfmt on a global (FDb) ctype: the Global anchor field is the
// singleton instance itself and carries no data, so — like Base and Bitfld —
// it passes the field check without printing, and the object holds the
// ctype's data fields. Anchor, Derived and Presence carry that to the
// degenerate end: their one and only data field carries nothing of its own (a
// Global, a Base, a presence mask), so the walk emits no member at all and
// each must still return an empty object rather than a null node. One ctype
// per cause keeps every arm of that rule load-bearing.
// The FDb ctype also carries the repeated fields, which reach their elements
// through the field's own Getary. A global's accessors take no parent
// argument, so those calls are emitted with none: a Tary whose element type
// prints as a string, a Tary whose element type has a Json cfmt of its own,
// and an Inlary. GlobAry is a second global ctype whose only data beside the
// anchor is one Tary, which keeps the collapse pinned apart from FDb's other
// members. Pins every emitted FmtJson body in the fixture.
void atf_comp::comptest_amc_JsonGlobal() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N %.FmtJson < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.val  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Anchor  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Anchor._anchor  arg:algo_lib.Anchor  reftype:Global  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Anchor.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Empty  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Derived  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Derived.base  arg:algo_lib.Empty  reftype:Base  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Derived.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Presence  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Presence.pmask  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.Presence.pmask  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Presence.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.ja  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.FDb.ja  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Elem  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Elem.v  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Elem.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.jb  arg:algo_lib.Elem  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.FDb.jb  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.GlobAry  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.GlobAry._globary  arg:algo_lib.GlobAry  reftype:Global  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.GlobAry.v  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.GlobAry.v  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.GlobAry.Json  printfmt:Auto  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.jc  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.jc  min:2  max:4  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A single-file universe with a Ptrary, an fcurs request for its cursor, and
// no amcdb.tcurs rows: the request makes the cursor generator run, and the
// generator reports the missing tcurs row as a generation error and keeps
// generating. check_fcurs flags the same request first (bad_fcurs),
// so the run accumulates both errors and exits with their count.
void atf_comp::comptest_amc_BadMissingTcurs() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FRow  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FRow.rowid  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.row  arg:algo_lib.FRow  reftype:Tpool  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.c_row  arg:algo_lib.FRow  reftype:Ptrary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ptrary  field:algo_lib.FDb.c_row  unique:Y  heaplike:N");
    atf_comp::ProcWrite(proc, "dmmeta.xref  field:algo_lib.FDb.c_row  inscond:false  via:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcurs  fcurs:algo_lib.FDb.c_row/curs  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A single-file universe with a bitset field and no amcdb.tcurs rows: the
// tcurs row is the only carrier of the dflt flag, so amc cannot know the
// bitcurs cursor is request-only. Without a dmmeta.fcurs request the cursor
// must not be emitted; the NBits witness proves generation itself ran.
void atf_comp::comptest_amc_CursUnrequested() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.TestBitset.bits.NBits|algo_lib.TestBitset.bits_bitcurs.%' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.TestBitset  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.TestBitset.bits  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fbitset  field:algo_lib.TestBitset.bits  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// A field default naming the containing instance retargets to the emitting
// context's instance name, in both spellings C++ gives that instance. The
// object spelling *this becomes the instance itself (Msg Init assigns
// ssizeof(parent)), and the pointer spelling this becomes its address,
// parenthesized, so that the member access in this->pad binds to the whole
// address expression rather than to the member (Ptr Init assigns
// (&parent)->pad). An identifier that merely starts with the same characters
// (*thisvalue) is not an instance reference and survives verbatim. Rec's default
// carries the object spelling next to that longer identifier and Ptr's carries
// the pointer spelling next to it, so each pins that one expression keeps the
// longer identifier while its real reference is retargeted.
void atf_comp::comptest_amc_DfltRetarget() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N '(algo_lib.Rec..Init|algo_lib.Msg..Init|algo_lib.Ptr..Init)' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.val  arg:u32  reftype:Val  dflt:\"*thisvalue + ssizeof(*this)\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Ptr  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ptr.pad  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Ptr.val  arg:u32  reftype:Val  dflt:\"u32(thisvalue + this->pad)\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.len  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.lenfld  field:algo_lib.Msg.len  extra:0  scale:1");
    atf_comp::ProcWrite(proc, "dmmeta.msgtype  ctype:algo_lib.Msg  type:1");
    atf_comp::ProcWriteEof(proc);
}

// amc run from a directory with no bin/acr (outside a checkout): the acr
// subprocess that rewrites the ctypelen/dispsig/tracefld/tracerec/userfunc
// tables cannot start, and the run must fail instead of exiting 0 with the
// tables silently unwritten. The command cds into the test's tempdir so
// the relative bin/acr path resolves nowhere; out's gen directories are
// created up front so the output-file writes succeed and only the acr
// failure is under test.
void atf_comp::comptest_amc_TableWriteAcrFail() {
    atf_comp::FProc &proc = atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir -p out/cpp/gen out/include/gen && $$OLDPWD/$bindir/amc -in_dir:- -out_dir:out -report:N < <(cat $$OLDPWD/test/amc/bootstrap.ssim -)'");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.val  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.buf  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.buf  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// amc pointed at an -out_dir whose gen subdirectories do not exist: every
// output-file write fails, and the run must fail naming each path instead
// of exiting 0 with the generated code silently missing.
void atf_comp::comptest_amc_OutfileWriteFail() {
    atf_comp::FProc &proc = atf_comp::ProcStart("bash -c 'cd $tempdir && $$OLDPWD/$bindir/amc -in_dir:- -out_dir:out -report:N < <(cat $$OLDPWD/test/amc/bootstrap.ssim -)'");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.val  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.buf  arg:u32  reftype:Inlary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.inlary  field:algo_lib.FDb.buf  min:0  max:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.FDb.String  printfmt:Tuple  read:N  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// amc -e whose acr -e child fails (exit 1, here via an editor that fails):
// the run must fail loudly before regenerating anything, instead of reading
// the nonzero exit as an edit that modified files and regenerating on top of
// a schema that never changed. The failure is reported and the phases that
// would read the input set are skipped, so the run ends through its own exit
// path and its report names the files it did not write. The command cds into the test's tempdir and
// symlinks bin and data from the checkout, so amc's relative bin/acr spawn
// and acr's data load both resolve while every write stays in the tempdir.
void atf_comp::comptest_amc_EditFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && ln -s $$OLDPWD/bin bin && ln -s $$OLDPWD/data data && mkdir temp"
                        " && EDITOR=false $$OLDPWD/$bindir/amc ctype:amc.FGen -e'");
}

// A FAST template whose presence mask is a field named nullable, not pmask.
// The presence accessors amc generates are named after the pmask row -- the
// row's field name prefixes the whole-word bitset accessors and its funcname
// spells the per-field ones -- so a codec that spells those names itself
// works only for a mask that happens to be called pmask. The first run pins
// the emitted encoder and decoder: the message's own presence reads through
// Value_NullableQ and writes through nullable_qSetBitVal, while the state
// record's two masks, named assigned and present, keep the accessors those
// names generate.
// The FAST codec qualifies each field's previous value through two pmasks of
// the namespace's FastState record, told apart by name; the second run gives
// the presence one another name, which leaves the codec with no accessor to
// call, and pins the diagnostic and the nonzero exit that replace the
// silently unnamed accessor. The row that is missing belongs to the
// namespace, so that universe also pins how often it is charged: two
// templates of the namespace need it, each through an encoder and a decoder,
// and it is reported once with exit code 1. The universe's other two
// namespaces draw nothing -- one holds no cfast ctype at all, the other holds
// one encoded as a type, which is coded inline by whichever message holds it
// and needs no state pmask of its own.
void atf_comp::comptest_amc_FastPmaskName() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N 'algo_lib.Msg..Fast%' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FastState  comment:\"Value the peer last saw for each field, and the two pmasks qualifying it\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FastState.assigned  arg:u64  reftype:Val  dflt:\"\"  comment:\"A value has been seen for the field\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.FastState.assigned  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FastState.present  arg:u64  reftype:Val  dflt:\"\"  comment:\"The last value seen was present\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.FastState.present  filter_print:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FastState.tid  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FastState.Value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Msg  comment:\"FAST template whose presence mask is named nullable, not pmask\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.id  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.nullable  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pmaskfld  field:algo_lib.Msg.nullable  filter_print:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Msg.Value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfast  ctype:algo_lib.Msg  id:1  encoding:template  reset:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ffast  ffast:algo_lib.Msg.Value.field  name:Value  id:1  pos:1  optional:Y  encoding:unsigned  op:copy  value:\"\"  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
    atf_comp::FProc &proc2 = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc2, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u8  reftype:Malloc  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.ctype  ctype:algo_lib.FastState  comment:\"Value the peer last saw for each field, and the two pmasks qualifying it\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.FastState.assigned  arg:u64  reftype:Val  dflt:\"\"  comment:\"A value has been seen for the field\"");
    atf_comp::ProcWrite(proc2, "dmmeta.pmaskfld  field:algo_lib.FastState.assigned  filter_print:N  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.FastState.seen  arg:u64  reftype:Val  dflt:\"\"  comment:\"The last value seen was present\"");
    atf_comp::ProcWrite(proc2, "dmmeta.pmaskfld  field:algo_lib.FastState.seen  filter_print:Y  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.FastState.tid  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.FastState.Value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.ctype  ctype:algo_lib.Msg  comment:\"FAST template whose presence mask is named nullable, not pmask\"");
    atf_comp::ProcWrite(proc2, "dmmeta.pack  ctype:algo_lib.Msg  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.Msg.id  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.Msg.nullable  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.pmaskfld  field:algo_lib.Msg.nullable  filter_print:Y  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.Msg.Value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.cfast  ctype:algo_lib.Msg  id:1  encoding:template  reset:N  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.ffast  ffast:algo_lib.Msg.Value.field  name:Value  id:1  pos:1  optional:Y  encoding:unsigned  op:copy  value:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.ctype  ctype:algo_lib.Msg2  comment:\"Second FAST template of the same namespace, coded by the same pmask row\"");
    atf_comp::ProcWrite(proc2, "dmmeta.pack  ctype:algo_lib.Msg2  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.Msg2.id  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:algo_lib.Msg2.Value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.cfast  ctype:algo_lib.Msg2  id:2  encoding:template  reset:N  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.ffast  ffast:algo_lib.Msg2.Value.field  name:Value  id:1  pos:1  optional:N  encoding:unsigned  op:copy  value:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.ns  ns:zz  nstype:lib  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.nscpp  ns:zz  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.ctype  ctype:zz.Qty  comment:\"cfast type encoding: coded inline by its holder, so zz gets no codec\"");
    atf_comp::ProcWrite(proc2, "dmmeta.field  field:zz.Qty.value  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc2, "dmmeta.cfast  ctype:zz.Qty  id:0  encoding:type  reset:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc2);
}

// Which argv accessors each of the two argv strfmts generates. The universe
// holds one ctype per strfmt over the same three fields, and both cfmts ask
// for print and read alike, so the only thing that varies between them is the
// strfmt in the cfmt key. Argv gets the whole surface: the two printers, the
// ToCmdline convenience that prefixes bin/ and inherits the verbosity flags,
// the NArgs token-count table, and the ReadArgv parser the namespace ReadArgv
// delegates to. ArgvGnu gets the two printers and nothing else, so its read:Y
// buys no command-line reader; the prototypes are printed rather than the
// bodies, since what is under test is which functions exist.
void atf_comp::comptest_amc_ArgvAccessor() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N -proto -showcomment:N 'algo_lib.Amc..%Argv%|algo_lib.Amc..ToCmdline|algo_lib.Amc..NArgs|algo_lib.Gnu..%Argv%|algo_lib.Gnu..ToCmdline|algo_lib.Gnu..NArgs' < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Gnu  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Gnu.c  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Gnu.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Gnu.tag  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.Gnu.tag  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Gnu.ArgvGnu  printfmt:Auto  read:Y  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Amc  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Amc.c  arg:u8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Amc.limit  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Amc.tag  arg:u32  reftype:Tary  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.tary  field:algo_lib.Amc.tag  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Amc.Argv  printfmt:Auto  read:Y  print:Y  sep:\"\"  genop:N  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// An fconst whose own value the field's store cannot hold. The symbol reaches
// the field through the generated Set, which for a bitfield masks the store, so
// the value the schema names is not the value the field ends up holding and no
// accessor can map it back to the symbol; amc rejects each such row by name and
// states the range the store does hold. The rejected values walk the stores: a
// value past the bits (2000 in ten bits), past the arg type (300 in a u8), past
// a signed arg type's maximum (2147483648 in an i32), a negative on an unsigned
// store, and a negative on bits that drop a signed arg type's sign.
// The value is a C++ integer literal, and amc reads it the way the compiler
// will: 0377 is octal 255 and fits a u8 store, 0400 is 256 and does not, a
// u/U/l/L suffix belongs to the literal's type rather than to its number, and a
// leading plus or minus is the number's sign. Sign and magnitude are carried
// apart, so an unsigned 64-bit store holds 0xffffffffffffffff while an i32
// store rejects -0x80000001, one past its minimum. A magnitude that outruns 64
// bits -- 0x10000000000000001, or twenty-three decimal digits -- fits no store
// and is reported against the widest one.
// The controls that draw nothing are the values at each store's own edges (the
// u32, u8 and u64 maxima, the i32 minimum in decimal and in hex, 1023 in ten
// bits, 127 in seven, -128 in eight in decimal and in hex), and every spelling
// that is not one whole integer literal and so carries no number to compare: a
// shift expression, an arithmetic one, digits with trailing text, a character
// literal, a name only the compiler resolves, and a binary literal, a spelling
// amc does not read. A string field's value is a string however it is spelled,
// so the string field carries one control reading csv and one reading 99999,
// and neither is bounded: the field's arg is a character array, which resolves
// to no integer type and so has no range to compare with. The mask of a flag on
// the top bit of a signed word is likewise unbounded, being a bitfield's mask
// rather than a value of the word.
void atf_comp::comptest_amc_BadFconstRange() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Sig8  comment:\"signed 8-bit value wrapped in a single field\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Sig8.value  arg:i8  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Sig8.String  printfmt:Raw  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Str4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Str4.ch  arg:char  reftype:Smallstr  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.smallstr  field:algo_lib.Str4.ch  length:4  strtype:rpascal  pad:\"\"  strict:Y");
    atf_comp::ProcWrite(proc, "dmmeta.cstr  ctype:algo_lib.Str4  strequiv:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Str4.String  printfmt:Raw  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:algo_lib.Str4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Rec.String  printfmt:Tuple  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.word  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.wide  arg:u32  reftype:Val  dflt:\"\"  comment:\"store is the whole arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.wide/argmax  value:4294967295  comment:\"the arg type's own maximum fits\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.wide/shifted  value:\"1<<3\"  comment:\"not a plain integer: nothing to compare\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.wide/negative  value:-1  comment:\"an unsigned store holds no negative\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.byte  arg:u8  reftype:Val  dflt:\"\"  comment:\"store is the whole arg type\"");
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
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.snum  arg:i32  reftype:Val  dflt:\"\"  comment:\"store is the whole arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.snum/argmin  value:-2147483648  comment:\"the arg type's own minimum fits\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.snum/past  value:2147483648  comment:\"past the signed arg type's maximum\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.snum/hexmin  value:-0x80000000  comment:\"the minimum, spelled as a signed hex literal\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.snum/hexpast  value:-0x80000001  comment:\"one past the minimum, spelled in hex\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.full  arg:u64  reftype:Val  dflt:\"\"  comment:\"store is the whole 64-bit arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.full/argmax  value:18446744073709551615  comment:\"the arg type's own maximum fits\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.full/highbit  value:0x8000000000000000  comment:\"past the signed maximum, and an unsigned store holds it\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.full/hexmax  value:0xffffffffffffffff  comment:\"the arg type's maximum, spelled in hex\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.full/hexwide  value:0x10000000000000001  comment:\"a magnitude past 64 bits fits no store\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.full/decwide  value:99999999999999999999999  comment:\"a decimal magnitude past 64 bits fits no store\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.narrow  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"bits hold less than the arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.narrow  offset:0  width:10  srcfield:algo_lib.Rec.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.narrow/maxstore  value:1023  comment:\"the widest value the bits hold\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.narrow/past  value:2000  comment:\"past the bits, would be stored masked\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.sig_edge  arg:algo_lib.Sig8  reftype:Bitfld  dflt:\"\"  comment:\"bits drop the arg type's sign\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.sig_edge  offset:10  width:7  srcfield:algo_lib.Rec.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.sig_edge/maxstore  value:127  comment:\"the widest value the bits hold\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.sig_edge/negative  value:-1  comment:\"the bits drop the sign, so no negative fits\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.sig_full  arg:algo_lib.Sig8  reftype:Bitfld  dflt:\"\"  comment:\"bits span the whole arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.sig_full  offset:17  width:8  srcfield:algo_lib.Rec.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.sig_full/argmin  value:-128  comment:\"the bits keep the sign, so the arg minimum fits\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.sig_full/hexmin  value:-0x80  comment:\"the arg minimum, spelled as a signed hex literal\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.name  arg:algo_lib.Str4  reftype:Val  dflt:\"\"  comment:\"a string field: its fconst value is a string\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.name/csv  value:csv  comment:\"a string, not a number to bound\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.name/digits  value:99999  comment:\"a string field's value is a string however it is spelled\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Flags  comment:\"a signed word carrying single-bit flags\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:algo_lib.Flags.String  printfmt:Bitset  read:Y  print:Y  sep:,  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Flags.value  arg:i32  reftype:Val  dflt:\"\"  comment:\"the word the flags source\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Flags.top  arg:bool  reftype:Bitfld  dflt:\"\"  comment:\"a flag on the word's top bit\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Flags.top  offset:31  width:1  srcfield:algo_lib.Flags.value  comment:\"\"");
    atf_comp::ProcWriteEof(proc);
}

// The declared bitfield widths an fconst's store range is computed from, over
// one universe holding a field per width. A bitfield's Set masks the store to
// the declared width, so the range an fconst value is checked against is built
// by shifting a u64 by that width -- and the width is a signed schema argument
// that a run reaches before the accessors refuse it, so it arrives here holding
// whatever the schema wrote. The widths from one to sixty-three are the ones
// that name bits inside the word: each of those fields carries the widest value
// its bits hold, which draws nothing, next to the value one past it, which is
// reported. A width of sixty-four spans the arg type and masks nothing, so the
// arg type's own maximum draws nothing. The three widths outside the range --
// negative, zero, and sixty-five bits of a u128 source field -- are reported as
// out of bounds naming the field, and the value 1 that sits on each of them
// draws no range complaint of its own: a width that names no bits bounds
// nothing. A run that computes a bound from one of them instead reports 1 as a
// value the field cannot hold, which the field's arg type holds perfectly well.
// The bitfield accessors end the run at the first width they refuse, which is
// the last line of the golden.
void atf_comp::comptest_amc_FconstBitfldWidth() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/amc -in_dir:- -out_dir: -report:N zzz < <(cat test/amc/bootstrap.ssim -)");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i8  likeu64:Y  bigendok:N  issigned:Y  comment:\"8-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u8  likeu64:Y  bigendok:N  issigned:N  comment:\"8-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i16  likeu64:Y  bigendok:Y  issigned:Y  comment:\"16-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u16  size:2  alignment:2  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u16  likeu64:Y  bigendok:Y  issigned:N  comment:\"16-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i32  likeu64:Y  bigendok:Y  issigned:Y  comment:\"32-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u32  size:4  alignment:4  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:u32  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u32  likeu64:Y  bigendok:Y  issigned:N  comment:\"32-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:i64  likeu64:Y  bigendok:Y  issigned:Y  comment:\"64-bit   signed integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u64  likeu64:Y  bigendok:Y  issigned:N  comment:\"64-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:u128  size:16  alignment:16  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:u128  likeu64:N  bigendok:Y  issigned:N  comment:\"128-bit unsigned integer\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:bool  likeu64:Y  bigendok:N  issigned:N  comment:\"dflt is a c++ expr: real dflt is 'N'\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:pad_byte  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.csize  ctype:pad_byte  size:1  alignment:1  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.cfmt  cfmt:pad_byte.String  printfmt:Extern  read:Y  print:Y  sep:\"\"  genop:Y  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.pack  ctype:pad_byte  comment:\"\"");
    atf_comp::ProcWrite(proc, "amcdb.bltin  ctype:char  likeu64:Y  bigendok:N  issigned:N  comment:char");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.malloc  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Rec  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.word  arg:u32  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.dword  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.dword2  arg:u64  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.oword  arg:u128  reftype:Val  dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.negw  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"width below the range: names no bits\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.negw  offset:0  width:-64  srcfield:algo_lib.Rec.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.negw/one  value:1  comment:\"a bad width bounds nothing, so the value draws no range complaint\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.zerow  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"width below the range: names no bits\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.zerow  offset:1  width:0  srcfield:algo_lib.Rec.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.zerow/one  value:1  comment:\"a bad width bounds nothing, so the value draws no range complaint\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.widew  arg:u128  reftype:Bitfld  dflt:\"\"  comment:\"width above the range: more bits than a u64 mask holds\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.widew  offset:0  width:65  srcfield:algo_lib.Rec.oword  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.widew/one  value:1  comment:\"a bad width bounds nothing, so the value draws no range complaint\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.w1  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"the narrowest width the range admits\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.w1  offset:2  width:1  srcfield:algo_lib.Rec.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.w1/maxstore  value:1  comment:\"the widest value one bit holds\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.w1/past  value:2  comment:\"past one bit, would be stored masked\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.w10  arg:u32  reftype:Bitfld  dflt:\"\"  comment:\"bits hold less than the arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.w10  offset:3  width:10  srcfield:algo_lib.Rec.word  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.w10/maxstore  value:1023  comment:\"the widest value ten bits hold\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.w10/past  value:2000  comment:\"past ten bits, would be stored masked\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.w63  arg:u64  reftype:Bitfld  dflt:\"\"  comment:\"the widest width the mask shift builds\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.w63  offset:0  width:63  srcfield:algo_lib.Rec.dword  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.w63/maxstore  value:0x7fffffffffffffff  comment:\"the widest value sixty-three bits hold\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.w63/past  value:0x8000000000000000  comment:\"past sixty-three bits, would be stored masked\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.Rec.w64  arg:u64  reftype:Bitfld  dflt:\"\"  comment:\"width spans the whole arg type\"");
    atf_comp::ProcWrite(proc, "dmmeta.bitfld  field:algo_lib.Rec.w64  offset:0  width:64  srcfield:algo_lib.Rec.dword2  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.fconst  fconst:algo_lib.Rec.w64/argmax  value:0xffffffffffffffff  comment:\"the arg type's maximum, which sixty-four bits hold\"");
    atf_comp::ProcWriteEof(proc);
}
