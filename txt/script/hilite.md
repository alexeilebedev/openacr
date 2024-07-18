## bin/hilite - Inline highlighter
<a href="#bin-hilite"></a>

The hilite tool is a perl script for highlighting strings in some output.

Usage:

    <some command> | hilite [-md] <expr1> <expr2> ...

Each expr is a perl regex that is highlighted using a separate color.
colors are picked automatically from a default sequence.

To specify a color explicitly, use its name: `expr1:green expr2:yellow`
The list of available colors is black, red, green, yellow, blue, magenta, cyan, white, darkblack, 
darkred, darkgreen, darkyellow, darkblue, darkmagenta, darkcyan, darkwhite.

Omitting `expr` matches the entire line, so `hilite :green` makes all lines green.

Optional flag ``-md`` prints selected items surronded by markdown color markers like this ``<span style="background-color: grey; color:green;">ITEM</span>``.
It is useful with the output of [ssimfilt - Tuple utility](/txt/exe/ssimfilt/README.md) with ``-mdtable`` option.
It provided the ability to color table items in markdown.

#### Blotter Mode
<a href="#blotter-mode"></a>

To highlight every other line, use `:green%2`. This is useful with values of 5
when the output is wide, creating a blotter effect.

If multiple patterns match a given input line, patterns specified later
on the command line override previous patterns; Each pattern can match multiple
times within the line.

#### Inline Specification
<a href="#inline-specification"></a>

Highlighting rules can be provided as part of the input.
If a line starts with the special word 'hilite.colors', e.g.

    hilite.colors <expr1> <expr2> ...

Then the previous rules are flushed,
and new exprs are processed as if they were read from the command line.
This allows commands to output hilite instructions that keep the text readable
yet support highlighting where appropriate.

#### Diff mode
<a href="#diff-mode"></a>

If `<expr>` is `-diff` or `-d`, hilite loads diff rules that support either line-
or word-diffs. Example of colorizing a diff file:

   cat patchfile | hilite -d | less -r

Removed lines or words are colored red, added lines or words are green, and index
lines are blue.

Hilite is not intended to compete with the various syntax highlighters; it is an
interactive tool intended to ease readibility or terminal-based output.

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL

Source file: [bin/hilite](/bin/hilite)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

