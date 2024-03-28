## fm - Fault management types


### Table Of Contents
<a href="#table-of-contents"></a>
* [Ctypes](#ctypes)

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### fm.AlarmMsg - 
<a href="#fm-alarmmsg"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|ams.MsgHeader|Base|
|code|fm.Code|Val||Alarm code|
|objtype|fm.Objtype|Val||Alarmed object type|
|objinst|fm.Objinst|Val||Alarmed object instance|
|flag|fm.Flag|Val||Flag: raised or cleared|
|severity|fm.Severity|Val||Perceived severity|
|n_occurred|i32|Val||How many times the alarm occurred since first_time|
|first_time|algo.UnTime|Val||Time of first occurrence|
|last_time|algo.UnTime|Val||Time of last occurrence|
|clear_time|algo.UnTime|Val||Time when the alarm has beed cleared (only for cleared alarms|
|update_time|algo.UnTime|Val||Time of last update|
|objtype_summary|fm.Summary|Val||Object type explained|
|summary|fm.Summary|Val||Alarm summary from inventory|
|description|fm.Description|Val||Alarm message from object|
|source|fm.Source|Val||Subsystem where detected|

#### fm.Code - 
<a href="#fm-code"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|Smallstr|

#### fm.Description - 
<a href="#fm-description"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|Smallstr|

#### fm.Flag - 
<a href="#fm-flag"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|char|Val|'C'|

#### fm.Objinst - 
<a href="#fm-objinst"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|Smallstr|

#### fm.Objtype - 
<a href="#fm-objtype"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|Smallstr|

#### fm.Severity - 
<a href="#fm-severity"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|char|Val|'1'|

#### fm.Source - 
<a href="#fm-source"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|Smallstr|

#### fm.Summary - 
<a href="#fm-summary"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ch|char|Smallstr|

