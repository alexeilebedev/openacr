# acr_nav M4 — Single-Panel Scrollable TUI [DONE]

## Context

M3 proved the schema works with batch output (cursor loops, hash lookups, Upptr traversal). M4 replaces batch mode with an interactive single-panel TUI: the ctype list, scrollable with arrow keys, quit with `q`. This teaches: **terminal raw mode, ANSI rendering, input-driven loop**.

No second panel yet — that's M5. No filter mode — that's M6. M4 is the simplest interactive program that works.

---

## Design decisions

### 1. Blocking read loop, not MainLoop/iohook

M4 uses a simple blocking `read(STDIN_FILENO)` loop. MainLoop/iohook is infrastructure for multiplexing concurrent I/O and time-based scheduling. M4 has exactly one input source (stdin) and zero timers — the event loop adds complexity without purpose. A blocking loop is SESE: read -> dispatch -> render -> repeat.

If M6 needs async behavior (lib_rl for filter input), switch then. Don't design for hypothetical requirements.

### 2. Keybind table drives dispatch

Raw terminal bytes -> key name string -> `ind_keybind_Find()` -> `p_navaction`. The keybind table is the single source of truth for key-to-action mapping. Adding a new keybind = one record, zero code.

The translation from raw bytes to key name strings is inherently procedural (ANSI/VT100 is a fixed external protocol, not application data). Once we have the key name, everything is data-driven.

### 3. If-chain for action dispatch (tracked debt -> M6)

DispatchAction uses a 5-way if-chain on action names. This is the "missing noun" anti-pattern, but it's explicitly deferred to M6 where gstatic dispatch replaces it. The keybind->navaction lookup IS factorized (data-driven). Only the last mile (dispatch) is unfactorized — acceptable at 5 entries, fixed when M6 adds mode-dependent dispatch for all 11 actions.

### 4. TTY detection: batch fallback

If `isatty(STDOUT_FILENO)` is false, fall back to M3 batch output. Follows the amc_vis pattern. `acr_nav | head` still works.

### 5. Raw termios, not readline

lib_rl provides readline, but M4 needs single-keypress input (no line editing). Raw termios is simpler and sufficient. M6 may use lib_rl for filter mode text input.

### 6. Ctrl-C as hardcoded quit

Ctrl-C (byte 3) is handled as an immediate quit *before* the keybind lookup. It's a terminal convention (external protocol), not an application keybind — it shouldn't be rebindable via the keybind table.

---

## Files to modify

**`cpp/acr_nav/acr_nav.cpp`** — complete rewrite. One file.

No schema changes needed. All UI state fields exist from M2: `FPanel.scroll_offset`, `FPanel.sel_row`, `FDb.term_hei`, `FDb.term_wid`, `FDb.running`, `FDb.p_cur_panel`, `FDb.p_cur_mode`.

---

## Implementation

### Function decomposition

```
Main()
  PopulateSelCtype()     -- filter ctypes by namespace regex
  if !isatty -> BatchOutput() -- M3 batch fallback
  else:
    DetectTerminal()     -- TIOCGWINSZ for dimensions
    EnterRawMode()       -- termios cfmakeraw + atexit handler
    p_cur_panel setup    -- point to ctype_list panel
    Render()             -- initial draw
    while(running):
      ReadKeyName()      -- raw bytes -> key name string
      ind_keybind_Find() -- lookup in keybind table
      DispatchAction()   -- handle navaction (5-way if-chain)
      AdjustScroll()     -- keep selection in view
      Render()           -- redraw
  ExitRawMode()          -- atexit restores terminal
```

### Includes and global state

```cpp
#include <termios.h>
#include <poll.h>

static struct termios acr_nav_orig_termios;
static bool acr_nav_raw_mode = false;
```

### Function: ExitRawMode

Registered via `atexit()` — ensures terminal restore even on exception.

```cpp
static void ExitRawMode() {
    if (acr_nav_raw_mode) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &acr_nav_orig_termios);
        acr_nav_raw_mode = false;
        // Clear screen, show cursor, reset attributes
        (void)write(STDOUT_FILENO, "\x1b[2J\x1b[H\x1b[?25h\x1b[0m", 18);
    }
}
```

### Function: EnterRawMode

```cpp
static void EnterRawMode() {
    tcgetattr(STDIN_FILENO, &acr_nav_orig_termios);
    atexit(ExitRawMode);
    struct termios raw = acr_nav_orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 1;   // block until 1 byte
    raw.c_cc[VTIME] = 0;  // no timeout
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    acr_nav_raw_mode = true;
    // Clear screen and hide cursor
    (void)write(STDOUT_FILENO, "\x1b[2J\x1b[H\x1b[?25l", 14);
}
```

Note: ISIG disabled — Ctrl-C does not generate SIGINT. Byte 3 is handled in ReadKeyName as a hardcoded quit.

### Function: DetectTerminal

```cpp
static void DetectTerminal() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    acr_nav::_db.term_wid = w.ws_col;
    acr_nav::_db.term_hei = w.ws_row;
}
```

