## bin/amc-update-schema - Migrate ssim data from the pre-ffunc schema to dmmeta.ffunc
<a href="#bin-amc-update-schema"></a>

Rewrite ssim data from the pre-`ffunc` schema to `dmmeta.ffunc`.

The commit that introduced `dmmeta.ffunc` removed the tables and columns amc
used to consume for per-field functions, and removed amc's ability to upgrade
the schema on the fly.  If you carry ssim records in the old form (a branch, a
downstream repo, or pending changes), run this script once after merging that
commit to rewrite them.

### Usage
<a href="#usage"></a>

```bash
amc-update-schema [DATADIR]   # DATADIR defaults to ./data
amc && acr -check % -x        # regenerate + canonicalize afterwards
```

The script is idempotent: a table already migrated (file absent, column gone)
is skipped.

### Rewrites
<a href="#rewrites"></a>

All per-field functions move into `dmmeta.ffunc`, whose pkey is `<field>.<name>`
(`<name>` is a tfunc; `extrn:Y` means the user supplies the body):

| Old                                  | New |
| ------------------------------------ | --- |
| `dmmeta.fcb fcb:<field>/<cbtype>`    | `ffunc:<field>.<cbtype> extrn:Y` |
| `dmmeta.findrem field:<field>`       | `ffunc:<field>.FindRemove extrn:N` |
| `dmmeta.fcleanup field:<field>`      | `ffunc:<field>.Cleanup extrn:Y` |
| `dmmeta.fuserinit field:<field>`     | `ffunc:<field>.Userinit extrn:Y` |
| `dmmeta.fbuf fbuftype:BytebufExtern` | `fbuftype:Bytebuf` + `ffunc:<field>.ScanMsg extrn:Y` |
| `dmmeta.finput ... extrn:Y`          | `ffunc:<field>.{Input,InputMaybe} extrn:Y` (the `extrn` column is dropped from `finput`) |
| `amcdb.tcb` (whole table)            | removed |

For `finput`, whether the user supplies `Input` (exception namespaces) or
`InputMaybe` is read from the existing `dmmeta.userfunc` row keyed
`acrkey:finput:<field>`, falling back to `InputMaybe`.
