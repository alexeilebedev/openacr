// (C) 2018-2019 NYSE | Intercontinental Exchange
//
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
// Contacting ICE: <https://www.theice.com/contact>
//
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Header: include/algo/using.h
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

using algo::Abs;
using algo::ArgProto;
using algo::Argtuple;
using algo::Argtuple_proto_curs;
using algo::Attr;
using algo::Bool;
using algo::BumpToPow2;
using algo::ByteAry;
using algo::BytesOf;
using algo::CeilingLog2;
using algo::Charset;
using algo::Clipped;
using algo::Clippedf;
using algo::CompareNoCase;
using algo::CreateDirRecurse;
using algo::CurrSchedTime;
using algo::CurrUnTime;
using algo::CurrUnixTime;
using algo::DCeiling;
using algo::DFloor;
using algo::DTrunc;
using algo::DeleteFile;
using algo::DirEntry;
using algo::DirFileJoin;
using algo::Dir_curs;
using algo::DirectoryQ;
using algo::DryrunQ;
using algo::EchoQ;
using algo::Errcode;
using algo::FailokQ;
using algo::FatalErrorExit;
using algo::Fildes;
using algo::FileFlags;
using algo::FileQ;
using algo::Fill;
using algo::FindStr;
using algo::FloorLog2;
using algo::GetCurDir;
using algo::GetHostname;
using algo::GetTypeTag;
using algo::GetWeekdayName;
using algo::GetaryFcn;
using algo::Geti64Fcn;
using algo::Hook;
using algo::I32Dec1;
using algo::I32Dec3;
using algo::IncrMT;
using algo::InitFcn;
using algo::Ipmask;
using algo::LineBuf;
using algo::Line_curs;
using algo::ListSep;
using algo::LnumStr6_U32;
using algo::LockAllMemory;
using algo::LspaceStr4;
using algo::MakeLower;
using algo::MakeUpper;
using algo::MaybeDirSep;
using algo::MaybeSpace;
using algo::MoveToCore;
using algo::NoOp;
using algo::ParseHex1;
using algo::ParseHex2;
using algo::ParseI32;
using algo::ParseNum4;
using algo::ParseNum8;
using algo::ParseOct1;
using algo::ParseOct3;
using algo::Pathcomp;
using algo::PrintDouble;
using algo::PtrAdd;
using algo::ReadBE32;
using algo::ReadBE64;
using algo::ReadLE16;
using algo::ReadLE32;
using algo::ReadLE64;
using algo::Refurbish;
using algo::RegxMacro;
using algo::RemDirRecurse;
using algo::Replace;
using algo::SECS_PER_DAY;
using algo::FileToString;
using algo::SECS_PER_HOUR;
using algo::SECS_PER_MIN;
using algo::SaneTerminalQ;
using algo::SchedTime;
using algo::SeqType;
using algo::SetnumFcn;
using algo::SetupExitSignals;
using algo::SetupFatalSignals;
using algo::SleepMsec;
using algo::SsimFname;
using algo::StringDesc;
using algo::StringIter;
using algo::StringToFile;
using algo::SysCmd;
using algo::TSwap;
using algo::TermStyle;
using algo::TimeStruct;
using algo::Tuple;
using algo::Tuple_attrs_curs;
using algo::U64Dec8;
using algo::UNTIME_PER_MSEC;
using algo::UNTIME_PER_SEC;
using algo::UNTIME_PER_USEC;
using algo::URL;
using algo::UnDiff;
using algo::UnDiffHMS;
using algo::UnDiffSecs;
using algo::UnTime;
using algo::UnescapeC;
using algo::UnixDiff;
using algo::UnixTime;
using algo::Word_curs;
using algo::ZeroBytes;
using algo::aParseNum16;
using algo::aParseNum4;
using algo::aParseNum8;
using algo::aryptr;
using algo::cstring;
using algo::double_PrintWithCommas;
using algo::double_WeakRandom;
using algo::eol;
using algo::get_cpu_hz;
using algo::get_cpu_hz_int;
using algo::get_cycles;
using algo::gitinfo_Get;
using algo::i32_NegateIf;
using algo::i32_Range;
using algo::i32_WeakRandom;
using algo::rdtscp;
using algo::strptr_Hash;
using algo::ToSchedTime;
using algo::u16_SubClip;
using algo::u32_CeilPow2;
using algo::u32_CvtLestr8Small;
using algo::u32_FmtBuf;
using algo::u32_Pow2Q;
using algo::u32_SubClip;
using algo::u64_BitScanForward;
using algo::u64_BitScanReverse;
using algo::u64_FmtBuf;
using algo::u64_FmtBufDec;
using algo::u64_ModPow2;
using algo::u64_Pow2Q;
using algo::u64_SubClip;
using algo::IOEvtFlags;
using algo::SchedTime;
using algo::memptr;
using algo::strptr;
using algo::tempstr;
using algo::THook;
