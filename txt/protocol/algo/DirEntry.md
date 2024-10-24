## algo.DirEntry - Directory entry


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)algo.DirEntry

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|dirname|algo.cstring|Val||Base directory name, e.g. /tmp|
|pattern|algo.cstring|Val||Shell wildcard to match, e.g. aa*bb|
|pathname|algo.cstring|Val||Current pathname, e.g. /tmp/aaCbb|
|filename|algo.cstring|Val||Current filename, e.g. aaCbb|
|mode|u64|Val|||
|size|u64|Val||Size of file in bytes|
|mtime|algo.UnTime|Val||Modification time of file|
|ctime|algo.UnTime|Val||Change time of file|
|eof|bool|Val||(internal) EOF flag -- no more dir entries left|
|match|bool|Val||(internal) File entry matches pattern|
|is_dir|bool|Val||File is a directory|
|dir_handle|DIR|Ptr||(internal) handle for scanning|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

