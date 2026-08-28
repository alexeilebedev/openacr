## amc Feature: Trace counters — ftrace, disptrace, usertracefld
<a href="#amc-feature-trace-counters-ftrace-disptrace-usertracefld"></a>

amc-generated processes maintain a per-namespace `trace`
record (`<ns>::_db.trace`) populated by a hand-picked set of
`u64` counters and `u64` cycle accumulators.  Several ssim
records contribute fields automatically:

| Source                          | What's added to `<ns>.trace` |
|---------------------------------|-------------------------------|
| `dmmeta.ftrace` on a `fstep`    | `step_<step>` (count) + `step_<step>_cycles` (sum of `rdtsc` deltas). |
| `dmmeta.ftrace` on a `Pool` field | `alloc_<pool>` and `del_<pool>` counters. |
| `dmmeta.ftrace` on an `Fbuf` field | `<fbuf>_n_read_byte`, `_n_read_msg` (input) or `_n_write_byte`, `_n_write_msg`, `_n_eagain` (output). |
| `dmmeta.disptrace` on a Dispatch | `dispatch_<disp>_<msg>` + optional `_cycles` per message type. |
| `dmmeta.usertracefld`           | Any user-defined `u64` field added by name. |

The trace record's existence is what enables `acr -t
<ns>.trace` to dump the live counters out of a running
process via reflection.

```ssim
dmmeta.field   field:abt.FDb.bh_syscmd  arg:abt.FSyscmd  reftype:Bheap
  dmmeta.ftrace  field:abt.FDb.bh_syscmd   # adds alloc/del counters
```

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [How tracing works](#how-tracing-works)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [ftrace on a step](#ftrace-on-a-step)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [ftrace on a pool](#ftrace-on-a-pool)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [ftrace on an fbuf](#ftrace-on-an-fbuf)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [disptrace](#disptrace)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [usertracefld](#usertracefld)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Reading traces at runtime](#reading-traces-at-runtime)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### How tracing works
<a href="#how-tracing-works"></a>

amc's `gen_trace` phase (cpp/amc/trace.cpp) does three things:

1. For each namespace with a global FDb, create the
   `<ns>.trace` ctype and a `Val` field
   `<ns>.FDb.trace` of that type.
2. For each `dmmeta.ftrace`, `dmmeta.disptrace`, and
   `dmmeta.usertracefld` record, add `u64` counter fields to
   the trace ctype with names following a strict naming
   convention.
3. Wire counter bumps into the corresponding code paths —
   the `_Alloc` / `_FreeMem` / step `_Call` / fbuf `_Refill`
   / dispatch arm code all `++` the matching trace field.

The `<ns>.trace` ctype is marked with `reflect:true` so the
runtime can find and read its fields by name.  Trace
counters are visible to `<ns>::GetTrace` (the function that
populates a snapshot for printing) and through `<ns>::_db.trace`
directly.

### ftrace on a step
<a href="#ftrace-on-a-step"></a>

```ssim
dmmeta.fstep   fstep:net.FDb.cd_recv  steptype:Inline
  dmmeta.ftrace  field:net.FDb.cd_recv
```

Adds:

```c++
struct net::trace {
    u64 step_cd_recv;
    u64 step_cd_recv_cycles;
};
```

Each invocation of the step's `_Call` does:

```c++
u64 t0 = algo::get_cycles();
cd_recv_Step();
net::_db.trace.step_cd_recv++;
net::_db.trace.step_cd_recv_cycles += algo::get_cycles() - t0;
```

…so you can read live throughput and cycle cost.

### ftrace on a pool
<a href="#ftrace-on-a-pool"></a>

For a pool field at the global level:

```ssim
dmmeta.field   field:abt.FDb.target  arg:abt.FTarget  reftype:Lary
  dmmeta.ftrace  field:abt.FDb.target
