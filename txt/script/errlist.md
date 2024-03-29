## bin/errlist - Open error list in editor


### Usage: `errlist <command>`
<a href="#usage--errlist--command--"></a>
The script executes `<command>` as a compilation command in `EDITOR`.
`emacs` and `vi[m]` are supported as standard editors.
Any output from the command which contains lines in the form `<filename>:<linenumber>: <text>`
can then be used to visit the corresponding location in the file and edit it.

Emacs: once inside the editor, `F7` can be used to re-execute the command without leaving.

Many standard utilities, including OpenACR, support output in this form.
Errlist can be used with

* `errlist ai` -- edit compilation erros
* `errlist abt_md -check` -- fix errors in readme files
* `errlist ff <expression>` -- find expression in files and open it
* `errlist src_func <ns> <regx>` -- visit functions

### See Also
<a href="#see-also"></a>
* [diff-to-errlist](diff-to-errlist.md) - Convert diff output to errlist

### Sources
<a href="#sources"></a>
The source code license is GPL

Source file: [bin/errlist](/bin/errlist)

