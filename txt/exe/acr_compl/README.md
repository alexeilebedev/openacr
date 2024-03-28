## acr_compl - ACR shell auto-complete for all targets


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)
* [Tests](#tests)

### Syntax
<a href="#syntax"></a>
```
acr_compl: ACR shell auto-complete for all targets
Usage: acr_compl [options]
    OPTION      TYPE    DFLT    COMMENT
    -data       string  "data"  Source for completions (dir or file or -)
    -schema     string  "data"  Source for schema information
    -line       string  ""      Simulates COMP_LINE (debug)
    -point      string  ""      Simulates COMP_POINT (debug). default: whole line
    -type       string  "9"     Simulates COMP_TYPE (debug)
    -install                    Produce bash commands to install the handler
    -debug_log  string  ""      Log file for debug information, overrides ACR_COMPL_DEBUG_LOG
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Options
<a href="#options"></a>

#### -data -- Source for completions (dir or file or -)
<a href="#-data"></a>

#### -schema -- Source for schema information
<a href="#-schema"></a>

#### -line -- Simulates COMP_LINE (debug)
<a href="#-line"></a>

#### -point -- Simulates COMP_POINT (debug). default: whole line
<a href="#-point"></a>

#### -type -- Simulates COMP_TYPE (debug)
<a href="#-type"></a>

#### -install -- Produce bash commands to install the handler
<a href="#-install"></a>

#### -debug_log -- Log file for debug information, overrides ACR_COMPL_DEBUG_LOG
<a href="#-debug_log"></a>

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/acr_compl/main.cpp](/cpp/acr_compl/main.cpp)||
|[cpp/acr_compl/shell.cpp](/cpp/acr_compl/shell.cpp)||
|[cpp/gen/acr_compl_gen.cpp](/cpp/gen/acr_compl_gen.cpp)||
|[include/acr_compl.h](/include/acr_compl.h)||
|[include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)||
|[include/gen/acr_compl_gen.inl.h](/include/gen/acr_compl_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`acr_compl` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.fflag](/txt/ssimdb/dmmeta/fflag.md)|Options for command-line flags|
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.fcmdline](/txt/ssimdb/dmmeta/fcmdline.md)|Annotate field that holds process command line|
|[dmmeta.falias](/txt/ssimdb/dmmeta/falias.md)|Alias field|
|[dmmeta.argvtype](/txt/ssimdb/dmmeta/argvtype.md)|Customize parsing of command lines (rarely used)|
|[dmmeta.anonfld](/txt/ssimdb/dmmeta/anonfld.md)|Omit field name where possible (command line, enums, constants)|

### Tests
<a href="#tests"></a>
The following component tests are defined for `acr_compl`.
These can be executed with `atf_comp <comptest> -v`
|COMPTEST|COMMENT|
|---|---|
|acr_compl.A01a||
|acr_compl.A01b||
|acr_compl.A01c||
|acr_compl.A02a||
|acr_compl.A02b||
|acr_compl.A02c||
|acr_compl.A03a||
|acr_compl.A03b||
|acr_compl.A03c||
|acr_compl.A04a||
|acr_compl.A04b||
|acr_compl.A04c||
|acr_compl.A05a||
|acr_compl.A05b||
|acr_compl.A05c||
|acr_compl.A06a||
|acr_compl.A06b||
|acr_compl.A06c||
|acr_compl.A07a||
|acr_compl.A07b||
|acr_compl.A07c||
|acr_compl.Acr01||
|acr_compl.Acr02||
|acr_compl.Acr03||
|acr_compl.Acr04||
|acr_compl.Acr05||
|acr_compl.Acr06||
|acr_compl.Acr07||
|acr_compl.BadExe||
|acr_compl.BadOpt||
|acr_compl.BadOptColon||
|acr_compl.BadOptColonSpace||
|acr_compl.BadOptSpace||
|acr_compl.Bare||
|acr_compl.CustomData||
|acr_compl.DblColon||
|acr_compl.DblColonList||
|acr_compl.DblSpace||
|acr_compl.DblSpaceList||
|acr_compl.EnumCtypeColon||
|acr_compl.EnumCtypeColonList||
|acr_compl.EnumCtypeSpace||
|acr_compl.EnumCtypeSpaceList||
|acr_compl.EnumFieldColon||
|acr_compl.EnumFieldColonList||
|acr_compl.EnumFieldSpace||
|acr_compl.EnumFieldSpaceList||
|acr_compl.FlagColon||
|acr_compl.FlagColonList||
|acr_compl.FlagSpace||
|acr_compl.FlagSpaceList||
|acr_compl.Install||
|acr_compl.NumColon||
|acr_compl.NumColonList||
|acr_compl.NumSpace||
|acr_compl.NumSpaceList||
|acr_compl.OptCumul||
|acr_compl.OptCumulAlias||
|acr_compl.OptD||
|acr_compl.OptDList||
|acr_compl.OptH||
|acr_compl.OptHList||
|acr_compl.OptNonCumul||
|acr_compl.OptNonCumulAlias||
|acr_compl.OptPkeyColon||
|acr_compl.OptPkeyColonFull||
|acr_compl.OptPkeyColonFullList||
|acr_compl.OptPkeyColonList||
|acr_compl.OptPkeyColonPrefix||
|acr_compl.OptPkeyColonPrefixList||
|acr_compl.OptPkeyColonSubstr||
|acr_compl.OptPkeyColonSubstrList||
|acr_compl.OptPkeySpace||
|acr_compl.OptPkeySpaceFull||
|acr_compl.OptPkeySpaceFullList||
|acr_compl.OptPkeySpaceList||
|acr_compl.OptPkeySpacePrefix||
|acr_compl.OptPkeySpacePrefixList||
|acr_compl.OptPkeySpaceSubstr||
|acr_compl.OptPkeySpaceSubstrList||
|acr_compl.OptRegxColon||
|acr_compl.OptRegxColonFull||
|acr_compl.OptRegxColonFullList||
|acr_compl.OptRegxColonList||
|acr_compl.OptRegxColonPrefix||
|acr_compl.OptRegxColonPrefixList||
|acr_compl.OptRegxColonSubstr||
|acr_compl.OptRegxColonSubstrList||
|acr_compl.OptRegxSpace||
|acr_compl.OptRegxSpaceFull||
|acr_compl.OptRegxSpaceFullList||
|acr_compl.OptRegxSpaceList||
|acr_compl.OptRegxSpacePrefix||
|acr_compl.OptRegxSpacePrefixList||
|acr_compl.OptRegxSpaceSubstr||
|acr_compl.OptRegxSpaceSubstrList||
|acr_compl.OptSig||
|acr_compl.OptSigList||
|acr_compl.OptV||
|acr_compl.OptVList||
|acr_compl.R01a||
|acr_compl.R01b||
|acr_compl.R01c||
|acr_compl.R01d||
|acr_compl.R02a||
|acr_compl.R02b||
|acr_compl.R02c||
|acr_compl.R02d||
|acr_compl.StrColon||
|acr_compl.StrColonList||
|acr_compl.StrSpace||
|acr_compl.StrSpaceList||
|acr_compl.T01||
|acr_compl.T02||
|acr_compl.T03||
|acr_compl.T04||
|acr_compl.T05||
|acr_compl.T06||
|acr_compl.T07||
|acr_compl.T08||
|acr_compl.T09||
|acr_compl.T10||

