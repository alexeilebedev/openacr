## src_lim: Rules
<a href="#src_lim-rules"></a>

`src_lim` refuses bad lines and stray files, and the `normalize` cijob runs it
over the whole tree.  [The src_lim README](/txt/exe/src_lim/README.md) says what
each check does.  What follows is the part of its design that the source does
not state.

It carries no Steps section, since it reads the tree once and exits.

### A credential is a bad line
<a href="#a-credential-is-a-bad-line"></a>

Suppose a token lands in a test script.  Nothing that builds the tree notices,
because the file compiles, every test passes, and the token sits there until
somebody publishes a package that carries the script.  A scanner that runs only
at publish time catches it late, after the token is in this tree's history and
in every clone of it.

A token is a line pattern that must not appear in the tree, which is exactly
what `dev.badline` already states for C++ idioms.  So the secret rules are
`dev.badline` rows, `secret_%`, and the badline scan covers every tracked file.
The pre-merge gate then refuses a credential in the commit that adds it, and
publishing a package runs the same rules over what it is about to push.

### Invariants
<a href="#invariants"></a>

- The badline scan reads every `dev.gitfile` except generated code, `extern/`
  and symlinks.  A rule scopes itself by `gitfile_regx`, so a code rule names the
  source directories it governs, and a secret rule names `%`.
- An exemption a file cannot carry inline is a path in the rule's
  `exempt_regx`, and the rule's `comment` says why.  `test/crt/%` is the one
  today: the test CA's private keys.
- A line reaches a rule's regular expression only when it contains the literal
  text that opens the expression.  A rule whose expression opens with an
  operator has no such text and is run on every line, so a new rule keeps a
  literal at its head.
