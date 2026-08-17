## fm - Fault management types


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
<!-- abt_md.toc_end -->

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### fm.AlarmMsg
<a href="#fm-alarmmsg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Base](/txt/exe/amc/reftype.md#base)|||
|code|[fm.Code](/txt/protocol/fm/README.md#fm-code)|[Val](/txt/exe/amc/reftype.md#val)||Alarm code|
|objtype|[fm.Objtype](/txt/protocol/fm/README.md#fm-objtype)|[Val](/txt/exe/amc/reftype.md#val)||Alarmed object type|
|objinst|[fm.Objinst](/txt/protocol/fm/README.md#fm-objinst)|[Val](/txt/exe/amc/reftype.md#val)||Alarmed object instance|
|flag|[fm.Flag](/txt/protocol/fm/README.md#fm-flag)|[Val](/txt/exe/amc/reftype.md#val)||Flag: raised or cleared|
|severity|[fm.Severity](/txt/protocol/fm/README.md#fm-severity)|[Val](/txt/exe/amc/reftype.md#val)||Perceived severity|
|n_occurred|i32|[Val](/txt/exe/amc/reftype.md#val)||How many times the alarm occurred since first_time|
|first_time|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftype.md#val)||Time of first occurrence|
|last_time|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftype.md#val)||Time of last occurrence|
|clear_time|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftype.md#val)||Time when the alarm has beed cleared (only for cleared alarms|
|update_time|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftype.md#val)||Time of last update|
|objtype_summary|[fm.Summary](/txt/protocol/fm/README.md#fm-summary)|[Val](/txt/exe/amc/reftype.md#val)||Object type explained|
|summary|[fm.Summary](/txt/protocol/fm/README.md#fm-summary)|[Val](/txt/exe/amc/reftype.md#val)||Alarm summary from inventory|
|description|[fm.Description](/txt/protocol/fm/README.md#fm-description)|[Val](/txt/exe/amc/reftype.md#val)||Alarm message from object|
|shelved|bool|[Val](/txt/exe/amc/reftype.md#val)|false|RFC 8632 shelving: published but hidden from the default operator view|
|n_flap|u32|[Val](/txt/exe/amc/reftype.md#val)||Reading transitions absorbed by the debounce window|

#### fm.Code
<a href="#fm-code"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftype.md#smallstr)|||

#### fm.Description
<a href="#fm-description"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftype.md#smallstr)|||

#### fm.Flag
<a href="#fm-flag"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|char|[Val](/txt/exe/amc/reftype.md#val)|'C'||

#### fm.Objinst
<a href="#fm-objinst"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftype.md#smallstr)|||

#### fm.Objtype
<a href="#fm-objtype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftype.md#smallstr)|||

#### fm.Severity
<a href="#fm-severity"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|char|[Val](/txt/exe/amc/reftype.md#val)|'1'||

#### fm.Summary
<a href="#fm-summary"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|[Smallstr](/txt/exe/amc/reftype.md#smallstr)|||
