## atf_ci - Normalization tests (see citest table)


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [atf_ci - Internals](/txt/gen/atf_ci/atf_ci.md)<br/>

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -citest -- Regx of tests to run
<a href="#-citest"></a>

#### -maxerr -- Exit after this many errors
<a href="#-maxerr"></a>

#### -cijob -- 
<a href="#-cijob"></a>

#### -cleanup -- (action) Remove the credentials this run installed and exit
<a href="#-cleanup"></a>

Selecting an environment installs an ssh credential.  When
`GITLAB_SSH_PRIVATE_KEY` names a file, `atf_ci` copies it to `~/.ssh/id_rsa`,
because reaching the environment's nodes and forwarding an agent into its
containers both need an identity.  On a CI runner that file then outlives the
job, since the runner's home directory is not part of the checkout that gets
cleaned.

That matters because of what the key opens.  It is installed into node accounts
whose sudoers file grants passwordless root, and the next job the runner is
handed is a job whose script an untrusted account may have written -- anyone who
can open a merge request.  So the credential has to be gone before that job
starts.

`-cleanup` removes it, and it removes only what this checkout installed.  Each
install is recorded as an `atf_ci.Credfile` row under `temp/atf_ci.cred`, and
the scrub takes the files that record names.  Working from a recorded fact
rather than from a list of likely names is what keeps it away from the rest of
`~/.ssh`: a runner's own `authorized_keys` lives there, which is how the
operator reaches the machine at all, and so does any identity they installed by
hand.  A file `atf_ci` never wrote is absent from the record, so the scrub
cannot reach it.

`temp/` is emptied by the runner's `git clean` as a job starts, so the record
describes this job and never an earlier one.  Run it from an `after_script`,
after whatever still needs the credential -- the `awsx2` job does exactly that,
following the stop command that reaches the environment over ssh:

```bash
bin/atf_ci -cleanup
```

It runs no citest, so it takes none of the citest prologue.  `-check_clean` in
particular does not apply: a test run refuses to start on a dirty tree, and
inheriting that refusal here would mean every job that modified the tree leaves
its credential behind.

#### -capture -- Capture the output of the test
<a href="#-capture"></a>

#### -check_clean -- Check for modifications after each test
<a href="#-check_clean"></a>
