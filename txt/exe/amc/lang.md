## amc Feature: foreign-language projection
<a href="#amc-feature-foreign-language-projection"></a>

`amc` writes the schema's ctypes in Go, Python, Rust and TypeScript as well as
C++.  A ctype written in another language is a projected ctype, and the set of
them is the projection.  Two tables choose what goes in it, and a third names
the dispatches a language speaks.

```ssim
dmmeta.nslang  nslang:fm/ts  comment:""
dmmeta.nslang  nslang:report/ts  comment:""
dmmeta.ctypelang  ctypelang:ams.Shmhdr/rs  comment:""
dmmeta.displang  displang:<dispatch>/py  comment:""
```

### Ssim inputs
<a href="#ssim-inputs"></a>

| Record | Role |
|---|---|
| `dmmeta.nslang  nslang:<ns>/<lang>` | Project every ctype of the namespace into the language. |
| `dmmeta.ctypelang  ctypelang:<ctype>/<lang>` | Project one ctype, for a namespace only part of which is wanted. |
| `dmmeta.displang  displang:<dispatch>/<lang>` | Write the dispatch's signature constant into the language, which a client sends to claim the schema it speaks.  In Rust the namespace holding the dispatch roots the crate.  Every message of the dispatch must already be projected. |
| `amcdb.lang  lang:<lang>  linecomment:<marker>  nblank:<n>` | The language, with the line-comment marker and the blank-line spacing its formatter expects. |

A projected ctype brings along every ctype it is made of: its base, and the
arg of each Val, Inlary, Opt, Varlen and Bitfld field.  So `nslang:fm/ts` also
writes the handful of `algo` and `ams` ctypes that fm's messages contain,
without writing the rest of `algo`.

### The three forms of a projected ctype
<a href="#the-three-forms-of-a-projected-ctype"></a>

What a language gets for a ctype follows from the ctype's own schema.

| The ctype is | The language gets |
|---|---|
| packed (`dmmeta.pack`) | the type, its schema defaults, and a codec that encodes and decodes the wire form byte for byte |
| unpacked plain data, its members scalars or nested ctypes | the type, its schema defaults, and its memory layout: a size constant and an offset constant per member, the offsets C++ is held to |
| anything else | the type alone, every member starting at the language's zero |

The first form is a message or a value on the wire.  The second is a struct that
a foreign program shares with a C++ one in memory, such as the shm ring header
`ams.Shmhdr`, which is how a native-api userproc in Go reads the ring its
gateway writes.  The third is the shape of a JSON reply or a table row, such as
`report.acr`.  A member typed by a key, a pattern or a string ctype is
declared as the language's string in that form.

### What comes out
<a href="#what-comes-out"></a>

Every language gets one file per namespace, written by one generation step per
namespace (`lang_go`, `lang_py`, `lang_rs`, `lang_ts`):

| Language | Output |
|---|---|
| Go | a package per namespace under `go/gen` |
| Python | a module per namespace under `py/gen` |
| Rust | a module per namespace in the crate under `rs/gen`, rooted at the namespace of the projected dispatch |
| TypeScript | a module per namespace under `ts/gen`; see [JavaScript / TypeScript emission](/txt/exe/amc/js.md) |

The Go and Rust emitters write final text, formatted as gofmt and rustfmt would
leave it, since `amc` runs on machines where neither toolchain is installed.  A
member whose name collides with a method the codec adds is renamed with a
trailing underscore.

### Checks
<a href="#checks"></a>

`amc` refuses a projection it cannot write correctly, before any file is
written, and names every offending field in one run.  After a refusal no
language module is generated at all.

- `amc.proj_wire`, `amc.proj_tail`, `amc.proj_layout` -- a packed ctype has no
  byte-exact wire form in Go, Python or Rust.  A codec that skipped the field
  would read every later field from the wrong offset.  TypeScript's codec has its
  own check, `amc.ts_wire`.
- `amc.proj_type` -- a member has no type in a projected language, such as a
  pointer, whose value is an address in one process.
- `amc.proj_lenfld` -- the length field of a projected message is a bitfield.
  Every codec stores the frame length at the length field's own slot, and a
  bitfield owns none.
- `amc.proj_dflt` -- a default has no literal form.  A product of decimal
  integers, such as `1024*1024*1024`, is folded to its value, and a negative
  default of an unsigned scalar to the value its bits hold.  A nested member
  with a default of its own needs a ctype that wraps one scalar, such as
  `algo.SeqType`, and the default is stated on that scalar.
- `amc.proj_bitfld` -- a bitfield's type has no scalar wire form.
- `amc.displang_msg` -- a projected dispatch speaks a message the projection
  does not hold.  Add an `nslang` or `ctypelang` row for it.
- `amc.rs_root` -- a second namespace holds a dispatch projected into Rust.  A
  crate has one root, and that namespace is it.

### Adding a language
<a href="#adding-a-language"></a>

1. Add an `amcdb.lang` row naming it.
2. Write an emitter, `cpp/amc/lang_<lang>.cpp`, registered by an `amcdb.gen` row
   named `lang_<lang>`.  It reads the projection, `ctype.c_lang`, and writes the
   three forms above.
3. Add `nslang` and `ctypelang` rows for what the language should carry.
4. Write the client above the codecs by hand, under `<lang>/<ns>` for the
   protocol of namespace `<ns>`.
5. Deliver the toolchain as an `ainst` package, so a build machine can run the
   tests.
