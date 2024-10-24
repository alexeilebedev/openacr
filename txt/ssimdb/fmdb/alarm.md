## fmdb.alarm -


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)fmdb.Alarm

* file:[data/fmdb/alarm.ssim](/data/fmdb/alarm.ssim)

italicised fields: *code, object, objtype, objinst, objprefix* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|alarm|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftypes.md#val)||Alarm identity: code@object|
|*code*|*[fm.Code](/txt/protocol/fm/README.md#fm-code)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Alarm code<br>@LL of alarm*|
|*object*|*[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Alarmed object: objtype.objinst<br>@LR of alarm*|
|*objtype*|*[fm.Objtype](/txt/protocol/fm/README.md#fm-objtype)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Alarmed object type<br>.LL of object*|
|*objinst*|*[fm.Objinst](/txt/protocol/fm/README.md#fm-objinst)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Alarmed object instance<br>.LR of object*|
|*objprefix*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Prefix of alarmed object instance (service name)<br>.LL of objinst*|
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
|source|[fm.Source](/txt/protocol/fm/README.md#fm-source)|[Val](/txt/exe/amc/reftypes.md#val)||Subsystem where alarm has been detected|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [lib_fm](/txt/lib/lib_fm/README.md) as [lib_fm.FAlarm](/txt/lib/lib_fm/README.md#lib_fm-falarm)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

