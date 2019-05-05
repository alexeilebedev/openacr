## Scriptlets

OpenACR includes a number of little scripts to ease program development.
Some of them are useful as stand-alone utilities

### Hilite: inline highlighter

The hilite tool is a perl script for highlighting strings in some output.

Usage:

    <some command> | hilite <expr1> <expr2> ...

Each expr is a perl regex that is highlighted using a separate color.
colors are picked automatically from a default sequence.

To specify a color explicitly, use its name: `expr1:green expr2:yellow`
The list of available colors is black, red, green, yellow, blue, magenta, cyan, white, darkblack, 
darkred, darkgreen, darkyellow, darkblue, darkmagenta, darkcyan, darkwhite.

Omitting `expr` matches the entire line, so `hilite :green` makes all lines green.

#### Blotter Mode

To highlight every other line, use `:green%2`. This is useful with values of 5
when the output is wide, creating a blotter effect.

If multiple patterns match a given input line, patterns specified later
on the command line override previous patterns; Each pattern can match multiple
times within the line.

#### Inline Specification

Highlighting rules can be provided as part of the input.
If a line starts with the special word 'hilite.colors', e.g.

    hilite.colors <expr1> <expr2> ...

Then the previous rules are flushed,
and new exprs are processed as if they were read from the command line.
This allows commands to output hilite instructions that keep the text readable
yet support highlighting where appropriate.

#### Diff mode

If `<expr>` is `-diff` or `-d`, hilite loads diff rules that support either line-
or word-diffs. Example of colorizing a diff file:

   cat patchfile | hilite -d | less -r

Removed lines or words are colored red, added lines or words are green, and index
lines are blue.

Hilite is not intended to compete with the various syntax highlighters; it is an
interactive tool intended to ease readibility or terminal-based output.

### Grephunk: patch file filter

Grephunk is a perl script that scan hunks on stdin
and filter them according them to specified pattern. A `hunk` is the name patch file
people use to refer to sections of a patch file that look like this:

    diff --git a/include/gen/atf_amc_gen.inl.h b/include/gen/atf_amc_gen.inl.h
    index fbcd12a..5871940 100644
    --- a/include/gen/atf_amc_gen.inl.h
    +++ b/include/gen/atf_amc_gen.inl.h
    @@ -385,0 +386,14 @@ inline bool atf_amc::Ctype2Attr_Update(atf_amc::Ctype2Attr &lhs, atf_amc::Ctype2
    +    atf_amc::DelType1_Uninit(*this);
    @@ -426 +440 @@ inline atf_amc::DispFilter::DispFilter() {
    -inline int atf_amc::pmask_Nbits(atf_amc::DispFilter &parent) {
    +inline int atf_amc::pmask_Nbits(atf_amc::DispFilter& parent) {

Here, there are two hunks, one for each line starting with `@@`.
The `filename` of a hunk is the contents of the `diff` line.

The `header` is everything from the word `diff` to the first `@@`.
Hunk body is everything between `@@` and the next `@@` or end of file.

Grephunk allows specifying patterns for any of these entities.
Patterns can apply to filename, line, hunk header, or the entire hunk.

Pattern prefix specifies its scope:
* no prefix: applies to hunk body
* l:    line pattern (applies to content only)
* f:    filename pattern (applies to diff header line)
* a:    applies to 'a' file (line starts with ---)
* b:    applies to 'b' file (line starts with +++)
* h:    hunk pattern -- applies to the entire hunk, including any header lines

Any number of patterns must be specified on the command line.
Filename pattern is specified with `f:<regex>`.
For instance,

    <some command> | grephunk abcd f:cpp

Searches fort hunks that match string `abcd` where the filename also
matches `cpp`

A pattern can be positive or negative, negative patterns are preceded with `!`.
So,

    <some command> | grephunk f:\!/gen/ | grephunk blah

Excludes files with /gen/ in the pathname, and then searches for string `blah`.

Grephunk outputs any hunk that matches any of the patterns. So, "OR" is assumed.
If you want "AND", just chain several grephunk invocations one after another, or use
a `blah.*blah` pattern.

In the example below, hunk must contain the word amc anywhere in it,
and filename should not have pattern /gen/

    grephunk amc f:!/gen/ h:'}\s+$'
