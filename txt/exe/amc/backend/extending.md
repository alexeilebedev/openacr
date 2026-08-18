## amc Backend: extending amc
<a href="#amc-backend-extending-amc"></a>

How to add a new reftype or a new gen-pipeline phase.  This is the
"add to the compiler" recipe.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [Adding a reftype](#adding-a-reftype)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Adding a gen phase](#adding-a-gen-phase)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Adding a tfunc](#adding-a-tfunc)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sandboxing changes](#sandboxing-changes)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Adding comptests](#adding-comptests)<br/>

### Adding a reftype
<a href="#adding-a-reftype"></a>

*To be written.*  Step list:
1. Add a row to `data/dmmeta/reftype.ssim`
2. Add a tclass to `data/amcdb/tclass.ssim`
3. Create `cpp/amc/<reftype>.cpp` with the `tclass_<X>` and
   `tfunc_<X>_*` functions
4. Add tfunc records to `data/amcdb/tfunc.ssim`
5. Run `acr -check % -x` and `ai`

### Adding a gen phase
<a href="#adding-a-gen-phase"></a>

*To be written.*  Step list:
1. Add a row to `data/amcdb/gen.ssim` with the desired position
2. Add the `gen_<name>` function in `cpp/amc/<name>.cpp`
3. Add the matching userfunc record

### Adding a tfunc
<a href="#adding-a-tfunc"></a>

*To be written.*  When you want a new generated function for an
existing reftype.

### Sandboxing changes
<a href="#sandboxing-changes"></a>

*To be written.*  Use `wt amc -reset -diff -- amc` so a
broken generator phase cannot prevent you from fixing itself.

### Adding comptests
<a href="#adding-comptests"></a>

*To be written.*  `atf_amc` covers the amc generator itself.
Where the test fixtures live and how to capture expected output.
