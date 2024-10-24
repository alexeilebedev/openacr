## fm - Fault management types


### Ctypes
<a href="#ctypes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Ctypes -->
Other ctypes in this namespace which don't have own readme files

#### fm.AlarmMsg - 
<a href="#fm-alarmmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftypes.md#base)|||
|code|[fm.Code](/txt/protocol/fm/README.md#fm-code)|[Val](/txt/exe/amc/reftypes.md#val)||Alarm code|
|objtype|[fm.Objtype](/txt/protocol/fm/README.md#fm-objtype)|[Val](/txt/exe/amc/reftypes.md#val)||Alarmed object type|
|objinst|[fm.Objinst](/txt/protocol/fm/README.md#fm-objinst)|[Val](/txt/exe/amc/reftypes.md#val)||Alarmed object instance|
|flag|[fm.Flag](/txt/protocol/fm/README.md#fm-flag)|[Val](/txt/exe/amc/reftypes.md#val)||Flag: raised or cleared|
|severity|[fm.Severity](/txt/protocol/fm/README.md#fm-severity)|[Val](/txt/exe/amc/reftypes.md#val)||Perceived severity|
|n_occurred|i32|[Val](/txt/exe/amc/reftypes.md#val)||How many times the alarm occurred since first_time|
|first_time|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Time of first occurrence|
|last_time|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Time of last occurrence|
|clear_time|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Time when the alarm has beed cleared (only for cleared alarms|
|update_time|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Time of last update|
|objtype_summary|[fm.Summary](/txt/protocol/fm/README.md#fm-summary)|[Val](/txt/exe/amc/reftypes.md#val)||Object type explained|
|summary|[fm.Summary](/txt/protocol/fm/README.md#fm-summary)|[Val](/txt/exe/amc/reftypes.md#val)||Alarm summary from inventory|
|description|[fm.Description](/txt/protocol/fm/README.md#fm-description)|[Val](/txt/exe/amc/reftypes.md#val)||Alarm message from object|
|source|[fm.Source](/txt/protocol/fm/README.md#fm-source)|[Val](/txt/exe/amc/reftypes.md#val)||Subsystem where detected|

#### fm.Code - 
<a href="#fm-code"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### fm.Description - 
<a href="#fm-description"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### fm.Flag - 
<a href="#fm-flag"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|char|[Val](/txt/exe/amc/reftypes.md#val)|'C'||

#### fm.Objinst - 
<a href="#fm-objinst"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### fm.Objtype - 
<a href="#fm-objtype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### fm.Severity - 
<a href="#fm-severity"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|char|[Val](/txt/exe/amc/reftypes.md#val)|'1'||

#### fm.Source - 
<a href="#fm-source"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### fm.Summary - 
<a href="#fm-summary"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Ctypes -->

