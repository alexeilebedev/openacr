// Copyright (C) 2025-2026 AlgoRND
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/kafka.cpp
//

#include "include/algo.h"
#include "include/amc.h"

static strptr FirstVer(strptr range) {
    int i(0);
    for(; i<ch_N(range); ++i) {
        if (!algo_lib::DigitCharQ(range[i])) {
            break;
        }
    }
    return FirstN(range,i);
}

static strptr LastVer(strptr range) {
    return Pathcomp(range,"-LR");
}

void amc::KafkaCodec(int dir) {
    bool encode = dir==0;
    bool decode = !encode;
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (ctype.c_ckafka) {
        bool header = ctype.c_ckafka->kind == dmmeta_kafka_type_kind_header;
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Name", name_Get(ctype));
        amc::FFunc& func = amc::CreateCurFunc(true);
        if (encode) {
            Ins(&R, func.ret  , "void", false);
            AddProtoArg(func, Refto("algo::ByteAry"), "buf");
        }
        if (decode) {
            AddRetval(func, "bool", "ok", "true");
            AddProtoArg(func, Refto("algo::memptr"), "buf");
        }
        AddProtoArg(func, Refto(ctype.cpp_type), "parent");
        AddProtoArg(func, "i16", "version");
        if (decode && ctype.c_ckafka->kind == dmmeta_kafka_type_kind_response) {
            Ins(&R,func.body,"parent.request_api_version = version;");
        }
        strptr vfirst;
        strptr vlast;
        cstring validchk;
        if (decode) {
            Ins(&R,func.body,"bool present(false);");
            Ins(&R,func.body,"(void)present; // avoid unused variable");
            Ins(&R,func.body,"u32 len(0);");
            Ins(&R,func.body,"(void)len; // avoid unused variable");
        }
        if (ch_N(ctype.c_ckafka->valid_versions)) {
            vfirst = FirstVer(ctype.c_ckafka->valid_versions);
            vlast = LastVer(ctype.c_ckafka->valid_versions);
            algo::ListSep ls(" && ");
            if (ch_N(vfirst)) {
                validchk << ls << "version>=" << vfirst;
            }
            if (ch_N(vlast)) {
                validchk << ls << "version<=" << vlast;
            }
        }
        if (ch_N(validchk)) {
            Set(R,"$validverchk",validchk);
            if (decode) {
                Ins(&R,func.body,"ok = $validverchk;");
                Ins(&R,func.body,"if (ok) {");
            } else {
                Ins(&R,func.body,"if ($validverchk) {");
            }
        }
        cstring flexverchk;
        strptr ffirst = FirstVer(ctype.c_ckafka->flexible_versions);
        if (ch_N(ffirst)) {
            flexverchk << "version>=" << ffirst;
            Set(R,"$flexverchk",flexverchk);
            Ins(&R,func.body,"bool flexible($flexverchk);");
        } else {
            Ins(&R,func.body,"bool flexible(false);");
        }
        Ins(&R,func.body,"(void)flexible; // avoid unused variable");
        cstring lastfldverchk;
        int count(0);
        int n_hdr_fld(0);
        if (FCtype *base = GetBaseType(ctype,NULL)) {
            n_hdr_fld = c_field_N(*base);
        }
        ind_beg(ctype_c_field_curs,field,ctype) {
            bool kafka = field.c_fkafka;
            bool base = field.reftype == dmmeta_Reftype_reftype_Base;
            bool tagged = field.c_fkafka && ch_N(field.c_fkafka->tagged_versions);
            if (kafka && !base && !tagged) {
                Set(R, "$Argtype", field.p_arg->cpp_type);
                Set(R, "$fldname", name_Get(field));
                Set(R, "$fldval", "parent.$fldname");
                cstring fldverchk;
                strptr first = FirstVer(field.c_fkafka->versions);
                strptr last = LastVer(field.c_fkafka->versions);
                algo::ListSep ls(" && ");
                if (ch_N(first) && first != vfirst) {
                    fldverchk << ls << "version>=" << first;
                }
                if (ch_N(last) && last != vlast) {
                    fldverchk << ls << "version<=" << last;
                }
                if (fldverchk != lastfldverchk) {
                    if (ch_N(lastfldverchk)) {
                        Ins(&R,func.body,"}");
                    }
                    if (ch_N(fldverchk)) {
                        Set(R,"$fldverchk",fldverchk);
                        Ins(&R,func.body,"if ($fldverchk) {");
                    }
                }
                lastfldverchk = fldverchk;
                cstring fldnulchk;
                cstring fldprschk;
                cstring fldprschkexpr;
                bool prsset(false);
                strptr nfirst = FirstVer(field.c_fkafka->nullable_versions);
                strptr nlast = LastVer(field.c_fkafka->nullable_versions);
                if (ch_N(nfirst)) {
                    algo::ListSep ls2(" && ");
                    if (ch_N(nfirst) && nfirst != first) {
                        fldnulchk << ls2 << "version>=" << nfirst;
                    }
                    if (ch_N(nlast) && nlast != last) {
                        fldnulchk << ls2 << "version<=" << nlast;
                    }
                    if (!ch_N(fldnulchk)) {
                        fldnulchk << "true";
                    }
                    if (c_pmaskfld_member_N(field)) {
                        Ins(&R,fldprschk,"$fldname_PresentQ(parent)",false);
                        prsset = true;
                    } else if (count < n_hdr_fld || header) {
                        vrfy(name_Get(field)=="client_id", "Can't cope with unknown nullable field in header");
                        fldprschk = "bool(ch_N(parent.client_id))";
                    }
                } else {
                    fldnulchk << "false";
                    fldprschk << "true";
                }
                Set(R,"$fldnulchk",fldnulchk);
                Set(R,"$fldprschk",fldprschk);
                if (count < n_hdr_fld || header) {
                    Set(R,"$fldflxchk","false");
                } else {
                    Set(R,"$fldflxchk","flexible");
                }
                if (encode) {
                    if (field.reftype == dmmeta_Reftype_reftype_Tary) {
                        Ins(&R,func.body,"lib_kafka::EncodeArrayLen(buf,$fldname_N(parent),$fldprschk,$fldnulchk,$fldflxchk);");
                        Ins(&R,func.body,"if ($fldprschk) {");
                        Ins(&R,func.body,"ind_beg($Cpptype_$fldname_curs,$fldname,parent) {");
                        Set(R,"$fldval","$fldname");
                    }
                    if (field.arg == "bool") {
                        Ins(&R,func.body,"algo::EncodeBoolean(buf,$fldval);");
                    } else if (field.arg == "i8") {
                        Ins(&R,func.body,"algo::EncodeI8(buf,$fldval);");
                    } else if (field.arg == "i16") {
                        Ins(&R,func.body,"lib_kafka::EncodeI16(buf,$fldval);");
                    } else if (field.arg == "i32") {
                        Ins(&R,func.body,"lib_kafka::EncodeI32(buf,$fldval);");
                    } else if (field.arg == "i64") {
                        Ins(&R,func.body,"lib_kafka::EncodeI64(buf,$fldval);");
                    } else if (field.arg == "u8") {
                        Ins(&R,func.body,"algo::EncodeU8(buf,$fldval);");
                    } else if (field.arg == "u16") {
                        Ins(&R,func.body,"lib_kafka::EncodeU16(buf,$fldval);");
                    } else if (field.arg == "u32") {
                        Ins(&R,func.body,"lib_kafka::EncodeU32(buf,$fldval);");
                    } else if (field.arg == "u64") {
                        Ins(&R,func.body,"lib_kafka::EncodeU64(buf,$fldval);");
                    } else if (field.arg == "double") {
                        Ins(&R,func.body,"lib_kafka::EncodeF64(buf,$fldval);");
                    } else if (field.arg == "algo.cstring") {
                        Ins(&R,func.body,"lib_kafka::EncodeString(buf,$fldval,$fldprschk,$fldnulchk,$fldflxchk);");
                    } else if (field.arg == "algo.ByteAry") {
                        Ins(&R,func.body,"lib_kafka::EncodeBytes(buf,$fldval,$fldprschk,$fldnulchk,$fldflxchk);");
                    } else if (field.arg == "kafka.RecordBatch") {
                        Ins(&R,func.body,"algo::ByteAry $fldname_buf;");
                        Ins(&R,func.body,"if ($fldprschk) {");
                        Ins(&R,func.body,"    lib_kafka::EncodeRecordBatch($fldname_buf,$fldval);");
                        Ins(&R,func.body,"}");
                        Ins(&R,func.body,"lib_kafka::EncodeBytes(buf,ary_Getary($fldname_buf),true,$fldnulchk,$fldflxchk);");
                    } else if (field.arg == "algo.Uuid") {
                        Ins(&R,func.body,"algo::EncodeUuid(buf,$fldval);");
                    } else if (field.arg == "kafka.AclOperations") {
                        Ins(&R,func.body,"lib_kafka::EncodeI32(buf,$fldval.value);");
                    } else {
                        Ins(&R,func.body,"$Argtype_KafkaEncode(buf,$fldval,version);");
                    }
                    if (field.reftype == dmmeta_Reftype_reftype_Tary) {
                        Ins(&R,func.body,"}");
                        Ins(&R,func.body,"}ind_end;");
                    }
                }
                if (decode) {
                    if (field.reftype == dmmeta_Reftype_reftype_Tary) {
                        Ins(&R,func.body,"ok = ok && lib_kafka::DecodeArrayLen(buf,len,present,$fldnulchk,$fldflxchk);");
                        Ins(&R,func.body,"for (u32 i=0; i<len; ++i) {");
                        Set(R,"$fldval","$fldname_Alloc(parent)");
                    }
                    if (field.arg == "bool") {
                        Ins(&R,func.body,"ok = ok && algo::DecodeBoolean(buf,$fldval);");
                    } else if (field.arg == "i8") {
                        Ins(&R,func.body,"ok = ok && algo::DecodeI8(buf,$fldval);");
                    } else if (field.arg == "i16") {
                        Ins(&R,func.body,"ok = ok && lib_kafka::DecodeI16(buf,$fldval);");
                    } else if (field.arg == "i32") {
                        Ins(&R,func.body,"ok = ok && lib_kafka::DecodeI32(buf,$fldval);");
                    } else if (field.arg == "i64") {
                        Ins(&R,func.body,"ok = ok && lib_kafka::DecodeI64(buf,$fldval);");
                    } else if (field.arg == "u8") {
                        Ins(&R,func.body,"ok = ok && algo::DecodeU8(buf,$fldval);");
                    } else if (field.arg == "u16") {
                        Ins(&R,func.body,"ok = ok && lib_kafka::DecodeU16(buf,$fldval);");
                    } else if (field.arg == "u32") {
                        Ins(&R,func.body,"ok = ok && lib_kafka::DecodeU32(buf,$fldval);");
                    } else if (field.arg == "u64") {
                        Ins(&R,func.body,"ok = ok && lib_kafka::DecodeU64(buf,$fldval);");
                    } else if (field.arg == "double") {
                        Ins(&R,func.body,"ok = ok && lib_kafka::DecodeF64(buf,$fldval);");
                    } else if (field.arg == "algo.cstring") {
                        Ins(&R,func.body,"ok = ok && lib_kafka::DecodeString(buf,$fldval,present,$fldnulchk,$fldflxchk);");
                    } else if (field.arg == "algo.ByteAry") {
                        Ins(&R,func.body,"ok = ok && lib_kafka::DecodeBytes(buf,$fldval,present,$fldnulchk,$fldflxchk);");
                    } else if (field.arg == "kafka.RecordBatch") {
                        Ins(&R,func.body,"algo::memptr $fldname_buf;");
                        Ins(&R,func.body,"ok = ok && lib_kafka::DecodeBytes(buf,$fldname_buf,present,$fldnulchk,$fldflxchk);");
                        Ins(&R,func.body,"present = present && elems_N($fldname_buf);");
                        Ins(&R,func.body,"if (present) {");
                        Ins(&R,func.body,"    ok = ok && lib_kafka::DecodeRecordBatch($fldname_buf,$fldval);");
                        Ins(&R,func.body,"}");
                    } else if (field.arg == "algo.Uuid") {
                        Ins(&R,func.body,"ok = ok && algo::DecodeUuid(buf,$fldval);");
                    } else if (field.arg == "kafka.AclOperations") {
                        Ins(&R,func.body,"ok = ok && lib_kafka::DecodeI32(buf,$fldval.value);");
                    } else {
                        Ins(&R,func.body,"ok = ok && $Argtype_KafkaDecode(buf,$fldval,version);");
                    }
                    if (field.reftype == dmmeta_Reftype_reftype_Tary) {
                        Ins(&R,func.body,"}");
                    }
                    if (prsset) {
                        Ins(&R,func.body,"if (present) {");
                        Ins(&R,func.body,"$fldname_SetPresent(parent);");
                        Ins(&R,func.body,"}");
                    }
                }
                // flexible version of ApiVersionsResponse still uses non-flexible header while request does
                if (count+1 == n_hdr_fld && ch_N(ffirst) && name_Get(ctype) != "ApiVersionsResponse") {
                    fldverchk = flexverchk;
                    if (fldverchk != lastfldverchk) {
                        if (ch_N(lastfldverchk)) {
                            Ins(&R,func.body,"}");
                        }
                        if (ch_N(fldverchk)) {
                            Set(R,"$fldverchk",fldverchk);
                            Ins(&R,func.body,"if ($fldverchk) {");
                        }
                    }
                    lastfldverchk = fldverchk;
                    if (encode) {
                        Ins(&R,func.body,"lib_kafka::EncodeUnsignedVarint(buf,0); // _tagged_fields for header");
                    }
                    if (decode) {
                        Ins(&R,func.body,"ok = ok && lib_kafka::DecodeUnsignedVarint(buf,len); // _tagged_fields for header");
                        Ins(&R,func.body,"ok = len == 0;");
                    }
                }
            }
            count += !base;
        }ind_end;
        if (ch_N(ffirst)) {
            cstring fldverchk = flexverchk;
            if (fldverchk != lastfldverchk) {
                if (ch_N(lastfldverchk)) {
                    Ins(&R,func.body,"}");
                }
                if (ch_N(fldverchk)) {
                    Set(R,"$fldverchk",fldverchk);
                    Ins(&R,func.body,"if ($fldverchk) {");
                }
            }
            lastfldverchk = fldverchk;
            if (encode) {
                Ins(&R,func.body,"lib_kafka::EncodeUnsignedVarint(buf,0); // _tagged_fields for type");
            }
            if (decode) {
                Ins(&R,func.body,"ok = ok && lib_kafka::DecodeUnsignedVarint(buf,len); // _tagged_fields for type");
                Ins(&R,func.body,"for (u32 i=0; ok && i<len; ++i) {");
                Ins(&R,func.body,"u32 field_tag, field_len;");
                Ins(&R,func.body,"algo::memptr field_val;");
                Ins(&R,func.body,"ok = ok && lib_kafka::DecodeUnsignedVarint(buf,field_tag);");
                Ins(&R,func.body,"ok = ok && lib_kafka::DecodeUnsignedVarint(buf,field_len);");
                Ins(&R,func.body,"ok = ok && algo::DecodeNBytes(buf,field_len,field_val);");
                Ins(&R,func.body,"}");
            }
        }
        if (ch_N(lastfldverchk)) {
            Ins(&R,func.body,"}");
        }
        if (ch_N(validchk)) {
            Ins(&R,func.body,"}");
        }
        if (decode) {
            MaybeUnused(func,"present");
            MaybeUnused(func,"len");
        }
    }
}

void amc::tfunc_Ctype_KafkaEncode() {
    KafkaCodec(0);
}

void amc::tfunc_Ctype_KafkaDecode() {
    KafkaCodec(1);
}