### Function: ReadKeyName

Maps raw terminal escape sequences to keybind-table key names.

```cpp
static bool ByteAvailable() {
    struct pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    return poll(&pfd, 1, 50) > 0;  // 50ms timeout
}

static tempstr ReadKeyName() {
    tempstr ret;
    char c = 0;
    int n = read(STDIN_FILENO, &c, 1);
    if (n <= 0) {
        acr_nav::_db.running = false;  // EOF or error
    } else if (c == 3) {
        acr_nav::_db.running = false;  // Ctrl-C: hardcoded quit (terminal convention)
    } else if (c == '\x1b') {
        if (!ByteAvailable()) {
            ret = "Escape";
        } else {
            char seq[4];
            seq[0] = 0;
            seq[1] = 0;
            (void)read(STDIN_FILENO, &seq[0], 1);
            if (seq[0] == '[') {
                (void)read(STDIN_FILENO, &seq[1], 1);
                if (seq[1] == 'A') {
                    ret = "Up";
                } else if (seq[1] == 'B') {
                    ret = "Down";
                } else if (seq[1] == 'C') {
                    ret = "Right";
                } else if (seq[1] == 'D') {
                    ret = "Left";
                } else if (seq[1] == '5') {
                    (void)read(STDIN_FILENO, &seq[2], 1);  // consume '~'
                    ret = "PgUp";
                } else if (seq[1] == '6') {
                    (void)read(STDIN_FILENO, &seq[2], 1);  // consume '~'
                    ret = "PgDown";
                }
            }
        }
    } else if (c == '\r' || c == '\n') {
        ret = "Enter";
    } else if (c == 127 || c == 8) {
        ret = "Backspace";
    } else if (c >= 32 && c < 127) {
        ret << c;  // printable: "q", "/", etc.
    }
    return ret;
}
```

The 50ms poll() timeout in ByteAvailable() distinguishes bare Escape from escape sequences. Fast enough to feel instant.

### Function: DispatchAction

5-way if-chain — replaced by gstatic dispatch in M6.

```cpp
static void DispatchAction(strptr action) {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    int n_items = acr_nav::zd_sel_ctype_N();
    if (action == "move_up") {
        panel.sel_row = i32_Max(0, panel.sel_row - 1);
    } else if (action == "move_down") {
        panel.sel_row = i32_Min(n_items - 1, panel.sel_row + 1);
    } else if (action == "page_up") {
        int page = acr_nav::_db.term_hei - 2;
        panel.sel_row = i32_Max(0, panel.sel_row - page);
    } else if (action == "page_down") {
        int page = acr_nav::_db.term_hei - 2;
        panel.sel_row = i32_Min(n_items - 1, panel.sel_row + page);
    } else if (action == "quit") {
        acr_nav::_db.running = false;
    }
}
```

### Function: AdjustScroll

```cpp
static void AdjustScroll() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    int visible = acr_nav::_db.term_hei - 2;
    if (panel.sel_row >= panel.scroll_offset + visible) {
        panel.scroll_offset = panel.sel_row - visible + 1;
    }
    if (panel.sel_row < panel.scroll_offset) {
        panel.scroll_offset = panel.sel_row;
    }
}
```

### Function: Render

Builds entire screen in a tempstr buffer, then single write() — gen/IO separation applied to rendering.

```cpp
static void Render() {
    tempstr buf;
    buf << "\x1b[H";  // cursor home
    int wid = acr_nav::_db.term_wid;
    int visible = acr_nav::_db.term_hei - 2;  // minus title bar and status bar
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;

    // Title bar (reverse video, full width)
    buf << "\x1b[7m";
    tempstr title;
    title << " " << panel.title << " (" << acr_nav::zd_sel_ctype_N() << ")";
    char_PrintNTimes(' ', title, i32_Max(0, wid - ch_N(title)));
    buf << title << "\x1b[0m\r\n";

    // Content rows
    int idx = 0;
    int row = 0;
    ind_beg(acr_nav::_db_zd_sel_ctype_curs, ctype, acr_nav::_db) {
        if (idx >= panel.scroll_offset && row < visible) {
            bool selected = (idx == panel.sel_row);
            if (selected) {
                buf << "\x1b[7m";
            }
            tempstr line;
            line << " " << ctype.ctype;
            char_PrintNTimes(' ', line, i32_Max(0, wid - ch_N(line)));
            buf << line;
            if (selected) {
                buf << "\x1b[0m";
            }
            buf << "\x1b[K\r\n";
            row++;
        }
        idx++;
    } ind_end;

    // Fill empty rows
    while (row < visible) {
        buf << "\x1b[K\r\n";
        row++;
    }

    // Status bar (reverse video)
    buf << "\x1b[7m";
    tempstr status;
    status << " q:quit  Up/Dn:navigate  PgUp/PgDn:page";
    tempstr pos;
    pos << (panel.sel_row + 1) << "/" << acr_nav::zd_sel_ctype_N();
    char_PrintNTimes(' ', status, i32_Max(1, wid - ch_N(status) - ch_N(pos)));
    status << pos;
    buf << status << "\x1b[0m";

    // Single write for minimal flicker
    (void)write(STDOUT_FILENO, buf.ch_elems, ch_N(buf));
}
```

