## dmmeta.llist - Options for Llist field
<a href="#dmmeta-llist"></a>

Amc generates 32 flavors of linked lists, almost as many as Heinz for ketchup.
Linked lists can be:

Singly or doubly linked (as indicated by letter s or d in the prefix)
Zero-terminated or circular (as indicated by letter z or c in the prefix)
Fifo or lifo (as indicated by presence of letter l in the prefix, l for lifo)
With or without count
With or without tail pointer

Circular linked lists are often used with steps, because it is convenient to call RotateFirst
to both grab an element off the head of the list, and move this element to the back.

Here is an example:
```
dmmeta.llist  field:atf_amc.FDb.zd_typed           havetail:Y  havecount:Y  comment:""
dmmeta.llist  field:atf_amc.FDb.cd_typed          havetail:N  havecount:Y  comment:""
```

The tail and count options come from the llist record. Remaining list options are specified
by its list type. Here is the full table of list types:
```
inline-command: acr listtype | ssimfilt ^ -t
LISTTYPE  CIRCULAR  HAVEPREV  INSTAIL  COMMENT
cd        Y         Y         Y        Circular doubly-linked queue
cdl       Y         Y         N        Circular double-linked lifo (stack)
cs        Y         N         Y        Circular singly-linked queue
csl       Y         N         N        Circular singly-linked lifo (stack)
zd        N         Y         Y        Zero-terminated doubly-linked queue
zdl       N         Y         N        Zero-terminated doubly-linked lifo (stack)
zs        N         N         Y        Zero-terminated singly-linked queue
zsl       N         N         N        Zero-terminated singly-linked lifo (stack)

```

The following functions are generated for a circular linked list:
```
inline-command: amc atf_amc.FDb.cd_typed.% -proto -report:N
// Return true if index is empty
// func:atf_amc.FDb.cd_typed.EmptyQ
bool                 cd_typed_EmptyQ() __attribute__((__warn_unused_result__, nothrow, pure));
// If index empty, return NULL. Otherwise return pointer to first element in index
// func:atf_amc.FDb.cd_typed.First
atf_amc::FTypeD*     cd_typed_First() __attribute__((__warn_unused_result__, nothrow, pure));
// Return true if row is in the linked list, false otherwise
// func:atf_amc.FDb.cd_typed.InLlistQ
bool                 cd_typed_InLlistQ(atf_amc::FTypeD& row) __attribute__((__warn_unused_result__, nothrow));
// Insert row into linked list. If row is already in linked list, do nothing.
// func:atf_amc.FDb.cd_typed.Insert
void                 cd_typed_Insert(atf_amc::FTypeD& row) __attribute__((nothrow));
// If index empty, return NULL. Otherwise return pointer to last element in index
// func:atf_amc.FDb.cd_typed.Last
atf_amc::FTypeD*     cd_typed_Last() __attribute__((__warn_unused_result__, nothrow, pure));
// Return number of items in the linked list
// func:atf_amc.FDb.cd_typed.N
i32                  cd_typed_N() __attribute__((__warn_unused_result__, nothrow, pure));
// Return pointer to next element in the list
// func:atf_amc.FDb.cd_typed.Next
atf_amc::FTypeD*     cd_typed_Next(atf_amc::FTypeD &row) __attribute__((__warn_unused_result__, nothrow));
// Return pointer to previous element in the list
// func:atf_amc.FDb.cd_typed.Prev
atf_amc::FTypeD*     cd_typed_Prev(atf_amc::FTypeD &row) __attribute__((__warn_unused_result__, nothrow));
// Remove element from index. If element is not in index, do nothing.
// func:atf_amc.FDb.cd_typed.Remove
void                 cd_typed_Remove(atf_amc::FTypeD& row) __attribute__((nothrow));
// Empty the index. (The rows are not deleted)
// func:atf_amc.FDb.cd_typed.RemoveAll
void                 cd_typed_RemoveAll() __attribute__((nothrow));
// If linked list is empty, return NULL. Otherwise unlink and return pointer to first element.
// func:atf_amc.FDb.cd_typed.RemoveFirst
atf_amc::FTypeD*     cd_typed_RemoveFirst() __attribute__((nothrow));
// If linked list is empty, return NULL.
// Otherwise return head item and advance head to the next item.
// func:atf_amc.FDb.cd_typed.RotateFirst
atf_amc::FTypeD*     cd_typed_RotateFirst() __attribute__((nothrow));
// Return reference to last element in the index. No bounds checking.
// func:atf_amc.FDb.cd_typed.qLast
atf_amc::FTypeD&     cd_typed_qLast() __attribute__((__warn_unused_result__, nothrow));

```

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Llist

* file:[data/dmmeta/llist.ssim](/data/dmmeta/llist.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||Index in question|
|havetail|bool|Val|true|Generate 'tail' field|
|havecount|bool|Val|false|Generate count of elements|
|listtype|[dmmeta.Listtype](/txt/ssimdb/dmmeta/listtype.md)|Pkey||<br>.RR_LL of field|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FLlist

