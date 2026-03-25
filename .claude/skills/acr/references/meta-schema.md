# Meta-Schema Reference

Key tables in `data/dmmeta/`. This is a curated subset — run `acr dmmeta.%` for the full current inventory.

## Core Schema

| Table | Purpose | Key fields |
|-------|---------|------------|
| `dmmeta.ns` | Namespace declaration | `ns`, `nstype`, `license`, `comment` |
| `dmmeta.nstype` | Namespace type vocabulary (exe, lib, ssimdb, protocol, objlist, none) | `nstype` |
| `dmmeta.ctype` | C type (struct) definition | `ctype` (ns.Name), `comment` |
| `dmmeta.field` | Field within a ctype | `field` (ns.Ctype.name), `arg`, `reftype`, `dflt` |
| `dmmeta.reftype` | Reference type constructor with properties | `reftype`, `isval`, `isxref`, `cascins`, `hasalloc`, `del`, `up`, `cancopy`, `usebasepool`, `isnew`, `inst`, `varlen` |
| `dmmeta.xref` | Cross-reference (derived index) | `field`, `inscond`, `via` |
| `dmmeta.cascdel` | Cascade delete declaration | `field` |

## Container Configuration

Each container reftype has a configuration table controlling generated behavior:

| Config table | For reftype | Configures |
|---|---|---|
| `dmmeta.llist` | Llist | List kind: single/double, tail, circular |
| `dmmeta.thash` | Thash | Hash function, bucket allocation |
| `dmmeta.inlary` | Inlary | Inline array capacity |
| `dmmeta.ptrary` | Ptrary | Pointer array options |
| `dmmeta.smallstr` | Smallstr | Fixed string length and type |

## Field Features

Tables that add behavior to individual fields:

| Table | Feature |
|-------|---------|
| `dmmeta.fconst` | Named constants (enum values as individual records) |
| `dmmeta.fcmp` | Comparison function generation |
| `dmmeta.fcurs` | Cursor (iterator) generation |
| `dmmeta.ffast` | Fast accessor generation |
| `dmmeta.fflag` | Boolean flag field handling |
| `dmmeta.finput` | Input/loading configuration |
| `dmmeta.foutput` | Output/serialization configuration |
| `dmmeta.fsort` | Sort function generation |
| `dmmeta.fstep` | Step function (periodic callback) |
| `dmmeta.ftrace` | Trace/debug instrumentation |
| `dmmeta.ftuple` | Tuple serialization |
| `dmmeta.funique` | Uniqueness constraint |
| `dmmeta.bitfld` | Bit-level field packing |
| `dmmeta.substr` | Substring extraction from composite primary keys |
| `dmmeta.falias` | Field aliasing |

## Type System

| Table | Purpose |
|-------|---------|
| `dmmeta.cpptype` | Maps ctype to C++ primitive type |
| `dmmeta.sqltype` | Maps ctype to SQL column type |
| `dmmeta.cfmt` | Print/read format for a ctype |
| `dmmeta.printfmt` | Print format configuration |
| `dmmeta.strfmt` | String format configuration |
| `dmmeta.cstr` | String type declaration |
| `dmmeta.csize` | Type size specification |
| `dmmeta.pack` | Packed struct directive |
| `dmmeta.cextern` | External (non-generated) type |

## Generator Configuration

| Table | Purpose |
|-------|---------|
| `amcdb.gen` | Generator registry: name → function, `perns` flag for scope |
| `amcdb.tclass` | Template class definitions |
| `amcdb.tfunc` | Template function definitions |
| `amcdb.tcurs` | Template cursor definitions |
| `dmmeta.nscpp` | C++ source file configuration per namespace |
| `dmmeta.nsx` | Extra namespace-level features |
| `dmmeta.nsinclude` | Additional `#include` directives |
| `dmmeta.main` | Main function generation |

## Build Configuration (data/dev/)

| Table | Purpose |
|-------|---------|
| `dev.target` | Build target declaration |
| `dev.targsrc` | Source files belonging to a target |
| `dev.targdep` | Target-to-target dependencies |
| `dev.syslib` | System library requirements |

## Test Configuration (data/atfdb/)

| Table | Purpose |
|-------|---------|
| `atfdb.comptest` | Component test cases |
| `atfdb.unittest` | Unit test declarations |
| `atfdb.citest` | CI test configurations |
| `atfdb.amctest` | AMC-specific tests |

## Querying the Schema

```bash
# List all tables in a namespace
acr dmmeta.% | head

# All fields of a specific ctype
acr dmmeta.field -where:field:amc.FDb.%

# Reftype properties
acr dmmeta.reftype -where:reftype:Thash

# All cross-references for a namespace
acr dmmeta.xref -where:field:amc.%

# Check integrity across everything
acr -check

# Show a record with all related records (up and down)
acr dmmeta.ctype -where:ctype:amc.FDb -xref

# Show a record with its immediate children
acr dmmeta.ctype -where:ctype:amc.FDb -ndown:1

# Tree view for human reading
acr dmmeta.ns -where:ns:amc -tree -ndown:2
```
