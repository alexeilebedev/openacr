## **ssim** tuples and SQL Database
<a href="#--ssim---tuples-and-sql-database"></a>
There is, by design, a full equivalence between ssim tuples and SQL DB columns and records. Each tuple represents one record in a table in a database. Each **"tuple head"**, presented as ```<namespace>.<table>``` defines database (via it's namespace) and table (via it's table) names.

### **ssimfile** as SQL table and **field** as a table column
<a href="#--ssimfile---as-sql-table-and---field---as-a-table-column"></a>
Each tuple **field** defines table column name (primary_key or key) and a record column value (value).
Each **ssimfile** can be understood as an SQL table, with this table first column is a **primary key**.

### Guarantees of ssimfile and SQL Database equivalence
<a href="#guarantees-of-ssimfile-and-sql-database-equivalence"></a>
Openacr verification process loads all **ssimfiles** to a temporary MariaDB instance and writes them back, guaranteeing full equivalence.

### Examples of SQL Database equivalence
<a href="#examples-of-sql-database-equivalence"></a>
A CLI tool [acr_my](/txt/exe/acr_my/README.md) allows **ssimfiles** to be loaded into MySQL, manipulated via SQL means and written back. The examples below show the ssim tuples equivalence to SQL records.

ssim tuple

```
...
dmmeta.ns  ns:lib_amcdb    nstype:lib       license:GPL   comment:"Library used by amc"
...
```

SQL record

```

MariaDB [dmmeta]> select * from ns where ns like "lib_amcdb%";
+-----------+--------+---------+---------------------+
| ns        | nstype | license | comment             |
+-----------+--------+---------+---------------------+
| lib_amcdb | lib    | GPL     | Library used by amc |
+-----------+--------+---------+---------------------+
1 row in set (0.001 sec)

MariaDB [dmmeta]>

```

ssim tuple

```
dmmeta.nstype  nstype:exe       comment:Executable
dmmeta.nstype  nstype:lib       comment:Library
dmmeta.nstype  nstype:none      comment:"Skip building"
dmmeta.nstype  nstype:objlist   comment:"Object files are added to dependents' link line"
dmmeta.nstype  nstype:protocol  comment:"Protocol (not a target)"
dmmeta.nstype  nstype:ssimdb    comment:"Ssim database (not a target)"
```

SQL record

```
MariaDB [dmmeta]> select * from nstype;
+----------+-------------------------------------------------+
| nstype   | comment                                         |
+----------+-------------------------------------------------+
| exe      | Executable                                      |
| lib      | Library                                         |
| none     | Skip building                                   |
| objlist  | Object files are added to dependents' link line |
| protocol | Protocol (not a target)                         |
| ssimdb   | Ssim database (not a target)                    |
+----------+-------------------------------------------------+
```

ssim tuple

```
dev.target  target:lib_amcdb
```

SQL record

```
MariaDB [dev]> select * from target where target like "lib_amcdb";
+-----------+----------------------------+
| target    | extra_column_for_roundtrip |
+-----------+----------------------------+
| lib_amcdb |                            |
+-----------+----------------------------+
```