```

Adds:

```c++
struct abt::trace {
    u64 alloc_FDb_target;
    u64 del_FDb_target;
};
```

`<pool>_Alloc` increments `alloc_<partrace>_<name>`;
`<pool>_Delete` increments `del_<...>`.  The "partrace" is
the canonical reference name for the parent type (FDb-rooted
pools get `FDb`).

### ftrace on an fbuf
<a href="#ftrace-on-an-fbuf"></a>

Direction-specific:

```ssim
dmmeta.field   field:net.FConn.in_buf  arg:net.MsgHdr  reftype:Fbuf
  dmmeta.fbuf    field:net.FConn.in_buf  fbufdir:in  ...
  dmmeta.ftrace  field:net.FConn.in_buf
```

For an input fbuf, adds:

```bash
n_read_byte    -- total bytes refilled
n_read_msg     -- total messages dispatched
```

For an output fbuf:

```bash
n_write_byte
n_write_msg
n_eagain       -- count of EAGAIN/EINTR retries
```

These let you spot a slow consumer, a stuck writer, or
runaway throughput at a glance.

### disptrace
<a href="#disptrace"></a>

```ssim
dmmeta.disptrace  dispatch:gate.In  cycle:Y
```

Adds one trace field per dispatch member:

```c++
struct gate::trace {
    u64 dispatch_In_HeartbeatMsg;
    u64 dispatch_In_HeartbeatMsg_cycles;   // only when cycle:Y
    u64 dispatch_In_LoginMsg;
    u64 dispatch_In_LoginMsg_cycles;
    u64 dispatch_In_Unkmsg;                // fallback bucket
    u64 dispatch_In_Unkmsg_cycles;
};
```

The dispatch's generated switch wraps each callback with
`++count` and (when `cycle:Y`) `+= cycles`.  See
[Dispatch / disptrace](/txt/exe/amc/dispatch.md#disptrace-cycle-and-count-tracing).

### usertracefld
<a href="#usertracefld"></a>

For application-defined counters that aren't driven by amc:

```ssim
dmmeta.tracefld     tracefld:gate.trace.connections
dmmeta.usertracefld tracefld:gate.trace.connections  comment:"current open connections"
```bash

amc adds a `u64 connections` field to `gate::trace`.  Your
code updates it directly:

```c++
gate::_db.trace.connections++;
```

These appear in the reflection output alongside the
automatic counters.

### Reading traces at runtime
<a href="#reading-traces-at-runtime"></a>

`<ns>::GetTrace(out)` snapshots the trace record into a
typed `<ns>::trace` value (suitable for printing or sending
over the wire).  amc registers each namespace's `GetTrace`
in `algo_lib::_db.imdb`, so any in-process tool can read
trace from any linked namespace by name:

```c++
algo_lib::FImdb &imdb = ind_imdb_Find("gate");
gate::trace t;
imdb.GetTrace(&t);
prlog(t);   // emits the full ssim-tuple form
```

Externally, a trace dump against a running process's metadata
produces the same data; `acr_in <ns> -data` lists the available
counters.

### Pitfalls
<a href="#pitfalls"></a>

- **ftrace only works at the global level.**  Pool ftraces
  on non-FDb parents are ignored (the trace record lives on
  FDb).
- **Cycle counters are wall-clock-ish on x86 (`rdtsc`).**
  Don't expect them to match CPU time exactly under
  frequency scaling; they're stable on locked-frequency
  cores.
- **disptrace + dispctx are independent.**  Both can be set
  on the same dispatch.
- **The trace ctype is auto-created.**  Don't hand-author a
  `<ns>.trace` ctype; amc creates it.  You can add
  `usertracefld` rows to extend it.
- **Counters never reset.**  They run for the lifetime of
  the process.  Compare snapshots if you want rate.
- **Trace fields are reflected.**  Any name change ripples
  to the `<ns>::trace` struct field name *and* the runtime
  reflection table — older external monitors must be
  updated together.

### See also
<a href="#see-also"></a>

- [Dispatch / disptrace](/txt/exe/amc/dispatch.md#disptrace-cycle-and-count-tracing)
- [Runtime / steps](/txt/exe/amc/runtime.md)
- [Reflection](/txt/exe/amc/reflection.md) — runtime read of trace state
- Source: `cpp/amc/trace.cpp`
- Spec tables: `acr 'dmmeta.ftrace:%'`, `acr 'dmmeta.usertracefld:%'`, `acr 'dmmeta.tracefld:%'`
