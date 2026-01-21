## algo - Basic types and functions


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Ctypes](#ctypes)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Functions](#functions)<br/>
<!-- dev.mdmark  mdmark:TOC  state:BEG_AUTO  param:Toc -->
&#128196; [algo.Attr - Tuple attribute](/txt/protocol/algo/Attr.md)<br/>
&#128196; [algo.Bool - Bool type for converting booleans to string](/txt/protocol/algo/Bool.md)<br/>
&#128196; [algo.Charset - Character set](/txt/protocol/algo/Charset.md)<br/>
&#128196; [algo.Comment - Generic comment for ssim tuples](/txt/protocol/algo/Comment.md)<br/>
&#128196; [algo.CppExpr - C++ expression](/txt/protocol/algo/CppExpr.md)<br/>
&#128196; [algo.DateCache - Fast conversion of date to string](/txt/protocol/algo/DateCache.md)<br/>
&#128196; [algo.Decimal - Decimal type with dynamic exponent](/txt/protocol/algo/Decimal.md)<br/>
&#128196; [algo.DirEntry - Directory entry](/txt/protocol/algo/DirEntry.md)<br/>
&#128196; [algo.Errcode - Error code](/txt/protocol/algo/Errcode.md)<br/>
&#128196; [algo.Errns - Error namespace](/txt/protocol/algo/Errns.md)<br/>
&#128196; [algo.Fildes - File descriptor primary key](/txt/protocol/algo/Fildes.md)<br/>
&#128196; [algo.FileFlags - File flags used with file functions](/txt/protocol/algo/FileFlags.md)<br/>
&#128196; [algo.IOEvtFlags - IO event flags, used in IOHook](/txt/protocol/algo/IOEvtFlags.md)<br/>
&#128196; [algo.IPoint - i32 - x,y](/txt/protocol/algo/IPoint.md)<br/>
&#128196; [algo.Imtable - In-memory table descriptor](/txt/protocol/algo/Imtable.md)<br/>
&#128196; [algo.Ipmask - Format: 10.0.0.0/16](/txt/protocol/algo/Ipmask.md)<br/>
&#128196; [algo.LineBuf - Line buffer](/txt/protocol/algo/LineBuf.md)<br/>
&#128196; [algo.Md5Digest -](/txt/protocol/algo/Md5Digest.md)<br/>
&#128196; [algo.Month - Helper type for month names](/txt/protocol/algo/Month.md)<br/>
&#128196; [algo.NumParseFlags -](/txt/protocol/algo/NumParseFlags.md)<br/>
&#128196; [algo.PrlogFcn - Function for text output](/txt/protocol/algo/PrlogFcn.md)<br/>
&#128196; [algo.SchedTime - CPU scheduler time (TSC value)](/txt/protocol/algo/SchedTime.md)<br/>
&#128196; [algo.SeqType - 64-bit sequence type](/txt/protocol/algo/SeqType.md)<br/>
&#128196; [algo.Sha1sig - SHA1 signature](/txt/protocol/algo/Sha1sig.md)<br/>
&#128196; [algo.TFNNtTestAlert -](/txt/protocol/algo/TFNNtTestAlert.md)<br/>
&#128196; [algo.TermStyle - terminal style](/txt/protocol/algo/TermStyle.md)<br/>
&#128196; [algo.TextJust - terminal justification](/txt/protocol/algo/TextJust.md)<br/>
&#128196; [algo.TstampCache - timestamp cache, for fast date to string conversion](/txt/protocol/algo/TstampCache.md)<br/>
&#128196; [algo.Tuple - Describes a ssim tuple](/txt/protocol/algo/Tuple.md)<br/>
&#128196; [algo.URL - URL parsed into components](/txt/protocol/algo/URL.md)<br/>
&#128196; [algo.UnDiff - Unix diff with nanosecond resolution (i64)](/txt/protocol/algo/UnDiff.md)<br/>
&#128196; [algo.UnTime - Unix time with nanosecond resolution (i64)](/txt/protocol/algo/UnTime.md)<br/>
&#128196; [algo.UnixDiff - Difference between two UnixTimes; units: seconds](/txt/protocol/algo/UnixDiff.md)<br/>
&#128196; [algo.UnixTime - Unix epoch time, in seconds](/txt/protocol/algo/UnixTime.md)<br/>
&#128196; [algo.cstring - Array of characters (not NULL-terminated)](/txt/protocol/algo/cstring.md)<br/>
&#128196; [algo.memptr - Pointer to a span of u8s](/txt/protocol/algo/memptr.md)<br/>
&#128196; [algo.strptr - Pointer to a span of chars](/txt/protocol/algo/strptr.md)<br/>

<!-- dev.mdmark  mdmark:TOC  state:END_AUTO  param:Toc -->

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Ctypes
<a href="#ctypes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Ctypes -->
Other ctypes in this namespace which don't have own readme files

#### algo.ByteAry - Byte array
<a href="#algo-byteary"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ary|u8|[Tary](/txt/exe/amc/reftypes.md#tary)|||

#### algo.DayRange - Range of days
<a href="#algo-dayrange"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|start|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|end|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.DryrunQ - Symbolic bool
<a href="#algo-dryrunq"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.EchoQ - Symbolic bool
<a href="#algo-echoq"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.FailokQ - Symbolic bool
<a href="#algo-failokq"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I32Dec1 - signed i32, scale 1e1
<a href="#algo-i32dec1"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I32Dec2 - signed i32, scale 1e2
<a href="#algo-i32dec2"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I32Dec3 - signed i32, scale 1e3
<a href="#algo-i32dec3"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I32Dec4 - signed i32, scale 1e4
<a href="#algo-i32dec4"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I32Dec5 - signed i32, scale 1e5
<a href="#algo-i32dec5"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I32RangeAry - Byte array
<a href="#algo-i32rangeary"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ary|[algo.i32_Range](/txt/protocol/algo/README.md#algo-i32_range)|[Tary](/txt/exe/amc/reftypes.md#tary)|||

#### algo.I64Dec1 - signed i64, scale 1e1
<a href="#algo-i64dec1"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I64Dec10 - signed i64, scale 1e10
<a href="#algo-i64dec10"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I64Dec2 - signed i64, scale 1e2
<a href="#algo-i64dec2"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I64Dec3 - signed i64, scale 1e3
<a href="#algo-i64dec3"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I64Dec4 - signed i64, scale 1e4
<a href="#algo-i64dec4"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I64Dec5 - signed i64, scale 1e5
<a href="#algo-i64dec5"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I64Dec6 - signed i64, scale 1e6
<a href="#algo-i64dec6"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I64Dec7 - signed i64, scale 1e7
<a href="#algo-i64dec7"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I64Dec8 - signed i64, scale 1e8
<a href="#algo-i64dec8"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I64Dec9 - signed i64, scale 1e9
<a href="#algo-i64dec9"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.I64Rate - 
<a href="#algo-i64rate"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|last|i64|[Val](/txt/exe/amc/reftypes.md#val)||Last value|
|delta|i64|[Val](/txt/exe/amc/reftypes.md#val)||Delta of last value|

#### algo.Imdb - In-memory database descriptor (reflection)
<a href="#algo-imdb"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|imdb|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Database name|
|InsertStrptrMaybe|[algo.ImdbInsertStrptrMaybeFcn](/txt/protocol/algo/README.md#algo-imdbinsertstrptrmaybefcn)|[Val](/txt/exe/amc/reftypes.md#val)|0|Insert new element given a string|
|Step|[algo.ImdbStepFcn](/txt/protocol/algo/README.md#algo-imdbstepfcn)|[Val](/txt/exe/amc/reftypes.md#val)|0|Perform one step (may be NULL)|
|MainLoop|[algo.ImdbMainLoopFcn](/txt/protocol/algo/README.md#algo-imdbmainloopfcn)|[Val](/txt/exe/amc/reftypes.md#val)|0|Loop!|
|GetTrace|[algo.ImdbGetTraceFcn](/txt/protocol/algo/README.md#algo-imdbgettracefcn)|[Val](/txt/exe/amc/reftypes.md#val)|0||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.ImdbGetTraceFcn - Function for retrieving address of trace record
<a href="#algo-imdbgettracefcn"></a>

#### algo.ImdbInsertStrptrMaybeFcn - Function to insert ssim tuple into in-memory database
<a href="#algo-imdbinsertstrptrmaybefcn"></a>

#### algo.ImdbMainLoopFcn - Main loop function
<a href="#algo-imdbmainloopfcn"></a>

#### algo.ImdbStepFcn - Single step function
<a href="#algo-imdbstepfcn"></a>

#### algo.ImrowNItemsFcn - Function to retrieve # of rows in a table
<a href="#algo-imrownitemsfcn"></a>

#### algo.ImrowPrintFcn - Function to print a row
<a href="#algo-imrowprintfcn"></a>

#### algo.ImrowPtr - Pointer to a record in memory
<a href="#algo-imrowptr"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.ImrowRowidFindFcn - Function to find rowid of a row given its address
<a href="#algo-imrowrowidfindfcn"></a>

#### algo.ImrowXrefXFcn - Function to x-reference a row
<a href="#algo-imrowxrefxfcn"></a>

#### algo.LnumStr10_U64 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr10_u64"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr11_U64 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr11_u64"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr12_U64 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr12_u64"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr13_U64_Base36 - number stored as ascii digits, left pad with '0', base 36
<a href="#algo-lnumstr13_u64_base36"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr16_U64_Base16 - number stored as ascii digits, padded with 0s, base 16
<a href="#algo-lnumstr16_u64_base16"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr1_U32 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr1_u32"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr20_U64 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr20_u64"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr22_U64 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr22_u64"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr2_U32 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr2_u32"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr3_U32 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr3_u32"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr4_U32 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr4_u32"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr5_U32 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr5_u32"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr5_U32_Base36 - number stored as ascii digits, left pad with '0', base 36
<a href="#algo-lnumstr5_u32_base36"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr6_U32 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr6_u32"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr7_U32 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr7_u32"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr7_U32_Base36 - number stored as ascii digits, left pad with '0', base 36
<a href="#algo-lnumstr7_u32_base36"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr8_U32 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr8_u32"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr8_U32_Base16 - number stored as ascii digits, padded with 0s, base 16
<a href="#algo-lnumstr8_u32_base16"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr8_U64 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr8_u64"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr9_U32 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr9_u32"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LnumStr9_U64 - number stored as ascii digits, left pad with '0'
<a href="#algo-lnumstr9_u64"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr10 - string padded on the left with spaces
<a href="#algo-lspacestr10"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr12 - string padded on the left with spaces
<a href="#algo-lspacestr12"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr14 - string padded on the left with spaces
<a href="#algo-lspacestr14"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr15 - string padded on the left with spaces
<a href="#algo-lspacestr15"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr20_I64 - number stored as ascii digits, left pad with ' '
<a href="#algo-lspacestr20_i64"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr20_U64 - number stored as ascii digits, left pad with ' '
<a href="#algo-lspacestr20_u64"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr3 - string padded on the left with spaces
<a href="#algo-lspacestr3"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr3_I16 - number stored as ascii digits, left pad with ' '
<a href="#algo-lspacestr3_i16"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr4 - string padded on the left with spaces
<a href="#algo-lspacestr4"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr5 - string padded on the left with spaces
<a href="#algo-lspacestr5"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr5_I16 - string padded on the left with spaces
<a href="#algo-lspacestr5_i16"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr6 - string padded on the left with spaces
<a href="#algo-lspacestr6"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr6_U32 - number stored as ascii digits, left pad with ' '
<a href="#algo-lspacestr6_u32"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr7_I32_Base36 - number stored as ascii digits, left pad with ' ', base 36
<a href="#algo-lspacestr7_i32_base36"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr8 - string padded on the left with spaces
<a href="#algo-lspacestr8"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.LspaceStr9 - string padded on the left with spaces
<a href="#algo-lspacestr9"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr1 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr1"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr10 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr10"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr100 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr100"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr1000 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr1000"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr11 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr11"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr12 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr12"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr129 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr129"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr13 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr13"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr14 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr14"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr15 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr15"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr151 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr151"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr16 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr16"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr17 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr17"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr18 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr18"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr19 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr19"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr2 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr2"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr20 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr20"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr21 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr21"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr24 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr24"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr25 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr25"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr28 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr28"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr3 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr3"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr30 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr30"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr32 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr32"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr33 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr33"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr35 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr35"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr36 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr36"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr4 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr4"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr40 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr40"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr41 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr41"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr43 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr43"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr44 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr44"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr48 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr48"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr5 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr5"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr50 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr50"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr54 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr54"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr55 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr55"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr6 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr6"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr60 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr60"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr62 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr62"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr66 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr66"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr6_U32 - number stored as ascii digits, right pad with null
<a href="#algo-rnullstr6_u32"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr7 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr7"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr8 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr8"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr80 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr80"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RnullStr9 - fixed-length string padded on the right with NUL characters
<a href="#algo-rnullstr9"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr10 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr10"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr100 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr100"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr11 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr11"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr12 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr12"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr128 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr128"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr14 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr14"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr15 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr15"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr16 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr16"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr18 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr18"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr2 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr2"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr20 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr20"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr200 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr200"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr21 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr21"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr24 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr24"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr240 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr240"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr25 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr25"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr26 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr26"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr3 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr3"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr31 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr31"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr32 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr32"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr4 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr4"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr40 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr40"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr5 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr5"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr50 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr50"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr6 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr6"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr64 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr64"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr7 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr7"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr75 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr75"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr8 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr8"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.RspaceStr9 - fixed-length string padded on the right with spaces
<a href="#algo-rspacestr9"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr1 - inline string with length field
<a href="#algo-smallstr1"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr10 - inline string with length field
<a href="#algo-smallstr10"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr100 - inline string with length field
<a href="#algo-smallstr100"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr150 - inline string with length field
<a href="#algo-smallstr150"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr16 - inline string with length field
<a href="#algo-smallstr16"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr2 - inline string with length field
<a href="#algo-smallstr2"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr20 - inline string with length field
<a href="#algo-smallstr20"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr200 - inline string with length field
<a href="#algo-smallstr200"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr249 - inline string with length field
<a href="#algo-smallstr249"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr25 - inline string with length field
<a href="#algo-smallstr25"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr250 - inline string with length field
<a href="#algo-smallstr250"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr255 - inline string with length field
<a href="#algo-smallstr255"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr3 - inline string with length field
<a href="#algo-smallstr3"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr30 - inline string with length field
<a href="#algo-smallstr30"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr32 - inline string with length field
<a href="#algo-smallstr32"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr4 - inline string with length field
<a href="#algo-smallstr4"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr40 - inline string with length field
<a href="#algo-smallstr40"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr5 - inline string with length field
<a href="#algo-smallstr5"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.Smallstr50 - inline string with length field
<a href="#algo-smallstr50"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### algo.StringAry - array of strings
<a href="#algo-stringary"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ary|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)|||

#### algo.U16Ary - Array of u16
<a href="#algo-u16ary"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ary|u16|[Tary](/txt/exe/amc/reftypes.md#tary)||Array of u16 values|

#### algo.U16Dec2 - u16, scale 1e2
<a href="#algo-u16dec2"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u16|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.U32Ary - Array of u16
<a href="#algo-u32ary"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ary|u32|[Tary](/txt/exe/amc/reftypes.md#tary)||Array of u16 values|

#### algo.U32Dec1 - u32, scale 1e1
<a href="#algo-u32dec1"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.U32Dec2 - u32, scale 1e2
<a href="#algo-u32dec2"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.U32Dec3 - u32, scale 1e3
<a href="#algo-u32dec3"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.U32Dec4 - u32, scale 1e4
<a href="#algo-u32dec4"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.U32Dec5 - u32, scale 1e5
<a href="#algo-u32dec5"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.U32LinearKey - 
<a href="#algo-u32linearkey"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.U64Ary - Array of u64
<a href="#algo-u64ary"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ary|u64|[Tary](/txt/exe/amc/reftypes.md#tary)||Array of u64 values|

#### algo.U64Dec10 - unsigned u64, scale 1e10
<a href="#algo-u64dec10"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.U64Dec2 - u64, scale 1e2
<a href="#algo-u64dec2"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.U64Dec4 - u64, scale 1e4
<a href="#algo-u64dec4"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.U64Dec5 - u64, scale 1e5
<a href="#algo-u64dec5"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.U64Dec6 - u64, scale 1e6
<a href="#algo-u64dec6"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.U64Dec7 - u64, scale 1e7
<a href="#algo-u64dec7"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.U64Dec8 - u64, scale 1e8
<a href="#algo-u64dec8"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.U64Dec9 - u64, scale 1e9
<a href="#algo-u64dec9"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.Uuid - 
<a href="#algo-uuid"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Inlary](/txt/exe/amc/reftypes.md#inlary)|||

#### algo.WDiff - Difference between two WinTimes
<a href="#algo-wdiff"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.WTime - Units: 100nsec; Number of units since Jan 1, 1600
<a href="#algo-wtime"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### algo.i32_Range - i32: beg,end
<a href="#algo-i32_range"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|beg|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|end|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Ctypes -->

### Functions
<a href="#functions"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Functions -->
Functions exported from this namespace:

```c++
inline f32 algo::Abs(f32 a) 
```

```c++
inline f64 algo::Abs(f64 a) 
```

```c++
inline f80 algo::Abs(f80 a) 
```

```c++
inline i32 algo::Abs(i32 a) 
```

```c++
inline i64 algo::Abs(i64 a) 
```

```c++
inline int algo::Abs(i8  a) 
```

```c++
inline int algo::Abs(i16 a) 
```

```c++
// Check if path PATH is an absolute pathname,
// meaning that it starts with / or ~
// On windows, the same test is in force, but in addition any pathname
// where the second character is a : (e.g. c:\blah) is recognized as an absolute
// path name
bool algo::AbsolutePathQ(strptr path) NOTHROW 
```

```c++
// Compare two strings whose start address is 16-byte aligned
// Do not use this function --
bool algo::AlignedEqual(strptr a, strptr b) 
```

```c++
void algo::Append(algo::u64_RangesList &p, u64 item) 
```

```c++
void algo::Attr_Print(algo::Attr &attr, algo::cstring &str) 
```

```c++
inline algo::Attr_curs &algo::Attr_curs_Access(Attr_curs &curs) 
```

```c++
void algo::Attr_curs_Next(Attr_curs &curs) 
```

```c++
void algo::Attr_curs_Reset(Attr_curs &curs, strptr line) 
```

```c++
inline bool algo::Attr_curs_ValidQ(Attr_curs &curs) 
```

```c++
// Begin append key/value to string LHS
int algo::BeginKeyval(cstring &lhs, const strptr &name) 
```

```c++
void algo::Bool_Print(algo::Bool row, algo::cstring &str) 
```

```c++
inline u32 algo::BumpToPow2(u32 i) 
```

```c++
inline u64 algo::BumpToPow2(u64 i) 
```

```c++
void algo::ByteAry_Print(algo::ByteAry &parent, algo::cstring &str) 
```

```c++
// read bytes in hex e.g: 00 01 ff
bool algo::ByteAry_ReadStrptrMaybe(algo::ByteAry &parent, strptr str) 
```

```c++
template<class T> inline algo::aryptr<u8> algo::BytesOf(const T &t) 
```

```c++
// When compiled with AOS_SSE42, use intrinsics.
// Otherwise, the function is defined in crc32.cpp and uses a software implementation
inline u32 algo::CRC32Step(u32 old, const u8 *x, size_t len) 
```

```c++
u32 algo::CRC32Step(u32 old, const u8 *data, size_t len) 
```

```c++
inline u32  algo::CeilingLog2(u32 orig) 
```

```c++
inline u64  algo::CeilingLog2(u64 orig) 
```

```c++
bool algo::Charset_ReadStrptrMaybe(algo::Charset &lhs, strptr rhs) 
```

```c++
// Read Charset from list of chars.
// Every character in RHS is simply added to the bitset
void algo::Charset_ReadStrptrPlain(algo::Charset &lhs, strptr desc) 
```

```c++
// Clipping to an open interval (exclisive upper limit) [0..hi-1] or [lo..hi-1]
inline int algo::Clipped(int a, int hi) 
```

```c++
inline int algo::Clipped(int a, int lo, int hi) 
```

```c++
// Clipping to a closed interval lo..hi
inline double algo::Clippedf(double a, double lo, double hi) 
```

```c++
int algo::CompareNoCase(strptr lhs, strptr s) 
```

```c++
// Read all bytes from IN_FD and copy them to OUT_FD, using read/write.
// Return success status.
bool algo::CopyFd(Fildes in_fd, Fildes out_fd) NOTHROW 
```

```c++
// Copy file FROM to file TO, creating the file if necessary
// with mode MODE.
// Throw exception on failure.
void algo::CopyFileX(strptr from, strptr to, int mode) 
```

```c++
void algo::CopyFromToB16(void *from, void *to, u32 n) 
```

```c++
// Recursively create directory S and all of its components,
// using mode MODE for newly created directories.
// if DO_THROW is specified, throw exceptions on failure.
// If DO_THROW is false, return success value.
// TODO: test on windows
bool algo::CreateDirRecurse(strptr s, bool do_throw = true, u32 mode = 0755) 
```

```c++
algo::Fildes algo::CreateReplacementFile(cstring &oldfname, cstring &newfname, int dfltmode) 
```

```c++
// Current value of get_cycles();
inline algo::SchedTime algo::CurrSchedTime() 
```

```c++
// Return unix-epoch time with nanosecond resolution
// On Linux, this function calls clock_gettime() which takes about
// 30ns and uses rdtsc() to increase underlying clock resolution
inline algo::UnTime algo::CurrUnTime() 
```

```c++
inline algo::UnixTime algo::CurrUnixTime()
```

```c++
inline double algo::DCeiling(double a) 
```

```c++
inline double algo::DFloor(double a) 
```

```c++
inline double algo::DRound(double a) 
```

```c++
inline double algo::DTrunc(double a) 
```

```c++
// DateCache -- Roughly 200x faster LocalDate
const algo::UnTime algo::DateCache_LocalDate(algo::DateCache &dc, UnTime in) 
```

```c++
algo::UnTime& algo::DayRange_curs_Access(algo::DayRange_curs &curs) 
```

```c++
void algo::DayRange_curs_Next(algo::DayRange_curs &curs) 
```

```c++
void algo::DayRange_curs_Reset(algo::DayRange_curs &curs, const algo::DayRange& range) 
```

```c++
bool algo::DayRange_curs_ValidQ(algo::DayRange_curs &curs) 
```

```c++
// Convert Decimal to double
double algo::Decimal_GetDouble(algo::Decimal parent) 
```

```c++
// Normalize decimal, so mantissa does not contain tail 0
void algo::Decimal_Normalize(algo::Decimal &parent) 
```

```c++
// Print Decimal
void algo::Decimal_Print(algo::Decimal parent, algo::cstring &str) 
```

```c++
// Read Decimal from string
bool algo::Decimal_ReadStrptrMaybe(algo::Decimal &parent, algo::strptr in_str) 
```

```c++
// Convert double to Decimal
void algo::Decimal_SetDouble(algo::Decimal &parent, double value) 
```

```c++
bool algo::DecodeBEF64(algo::memptr &buf, double &result) 
```

```c++
// two-byte, big endian, signed
bool algo::DecodeBEI16(algo::memptr &buf, i16 &result) 
```

```c++
// four-byte, big-endian signed
bool algo::DecodeBEI32(algo::memptr &buf, i32 &result) 
```

```c++
// eight-byte, big-endian signed
bool algo::DecodeBEI64(algo::memptr &buf, i64 &result) 
```

```c++
// two-byte, big-endian
bool algo::DecodeBEU16(algo::memptr &buf, u16 &result) 
```

```c++
// four-byte, big-endian
bool algo::DecodeBEU32(algo::memptr &buf, u32 &result) 
```

```c++
// eight-byte, big-endian
bool algo::DecodeBEU64(algo::memptr &buf, u64 &result) 
```

```c++
// single byte, boolean
bool algo::DecodeBoolean(algo::memptr &buf, bool &result) 
```

```c++
// single byte, signed
bool algo::DecodeI8(algo::memptr &buf, i8 &result) 
```

```c++
// N bytes, raw
bool algo::DecodeNBytes(algo::memptr &buf, int n, algo::memptr &result) 
```

```c++
// N bytes as chars
bool algo::DecodeNChars(algo::memptr &buf, int n, strptr &result) 
```

```c++
// helper: N bytes as chars
template<typename T> inline bool algo::DecodeNChars(algo::memptr &buf, int n, T &result) 
```

```c++
// single byte
bool algo::DecodeU8(algo::memptr &buf, u8 &result) 
```

```c++
// UUID - raw bytes
bool algo::DecodeUuid(algo::memptr &buf, algo::Uuid &result) 
```

```c++
// continuation bit (bit 7 of each byte), little-endian, u32, signed zigzag
// sign - bit 0 (lsb)
// unsigned mantissa - bits 1..N (msb)
bool algo::DecodeVLCLEI32Z(algo::memptr &buf, i32 &result) 
```

```c++
// continuation - bit 7 (msb) of each byte, little-endian, signed zigzag:
// sign - bit 0 (lsb)
// unsigned mantissa - bits 1..N (msb)
bool algo::DecodeVLCLEI64Z(algo::memptr &buf, i64 &result) 
```

```c++
// continuation bit (bit 7 of each byte), little-endian, u32
bool algo::DecodeVLCLEU32(algo::memptr &buf, u32 &result) 
```

```c++
// continuation bit (bit 7 of each byte), little-endian, u64
bool algo::DecodeVLCLEU64(algo::memptr &buf, u64 &result) 
```

```c++
// zero-terminated string
bool algo::DecodeZeroterm(algo::memptr &buf, strptr &result) 
```

```c++
// zero-terminated string
bool algo::DecodeZeroterm(algo::strptr &buf, strptr &result) 
```

```c++
// Delete file F.
// Return success code.
bool algo::DeleteFile(strptr f) NOTHROW 
```

```c++
// Return a human-readable description of STATUS as returned by wait() / waitpid()
tempstr algo::DescribeWaitStatus(int status) 
```

```c++
// Combine paths A and B, placing a slash in the middle if necessary.
// If B is already an absolute pathname, just return that.
tempstr algo::DirFileJoin(strptr a, strptr b) NOTHROW 
```

```c++
algo::DirEntry &algo::Dir_curs_Access(Dir_curs &curs) NOTHROW 
```

```c++
void algo::Dir_curs_Next(Dir_curs &curs) NOTHROW 
```

```c++
// Begin scanning files matching shell pattern PATTERN.
void algo::Dir_curs_Reset(Dir_curs &curs, strptr pattern) NOTHROW 
```

```c++
bool algo::Dir_curs_ValidQ(Dir_curs &curs) NOTHROW 
```

```c++
// Test whether PATH is an existing directory
bool algo::DirectoryQ(strptr path) NOTHROW 
```

```c++
// Elapsed time in seconds between two SchedTimes.
inline double algo::ElapsedSecs(algo::SchedTime start, algo::SchedTime end) 
```

```c++
void algo::EncodeBEF64(algo::ByteAry &buf, double value) 
```

```c++
void algo::EncodeBEI16(algo::ByteAry &buf, i16 value) 
```

```c++
void algo::EncodeBEI32(algo::ByteAry &buf, i32 value) 
```

```c++
void algo::EncodeBEI64(algo::ByteAry &buf, i64 value) 
```

```c++
void algo::EncodeBEU16(algo::ByteAry &buf, u16 value) 
```

```c++
void algo::EncodeBEU32(algo::ByteAry &buf, u32 value) 
```

```c++
void algo::EncodeBEU64(algo::ByteAry &buf, u64 value) 
```

```c++
void algo::EncodeBoolean(algo::ByteAry &buf, bool value) 
```

```c++
void algo::EncodeI8(algo::ByteAry &buf, i8 value) 
```

```c++
void algo::EncodeU8(algo::ByteAry &buf, u8 value) 
```

```c++
// UUID - raw bytes
void algo::EncodeUuid(algo::ByteAry &buf, algo::Uuid &value) 
```

```c++
// variable-length, continuation bit, i32, zigzag
void algo::EncodeVLCLEI32Z(algo::ByteAry &buf, i32 value) 
```

```c++
// variable-length, continuation bit, i64, zigzag
void algo::EncodeVLCLEI64Z(algo::ByteAry &buf, i64 value) 
```

```c++
// variable-length, continuation bit, u32
void algo::EncodeVLCLEU32(algo::ByteAry &buf, u32 value) 
```

```c++
// variable-length, continuation bit, u64
void algo::EncodeVLCLEU64(algo::ByteAry &buf, u64 value) 
```

```c++
// End append key/value to string LHS
cstring &algo::EndKeyval(cstring &lhs, int start) 
```

```c++
bool algo::EndsWithQ(strptr s, strptr sstr) 
```

```c++
// Decode error using algo_lib table of decoders
void algo::Errcode_Print(algo::Errcode &row, algo::cstring &str) 
```

```c++
// SSE implementation of fast exponentiation idea from
// http//nic.linotune.com/pubs/Schraudolph99.pdf
// ExpFast(x) goes bad at x < -709 (exp doesn't)
// Where it works, ExpFast makes a steady relative error of about 2%.
double algo::ExpFast(double y) 
```

```c++
// Taylor series expansion. For x around 0.01, this produces 2 digits per
// iteration, so with N=4 will be reasonably fast.
inline double algo::ExpTaylor(double x, int n) 
```

```c++
void algo::FatalErrorExit(const char *a) NORETURN 
```

```c++
// Read all bytes from IN_FD as a string.
// NUL characters in file will appear in the string --
// TODO: add option to sanitize string
// If FD is non-blocking, reading will terminate at first EAGAIN.
// If an error occurs, all data prior to occurrence of the error is returned.
const tempstr algo::FdToString(Fildes in_fd) NOTHROW 
```

```c++
inline strptr &algo::FileLine_curs_Access(algo::FileLine_curs &curs) 
```

```c++
void algo::FileLine_curs_Next(algo::FileLine_curs &curs) 
```

```c++
void algo::FileLine_curs_Reset(algo::FileLine_curs &curs, algo::Fildes fd) 
```

```c++
void algo::FileLine_curs_Reset(algo::FileLine_curs &curs, strptr filename) 
```

```c++
inline bool algo::FileLine_curs_ValidQ(algo::FileLine_curs &curs) 
```

```c++
// Test whether FNAME refers to a valid filesystem entity (file, directory, or special file)
// If FNAME is a soft link, then TRUE is returned even if the link points to
// a non-existent location.
bool algo::FileObjectExistsQ(strptr fname) NOTHROW 
```

```c++
// Test if F refers to an existing regular file (i.e. not a special file or directory)
bool algo::FileQ(strptr fname) NOTHROW 
```

```c++
// Return contents of file as string.
// File is trimmed at first NULL character
// Implementation note:
// linux files in /proc directory lie about their size
// and are sometimes zero-terminated.
// File is read using a "safe" method of succesively calling read.
// relying on reported file size or using mmap does not work in all cases
// Todo: test on windows
const tempstr algo::FileToString(const strptr& fname, algo::FileFlags flags = algo::FileFlags()) 
```

```c++
template<class T> inline void algo::Fill(const aryptr<T> &lhs, const T &t) 
```

```c++
template<class T, class U> inline int algo::Find(const algo::aryptr<T> &lhs, const U&t) 
```

```c++
// Search for character/string from left to right
// If found, return index where match occurs.
// If not found, return -1
int algo::FindChar(strptr lhs, char c) 
```

```c++
int algo::FindFrom(strptr s, char c, int from) 
```

```c++
int algo::FindFrom(strptr s, strptr t, int from) 
```

```c++
int algo::FindFrom(strptr s, strptr t, int from, bool case_sensitive) 
```

```c++
int algo::FindStr(strptr lhs, strptr t) 
```

```c++
int algo::FindStr(strptr lhs, strptr t, bool case_sensitive) 
```

```c++
template<class T> inline algo::aryptr<T> algo::FirstN(const algo::aryptr<T> &lhs, u32 n) 
```

```c++
inline u32  algo::FloorLog2(u32 i) 
```

```c++
inline u64  algo::FloorLog2(u64 i) 
```

```c++
// Reeturn Errcode corresponding to a UNIX error code VAL
Errcode algo::FromErrno(i64 val) 
```

```c++
algo::Uuid algo::GenerateRandomUuid() 
```

```c++
void algo::GenerateRandomUuid(algo::Uuid &uuid) 
```

```c++
// Return current directory name.
tempstr algo::GetCurDir() NOTHROW 
```

```c++
// Strip last slash and everything after it in A,
// and return that. This is equivalent to Pathcomp(A,"/RL");
strptr algo::GetDirName(strptr a) NOTHROW 
```

```c++
const tempstr algo::GetDomainname() 
```

```c++
// Return extension of pathname A, including the dot.
// GetFileExt("abcd/e.txt") -> ".txt"
// GetFileExt("abcd/e.txt/f") -> ""
// This function is equivalent to Pathcomp(A,"/RR.RR")
strptr algo::GetFileExt(strptr a) NOTHROW 
```

```c++
// Strip extension from PATH, then strip directory name from PATH,
// and return the remainder.
// c.d      -> c
// /a/b/c.d -> c
strptr algo::GetFileName(const strptr& path) NOTHROW 
```

```c++
// Return size of file referred to by FD.
// On error, return zero.
i64 algo::GetFileSize(Fildes fd) NOTHROW 
```

```c++
// Calculate size of file referred to by FILENAME.
// If file is not found or an error occurs, 0 is returned.
i64 algo::GetFileSize(strptr filename) NOTHROW 
```

```c++
// Wrapper for c library realpath function.
// On Windows: read path and expand all soft links along the way; Also eat ..'s.
// The following table shows successive values of LEFT and RIGHT as the loop executes
// ExpandLinks("bin/amc")
// LEFT                                   RIGHT
// bin                                    amc
// bin/amc
// link=../build/release/amc
// bin                                    ../build/release/amc
// build/release/amc
// build                                  release/amc
// build/release                          amc
// link=CYGWIN_NT-cl.release-x86_64
// build                                  CYGWIN_NT-cl.release-x86_64/amc
// build/CYGWIN_NT-cl.release-x86_64      amc
// build/CYGWIN_NT-cl.release-x86_64/amc
tempstr algo::GetFullPath(strptr path) NOTHROW 
```

```c++
algo::TimeStruct algo::GetGMTimeStruct(algo::UnTime U) 
```

```c++
const tempstr algo::GetHostname() 
```

```c++
// Read characters up to newline or end of file.  Return
// resulting substring, skipping the newline
// (which is either \n or \r\n).
strptr algo::GetLine(StringIter &S) 
```

```c++
TimeStruct algo::GetLocalTimeStruct(UnTime U) 
```

```c++
TimeStruct algo::GetLocalTimeStruct(UnixTime U) 
```

```c++
// Inverse mapping to abbreviated month names (Jan, Feb, etc.)
// If short_name==false, returns full name (January, February, etc.)
const strptr algo::GetMonthNameZeroBased(int index) 
```

```c++
const strptr algo::GetMonthNameZeroBasedShort(int index) 
```

```c++
int algo::GetMonthZeroBased(strptr month) 
```

```c++
// Extract Namespace name from "ns.name" format.
// This is equivalent to Pathcomp(s,".LL")
strptr algo::GetNs(strptr s) 
```

```c++
// Return current file position on FD
// There is no provision to return an error code; only the offset is returned
// (zero on failure?)
i64 algo::GetPos(Fildes fd) 
```

```c++
template<class T> inline algo::aryptr<T> algo::GetRegion(const algo::aryptr<T> &lhs, u32 lo, u32 n) 
```

```c++
// Return a run of characters up to next occurence of SEP (or to end of string)
// Skip any characters equal to SEP that follow the run
// sep="|", string = "a||b|"; return value -> "a", rest -> "b|";
// sep="|", string = "a";     return value -> "a", rest -> "";
// sep="|", string = "|x";    return value -> "" , rest -> "x";
strptr algo::GetTokenChar(algo::StringIter &S, char sep) 
```

```c++
// Search for the next occurence of SEP. If not found, assume it occurs at EOF
// Return any characters between current position and the occurence of SEP
// Skip SEP, if needed.
strptr algo::GetTokenStrptr(StringIter &iter, const strptr& separator) 
```

```c++
// Skip any leading whitespace in STR.
// Read and return next word.
strptr algo::GetTypeTag(strptr str) 
```

```c++
// empty string -> 0
// invalid weekday -> -1
// All other weeks days map to 0..6, numbers compatible with struct tm's tm_wday field
int algo::GetWeekday(strptr wday)
```

```c++
const strptr algo::GetWeekdayName(int index)
```

```c++
int algo::GetWholeMonthZeroBased(strptr s) 
```

```c++
// Skip leading characters matching SEP
// Return run of characters up to next matching SEP, or EOF.
// Do not skip trailing separators.
strptr algo::GetWordCharf(algo::StringIter &iter, bool (*sep)(u32) = algo_lib::WhiteCharQ) 
```

```c++
template<class T> inline void algo::Getary_Set(StringDesc &desc, algo::aryptr<char>(*fcn)(T&)) 
```

```c++
template<class T> inline void algo::Geti64_Set(StringDesc &desc, i64(*fcn)(T&, bool &)) 
```

```c++
i64 algo::I64Power10(u32 power) 
```

```c++
template<class T> inline void algo::Init_Set(StringDesc &desc, void(*fcn)(T&)) 
```

```c++
// insert TEXT into OUT, indenting as necessary;
// Initial indentation is INDENT, it's adjusted as necessary as { and } are found
// in the TEXT.
// Each indent is 4 spaces.
// Trailing //-sytle comments are stripped
// /* */-style comments are not supported
// Lines beginning with # (#ifdef, etc) are printed at column zero.
void algo::InsertIndent(algo::cstring &out, strptr text, int indent) 
```

```c++
void algo::Ipmask_Print(algo::Ipmask &row, algo::cstring &str) 
```

```c++
bool algo::Ipmask_ReadStrptrMaybe(Ipmask &row, algo::strptr str) 
```

```c++
template<class T> inline algo::aryptr<T> algo::LastN(const algo::aryptr<T> &lhs, u32 n) 
```

```c++
// Check if the Pathcomp expression PATHCOMP
// refers to the leftmost part of the key
// (The third character of each Pathcomp step must be 'L')
bool algo::LeftPathcompQ(algo::strptr pathcomp) 
```

```c++
// Limit length of string S ot at most LEN characters
// If S is trimmed, append "..." to the end
tempstr algo::LimitLengthEllipsis(strptr s, int len) 
```

```c++
inline strptr &algo::Line_curs_Access(Line_curs &curs) 
```

```c++
// Scan for next line.
void algo::Line_curs_Next(Line_curs &curs) 
```

```c++
void algo::Line_curs_Reset(Line_curs &curs, algo::cstring &text) 
```

```c++
// Line_curs uses strptr to hold on to the string being scanned
// (to avoid copying a potentially huge string)
// The use of "strptr&" prevents passing a temporary.
// Line_curs handling of newlines is identical to that of FileLine_curs
void algo::Line_curs_Reset(Line_curs &curs, algo::strptr &text) 
```

```c++
inline bool algo::Line_curs_ValidQ(Line_curs &curs) 
```

```c++
// Accept data block DATA as input for the line buffer
// LinebufNext will attempt to return pointers to a subregion of DATA until
// it returns false, so this block must be valid until then.
// If IMPLIED_EOF is set, then this is the last call to LinebufBegin.
// LinebufNext will know to return a partial line (with no \n) and set the
// EOF flag
void algo::LinebufBegin(LineBuf &L, memptr data, bool implied_eof) 
```

```c++
// Scan memory provided by LinebufBegin for line separator (hard-coded to be '\n')
// If the function returns FALSE, and there is a partial line remaining,
// it is saved to an internal buffer in LineBuf (called BUF).
// It will be subsequently recombined with incoming data to form a contiguous line,
// but copying memory is avoided whenever possible.
// Usage:
// LinebufBegin(linebuf,data,true);
// for (strptr line; LinebufNext(linebuf,line); ) {
// <process line>
// }
bool algo::LinebufNext(LineBuf &L, strptr &result) 
```

```c++
void algo::ListSep_Print(const algo::ListSep &t, algo::cstring &str) 
```

```c++
const algo::UnTime algo::LocalDate(UnTime in) 
```

```c++
const algo::UnixTime algo::LocalDate(UnixTime in) 
```

```c++
// lock all presently, and future allocated bytes in physical memory.
// Return success value. Errno provides more info.
bool algo::LockAllMemory() 
```

```c++
// Return Errcode corresponding to namespace <ns> and code VAL
// The namespace should have a decoder function (decode_Call(*errns_Find(ns), val))
Errcode algo::MakeErrcode(algo::Errns ns, i64 val) 
```

```c++
void algo::MakeLower(strptr s) 
```

```c++
void algo::MakeUpper(strptr s) 
```

```c++
// Append / to string STR unless STR already ends in one.
// str << dirname << MaybeDirSep << filename.
// The separator is always /. To support windows-specific pathnames,
// use ToWindows path where appropriate.
void algo::MaybeDirSep(cstring &str) 
```

```c++
// Append a space unless the string already ends with a space
void  algo::MaybeSpace(cstring &str) 
```

```c++
// Return modification time of file FILENAME
// If file is not found or is not accessible, return 0.
algo::UnTime algo::ModTime(strptr filename) 
```

```c++
// scan STR until character SEP is found.
// if SEP is not found, it is assumed to be beyond the right side of the string
// set LEFT to point to the characters to the left of found location.
// set STR to point to the characters to the right of found location.
// Example:
// str = "abc.def", sep='.'; After NextSep, str="def", left="abc"
// str = "abc-def", sep='.'; After NextSep, str="", left="abc-def"
void algo::NextSep(algo::strptr &str, char sep, algo::strptr &left) 
```

```c++
bool algo::NullUuidQ(const algo::Uuid &uuid) 
```

```c++
// Open file FILENAME with flags FLAGS, return resulting file descriptor
// Possible flags:
// write   -> open file for writing, create file if missing
// if write is specified, and append is not, then file is truncated upon opening
// append  -> open file in append mode (automatically sets 'write' flag)
// in append mode, seek to end of file after opening
// read    -> open flag in read-only mode
// _throw  -> throw exception if an error occurs
// NOTE: this function doesn't set O_APPEND flag, because it doesn't work'
// on NFS mounted filesystems.
// Without O_APPEND, two processes cannot reliably append to the same file.
algo::Fildes algo::OpenFile(const strptr& filename, algo::FileFlags flags) 
```

```c++
algo::Fildes algo::OpenRead(const strptr& filename, algo::FileFlags flags = algo::FileFlags()) 
```

```c++
algo::Fildes algo::OpenWrite(const strptr& filename, algo::FileFlags flags = algo::FileFlags()) 
```

```c++
inline int algo::P1Mod(int a, int b)  
```

```c++
// C           single hex character, upper or lowercase
// RETURN      value of hex character
// RESULT      value of hex digit
// RETURN      success code
inline int algo::ParseHex1(u32 c, u8 &result) 
```

```c++
// STR         little-endian string up to 2 chars in length
// LEN         length of STR
// RESULT      value of hex number encoded in STR
// RETURN      number of characters to skip (0 - unsuccessful)
int algo::ParseHex2(u32 str, int len, u8 &result) 
```

```c++
// Convert string to numeric type.
// If there is an error, or string is empty, return DFLT.
// If there is junk at the end of the string, silently ignore it.
// Use StringIter version to check for junk, or use -X version to throw exception.
inline int algo::ParseI32(strptr str, int dflt) 
```

```c++
int algo::ParseI32(StringIter &iter, int dflt) 
```

```c++
i64 algo::ParseI64(StringIter &iter, i64 dflt) 
```

```c++
inline i64 algo::ParseI64(strptr str, i64 dflt) 
```

```c++
inline u32 algo::ParseNum4(u32 x, u32 &ok) 
```

```c++
u32 algo::ParseNum4(u32 x, u32 len, u32 &ok) 
```

```c++
inline u32 algo::ParseNum8(u64 x, u32 &ok) 
```

```c++
u32 algo::ParseNum8(u64 x, u32 len, u32 &ok) 
```

```c++
inline int algo::ParseOct1(u32 c, u8 &result) 
```

```c++
// STR        little-endian string up to 4 chars
// LEN        length of STR
// RESULT     Octal value encoded in str
// RETURN     number of characters to skip (0 - unsuccessful)
int algo::ParseOct3(u32 str, int len, u8 &result) 
```

```c++
inline u32 algo::ParseU32(strptr str, u32 dflt) 
```

```c++
u32 algo::ParseU32(StringIter &iter, u32 dflt) 
```

```c++
inline u64 algo::ParseU64(strptr str, u64 dflt) 
```

```c++
// Convert string to numeric type.
// If there is an error, or string is empty, return DFLT.
// If there is junk at the end of the string, silently ignore it.
// Use StringIter version to check for junk, or use -X version to throw exception.
u64 algo::ParseU64(StringIter &iter, u64 dflt) 
```

```c++
algo::UnTime algo::ParseUnTime(StringIter &s, const algo::strptr spec) 
```

```c++
algo::UnTime algo::ParseUnTime(const algo::strptr& s, const algo::strptr spec) 
```

```c++
// Parses HTTP URL
// Place path part as value of first attr,
// other attrs are key-value pairs in order as the appear
void algo::ParseUrl(algo::Tuple &out, strptr in) 
```

```c++
// S         source string
// EXPR      string in the form (XYZ)*
// - X is the character to search
// - Y is L to search from left, R to search from right
// - Z is which site to pick: L=left, R=right
// If the character to search is not found, it assumed to be found
// at the far end of the string with respect to search direction.
// Function processes characters from expr in groups of 3.
// Example:
// s = "a.b.c.d"
// expr = ".LR.LL"
// first, search for . from left; pick right hand side (b.c.d)
// then, search for . from left. pick left hand side (b)
// result: b
// More examples:
// s = "abcd"; expr = ".LL"; result = "abcd"
// s = "abcd"; expr = ".LR"; result = ""
strptr algo::Pathcomp(strptr s, strptr expr) 
```

```c++
// Examine string VALUE
// Determine if it needs quoting as an ssim key/value,
// and return appropriate quote character. If no quote character is needed,
// return NUL.
char algo::PickSsimQuoteChar(strptr value) 
```

```c++
// Append key-value pair NAME:VALUE to string STR
void algo::PrintAttr(cstring &str, const algo::strptr &name, const strptr &value) 
```

```c++
// Print a string suitable for parsing with Tuple
// If the string doesn't need quotes, it is printed as-is.
// If quotes are needed, first determine whether to use single or double quotes to minimize
// escaping. Then print the string using char_PrintCppEsc for quoting.
// Empty string is printed as ""
void algo::PrintAttrSpace(cstring &str, algo::strptr name, const strptr &value) 
```

```c++
// Append key-value pair NAME:VALUE to string STR, and remove all
// characters from VALUE (used by amc)
void algo::PrintAttrSpaceReset(cstring &str, const algo::strptr &name, algo::cstring &value) 
```

```c++
// Append key-value pair NAME:VALUE to string STR,
// and remove all characters from VALUE (used by amc)
void algo::PrintAttrSpaceReset(cstring &str, const char *name, algo::cstring &value) 
```

```c++
// Default implementation of prlog handler
// This function is called via algo_lib::_db.Prlog pointer.
// The pointer may be set by the application to replace the function with something else.
// 
// Notes on WriteFile use:
// we must use WriteFile (which contains a loop) to write all the bytes out,
// otherwise some terminals push back and refuse the data.
void algo::Prlog(algo_lib::FLogcat *logcat, algo::SchedTime tstamp, strptr str) 
```

```c++
template<class T, class U> inline T algo::PtrAdd(U *ptr, int_ptr offset) 
```

```c++
void algo::Ptr_Print(void *ptr, algo::cstring &out) 
```

```c++
// Read big-endian word
inline u16 algo::ReadBE16(const void *val) 
```

```c++
inline u32 algo::ReadBE32(const void *val) 
```

```c++
inline u64 algo::ReadBE64(const void *val) 
```

```c++
// Read little-endian word
inline u16 algo::ReadLE16(const void *val) 
```

```c++
inline u32 algo::ReadLE32(const void *val) 
```

```c++
inline u64 algo::ReadLE64(const void *val) 
```

```c++
// Read soft link and return resulting path.
// If PATH is not a soft link, return empty string
// This is not the function to resolve symlinks (if link temp/x points to y, then
// this function will return string "y", not "temp/y"), use GetFullPath for
// full service
tempstr algo::ReadLink(strptr path) 
```

```c++
// Refurbish: destroy and re-create object in place.
// This function is exception-safe: if ~T throws an exception, T::T still gets
// called.
// Implementation note: explicit calls to destructor are checked for NULL pointer
// by GCC, so the nonnull attribute is required.
template<class T> inline void algo::Refurbish(T &t) F_NONNULL 
```

```c++
// Remove directory.
// Return TRUE if operaiton succeeds
// The function will fail if the directory is not empty. For that case,
// use RemDirRecurse.
bool algo::RemDir(strptr name) 
```

```c++
// Destroy directory NAME, and any subdirectories.
// If REMOVE_TOPMOST is specified, delete the directory as well,
// (otherwise, only the contents are deleted).
// If filesystem entry exists and is NOT a directory, return true.
bool algo::RemDirRecurse(strptr name, bool remove_topmost) 
```

```c++
int algo::Replace(cstring &str, const strptr& from, const strptr& to, bool case_sensitive = true, bool preserve_case = true) 
```

```c++
// Replace extension in pathname A with string B.
tempstr algo::ReplaceExt(strptr a, strptr b) NOTHROW 
```

```c++
// F  ("a.txt","b") -> "b.txt";
// F  ("a.txt","/b.jpg") -> "b.jpg.txt"
tempstr algo::ReplaceFileName(const strptr& a, const strptr& b) 
```

```c++
template<class T> inline algo::aryptr<T> algo::RestFrom(const algo::aryptr<T> &lhs, u32 n) 
```

```c++
bool algo::SafeStringToFile(const strptr& str, const strptr& filename) 
```

```c++
// Compare STR to contents of FILENAME.
// If they differ, use a tempfile construction to atomically UPDATE file
// with contents of STR.
// NOTE: This will result in a new inode for file!
// No exceptions are thrown. If the function fails, check errno.
// Default mode for new file is provided by MODE.
// If the file is being replaced, MODE is ignored and copied from the old file.
bool algo::SafeStringToFile(const strptr& str, const strptr& filename, int dfltmode)
```

```c++
// SameSign returns false if one of the arguments is zero.
template<typename T> inline bool algo::SameSignQ(T a, T b) 
```

```c++
bool algo::SaneTerminalQ() 
```

```c++
// Set file position of FD to OFF
// Return success status
bool algo::SeekFile(Fildes fd, i64 off) 
```

```c++
inline strptr &algo::Sep_curs_Access(algo::Sep_curs &curs) 
```

```c++
void algo::Sep_curs_Next(algo::Sep_curs &curs) 
```

```c++
void algo::Sep_curs_Reset(algo::Sep_curs &curs, strptr line, char sep) 
```

```c++
inline bool algo::Sep_curs_ValidQ(algo::Sep_curs &curs) 
```

```c++
// Set blocking mode on file descriptor FD to BLOCKING
// Return result of FCNTL (0==success)
int algo::SetBlockingMode(Fildes fildes, bool blocking) NOTHROW 
```

```c++
// Change current directory to DIR and return success status
// errno is set as witih chdir() call
bool algo::SetCurDir(strptr in_dir) NOTHROW 
```

```c++
// Change TZ environment variable to specified value
// and notify C runtime lib of the change
void algo::SetTz(strptr zone) 
```

```c++
template<class T> inline void algo::SetnumMaybe_Set(StringDesc &desc, bool(*fcn)(T&, i64 num)) 
```

```c++
void algo::SetupExitSignals(bool sigint = true) 
```

```c++
// catch fatal signals and show backtrace
void algo::SetupFatalSignals() 
```

```c++
// print binary octet string as hex
void algo::Sha1sig_Print(algo::Sha1sig &sha1sig, algo::cstring &out) 
```

```c++
bool algo::Sha1sig_ReadStrptrMaybe(algo::Sha1sig &sha1sig, algo::strptr str) 
```

```c++
void algo::ShowStackTrace(uintptr_t start_ip, cstring &out) 
```

```c++
// if next character matches WHAT, skip and return true
// otherwise return false
inline bool algo::SkipChar(StringIter &iter, char what) 
```

```c++
// Case-sensitive version
bool algo::SkipStrptr(StringIter &iter, strptr str) 
```

```c++
// if the next characters match STR, skip and return true
// optionally do case sensitive comparison
bool algo::SkipStrptr(StringIter &iter, strptr str, bool case_sens) 
```

```c++
// Call sleep/usleep, giving up MS milliseconds
void algo::SleepMsec(int ms) 
```

```c++
// TODO generate with amc
lib_json::FNode *algo::Smallstr50_FmtJson(algo::Smallstr50 &value, lib_json::FNode *parent) 
```

```c++
// Construct ssim filename from key
// SsimFname("data", "abcd") -> data/abcd.ssim
// SsimFname("data/", "abcd.ef") -> data/abcd/ef.ssim
tempstr algo::SsimFname(strptr root, strptr rel) 
```

```c++
bool algo::StartsWithQ(strptr s, strptr sstr, bool case_sensitive = true) 
```

```c++
// Check strings for equality, optionally in a case-insensitive way
bool algo::StrEqual(strptr a, strptr b, bool case_sens) 
```

```c++
// Copy bytes from one location to another in blocks of 16 bytes.
void algo::StreamFromToB16(void *from, void *to, u32 n) 
```

```c++
// Replace contents of file FILENAME with string STR.
// If CHECK_SAME is specified, first compare contents and do not perform a write
// if the contents are the same.
// FLAGS may specify algo_FileFlags__throw, in which case an exception is thrown on error
bool algo::StringToFile(const strptr& str, const strptr& filename, algo::FileFlags flags = algo_FileFlags__throw, bool check_same = true)
```

```c++
algo::UnDiff algo::StripDate(UnTime t) 
```

```c++
// Strip last component of a path.
// If the path ends in a slash, the slash is stripped as well.
// So, StripDirComponent("abcde/d/") -> abcde/
// So, StripDirComponent("abcde/d") -> abcde/
strptr algo::StripDirComponent(strptr a) NOTHROW 
```

```c++
// Remove everything up to and including the last slash in A,
// and return that. This is equivalent to Pathcomp(A,"/RR");
strptr algo::StripDirName(strptr a) NOTHROW 
```

```c++
// Remove pathname A with any extension stripped.
// StripExt("abcd/e.txt") -> "abcd/e"
// StripExt("abcd/e.txt/f") -> "abcd/e/f"
// (The last slash shields the extension, so this function has no corresponding
// Pathcomp expression)
strptr algo::StripExt(strptr a) NOTHROW 
```

```c++
// if IDENT starts with NS_NAME., return ident without prefix
// otherwise return ident
// if NS_NAME is empty, removes any dotted prefix from IDENT.
// NS_NAME      name of 'current' namespace (optional)
// IDENT        identifier in question
// 
// StripNs("abc", "abc.def") -> "def"
// StripNs(""   , "abc.def") -> "def"
// StripNs("abc", "def.ghi") -> "def.ghi"
strptr algo::StripNs(strptr ns_name, strptr ident) 
```

```c++
// Skip any leading whitespace.
// Read next word (GetWordCharf) and compare to typetag.
// Skip any subsequent whitespace as well.
// Return true if type typetag matches
// if typetag doesn't match, input string remains unchanged
// " a  b " -> typetag is "a", rest is "b "
// "a    b" -> typetag is "a", rest is "b"
bool algo::StripTypeTag(strptr &in_str, strptr typetag) 
```

```c++
// This function echoes the corresponding MySQL function.
// Treat STR as an array of tokens separated by C.
// Locate and return tokens 1..IDX. Negative numbers cause scanning from the right.
// Indexes are 1-based
// Example:
// SubstringIndex("a.b.c", '.',  1) -> "a"
// SubstringIndex("a.b.c", '.',  2) -> "a.b"
// SubstringIndex("a.b.c", '.',  3) -> "a.b.c"
// SubstringIndex("a.b.c", '.',  4) -> "a.b.c"
// SubstringIndex("a.b.c", '.', -1) -> "c"
// SubstringIndex("a.b.c", '.', -2) -> "b.c"
// SubstringIndex("a.b.c", '.', -3) -> "a.b.c"
strptr algo::SubstringIndex(strptr str, char c, int idx) 
```

```c++
// Execute unix command and return status code.
// Execute command using system().
// fail_ok: if true, command must succeed or throw exception
// dry_run: if true, do not run command, just print
// echo: print command. command alwyas echoed if verbose mode is on
int algo::SysCmd(strptr cmd, FailokQ fail_ok = FailokQ(true), DryrunQ dry_run = DryrunQ(false), EchoQ echo = EchoQ(false)) 
```

```c++
// Execute unix command and return output.
tempstr algo::SysEval(strptr cmd, FailokQ fail_ok, int max_output, bool echo = false) 
```

```c++
inline algo::i32_Range algo::TFind(const strptr &s, char match) 
```

```c++
inline algo::i32_Range algo::TRevFind(const strptr &s, char match) 
```

```c++
template<class T> inline void algo::TSwap(T &a, T &b)  
```

```c++
tempstr algo::Tabulated(strptr in, strptr sep) 
```

```c++
// Locate all occurences of SEP in string IN, scanning line-by-line.
// Compute column widths; Scan string IN again.
// Expand each column to the required width, leaving COLSPACE characters between
// columns.
// String FMT specifies, for each column, whether to align its contents left,center,or right
// ('l','c','r'). Default is 'l'.
// Example text
// aa\tcc\tbb
// asdfasdf\t\tyy
// Tabulated(text, "\t", "rl", 2) ->
// aa  cc  bb
// asdfasdf      yy
tempstr algo::Tabulated(strptr in, strptr sep, strptr fmt, int colspace) 
```

```c++
void algo::Throw() NORETURN 
```

```c++
// The several variants of Throw are all the same -- the goal is to get as many
// setup instructions out of the execution path as possible, to avoid polluting instruction
// cache.
void algo::Throw(strptr text, Errcode err) NORETURN 
```

```c++
// print TIME to STR, according to spec SPEC
// The following control characters are supported in SPEC.
// %Y     Year printed as 4 digits, e.g. 2002
// %y     Last 2 digits of year printed as 2 digits
// %B     Long month name (February)
// %b     Short month name (Feb)
// %a     Week day name (Monday)
// %m     Month printed as 2 digits, with January being 01
// %d     Day of month printed as at least 2 digits (or WIDTH)
// %.X    where X is a number -- set width
// %I     Hour using 12-hour format
// %H     Hour using 24-hour format, printed as 2 digits
// %M     Minute, printed as 2 digits
// %S     Second, printed as 2 digits
// %T     Shortcut for %H:%M:%S.%.9X
// %X     Print fractions of a second according to width
// %x     Print milliseconds with 3 decimal places
// %-     Print - sign if timespec is negative
// %%     Print % sign
void algo::TimeStruct_Print(const TimeStruct &time, algo::cstring &str, const algo::strptr &spec) 
```

```c++
bool algo::TimeStruct_Read(TimeStruct &out, algo::StringIter &iter, const strptr& spec) 
```

```c++
inline char algo::ToLower(char i)  
```

```c++
// Note: cstring << algo::ToLower(int) will output an *integer*, not a character
inline int algo::ToLower(int i)  
```

```c++
inline algo::SchedTime algo::ToSchedTime(double secs) 
```

```c++
// Convert scheduler time units to seconds.
inline double algo::ToSecs(SchedTime ticks) 
```

```c++
inline double algo::ToSecs(UnDiff t) 
```

```c++
inline double algo::ToSecs(UnTime t) 
```

```c++
inline double algo::ToSecs(algo::WDiff t) 
```

```c++
inline algo::strptr algo::ToStrPtr(memptr buf)  
```

```c++
TimeStruct algo::ToTimeStruct(UnDiff   U) 
```

```c++
TimeStruct algo::ToTimeStruct(UnixDiff U) 
```

```c++
algo::UnDiff algo::ToUnDiff(const TimeStruct &S) 
```

```c++
inline algo::UnDiff algo::ToUnDiff(SchedTime ticks) 
```

```c++
inline algo::UnDiff algo::ToUnDiff(algo::WDiff d) 
```

```c++
algo::UnTime algo::ToUnTime(WTime s) 
```

```c++
algo::UnTime algo::ToUnTime(const TimeStruct &S) 
```

```c++
inline algo::UnTime algo::ToUnTime(UnixTime t) 
```

```c++
algo::UnixDiff algo::ToUnixDiff(const TimeStruct &S) 
```

```c++
inline algo::UnixDiff algo::ToUnixDiff(algo::WDiff d) 
```

```c++
algo::UnixTime algo::ToUnixTime(const TimeStruct &S) 
```

```c++
inline algo::UnixTime algo::ToUnixTime(UnTime t) 
```

```c++
inline algo::UnixTime algo::ToUnixTime(algo::WTime nt) 
```

```c++
inline char algo::ToUpper(char i)  
```

```c++
// Note: cstring << algo::ToUpper(int) will output an *integer*, not a character
inline int algo::ToUpper(int i)  
```

```c++
inline algo::WDiff algo::ToWDiff(algo::UnDiff d) 
```

```c++
inline algo::WDiff algo::ToWDiff(algo::UnixDiff d) 
```

```c++
inline algo::WTime algo::ToWTime(algo::UnTime s) 
```

```c++
inline algo::WTime algo::ToWTime(algo::UnixTime t) 
```

```c++
// Convert unix path to windows path
// This replaces slashes with backslashes
tempstr algo::ToWindowsPath(strptr path) 
```

```c++
// Scan S. For any character in S that occurs in string FROM, replace with corresponding
// character from string TO.
// Example:
// Translate("aabcd", "bd", "xy") -> "aaxcy"
void algo::Translate(strptr s, strptr from, strptr to) 
```

```c++
// TrimmedLeft + TrimmedRight
strptr algo::Trimmed(strptr s) 
```

```c++
// Strip leading whitespace, return new strptr.
strptr algo::TrimmedLeft(strptr s) 
```

```c++
// Strip trailing whitespace, return new strptr.
strptr algo::TrimmedRight(strptr s) 
```

```c++
// Truncate file indicated by FD to size SIZE.
// Return success status
// TODO: Test on windows
bool algo::TruncateFile(Fildes fd, i64 size) 
```

```c++
// Try parse Decimal [+-][0-9]*[.][0-9]*
// Stop after first non-conforming char, error if no digit has been met.
bool algo::TryParseDecimal(algo::StringIter &iter, algo::Decimal &result) 
```

```c++
// Read a series of digits and return resulting number.
// Return success code
bool algo::TryParseDigits(algo::StringIter &S, double &result) 
```

```c++
bool algo::TryParseDouble(algo::StringIter &iter, double &result) 
```

```c++
// TODO: document these more carefully
// Read a series of digits N, returning N / pow(10, length(N))
// If successful, advance index. Otherwise, leave index where it was.
bool algo::TryParseFraction(algo::StringIter &S, double &result) 
```

```c++
bool algo::TryParseI32(algo::StringIter &iter, i32 &result) 
```

```c++
bool algo::TryParseI64(algo::StringIter &iter, i64 &result) 
```

```c++
bool algo::TryParseU32(algo::StringIter &iter, u32 &result) 
```

```c++
bool algo::TryParseU64(algo::StringIter &iter, u64 &result) 
```

```c++
bool algo::TryParseU128(algo::StringIter &iter, u128 &result) 
```

```c++
void algo::Tuple_Print(algo::Tuple &row_, algo::cstring &str) 
```

```c++
// T             target tuple. the tuple is not emptied before parsing.
// STR           source string
// ATTRONLY        if set, all loaded attrs are appended to the ATTRS
// array. otherwise, the first attr becomes HEAD.
// CMT_CHAR      character at which to stop parsing.
// Parse sequence of attrs (name-value pairs) into tuple T.
// Roughly:
// ATTR       -> VALUE | VALUE ':' VALUE
// VALUE      -> IDENTIFIER | C++-STRING
// Return value: FALSE if unterminated string or other badness found in data.
bool algo::Tuple_ReadStrptr(algo::Tuple &tuple, strptr str, bool attronly) 
```

```c++
bool algo::Tuple_ReadStrptrMaybe(Tuple &row, algo::strptr s) 
```

```c++
void algo::URL_Print(algo::URL &url, algo::cstring &str) 
```

```c++
// Parse a URL from string STR to OUT.
// The format of a URL is
// protocol://user:password@somehost:port<-source_interface/dir1/dir2
// URL fields are
// protocol, username, password, server, port, host, source_addr_host, dir
// 
// 
// Windows pathnames are supported, e.g.
// when parsing
// file://c:/dir/dir2
// c: will not be parsed as a username, but as part of the pathname.
// 
bool algo::URL_ReadStrptrMaybe(URL &out, algo::strptr str) 
```

```c++
inline algo::UnDiff algo::UnDiffHMS(int h, int m, int s) 
```

```c++
inline algo::UnDiff algo::UnDiffMsecs(const i64 i) 
```

```c++
inline algo::UnDiff algo::UnDiffSecs(double d) 
```

```c++
inline algo::UnDiff algo::UnDiffSecs(i32 i) 
```

```c++
inline algo::UnDiff algo::UnDiffSecs(i64 i) 
```

```c++
inline algo::UnDiff algo::UnDiffUsecs(const double d) 
```

```c++
void algo::UnDiff_Print(UnDiff row, algo::cstring &str) 
```

```c++
void algo::UnDiff_PrintSpec(UnDiff   t, algo::cstring &out, const algo::strptr &spec) 
```

```c++
bool algo::UnDiff_ReadStrptrMaybe(UnDiff &row, algo::strptr str) 
```

```c++
inline algo::UnTime algo::UnTimeSecs(double d) 
```

```c++
void algo::UnTime_Print(algo::UnTime row, algo::cstring &str) 
```

```c++
void algo::UnTime_PrintSpec(UnTime t, algo::cstring &out, const algo::strptr &spec) 
```

```c++
// Read time from STR to ROW
// Return success code.
// If funciton does not succeed, ROW is not modified
// Several formats are supported:
// %Y-%m-%dT%T
// %Y-%m-%d %T
// %Y/%m/%d %T
// %Y-%m-%d
// %Y/%m/%d
// Where %T is %H:%M:%S.%X
// And %X is the nanosecond portion
bool algo::UnTime_ReadStrptrMaybe(algo::UnTime &row, algo::strptr str) 
```

```c++
// Given a string of up to 4 characters encoded in u32 STR
// (LSB = first char, etc.)
// Decode character into RESULT, returning number of characters read.
// The character is C++-escaped: \a, \b, \f, \n, \r, \t, \v, \', \", \\, \? are supported
// as well as octal (\OOO) and hex (\xHH) sequences.
int algo::UnescapeC(u32 str, int len, u8 &result) 
```

```c++
inline algo::UnixDiff algo::UnixDiffHMS(i64 h, int m, int s) 
```

```c++
void algo::UnixDiff_Print(UnixDiff row, algo::cstring &str) 
```

```c++
void algo::UnixDiff_PrintSpec(UnixDiff t, algo::cstring &out, const algo::strptr &spec) 
```

```c++
void algo::UnixTime_Print(algo::UnixTime row, algo::cstring &str) 
```

```c++
void algo::UnixTime_PrintSpec(UnixTime t, algo::cstring &out, const algo::strptr &spec) 
```

```c++
bool algo::UnixTime_ReadStrptrMaybe(algo::UnixTime &row, algo::strptr str) 
```

```c++
// perform standard url percent decoding (for raw url, path),
// and optionally translate '+' to space (for application/x-www-form-urlencoded)
tempstr algo::UrlDecode(strptr in, bool plus) 
```

```c++
void algo::Uuid_Print(algo::Uuid &parent, algo::cstring &str) 
```

```c++
bool algo::Uuid_ReadStrptrMaybe(algo::Uuid &parent, strptr str) 
```

```c++
// Default, and invalid, value for Fildes is -1. 0 is a valid value (stdin)!
inline bool algo::ValidQ(Fildes fd) 
```

```c++
void algo::WDiff_Print(algo::WDiff row, algo::cstring &str) 
```

```c++
bool algo::WDiff_ReadStrptrMaybe(algo::WDiff &parent, algo::strptr in_str) 
```

```c++
void algo::WTime_Print(algo::WTime row, algo::cstring &str) 
```

```c++
bool algo::WTime_ReadStrptrMaybe(algo::WTime &parent, algo::strptr in_str) 
```

```c++
inline strptr &algo::Word_curs_Access(Word_curs &curs) 
```

```c++
void algo::Word_curs_Next(Word_curs &curs) 
```

```c++
void algo::Word_curs_Reset(Word_curs &curs, strptr text) 
```

```c++
inline bool algo::Word_curs_ValidQ(Word_curs &curs) 
```

```c++
// Write NWRITE bytes starting at START to file descriptor FILDES.
// Go until all bytes are written on an error occurs.
// If FILDES is non-blocking, spin indefinitely until bytes do get through.
// At the end, return success status (TRUE if all bytes written)
bool algo::WriteFile(algo::Fildes fildes, u8 *start, int nwrite) 
```

```c++
// Write BYTES to file FD;
// If FD is non-blocking, the function may fail.
// Throw exception on error. Upon completion, the function has succeeded
void algo::WriteFileX(Fildes fd, memptr bytes) 
```

```c++
strptr algo::Yield(algo::u64_RangesList &p) 
```

```c++
template<class T> inline void algo::ZeroBytes(T &t) 
```

```c++
// Same thing but with tempstr.
// Typical usage is to take a strptr
// expression and pass it to some unix call
// some_unix_call(Zeroterm(tempstr(some_strptr)))
// 
char *algo::Zeroterm(const tempstr &rhs) 
```

```c++
// Append NUL character to the end of the string and return a
// pointer to the C string.
// There is no guarantee that there are no NUL characters in the middle
// of STR, that is up to the user.
// cstring is normally NOT null terminated (it is length-based),
// so this function is necessary when passing strings to C library functions
// or the OS
char *algo::Zeroterm(cstring &rhs) 
```

```c++
inline u32 algo::aParseNum4(strptr s, u32 &ok) 
```

```c++
inline u32 algo::aParseNum8(strptr s, u32 &ok) 
```

```c++
u64 algo::aParseNum16(strptr s, u32 &ok) 
```

```c++
// Append new attribute with specified name and value.
void algo::attr_Add(Tuple &T, strptr name, strptr value) 
```

```c++
Attr* algo::attr_Find(Tuple &tuple, strptr name, int occurence = 0) 
```

```c++
strptr algo::attr_GetString(Tuple &T, strptr name, strptr dflt = strptr()) 
```

```c++
// Update atribute if exists, otherwise create new one
void algo::attr_Set(Tuple &T, strptr name, strptr value) 
```

```c++
algo::i32_Range algo::ch_FindFirst(const algo::strptr &s, char match) 
```

```c++
algo::i32_Range algo::ch_FindLast(const algo::strptr &s, char match) 
```

```c++
inline int algo::ch_First(const strptr &s, int dflt = 0) 
```

```c++
inline algo::aryptr<char> algo::ch_FirstN(const strptr &lhs, u32 n) 
```

```c++
inline algo::aryptr<char> algo::ch_GetRegion(const strptr &lhs, u32 lo, u32 n) 
```

```c++
inline int algo::ch_Last(const strptr &s, int dflt = 0) 
```

```c++
inline algo::aryptr<char> algo::ch_LastN(const strptr &lhs, u32 n) 
```

```c++
inline int algo::ch_N(const strptr &s) 
```

```c++
inline int algo::ch_N(const tempstr &str) 
```

```c++
template<class T> inline int algo::ch_N(const aryptr<T> &ary) 
```

```c++
inline algo::aryptr<char> algo::ch_RestFrom(const strptr &lhs, u32 n) 
```

```c++
// Write character C into string OUT, using C++ character escapement rules
// QUOTE_CHAR also gets escaped.
void algo::char_PrintCppEsc(char c, algo::cstring &out, char quote_char) 
```

```c++
// print character as c++ character, surrounded by single quotes
// and properly escaped according to c++ rules
void algo::char_PrintCppSingleQuote(int c, algo::cstring &out) 
```

```c++
// Append N instances of character C to string OUT.
void algo::char_PrintNTimes(char c, algo::cstring &out, int n) 
```

```c++
// prints a character suitable for appearance inside an XML string.
// character is either printed as-is, or is escaped so that its meaning does not change.
void algo::char_PrintXml(char c, algo::cstring &out, bool quoted) 
```

```c++
lib_json::FNode *algo::cstring_FmtJson(algo::cstring &value, lib_json::FNode *parent) 
```

```c++
inline u32 algo::cstring_Hash(u32 prev, const algo::strptr &val) 
```

```c++
void algo::cstring_Print(algo::cstring &row, algo::cstring &str) 
```

```c++
// Implementations of built-in read functions for AMC.
// basis function for reading tuples.
// scan ITER for identifier, or quoted string.
// return FALSE if attribute is malformed (i.e. unterminated string)
bool algo::cstring_ReadCmdarg(cstring &out, algo::StringIter &S, bool is_value) 
```

```c++
bool algo::cstring_ReadStrptrMaybe(algo::cstring &row, algo::strptr str) 
```

```c++
// User-defined cleanup trigger for dir_handle field of ctype:algo.DirEntry
void algo::dir_handle_Cleanup(algo::DirEntry &dir_entry) 
```

```c++
inline double algo::double_NegateIf(double x, bool i) 
```

```c++
// print value*100
// Example:
// double_PrintPercent(0.334, str, 1) -> "33.4%"
void algo::double_PrintPercent(double value, algo::cstring &str, int prec) 
```

```c++
// Print double D into string OUT with PRECISION digits
// after the decimal point.
// If OMIT_ZEROS is specified, trailing zeros that are safe to omit are omitted.
// If COMMAS is specified, the large numbers are printed in groups of 3 digits
// with commas between them.
void algo::double_PrintPrec(double d, algo::cstring &out, int precision, bool omit_zeros, bool commas) 
```

```c++
// ignore:bigret
algo::cstring algo::double_PrintWithCommas(double value, int prec) 
```

```c++
void algo::double_PrintWithCommas(double value, algo::cstring &str, int prec) 
```

```c++
double algo::double_WeakRandom(double scale) 
```

```c++
template<class T> inline int algo::elems_N(const aryptr<T> &ary) 
```

```c++
// Append and end-of-line sequence to string S.
inline void algo::eol(cstring &s) 
```

```c++
inline float algo::float_NegateIf(float x, bool i) 
```

```c++
// get CPU HZ value as double
inline double algo::get_cpu_hz() 
```

```c++
// get CPU HZ value as u64
inline u64 algo::get_cpu_hz_int() 
```

```c++
// cpu_hz (untyped SchedTime)
// use this for timestamps.
// these calls may be pipelined and reordered, so measuring instruction
// latency with these is not possible. for that, use rdtscp
inline u64 algo::get_cycles() 
```

```c++
strptr algo::gitinfo_Get() 
```

```c++
inline int algo::i32_NegateIf(int x, bool i) 
```

```c++
void algo::i32_Range_Print(algo::i32_Range &r, algo::cstring &o) 
```

```c++
i32 algo::i32_WeakRandom(i32 modulo) 
```

```c++
inline i64 algo::i64_NegateIf(i64 x, bool i) 
```

```c++
// pads with zeros on the left so that at least 'atleast' characters are output.
void algo::i64_PrintPadLeft(i64 num, algo::cstring &out, int atleast) 
```

```c++
// print bytes in hex e.g: 00 01 ff
void algo::memptr_Print(algo::memptr parent, algo::cstring &str) 
```

```c++
inline algo::aryptr<char> algo::memptr_ToStrptr(algo::aryptr<u8> rhs) 
```

```c++
inline algo::cstring &algo::operator<<(algo::cstring &out, const ListSep &t) 
```

```c++
inline algo::cstring &algo::operator<<(algo::cstring &out, const char* t) 
```

```c++
inline algo::cstring &algo::operator<<(algo::cstring &out, const strptr &t) 
```

```c++
inline algo::cstring &algo::operator<<(algo::cstring &out, void (*t)(algo::cstring &)) 
```

```c++
template<class T> inline algo::aryptr<T> algo::qGetRegion(const algo::aryptr<T> &lhs, u32 lo, u32 n) 
```

```c++
template<class T> inline T &algo::qLast(const algo::aryptr<T> &ary) 
```

```c++
inline int algo::range_N(const i32_Range &rhs) 
```

```c++
// use this for performance measurements.
// according to Intel software manual, lfence followed by rdtsc
// is the beez knees.
inline u64 algo::rdtscp() 
```

```c++
// compatibility
void algo::reset(algo::cstring &lhs) 
```

```c++
inline i32 algo::strptr_Cmp(algo::strptr a, algo::strptr b) 
```

```c++
// Compare two strings for equality, case-sensitively
inline bool algo::strptr_Eq(algo::strptr a, algo::strptr b) 
```

```c++
lib_json::FNode *algo::strptr_FmtJson(const algo::strptr value, lib_json::FNode *parent) 
```

```c++
inline u32 algo::strptr_Hash(u32 prev, algo::strptr val) 
```

```c++
// Test if string A is lexicographically less than string B.
inline bool algo::strptr_Lt(algo::strptr a, algo::strptr b) 
```

```c++
void algo::strptr_Print(const algo::strptr &row, algo::cstring &str) 
```

```c++
void algo::strptr_PrintAligned(algo::strptr str, algo::cstring &out, int nplaces, algo::TextJust align, char c) 
```

```c++
// print base64-encoded string
void algo::strptr_PrintBase64(strptr str, cstring &out) 
```

```c++
// Print STR to OUT in a way that's acceptable as input for bash.
void algo::strptr_PrintBash(strptr str, algo::cstring &out) 
```

```c++
// Print string, replacing '_x' with 'X', and capitalizing first word.
void algo::strptr_PrintCamel(algo::strptr str, algo::cstring& out) 
```

```c++
// copy string B to TO, using ORIG as a case template
// I.e. PrintCopyCase("AbcD", to, "somestring") -> "SomEstring"
void algo::strptr_PrintCopyCase(const algo::strptr &orig, algo::cstring &to, const algo::strptr &b) 
```

```c++
// Print STR as a C++ string to OUT.
void algo::strptr_PrintCpp(algo::strptr str, algo::cstring &out) 
```

```c++
// Print STR, surrounded by quotes as C++ string
// surrounded by QUOTE_CHAR quotes, to buffer OUT.
// All string characters are escaped using char_PrintCppEsc.
void algo::strptr_PrintCppQuoted(algo::strptr str, algo::cstring &out, char quote_char) 
```

```c++
// Print CSV field, auto-determine quotes
void algo::strptr_PrintCsv(algo::strptr str, algo::cstring &out) 
```

```c++
// Append STR to OUT, using comma-separated-values encoding
// If QUOTE is 0, the need for quotes and the type of quote is determined automatically.
// If it is any other value, then that type of quote is used
// According to the CSV standard, the quote itself is the escape character.
void algo::strptr_PrintCsv(algo::strptr str, algo::cstring &out, char quote) 
```

```c++
// Print string, escaping characters in a way suitable for graphviz dot format
void algo::strptr_PrintDot(strptr s, algo::cstring &out) 
```

```c++
// Convert CamelCase to lower_under.
// A consecutive run of caps gets a single leading underscore.
// Leading cap doesn't get an underscore
// Example: "abcd testString"  -> "abcd test_string"
// Example: "Abcd" -> "abcd"
// Example: "BBOPrice" -> "bboprice"
// Example: "SymbolID" -> "symbol_id"
// Example: "A_B" -> "a_b"
void algo::strptr_PrintLowerUnder(algo::strptr str, algo::cstring &out) 
```

```c++
// Append STR to OUT, padding start with spaces.
void algo::strptr_PrintPadLeft(algo::strptr str, algo::cstring &out, int nplaces) 
```

```c++
// Append STR to OUT, padding start with character FILL
void algo::strptr_PrintPadLeft(algo::strptr str, algo::cstring &out, int nplaces, char fill) 
```

```c++
// Append STR to OUT, and pad remainder with space characters
void algo::strptr_PrintPadRight(algo::strptr str, algo::cstring &out, int nplaces) 
```

```c++
// Append STR to OUT, and pad remainder with character FILL
void algo::strptr_PrintPadRight(algo::strptr str, algo::cstring &out, int nplaces, char fill) 
```

```c++
// Print a string as a classic regex, escaping all special
// characters. This regex will only match the specified string.
void algo::strptr_PrintRegxSql(algo::strptr value, algo::cstring &str) 
```

```c++
void algo::strptr_PrintSql(algo::strptr str, algo::cstring &out) 
```

```c++
// Append STR to OUT as an SQL string using quote character Q.
// Escaped characters:
// '\' (backslash)
// ' quoted with '' if q is ' and not quoted otherwise. (Compatible with MariaDB & T-SQL.)
// " quoted with "" if q is " and not quoted otherwise. (Compatible with MariaDB & T-SQL.)
// NUL (ascii zero)
// \n
// \r
// \t
void algo::strptr_PrintSql(algo::strptr str, algo::cstring &out, char q) 
```

```c++
// Print STR as a valid SSIM key/value to OUT.
// SSIM tokens use quotes only when the printed value contains
// characters outside of the 'safe' set a-zA-Z0-9_;&*^%$@.!:,+/-
void algo::strptr_PrintSsim(algo::strptr str, algo::cstring &out) 
```

```c++
// Print string NAME as a TeX identifier to OUT, converting digits to letters
// (0 -> zero, etc) and squashing other characters not belonging to TeX
// identifier character class
void algo::strptr_PrintTexIdent(strptr name, algo::cstring &out) 
```

```c++
// print STR to OUT, escaping TeX characters that switch modes
// (_, #, &, $)
void algo::strptr_PrintTexString(strptr str, algo::cstring &out) 
```

```c++
// encode uri component.
// before ? use plus=false (no + substitution by space)
void algo::strptr_PrintUri(strptr str, algo::cstring &out, bool plus) 
```

```c++
// Assuming SRC is a number, Transfer SRC to OUT, inserting
// commas between groups of 3 numbers.
// 0 -> 0
// 11 -> 11
// 222 -> 222
// 3333 -> 3,333
// 4567.3 -> 4,567.3
// 1.11111 -> 1.11111
void algo::strptr_PrintWithCommas(strptr src, algo::cstring &out) 
```

```c++
// Print string using XML rules.
void algo::strptr_PrintXml(algo::strptr s, algo::cstring &out, bool quoted) 
```

```c++
bool algo::strptr_ReadStrptrMaybe(strptr , strptr )
```

```c++
// Escape S according to bash rules and return result
tempstr algo::strptr_ToBash(strptr str) 
```

```c++
inline algo::aryptr<u8> algo::strptr_ToMemptr(algo::aryptr<char> rhs) 
```

```c++
// Escape S according to ssim rules and return result
tempstr algo::strptr_ToSsim(strptr str) 
```

```c++
// Output string STR to OUT changing all characters to uppercase.
void algo::strptr_ToUpper(algo::strptr str, algo::cstring& out) 
```

```c++
// Assuming STR is a number, remove any unnecessary characters from the right of it.
// Unnecessary characters are trailing zeros after a dot.
// If the string contains no dot, nothing is done.
// If a trailing '.' or a single '-' remains, it is removed as well.
// If the resulting string is empty, a single zero is returned.
// 
// 0         -> 0
// 0.1       -> 0.1
// 0.0       -> 0
// 12345.000 -> 12345
// -0        -> -0
// .0        -> 0
// -.0       -> 0
// -0.0      -> -0
// -10.0     -> -10
// ->            empty string is not touched
// Since the string may be edited, you can't pass compile-time constants
// to this function. In fact the only reason this function is not private to algo_lib
// is because of unit testing.
// If the initial string is empty, nothing is done.
// BEWARE: this function will happily convert 1e+60 to 1e+6
// 
void algo::strptr_TrimZerosRight(strptr &str) 
```

```c++
// Find first occurence of MATCH in S
// If found, return range corresponding to the match.
// If not found, return range (S.n_elems,S.n_elems) -- an empty range positioned at
// the end of S
algo::i32_Range algo::substr_FindFirst(const aryptr<char> &s, const aryptr<char> &match) 
```

```c++
// Same as above but search right-to-left.
// In case of failure, return range (0,0) -- empty range positioned at start of S.
algo::i32_Range algo::substr_FindLast(const aryptr<char> &s, const aryptr<char> &match) 
```

```c++
// Allowing only for fixed length formats.
void algo::tstamp_cache_Init(TstampCache& cache, strptr spec, bool gmt)
```

```c++
void algo::tstamp_cache_Print(algo::TstampCache& cache, UnTime t, cstring& out)
```

```c++
inline u32 algo::u8_BitScanForward(u8 v) 
```

```c++
inline u32 algo::u8_BitScanReverse(u8 v) 
```

```c++
inline u32 algo::u8_Count1s(u8 x) 
```

```c++
// reverse bit order in a byte
inline u8 algo::u8_ReverseBits(u8 b) 
```

```c++
inline u32 algo::u16_BitScanForward(u16 v) 
```

```c++
inline u32 algo::u16_BitScanReverse(u16 v) 
```

```c++
inline u32 algo::u16_Count1s(u16 x) 
```

```c++
inline u32 algo::u16_FmtBuf(u32 value, u8 *buf) 
```

```c++
inline u16 algo::u16_SubClip( u16 a, u16 b) 
```

```c++
// Forward:  returns 0-based index of least significant bit that is set
// Reverse:  returns 0-based index of most  significant bit that is set.
// input argument must not be zero.
// input result in 0 is undefined (see Intel manual)
// http://www.intel.com/content/dam/doc/manual/64-ia-32-architectures-software-developer-vol-2a-2b-instruction-set-a-z-manual.pdf
inline u32 algo::u32_BitScanForward(u32 v) 
```

```c++
inline u32 algo::u32_BitScanReverse(u32 v) 
```

```c++
inline u32   algo::u32_CeilPow2(u32 a, u32 b) 
```

```c++
inline u32 algo::u32_Count1s(u32 x) 
```

```c++
// Convert VALUE to a string, encoded as a u64.
// Value must be >=0 and <100000000.
// Resulting u64 is little-endian -- MSB contains first byte
// of the resulting string. To be used as an ascii string, value
// 0x3030303030303030 (8 ascii '0's) must be added to the result.
// Don't use this function. Use u32_FmtBufSmall, and u64_FmtBuf.
u64 algo::u32_CvtLestr8Small(u32 value) 
```

```c++
// Format VALUE, which is an arbitrary u32,
// as a string and write it to buffer BUF.
// Return length of resulting string.
// Buffer must be big enough.
u32 algo::u32_FmtBuf(u32 value, u8 *buf) 
```

```c++
// Format VALUE, which should be >=0 and <100000000,
// as a string and write it to buffer BUF. Return length of resulting string.
// Buffer must be big enough.
u32 algo::u32_FmtBufSmall(u32 value, u8 *buf) 
```

```c++
inline u32    algo::u32_ModPow2(u32 a, u32 b) 
```

```c++
inline bool     algo::u32_Pow2Q(u32 i) 
```

```c++
inline u32 algo::u32_SubClip( u32 a, u32 b) 
```

```c++
inline u64 algo::u64_BitScanForward(u64 v) 
```

```c++
inline u64 algo::u64_BitScanReverse(u64 v) 
```

```c++
inline u64   algo::u64_CeilPow2(u64 a, u64 b) 
```

```c++
inline u32 algo::u64_Count1s(u64 x) 
```

```c++
// Format VALUE, which is an arbitrary u64,
// as a string and write it to buffer BUF.
// Return length of resulting string.
// Buffer must be big enough.
u32 algo::u64_FmtBuf(u64 value, u8 *buf) 
```

```c++
u32 algo::u64_FmtBufDec(u64 num, int scale, u8 *buf) 
```

```c++
// Print scaled decimal.
// VALUE           integer value
// SCALE           log10 to get the real value: real value = value / pow(10,scale)
// BUF             target buffer. buffer must be large enough.
// FIXED           if true, make sure there are exactly SCALE characters after decimal point (and always emit decimal point)
// RETURN          number of characters printed.
// Example:
// u64_FmtBufDec(0    , 2, .. false ) -> "0"
// u64_FmtBufDec(0    , 2, .. true  ) -> "0.00"
// u64_FmtBufDec(123  , 1, .. false ) -> "12.3"
// u64_FmtBufDec(123  , 2, .. false ) -> "1.23"
// u64_FmtBufDec(123  , 2, .. true  ) -> "1.23"
// u64_FmtBufDec(1230 , 3, .. false ) -> "1.23"
// u64_FmtBufDec(1230 , 3, .. true  ) -> "1.230"
u32 algo::u64_FmtBufDec(u64 num, int scale, u8 *buf, bool fixed) 
```

```c++
inline u64    algo::u64_ModPow2(u64 a, u64 b) 
```

```c++
inline bool     algo::u64_Pow2Q(u64 i) 
```

```c++
void algo::u64_PrintBase32(u64 k, algo::cstring &str) 
```

```c++
// Print value VALUE to string OUT,
// producing least ATLEAST hex characters.
// Include prefix 0x.
void algo::u64_PrintHex(u64 value, algo::cstring &out, int atleast) 
```

```c++
// Print value VALUE to string OUT,
// producing least ATLEAST hex characters.
// if PREFIX is specified, include prefix 0x.
void algo::u64_PrintHex(u64 value, algo::cstring &out, int atleast, bool prefix) 
```

```c++
void algo::u64_PrintHex(u64 value, algo::cstring &out, int atleast, bool prefix,  bool caps) 
```

```c++
u64 &algo::u64_Ranges_curs_Access(algo::u64_Ranges_curs &curs) 
```

```c++
void algo::u64_Ranges_curs_Next(algo::u64_Ranges_curs &curs) 
```

```c++
void algo::u64_Ranges_curs_Reset(algo::u64_Ranges_curs &curs, strptr str) 
```

```c++
bool algo::u64_Ranges_curs_ValidQ(algo::u64_Ranges_curs &curs) 
```

```c++
// unsigned subtract with clipping
inline u64 algo::u64_SubClip( u64 a, u64 b) 
```

```c++
inline u32 algo::u128_Count1s(u128 x) 
```

```c++
void algo::u128_PrintHex(u128 value, algo::cstring &out, bool prefix) 
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Functions -->