### Function: PopulateSelCtype

Extracted from M3 Main(). Same logic.

```cpp
static void PopulateSelCtype() {
    ind_beg(acr_nav::_db_ctype_curs, ctype, acr_nav::_db) {
        bool match = Regx_Match(acr_nav::_db.cmdline.ns, ns_Get(ctype));
        if (match) {
            zd_sel_ctype_Insert(ctype);
        }
    } ind_end;
}
```

### Function: BatchOutput

M3 batch output preserved as fallback when stdout is not a terminal.

```cpp
static void BatchOutput() {
    ind_beg(acr_nav::_db_zd_sel_ctype_curs, ctype, acr_nav::_db) {
        prlog(ctype.ctype << "  (" << c_field_N(ctype) << " fields)");
        ind_beg(acr_nav::ctype_c_field_curs, field, ctype) {
            tempstr out;
            out << "  " << name_Get(field);
            char_PrintNTimes(' ', out, i32_Max(1, 24 - ch_N(out)));
            out << field.p_arg->ctype;
            char_PrintNTimes(' ', out, i32_Max(1, 52 - ch_N(out)));
            out << field.p_reftype->reftype;
            prlog(out);
        } ind_end;
    } ind_end;
    prlog("acr_nav.report"
          << Keyval("n_ctype", ctype_N())
          << Keyval("n_field", field_N())
          << Keyval("n_sel_ctype", zd_sel_ctype_N())
          << Keyval("n_keybind", keybind_N())
          << Keyval("n_panel", panel_N())
          << Keyval("n_navaction", navaction_N())
          << Keyval("n_navmode", navmode_N()));
}
```

### Function: Main (rewritten)

```cpp
void acr_nav::Main() {
    PopulateSelCtype();
    bool is_tty = isatty(STDOUT_FILENO);
    if (!is_tty) {
        BatchOutput();
    } else {
        DetectTerminal();
        EnterRawMode();
        _db.p_cur_panel = ind_panel_Find("ctype_list");
        vrfy(_db.p_cur_panel, "panel 'ctype_list' not found");
        _db.p_cur_mode = ind_navmode_Find("browse");
        vrfy(_db.p_cur_mode, "navmode 'browse' not found");
        _db.p_cur_panel->sel_row = 0;
        _db.p_cur_panel->scroll_offset = 0;
        Render();
        _db.running = true;
        while (_db.running) {
            tempstr key_name = ReadKeyName();
            acr_nav::FKeybind *keybind = ind_keybind_Find(key_name);
            if (keybind) {
                DispatchAction(keybind->p_navaction->navaction);
                AdjustScroll();
                Render();
            }
        }
        ExitRawMode();
    }
}
```

---

## Verification

```bash
src_hdr -write -target acr_nav    # Update header (if needed)
abt -build -install acr_nav       # Must compile clean
acr_nav dmmeta                    # TUI: ~200 dmmeta ctypes, scrollable
acr_nav                           # TUI: all ~1380 ctypes
acr_nav dmmeta | head -5          # Batch fallback: same as M3
acr_nav nonexistent               # TUI with 0 entries, q to exit
acr -check                        # Referential integrity
```

Spot-check:
- Up/Down moves selection highlight, scrolls at edges
- PgUp/PgDn jumps by page
- q exits cleanly, terminal works normally after
- Ctrl-C exits cleanly, terminal works normally after
- Status bar shows position (e.g., "42/1380")
- No flicker or rendering artifacts

---

## Expected screen

```
 Types (1380)                                     <- reverse video title bar
 algo.Bool
 algo.Charset
 algo.Comment
 algo.Smallstr100                                 <- reverse video selected row
 algo.Smallstr150
 algo.Smallstr200
 ...
 q:quit  Up/Dn:navigate  PgUp/PgDn:page  42/1380 <- reverse video status bar
```

---

## Notes

- `atexit(ExitRawMode)` covers signal paths — algo_lib's signal handler lets the program exit normally through atexit.
- `\x1b[K` (clear to end of line) after each content line prevents stale characters from wider previous lines.
- `OPOST` disabled in raw mode — must use `\r\n` not `\n` for line breaks in Render().
- The Llist traversal in Render() is O(n) per frame for ~1380 ctypes. Acceptable. No optimization needed.
- `(void)write(...)` suppresses compiler warnings on unused return value — standard codebase pattern.

---

## What M4 does NOT do (deferred)

| Feature | Milestone |
|---------|-----------|
| Second panel (fields) | M5 |
| Enter/Backspace navigation | M5 |
| Left/Right panel switching | M5 |
| Filter mode (/) | M6 |
| gstatic dispatch (no if-chain) | M6 |
| Terminal resize handling (SIGWINCH) | M7 |
| Color coding by reftype | M7 |
