## bin/grephunk - Patch file filter
<a href="#bin-grephunk"></a>

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

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL

Source file: [bin/grephunk](/bin/grephunk)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

