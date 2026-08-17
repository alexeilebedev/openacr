## amcdb.tcond - Condition a tclass reports, which an fcond binds to an index
<a href="#amcdb-tcond"></a>
A tclass generates functions, and those functions know things no field
assignment records: a socket became readable, an output buffer drained past
its low-water mark, a peer hung up.  Each such moment is a condition, and a
record waiting on it is registered by membership in an index.

`amcdb.tcond` is the vocabulary of conditions one tclass can report, one row
per condition, keyed `<tclass>.<name>`.  It is the condition analogue of
[amcdb.tfunc](/txt/ssimdb/amcdb/tfunc.md), which declares the functions a
tclass generates.

A row here only names the condition; it binds nothing.  A
[dmmeta.fcond](/txt/ssimdb/dmmeta/fcond.md) record on a field of that
reftype is what says which index the record enters while the condition
holds.  A condition with no fcond record is not reported at all.

```
amcdb.tcond   tcond:Fbuf.space  comment:"a congested out buffer drained past its low-water mark"
dmmeta.fcond  fcond:<ns>.FConn.out/space  ins:<ns>.FDb.cd_session_space  via:<ns>.FConn.p_session  rem:N  comment:""
```

The full mechanism, and the division of responsibility between the tclass
and the fcond record, is described in [amc fcond](/txt/exe/amc/fcond.md).

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)amcdb.Tcond

* file:[data/amcdb/tcond.ssim](/data/amcdb/tcond.ssim)

italicised fields: *tclass, name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tcond|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|*tclass*|*[amcdb.Tclass](/txt/ssimdb/amcdb/tclass.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.RL of tcond*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>.RR of tcond*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FTcond](/txt/gen/amc/amc.md#amc-ftcond)
