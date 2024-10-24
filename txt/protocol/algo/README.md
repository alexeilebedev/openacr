## algo - Basic types and functions


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
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

#### algo.U16Dec2 - u16, scale 1e2
<a href="#algo-u16dec2"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u16|[Val](/txt/exe/amc/reftypes.md#val)|||

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

