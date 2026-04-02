# Runtime State Inspection (generalized headless)

This is an amc-level capability, not specific to any one tool.

## The idea

Headless mode in acr_nav serializes curated projections (Screen, PanelState, VisibleField) as ssimfile records. But every amc-generated program already has typed pools in `_db`. If amc could generate a generic state-dump function, any OpenACR program could expose its full in-memory state on demand -- headless mode for free, from the schema.

This is the "freeze a running program and look at its tables" idea turned into a general capability.

**Value:** High for programs under active development. Mature tools (amc, acr) are already debugged -- the payoff is for new servers and services being built, where Claude Code needs to inspect evolving runtime state daily.
**Size:** Medium. Phase 1 (pool census) is a single amc generator. Phase 2 (filtered dump) adds serialization. Phase 3 (control endpoint) adds HTTP exposure.
**Primary consumer:** Claude Code as agent -- inspecting programs at runtime during development, and auto-testing them similar to acr_nav headless.

## What generalizes cleanly (output/dump side)

amc knows every pool in FDb via the `zd_inst` linked list (discovery in `cpp/amc/gen.cpp:460`, `gen_detectinst()` checks `reftype.inst` flag). ~120 pool-typed fields (inst:Y reftypes) exist across all FDb structs — Lary accounts for ~116 of these. A new `amcdb.gen` phase could generate a state-dump function -- one record in `gen.ssim`, one generator, every program gets it. Passes the factorization test.

**Print gap (verified 2026-04-02):** Print is NOT generated for every ctype. Only 830 of 1468 ctypes have `cfmt` entries with `print:Y`. 619 have no cfmt at all (mostly extern types like DIR, SSL_CTX). 19 explicitly opt out. MVP approach: census (counts) for all pools, record dump only for ctypes that have cfmt. Full dump would require field-by-field serialization independent of cfmt -- Phase 2 work.

## What doesn't generalize (input/step side)

`SendKey` is an acr_nav verb -- meaningful because it's a TUI with keybinds. A batch tool has args + stdin. A server has protocol messages (HTTP, WebSocket, Kafka, etc.). The "step" verb is program-specific even though the "dump" verb is universal. More precise: **any tool becomes agent-inspectable.** Agent-testable requires per-program input verbs.

## Where the value is highest

**Long-running servers under active development.** This is where inspectability pays off most:

| Property | Why census helps |
|---|---|
| **Long-running** | State accumulates -- leaks, stuck messages, orphaned connections are invisible without inspection |
| **Multi-protocol** | Need to verify state at each protocol boundary (ingress vs egress) |
| **Concurrent** | Multiple producers/consumers -- counts reveal imbalances that logs bury |
| **Tiered storage** | Pool counts show where data lives right now (memory vs disk vs object store) |
| **Multi-tenant** | Per-tenant pool counts reveal isolation violations or resource hogging |

The diagnostic scenario: a consumer receives 97 of 100 messages. Where are the other 3? Pool census answers in seconds -- `FMessage n:100, FDelivery n:97, FPendingAck n:3` -- problem is in the ack path, not ingestion. No rebuild, no reproduce, no log archaeology.

For mature batch tools (amc, acr, abt), the value is lower -- they already work. The investment pays off for programs that are *being built*, not programs that are *already built*.

## Curated vs raw

Raw pool dumps are the truth -- curated views are opinions that drift. But raw dumps of a real server (thousands of records, runtime artifacts like file descriptors and computed caches) are a firehose. ACR answer: generate the raw dump (free from schema), let programs also define curated views as additional ctypes. Both, not either/or. The curated views are just more records -- they pass the factorization test.

## Tpool traversal (verified)

Full pool scan has a practical obstacle: Tpool generates zero cursor functions (confirmed: not in `amcdb/tcurs.ssim`, no iteration in `cpp/amc/tpool.cpp`). Free-list allocator -- can alloc/free but not scan. Lary, Tary, Ptrary, Inlary all have cursors; Tpool and Sbrk do not.

The right approach: **dump everything reachable**, not everything allocated. Follow access paths from `_db`, not scan pools. For pools with cursors (Lary, Tary, etc.) -- direct iteration. For Tpool records -- reach them via xref indexes (Thash, Llist) hanging off `_db`. Output as ssim tuples with regex filtering. This is essentially `acr` for runtime state.

## Trigger mechanism

Depends on program type:

| Program type | Trigger | Output channel |
|---|---|---|
| Batch tool | `-dump` flag, emits state before exit | file or stderr |
| TUI | stdin command (proven with acr_nav headless) | stdout interleaved |
| Long-running server | REST/WebSocket control endpoint, or SIGUSR1 | HTTP response or file |

For servers, a control endpoint (`/debug/census`, `/debug/dump?ctype=ns.FStream`) fits naturally -- most servers already have HTTP interfaces. Claude Code hits the endpoint between test steps: send messages, inspect state, verify, continue.

## Phased implementation

**Phase 1 -- Pool census.** Generate a function that emits ctype name + record count for every pool in FDb. One line per pool. Zero serialization, zero Print dependency. Count access varies by reftype: Lary/Tary/Ptrary/Inlary use `_n` field; Tpool/Lpool need freelist math. Lary covers ~116 of ~120 pools so `_n` handles almost everything. Output: `ns.FCtype n:423`. Smallest useful increment. Single `amcdb.gen` record + one generator function.

**Phase 2 -- Filtered record dump.** Add regex filtering on ctype name. For matching pools, iterate records and emit as ssim tuples. Field-by-field serialization generated by amc directly (no cfmt dependency). Query format: `-dump:"ns.FCtype:dmmeta.%"`.

**Phase 3 -- Control endpoint.** Generator that auto-exposes census and dump via HTTP for long-running servers. Same ssim output format, accessible via REST. Programs get it by adding one record.

## Generator mechanics (verified)

94 generators exist in `data/amcdb/gen.ssim` (as of 2026-04-02). Adding a new one requires: 1 ssimfile record (`amcdb.gen gen:state_dump perns:Y`), 1 C++ function (`amc::gen_state_dump`). Uniform `void()` contract. The dispatch loop in `cpp/amc/main.cpp` (`Main_Gen()`) calls all generators via `gen.step()` function pointers -- no special cases needed.
