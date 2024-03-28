## dev.compiler - One of the known compilers


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Compiler

* file:[data/dev/compiler.ssim](/data/dev/compiler.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|compiler|algo.Smallstr50|Val|
|ranlib|algo.Smallstr50|Val|
|ar|algo.Smallstr50|Val|
|link|algo.Smallstr50|Val|
|libext|algo.Smallstr50|Val|
|exeext|algo.Smallstr20|Val|
|pchext|algo.Smallstr20|Val|
|objext|algo.Smallstr20|Val|
|rc|algo.Smallstr50|Val|
|comment|algo.Comment|Val|

### Related
<a href="#related"></a>
These ssimfiles reference dev.compiler

* [dev.builddir via compiler](/txt/ssimdb/dev/builddir.md) - Compiler name (e.g. g++,clang)

### Used In Commands
<a href="#used-in-commands"></a>
* [abt -compiler](/txt/exe/abt/README.md) - Set compiler.

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/exe/abt/README.md) as abt.FCompiler

