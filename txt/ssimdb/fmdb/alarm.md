## fmdb.alarm -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)fmdb.Alarm

* file:[data/fmdb/alarm.ssim](/data/fmdb/alarm.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|alarm|algo.Smallstr200|Val||Alarm identity: code@object|
|code|fm.Code|Val||Alarm code<br>@LL of alarm|
|object|algo.Smallstr200|Val||Alarmed object: objtype.objinst<br>@LR of alarm|
|objtype|fm.Objtype|Val||Alarmed object type<br>.LL of object|
|objinst|fm.Objinst|Val||Alarmed object instance<br>.LR of object|
|objprefix|algo.Smallstr50|Val||Prefix of alarmed object instance (service name)<br>.LL of objinst|
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
|source|fm.Source|Val||Subsystem where alarm has been detected|

### Used In Executables
<a href="#used-in-executables"></a>
* [lib_fm](/txt/lib/lib_fm/README.md) as lib_fm.FAlarm

