acr -merge -write <<EOF
dev.license  license:GPL  comment:""
dmmeta.nstype  nstype:exe  comment:Executable
  dmmeta.ns  ns:sample  nstype:exe  license:GPL  comment:""
    dev.target  target:sample
      dev.targdep  targdep:sample.algo_lib  comment:""
      dev.targdep  targdep:sample.lib_prot  comment:""
      dev.targsrc  targsrc:sample/cpp/gen/sample_gen.cpp        comment:""
      dev.targsrc  targsrc:sample/cpp/sample/sample.cpp         comment:""
      dev.targsrc  targsrc:sample/include/gen/sample_gen.h      comment:""
      dev.targsrc  targsrc:sample/include/gen/sample_gen.inl.h  comment:""
      dev.targsrc  targsrc:sample/include/sample.h              comment:""

    dmmeta.ctype  ctype:sample.FDb  comment:""
      dmmeta.field  field:sample.FDb._db      arg:sample.FDb      reftype:Global  dflt:""  comment:""
      dmmeta.field  field:sample.FDb.cmdline  arg:command.sample  reftype:Val     dflt:""  comment:""
        dmmeta.fcmdline  field:sample.FDb.cmdline  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:""

      dmmeta.field  field:sample.FDb.rec      arg:sample.FRec  reftype:Tpool  dflt:""  comment:""
      dmmeta.field  field:sample.FDb.ind_rec  arg:sample.FRec  reftype:Thash  dflt:""  comment:""
        dmmeta.thash  field:sample.FDb.ind_rec  hashfld:sample.FRec.rec  unique:Y  comment:""
        dmmeta.xref  field:sample.FDb.ind_rec  inscond:true  via:""

      dmmeta.field  field:sample.FDb.bh_rec  arg:sample.FRec  reftype:Bheap  dflt:""  comment:""
        dmmeta.sortfld  field:sample.FDb.bh_rec  sortfld:sample.FRec.rec
        dmmeta.fstep  fstep:sample.FDb.bh_rec  steptype:InlineRecur  comment:""
          dmmeta.fdelay  fstep:sample.FDb.bh_rec  delay:1.0  scale:N  comment:""
        dmmeta.xref  field:sample.FDb.bh_rec  inscond:true  via:""

      dmmeta.ctypelen  ctype:sample.FDb  len:88  alignment:8  padbytes:7  plaindata:N

    dmmeta.ctype  ctype:sample.FRec  comment:""
      dmmeta.field  field:sample.FRec.rec  arg:sample.Reckey  reftype:Val  dflt:""  comment:""
      dmmeta.ctypelen  ctype:sample.FRec  len:64  alignment:8  padbytes:4  plaindata:N

    dmmeta.ctype  ctype:sample.Reckey  comment:""
      dmmeta.ccmp  ctype:sample.Reckey  extrn:N  genop:Y  order:Y  minmax:N  comment:""
      dmmeta.chash  ctype:sample.Reckey  hashtype:CRC32  comment:""
      dmmeta.cpptype  ctype:sample.Reckey  ctor:Y  dtor:Y  cheap_copy:N
      dmmeta.field  field:sample.Reckey.key1  arg:i32              reftype:Val  dflt:""  comment:""
      dmmeta.field  field:sample.Reckey.key2  arg:double           reftype:Val  dflt:""  comment:""
      dmmeta.field  field:sample.Reckey.key3  arg:algo.Smallstr20  reftype:Val  dflt:""  comment:""
      dmmeta.cfmt  cfmt:sample.Reckey.String  printfmt:Sep  read:Y  print:Y  sep://  genop:Y  comment:""
      dmmeta.ctypelen  ctype:sample.Reckey  len:40  alignment:8  padbytes:6  plaindata:Y

    dmmeta.main  ns:sample  ismodule:N
    dmmeta.nscpp  ns:sample  comment:""
    dmmeta.nsx  ns:sample  genthrow:Y  correct_getorcreate:Y  pool:algo_lib.FDb.malloc  sortxref:Y  pack:N  comment:""
    dmmeta.tracerec  tracerec:sample.trace  comment:""
report.acr  n_select:41  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
EOF
