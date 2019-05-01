## Tutorial: Hello Meta World

Follow the steps below to create a new sample program.
The program will load ctype and field tables, and cross-reference
them in two ways.
It will list all data structures in all executables, and compute all
back-references, i.e. for each struct (ctype), it will show all the
places where the struct is referenced.

Use this as a starting point for creating your own data model analysis tool.

~~~
acr_ed -create -target samp_hi -write
cat > cpp/samp/samp_hi.cpp << EOF
#include "include/algo.h"
#include "include/gen/samp_hi_gen.h"
#include "include/gen/samp_hi_gen.inl.h"
// Load ctype and field, print all fields and use sites
void samp_hi::Main() {
    prlog("Hello Meta World!");
    ind_beg(samp_hi::_db_ctype_curs,ctype,samp_hi::_db) {
        prlog("ctype "<<ctype.ctype);
        ind_beg(samp_hi::ctype_zd_field_curs,field,ctype) {
            prlog("    has field "<<field.field<<" of type "<<field.arg<<" reftype:"<<field.reftype);
        }ind_end;
        ind_beg(samp_hi::ctype_zd_arg_curs,arg,ctype) {
            prlog("    is referred to by field "<<arg.field<<" using "<<field.reftype);
        }ind_end;
    }ind_end;
}
EOF
acr_ed -create -finput -target samp_hi -ssimfile dmmeta.ctype -indexed -write
acr_ed -create -finput -target samp_hi -ssimfile dmmeta.field -write
acr_ed -create -field samp_hi.FField.p_ctype  -write
acr_ed -create -field samp_hi.FCtype.zd_field -arg samp_hi.FField -xref -via:samp_hi.FDb.ind_ctype/dmmeta.Field.ctype  -write
acr_ed -create -field samp_hi.FCtype.zd_arg   -arg samp_hi.FField -xref -via:samp_hi.FDb.ind_ctype/dmmeta.Field.arg    -write
amc
abt -install samp_hi
acr ns:samp_hi -t
samp_hi
~~~

