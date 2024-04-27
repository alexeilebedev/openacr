## **ssim** tuples and **ssimfiles**
<a href="#--ssim---tuples-and---ssimfiles--"></a>
All information in Openacr is expressed in **"ssim"** tuples format.

This description deals with the definitions. 

Ssim manipulations are given in [Ssimfiles](/txt/recipe/ssim/README.md) and all ssimfiles are given in [Ssim Databases](/txt/ssimdb/README.md).

### **ssim** tuples
<a href="#--ssim---tuples"></a>
The **ssim** tuple format is:

```
<namespace>.<table>   <primary_key>  [<key>:<value>  ...  <key>:<value>]
```

The format consists of several tokens separated by spaces. First two tokens are mandatory, the rest is optional.

First token, called **"tuple head"**, has two parts, **namespace**.**table**, separated by dot "."

Subsequent tokens are called **fields**.

Each **field** has two parts, **key:value"**, separated by colon ":".

First **field's** **value** is always unique across all tuples with a given **"tuple head"**.

The optional rest of the **fields** also have two parts, **key:value**, separated by colon ":"

First **field** is a **primary key**, and each **field** is a DB table **column** in SQL Database sense. See [**ssim** tuples and SQL Database](/txt/fun/ssim/fun_02.md) chapter.

### Keeping tuples as **ssimfile** *.ssim files
<a href="#keeping-tuples-as---ssimfile-----ssim-files"></a>
All tuples are kept in text files with ***.ssim** extension in "data" directory. The file name is fully defined by the **"tuple head"**.
It is formed as **data/namespace/table.ssim**. Here **namespace** and **table** are coming from **"tuple head"** in the tuple. Each line in the **.ssim** file is a tuple.

### Examples of **ssim** tuples
<a href="#examples-of---ssim---tuples"></a>
```
dmmeta.ns  ns:lib_amcdb    nstype:lib       license:GPL   comment:"Library used by amc"

dev.target  target:lib_amcdb

dmmeta.nstype  nstype:lib  comment:Library
```

### Examples of **ssimfiles** files
<a href="#examples-of---ssimfiles---files"></a>
Tuples with **"tuple head"**  **dmmeta.ns** are kept in **data/dmmeta/ns.ssim** file

Tuples with **"tuple head"** **dev.target** are kept in **data/dev/target.ssim** file

Tuples with **"tuple head"** **dmmeta.nstype** are kept in **data/dmmeta/nstype.ssim** file

There are more than 200 ssimfiles that come with openacr distribution - they can be found in "data" directory.

