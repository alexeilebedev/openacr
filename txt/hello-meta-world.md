## Hello Meta World

Follow the steps below to create a new sample program.

The program will print the names of all of its own structures, and 
their fields, cross-referenced twice: first, by membership and 
then by computing all back-references.

This seems like an appropriately self-referential way to say hello
using the tools at our disposal. Having a program print its own data 
structure is also mind-boggling if you think about it for too long.

Use this as a starting point, or to get motivated to read one of the tutorials.

~~~
acr_ed -create -target hi -write
cat > cpp/samp/hi.cpp << EOF
#include "include/hi.h"
void hi::Main() {
    prlog("Hello Meta World!");
    ind_beg(hi::_db_ctype_curs,ctype,hi::_db) {
        if (ns_Get(ctype) == dmmeta_Ns_ns_hi) {
            prlog("ctype "<<ctype.ctype);
            ind_beg(hi::ctype_zd_field_curs,field,ctype) {
                prlog("    has field "<<field.field<<" of type "<<field.arg<<" reftype:"<<field.reftype);
            }ind_end;
            ind_beg(hi::ctype_zd_arg_curs,arg,ctype) {
                prlog("    is referred to by field "<<arg.field<<" using "<<arg.reftype);
            }ind_end;
        }
    }ind_end;
}
EOF
acr_ed -create -finput -target hi -ssimfile:dmmeta.ctype -indexed -write
acr_ed -create -finput -target hi -ssimfile:dmmeta.field -write
acr_ed -create -field:hi.FField.p_ctype  -arg:hi.FCtype -xref -via:hi.FDb.ind_ctype/dmmeta.Field.ctype -write
acr_ed -create -field:hi.FCtype.zd_field -arg:hi.FField -xref -via:hi.FDb.ind_ctype/dmmeta.Field.ctype  -write
acr_ed -create -field:hi.FCtype.zd_arg   -arg:hi.FField -xref -via:hi.FDb.ind_ctype/dmmeta.Field.arg    -write
abt -install hi
hi
~~~
