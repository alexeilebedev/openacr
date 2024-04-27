## **fldfunc fields** in **ssim** tuples
<a href="#--fldfunc-fields---in---ssim---tuples"></a>
This description deals with the definitions. 

Ssim manipulations are given in [Ssimfiles](/txt/recipe/ssim/README.md) and all ssimfiles are given in [Ssim Databases](/txt/ssimdb/README.md).

### **fldfunc-fields** in **ssim tuples**
<a href="#--fldfunc-fields---in---ssim-tuples--"></a>
Each **field** in a **ssimfile** tuple is a **key:value** pair. The **ssimfile** definitions, described in [How to define a new ssimfile](/txt/recipe/ssim/recipe_01.md) allow a **field** to be partitioned into additional **fldfunc** **fields** of **key:value** pair format. These additional **fields** provide **keys** (names) for the substrings of the **value** of the original **key:value** pair.

The **value** string is subdivided into substrings, with the explicit separator character, defined and visible in the string. The substrings provide new **values**, with the new **keys** coming from **ssimfile** definitions. The definitions creation is described here:[How to define a new ssimfile](/txt/recipe/ssim/recipe_01.md).

### Substring extraction mechanism
<a href="#substring-extraction-mechanism"></a>
The substring extraction mechanism is defined by the expression

```
<separator character><direction_1direction_2>[<separator character><direction_1direction_2>...]
```

direction1 and direction2 are either L(eft) or R(ight), so an example of the expression would be

```
new_field expr:.LL/RL
```

which opens up as this algorithm, always executed left to right of the expr: "going from the L(eft), take the L(eft) reminder of the string after the first dot "." is found. Next, with the resulting string, go from the R(igt) till first slash "/" is found, and take the L(eft) reminder.

So if ``field:c1/c2c3.c4c5`` is the expression, first part of the algorithm, ``.LL``, will result in ``c1/c2c3`` substring, and second part, ``/RL``, will deliver ``c1``. So the final **fldunc**``new_field`` will be ``new_field:c1``

### Examples of **fldfunc fields**
<a href="#examples-of---fldfunc-fields--"></a>
A CLI tool [acr](/txt/exe/acr/README.md) shows ssimfiles with and without **fldfunc fields**.

A tuple kept in the **ssimfile** would be this:

```
...
dmmeta.ctype  ctype:dev.Cfg  comment:"Compiler configuration"
...
```

an expanded tuple with **fldfunc** fields would be this

```
...
dmmeta.ctype  ctype:dev.Cfg  ns:dev  name:Cfg  comment:"Compiler configuration"
...
```

here the value of``ctype:dev.Cfg`` is expanded into two additional **fldfunc fields** ``ns:dev and name:Cfg``

the **value** separator in this case is dot "." and the``ns`` and ``name`` **keys** came from **ssimfile** description, not present in the tuple.

The description that defines the substrings is this:

```
name  expr:.RR
ns    expr:.RL
```

A more involved example would be this tuple:

```
dev.tool_opt  tool_opt:"Linux-g++.%-%/mysql2ssim-LINK:-L /usr/lib64/mysql"  comment:""
```

an expanded tuple with **fldfunc** fields would be this

```
dev.tool_opt  tool_opt:"Linux-g++.%-%/mysql2ssim-LINK:-L /usr/lib64/mysql"  uname:Linux  compiler:g++  cfg:%  arch:%  target:mysql2ssim  opt_type:LINK  opt:"-L /usr/lib64/mysql"  sortfld:Linux-g++  comment:""
```

here the value of
``tool_opt:"Linux-g++.%-%/mysql2ssim-LINK:-L /usr/lib64/mysql"`` is expanded into 8 additional fields **fldfunc fields**:``arch, cfg, compiler, opt, opt_type, sortfld, target, uname``

The description that defines the substrings is this:

```
arch      expr:/LL.LR-LR
cfg       expr:/LL.LR-LL
compiler  expr:/LL.LL-LR
opt       expr:/LR:LR   
opt_type  expr:/LR:LL-LR
sortfld   expr:.LL      
target    expr:/LR:LL-LL
uname     expr:/LL.LL-LL
```

We can see that "/", ".", ":", and "-" all used as the separators in this case, and the expression mechanisms are quite involved.

