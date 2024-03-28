## AMC release notes
<a href="#amc-release-notes"></a>

commit fd4e3e9e92da625d46253a9d13f1886454163331
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Apr 26 08:51:30 2019 -0400

    target:amc: disallow unique hash on non-unique field

commit 621ede1ab5e0a895b1faa1ef873c2351e5a35fda
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Mar 28 09:53:00 2019 -0400

    amc: ensure that sizes of all ctypes can be calculated
    amc: add tclass Protocol for generation of StaticCheck function

commit 88599d10bb8519ccc97517347e27bb70bd4698ce
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Mar 27 15:22:52 2019 -0400

    target:amc: lpool_ReallocMem should return NULL on failure

commit fb8f9d94539a67ad60d2218053fdcdbeb5604384
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Mar 26 13:21:49 2019 -0400

    amc: generate string fields as u8, not char, to prevent anyone from casting

commit cbd8e35939ef93c6a9fd85a0f8a06ea29ed267e6
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Mar 19 08:32:54 2019 -0400

    amc: add message cursor generator
    atf_amc: add unit tests for message cursor

commit 30fa41e000e9cb940266881b264e7cff3c54d17e
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Mar 6 11:01:26 2019 -0500

    amc: exec: waitpid: handle syscall interrupt scenario
    amc: define command.sh for consistent use with SysCmd

commit 1d62da4f044870c728b70b4bdac866fab22059f7
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Feb 24 12:34:34 2019 -0500

    abt, amc: link only with algo_lib
    amc: define dmmeta.ssimvolatile table,

commit 89237d7c41308d673c6652f1c4b403061c8ea313
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Feb 21 13:54:16 2019 -0500

    amc: create a _proc struct for spawning executables (similar to _curs in concept)
    amc: add ExecX function for Exec reftype

commit f69f9a05e78c4de3ef905c2d41724468394ebf76
Author: Hayk Mkrtchyan <hmkrtchy@theice.com>
Date:   Tue Nov 13 16:43:28 2018 -0500

    AB-25999: amc: Make strict dispatch be non-strict on Opt and Varlen

commit c4e8ac36b20b98e6e427d011f9d9d45985b21ed9
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Nov 9 09:38:43 2018 -0500

    amc: Varlen.N needs to know size of arg;

commit d8e15fd6bd0d77ac3c4a2d2613141530ef51330d
Author: Jonathan Joshua <jonathan.joshua@theice.com>
Date:   Sun Oct 21 18:57:39 2018 -0400

    AB-25296: amc: Generate fix encode/decode by parser

commit fd12b8dbfbb7769c7f15276a044ae730fda684b0
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Oct 18 13:56:58 2018 -0400

    amc: allow Castdown more than 1 level deep
    amc: add Delptr reftype (value that's allocated dynamically on-demand)
    atf_amc: add testcase for Delptr

commit 574d16a8f278896c75f7a68aa6b4d51caff87aed
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Oct 11 15:50:58 2018 -0400

    target:amc: Make it a fatal error if certain LoadStatic inserts fail

commit e5711af2eee017ac352d45abd73723aaa34496a6
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Sep 11 09:00:53 2018 -0400

    amc: add bounds check to charset match

commit 66355ea500bb601e2fc7f073701667b2590c0167
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Sep 7 18:06:55 2018 -0400

        amc: add nsinclude table to allow the above to happen
    amc: do not generate FieldIds for fields that are never read. (-30K LOC)
    amc: FieldId: do not increment counter if ID is unused, eliminates gaps in field IDs

commit 22efbc3eeefecbb0f1eb2d01796337870b5e2730
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Sep 5 14:43:49 2018 -0400

    amc: add charset support as field reftype Charset

commit cfaacaeb911dfb148498fc6e2d61713eec506ac3
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Aug 31 11:48:06 2018 -0400

    amc: convert gstatic isfunc:Y to reftype:Hook and the appropriate functions
    amc: make hook functions a lot more type-safe and add some amc-level checks
    atf_amc: add several unit tests -- cpp/atf/amc/hook.cpp

commit 3b67babac9c230a27a2d269c160148bd460c0672
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Aug 16 11:53:06 2018 -0400

    amc: do not limit padded strings to 256 bytes.
    amc: define read/print for inlary and fixary
    atf_amc: add unit test for Inlary Read/Print

commit e293e7c3108fce048789f48be273d2f3b986b425
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jul 27 16:49:26 2018 -0400

    amc: for non-unique Ptrary, rename InsertMaybe to ScanInsertMaybe, marking

commit 6eb0e18ce55b05584d2e2f9ea1e2ec8a8781dec7
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jul 26 10:45:09 2018 -0400

    amc: based on profiling, make Tary.Init function inline since it's only 2 instructions.
    amc: add _timeout option to Exec types

commit 990c0edf1664a12ac1a7550486800f4a64c9378e
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Jul 18 13:40:34 2018 -0400

    amc: when reading a tuple using dispatch, detect both ctype name and ssimfile name variants
    atf_amc: add unit test for new behavior
    amc: organize dispatch generators under cpp/amc/disp
    amc: disallow dispatch read when ctype is not formatted as a tuple

commit 8654a602119e3455f9231f465d4df44c2d6c15e9
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Jul 18 10:08:41 2018 -0400

    amc: generate GetMsgLength, ToMsgMemptr functions
    atf_amc: add tests for GetMsgLength, ToMsgMemptr.

commit b2e875c1c95fb8df381c1806446a07e66dbc95e7
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jul 12 17:42:12 2018 -0400

    - amc: add cdecl table, to make sure forward-declared structs actually exist

commit 19cbc5959f8bb593b2700a22c139e88962e3feed
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jul 5 15:14:44 2018 -0400

    amc: extend size calculations to all ctypes in protocol namespaces
    target:amc: include Opt.Get, Varlen.Getary functions
    amc: add 'outfile' submodule
    amc: add 'include' submodule
    amc: add 'enum' submodule
    amc: add 'signature' submodule
    amc: HdrIncludesQ query to check if one header includes another
    amc: document 20 functions

commit 9134acf48a6c79891aa4ba66761c5e0d48089038
Author: Hayk Mkrtchyan <hmkrtchy@theice.com>
Date:   Mon Jun 25 10:55:21 2018 -0400

    AB-23259: amc: Generate helper functions for ctype state

commit cfa54714f75da204d95f41f778ac1ea5cb57f496
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Jun 19 17:35:37 2018 -0400

    target:amc: _Reserve function on Tary now exits process if

commit e59c23a7190153ccca4ab013448f131796568217
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu May 31 10:51:22 2018 -0400

    amc: add consistency check for field naming (unpredictable underscores)

commit 65666c6deeb3e4aca3663a190132ae808fe70b08
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu May 24 12:51:44 2018 -0400

    amc: refine versionsort implementation, use u64
    atf_amc: add unit tests for versionsort covering edge cases (e.g. '/' before '0' and ':' after '9')

commit 118de220c38cad7b4f586af73c96b700b644778c
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue May 22 13:54:29 2018 -0400

    amc: stricter generation of GetOrCreate when correct_getorcreate is set
    amc: add -trace regx option

commit 8a8b6ad2a49126708c96afa640fec628644f0a21
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed May 16 09:12:29 2018 -0400

    amc: disallow constructors with >10 parameters
    amc: -e - speed up 2x
    amc: 'infinity pool' check:
    amc: detect 3 potential memory corruptions due to use of non-varlen pool for varlen records

commit cca4adeb94ab52c7559a1b47bc17d8f1861561a4
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon May 7 14:18:56 2018 -0400

    amc: use operator < only for Atree comparisons. TODO: use _Lt
    atf_amc: add Avl range search check

commit aaca27a3b41b79db310ca9433e1eb32cdf16f6e1
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Apr 18 09:44:28 2018 -0400

    amc: delete TimeHookOnce step type, since it can be misused.
    amc: convert all instances of TimeHookOnce to InlineOnce

commit 07e39d08fb78b7655749ccf06da04206e3e66539
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Apr 13 15:07:13 2018 -0400

    amc: add check for typefld with no associated msgtypes (fixes crash)

commit 088ecbb8e8ea90add95d43090a0e7b614e48d2ce
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Apr 12 12:15:51 2018 -0400

    amc: TimeHookOnce should not deschedule entire queue when head element exipre=0.
    atf_amc: organize by test type
    atf_amc: add fstep_* group of tests; check TimeHookOnce behavior

commit 7033bf08a020cd86addaa03dcb9fb740c0735cfd
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Apr 10 15:12:55 2018 -0400

    amc: add gsymbol table, specifying namespace/ssimfile to use in generatnig C++ symbols

commit 51e19d71df2b6315e10885c8f290c928dd78736d
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Apr 5 15:58:33 2018 -0400

    amc: -e hangs terminal with vi as EDITOR

commit 5ad66123717ed4a13fd9c8e387cfe9b7d5bedf20
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Apr 4 14:39:11 2018 -0400

    amc: amc -e now supports dispatch, step name.

commit 23000cea99986e3082e0fb9f3688c67ee8db2d96
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Feb 23 17:45:50 2018 -0500

    amc: require that all ssimns ctypes have ssimfiles
    amc: disallow lib namespace from referencing an exe namespace;

commit 4de60270593e8fe57a4a8c8c08571b10ea60ecf8
Author: Hayk Mkrtchyan <hmkrtchy@theice.com>
Date:   Mon Jan 29 18:43:25 2018 -0500

    AB-21366: target:amc: make amc independent of ordname table

commit aaf7810cd99187c03da66ccc3c1f8f14e1e121a0
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jan 11 10:12:40 2018 -0500

    amc: compute size of structs with pad bytes, populate field offset assertions for xdp11, etc.
    amc: remove inconsistent bltin lookup (no bugs in codegen); tabulate use of fbigend

commit 23ad953fe65a31c46e2393b544c57e76b0e47a88
Author: Hayk Mkrtchyan <hmkrtchy@theice.com>
Date:   Thu Dec 21 15:18:58 2017 -0500

    AB-21040: target:amc: print opt field only when it has value

commit ff032ed788d981e7ca18fef4c8dff5ed80440cf8
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Dec 1 10:44:12 2017 -0500

    AB-20774: amc: faster field dispatch when converting string->struct

commit 3ad837651d577f76bb21486b285418650302c30a
Author: Hayk Mkrtchyan <hmkrtchy@theice.com>
Date:   Wed Nov 29 18:10:07 2017 -0500

    AB-20619: target:amc: remove duplicates from fcmap

commit 52b6d559679b04143e79f5646afea765a83101f4
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Nov 27 16:48:49 2017 -0500

    amc: add 'stripcomment' option for Base, apply to MENG tables

commit 65b135cbcb6e10ba64b81c68807a5ad02dafde46
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Nov 21 11:22:21 2017 -0500

    AB-20595: amc: support raw gconst printing
    amc: print gconst fields as raw within tuple where pkey is present

commit 0066e0cba28406036a41fd8e4ed36b44e93df49c
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Nov 17 14:29:33 2017 -0500

    amc: remove sorted Ptrary -- potential performance problem

commit ed6daa5e88fbea1afa0f40105519a193ef34bd2f
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Nov 15 15:29:17 2017 -0500

    AB-20493: amc: calculate ctypelen correctly for all types
    amc: always output ctype len

commit e6698b0ef88ec0485c008a6da6f2c717a4aa9dfb
Author: Hayk Mkrtchyan <hmkrtchy@theice.com>
Date:   Mon Oct 30 15:51:12 2017 -0400

    AB-20279: target:amc: Opt fix

commit 9218e84f9d390f29c44e86b304fe9af0a42a3e70
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Oct 15 13:31:59 2017 -0400

    amc: trace sbrk

commit 9d1aab5526cc64c719f30c7346a1fa6f2a7f0ac9
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Sep 28 13:57:12 2017 -0400

    AB-6322: src_func, amc: correct syntax error in output

commit a385d4a6013c232960e4e8de41941f1551befa44
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Sep 22 16:03:32 2017 -0400

    AB-19561: amc: prevent copying in certain scenarios

commit e4cfb6b6eaa683c44cfee94e138d711f59e17d3b
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jun 9 16:12:09 2017 -0400

    amc:

commit 69a1c32feafed979aaa9dd8e44bdf79e925be230
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon May 1 16:28:15 2017 -0400

    amc: add code for Regx + CopyIn

commit 80ad76be9592e95c8a71bdb03aa969b0bb4b6b85
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Apr 4 01:17:05 2017 -0400

    amc: include information about parse error

commit 001b110fdbc473c831b132621b14704167e67d76
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Mar 31 17:46:19 2017 -0400

    amc: implement Alloc for types derived from Opt/Varlen

commit 10e3d47d5095c0046d029a1d088b6607699791de
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Mar 31 12:42:28 2017 -0400

    amc: fix crash on missing gstatic

commit fc0f6a23b298400e05edfef0eec18d51cb04a64e
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Mar 29 10:32:54 2017 -0400

    amc: add pnew types Temp, Fdsend

commit 7a7e27c66e1a3492fb5de3cb9d1f3dc65d358a8f
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Mar 27 17:59:31 2017 -0400

    amc: auto-indent all output

commit e1b5d88b768c3ec7ad4e07dac02a0553c650ebae
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Mar 24 14:01:49 2017 -0400

    amc: add pnew support for Opt fields
    amc: add ByteAry pnewtype (_FmtByteAry)

commit 99869ce66a324d97afc7944e76d3092c80ddd17c
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Mar 23 11:43:39 2017 -0400

    target:amc: move xrefvia, xreffld into xref, update acr_ed

commit 74dac5c56adc906117f30725a41912ca171d2e01
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Mar 22 13:07:00 2017 -0400

    AB-14612: amc: implement Alloc for Opt field

commit dade801b4553ce0d1b55329d92cb8c4affbd6486
Author: slokegow <shreejith.billenahalli@theice.com>
Date:   Fri Mar 10 18:08:07 2017 -0500

      - amc: Decrement counter only if it is > 0

commit 93bf9b9c8d505c4883b357023abe57c7f35d274a
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Mar 8 16:53:27 2017 -0500

    amc: always initialize return value (no bugs found, preemptive correctness)
    amc: implement dangling pointer check. fix 6 dangling pointers.

commit e82d435a095a542ae8adc14f393e7a70f82adf6c
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Mar 7 14:30:35 2017 -0500

    amc: revise bad-tag strategy to allow multiple errors to get saved

commit ae17be161ce82322679f3decb4da9258a72de250
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Mar 7 09:35:57 2017 -0500

    amc: add dmmeta.Dispatch.call attribute

commit cfc890329bcc2a5f6aec279ad9e2c60a23772159
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Mar 3 11:28:40 2017 -0500

    amc: combine displen, dispread, dispprint into dispatch
    amc: eliminate dispatch.ctype_hdr field, determine it automatically.

commit c1339c99fa2a0b19d6d1500a66c6a916ef7795ca
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Mar 2 12:04:58 2017 -0500

    amc: add amc_disp file

commit c36230273a1b579dfec8fc2e1bbeecbe30d7d656
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Feb 15 19:01:27 2017 -0500

    amc: generate fewer and less reachable operators for faster compilation (10%, potentially fewer bugs)

commit 3f9d24fc7258dc4f31d1ac0d356885daa7676da5
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Feb 7 10:25:21 2017 -0500

    amc: improve code generation; uncached build times are <20% (2m43sec -> 2m10sec)

commit 90ae9f8cde388fbb58b8f7def36ee56f8787f525
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Jan 18 17:01:33 2017 -0500

    amc: Gstatic: more compact strings (-25K LOC)

commit d2b053e43a5bf5e15ff97963cab2f8b32adcbbe2
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Jan 9 15:18:49 2017 -0500

    AB-16014: target:amc: kill thash density option

commit 2ebb91dc48a1b7539895bb2841e5b9507d5bd04e
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Dec 27 17:16:31 2016 -0500

    AB-15791: amc: correct calculation of pmask width for dispfilter

commit cab8f858de40190af1f6b5c613c760fa37d70f0c
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Dec 27 12:11:57 2016 -0500

    amc: apply Bitfld defaults when initializing field

commit 7f0bf0b32b3de21e483a42d6492d71557fd30b5b
Author: slokegow <shreejith.billenahalli@theice.com>
Date:   Thu Dec 15 14:19:51 2016 -0500

    amc: compute do_trace flag for fbuf correctly

commit d2143af5de47ca66f58f2ae7b596931eda96536e
Author: Hayk Mkrtchyan <hmkrtchy@theice.com>
Date:   Mon Dec 12 10:37:28 2016 -0500

    AB-15668: target:amc: Don't generate code that requires fcast

commit 6c96f5482488a1431ddfc93c0f931c2ec757d875
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Nov 30 11:14:59 2016 -0500

    AB-15529: target:amc: enforce 'unused result' at compile time

commit 67224edda0632c5aba468fdb5cbbd6821e5f2c54
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Nov 25 17:03:27 2016 -0500

    AB-15123: amc: do not allow having reference to object without pool defined.

commit d25aba39f01b30ae64ebedee66a729a63102b032
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Nov 25 14:56:26 2016 -0500

    target:amc: disallow 2 or more "slow" (O(N)) access paths

commit fc3de2090aa7fb8e860eb7a195f54ca0f8a29b00
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Nov 23 16:45:24 2016 -0500

    amc: clean up more ftraces

commit 442952a29e644a7802256fbe2b73a7509ca3928f
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Nov 23 14:13:38 2016 -0500

    target:amc: if ftrace not specified, do not count step cycles

commit b70a551abaab0bb8c0927900033e2184f41632e2
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Nov 18 23:43:57 2016 -0500

    amc: reduce memory usage 2x. speed up 20%.

commit 107f5961f2457cecb7d8c24637ffd9c8b60c60d0
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Nov 18 10:56:30 2016 -0500

    amc: reduce TableId enum.

commit 459dd195140a9c4e36fef14d6d8357cc46b182f7
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Nov 15 14:53:36 2016 -0500

    amc: more compact fconst output

commit b835b21f6070050964a34d02573250849e4d0b85
Author: Ezequiel Fernandes <Ezequiel.Fernandes@theice.com>
Date:   Fri Nov 11 14:58:56 2016 -0500

    amc: do not generate EqEnum if no ccmp for ctype. make ctype-to-enum comparison stricter.

commit 54bf8f4c177f397de79518d6a98c08f894590d57
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Nov 11 11:14:00 2016 -0500

    amc: add table (ftrace) to allow disabling tracing on some fields

commit 6c0a8d8606bfd5d54e4912271597e8d55ab5d865
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Nov 9 09:53:31 2016 -0500

    amc: stricter check for Base field. no bugs found.

commit 3918c1298bfcb0ae4af27239cf760eed463b0886
Author: Hayk Mkrtchyan <hmkrtchy@theice.com>
Date:   Tue Nov 8 14:58:02 2016 -0500

    AB-14818: target:amc: streamline fcmap conversion funcs

commit 07887dfa4318b9a23036aa93ad4e2267eeafa2dd
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Nov 4 18:19:37 2016 -0400

    amc: count # of saved files correctly.

commit a05d06d19c12a0c522273d7818dbdd525ec10e24
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Oct 31 14:25:23 2016 -0400

    amc: SaveSsimfile: use SafeStringToFile to avoid corrupted output

commit fc1f463d3f86305d5638f32e3dafff1748c4f6b4
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Oct 17 12:49:24 2016 -0400

    amc: add -report option

commit 675ebb87e144938e88461c9532f8f4c534ef996a
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Oct 10 19:31:08 2016 -0400

    unit_amc: add explicit test for cppfunc lenfld + Varlen -- it already works

commit d75ab7cc9d6aeb332fbadd10a193b18c3c448a39
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Oct 7 14:56:52 2016 -0400

    amc: check common Smallstr errors, add dmmeta.Smallstr.strict option,

commit fc684b5a897501838089dbb081595ba5e685cc78
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Mon Oct 3 18:12:52 2016 -0400

    amc: make it an error to include Smallstr in a packed type (not wire-safe)
    amc: declare Pkey typedefs.
    amc: generate 10MB less headers by omitting unnecessary whitespace (-170K LOC from headers)

commit c0afc6ab8d7c6327f6d0f1c0edb0d7ba3cb60973
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Sep 23 11:58:22 2016 -0400

    AB-14357: target:amc: Thash Remove: check if item is in hash before removing

commit dd6853b7d30d838f4fe904f80baecc46ee6353bc
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Sep 20 13:49:38 2016 -0400

    AB-14269: target:amc: check pack consistency, fix errors.
    amc: if namespace is (nsx.pack), check that all ctypes are packed.
    amc: if namespace is protocol, require nsproto
    amc: if namespace is ssimdb, require nsdb

commit c874c9ede159082a639fdb51caf5b73fa9528825
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Sep 16 16:55:08 2016 -0400

    amc: compute size of all structs.
    amc: validate dmmeta.csize against computed value

commit f151555b9c8c4e50c7574f2461f5651c0dd9f8aa
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Sep 16 11:55:07 2016 -0400

    amc: fix null pointer dereference if missing msgtype

commit 0444c525f5e70168464c48a6df43bfba6dc24bb1
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Sep 12 11:13:50 2016 -0400

    AB-14097: amc: define symbol 'parent' in cppfunc fields

commit a95d4c3e00303ecc84ef5cedbb936cd3db000688
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Sep 7 13:40:41 2016 -0400

    AB-13960: amc: generate less whitespace -90K LOC)

commit cf76ada3bb2bbcb0c010e46cb638d0e9a25b9b7b
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Sep 6 11:59:36 2016 -0400

    amc: add refdata dependency check. it is an error if:

commit 358dd74d2478bc9786d441b25e2cd7bf70f599a1
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Sep 2 17:47:52 2016 -0400

    amc: check for dependencies between xrefs and disallow them, all except dependency on Upptr.
    amc: process Upptrs first
    amc: when caching xref pointer, validate that lookup path matches.
    amc: catch 6 bad xrefflds which were incorrect but not checked because of caching

commit 80eecc452e6bd03dd9e3092ec0c188e19627a787
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Aug 29 15:37:15 2016 -0400

    amc: uniform trace field naming (remove ad-hoc lowcase name of parent, define function Partrace)
    amc: generate static function for each substr:

commit ead1911c0db0088132b9ba4fd9a50394612d7bb3
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Aug 25 14:25:10 2016 -0400

    amc: use ns comment for -help screen. remove redundant/inconsistent target comment.

commit a25c96e072e22cee62396e18c3da51d36c4e809a
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Aug 22 09:16:44 2016 -0400

    amc: generate user trace fields before machine trace fields

commit d01f4b6980ec6cd5628b3559542e384e45063e4a
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jul 29 14:28:10 2016 -0400

    amc: stricter check for double deletion via cascdel (e.g. Val + Ptr)
    amc: add fnoalias exception list for 8 cases that fail cascdel check

commit ad79721f656f15b44c7d1fa6edc021c8498ef2b7
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jul 28 16:30:36 2016 -0400

    amc: disallow xref + cppfunc (no violations found)

commit 1d78587d790de46b82d2acb98d9c828f6516c2d5
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Mon Jul 25 12:03:07 2016 -0400

    AB-13334: amc: require nsx

commit 464446e66ad19f72402fe52d88e93f8eb296baa8
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Jul 6 10:18:07 2016 -0400

    amc: add reftype prefix check.

commit 34ec2ee47d07e43c9e4d7b3e042f7d13e506763b
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jun 9 11:57:39 2016 -0400

    amc: add CompactTuple print format.
    amc: fix ambiguity when printing bash command line and command has more than one anonfld argument.

commit 2bdba743e90c75524b5e69295fefda46352ea4ab
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jun 2 15:05:37 2016 -0400

    amc: better error message for Val+Finput

commit c3321cf8bcc014263024ac775a675007394ee00f
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue May 17 12:00:27 2016 -0400

    amc: fix null pointer dereference

commit 75bf762cb4586a7d49c3400af7082ab74c03b7bf
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed May 4 10:10:23 2016 -0400

    amc: combine nsx,nsalloc; fold cargtype into cstr

commit 4f7d1e45f3c613fe0a83e477f5bb7c82f26fe019
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Apr 20 10:16:37 2016 -0400

    amc: add Exec.Kill function

commit 3f96fe53074a810ccdf1a3bd2e276420a6fe9f04
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Apr 18 10:46:52 2016 -0400

    amc: don't print bool bitflds equal to default

commit 623f3bf19e50417e386bcec43c65f4957d853641
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Apr 15 10:19:02 2016 -0400

    AB-11095: amc: force fbuf names to be consistent with direction.

commit 9e99a0f9f956024a3a19bbe205768fe22431db36
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Apr 14 18:51:37 2016 -0400

    AB-11072: amc: fix bheap comparison of fldfunc keys

commit 9e29132136e22a82f92ab35245146d105441c91a
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Apr 5 14:08:32 2016 -0400

    amc: limit reflection to necessary fields (-70K sloc)

commit 57f858ca3783da4c8d78a1118df8cdf8bf24593b
Author: slokegow <shreejith.billenahalli@theice.com>
Date:   Fri Mar 25 13:42:29 2016 -0400

     - amc: Add a counter for EAGAIN for fbuf with read:N

commit 7fcca97393a38182400b4c3f1d9f541d83e7d067
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Mar 11 15:11:17 2016 -0500

    amc: better inlining of hash functions

commit 5903eef7145c53592b1cda2fc39da4a815474995
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Mar 10 17:40:42 2016 -0500

    amc: carry bitfields across Base.

commit 72ceb71816b375e514af1bf35302c421e68cb38e
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Mar 6 10:04:56 2016 -0500

    amc: add floadtuples

commit 7e29d1b8b8b0740cfb6e740aebae6692f842b5e6
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Mar 4 10:26:33 2016 -0500

    amc: make field name collision an error

commit 3b72020af3ea26b81c46146e357e3c3ddc4b77c5
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Mar 3 13:21:28 2016 -0500

    amc: add fcmdline table, annotate all command lines.

commit 57a6d11585df0b6dd35c7754bd6ee1f0fcbda144
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Feb 29 19:07:41 2016 -0500

    amc: Thash.GetOrCreate: generate only if impool exists

commit 47b70aa7c1d6bedaf53229e4dcaf731f3c6a8ffc
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Feb 25 16:16:16 2016 -0500

    amc: allow recognize ctype name if no base specified

commit 36bb010adb5b0a2feed4b0448f6634d54c05b974
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Feb 24 10:49:08 2016 -0500

    amc: remove unnecessary _Update restriction on field
    amc: _Update: remove const qualifier (breaks builds)

commit d6c1a14a11560e2d8daa2f2b4a20bcfc75402123
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Feb 24 10:49:08 2016 -0500

    amc: remove unnecessary _Update restriction on field

commit adcd5382eea311af7329ffd046976605c63b7636
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Feb 23 11:08:53 2016 -0500

    amc: simplify bltin
    amc: fcmp: don't revert to Cmp

commit 6d0da991584542515f5430cee47f6d56997ddb2d
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Feb 21 10:17:24 2016 -0500

    amc: account for dispatch/pnew when calculating headers; fix some TODOs; generate less 'print fodder (reduce 120K lines)
    amc: do not automatically include .inl.h in .h -- .cpp file must do that

commit adbebdf6ddd705401c37328331de4a5f85e8226c
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Feb 21 10:03:17 2016 -0500

    amc: Ptrary,Bheap,Thash reserve: Fatal if no memory

commit 2b0fa6650d44e16130f48767c3457386f9b66adc
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Feb 20 18:12:42 2016 -0500

    amc: xref failures are now UNLIKELY.

commit 3b2e4c7adc6714d796d31512daba68beee12d6bb
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Feb 20 16:15:31 2016 -0500

    amc: add support for Opt field which has msgtype but no length.

commit f4b8a46330c3c5122132173442614d3315b181ab
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Feb 20 15:38:16 2016 -0500

    amc: remove safeptr.

commit bac3da4189912aa5131c2fb6f4a848d04006231e
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Feb 20 13:56:29 2016 -0500

    amc: do not generate bck()
    amc: bheap Remove - detect 'wrong heap' remove and ignore (any better strategy?)

commit 4ebadfebd6c69db00802f49d8eb57efa659d6449
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Feb 19 19:19:04 2016 -0500

    amc: xreffld cannot be the same as the field itself

commit 090b4df4810278e32a5a80d442d0153eb4e53d14
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Feb 19 15:23:24 2016 -0500

    amc: more lenient _UpdateMaybe

commit cfbe7a188af08b51780661c826fc453e4e15cfee
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Feb 18 22:31:00 2016 -0500

    amc: Insert: use CopyIn/CopyOut
    amc: Add finput.update -- currently works only if all access paths are 'always succeed', basically means

commit 4eb6a79db4660925f5eac78bddb8fa9da676cfdf
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Feb 11 11:57:18 2016 -0500

    amc: create nsx record for all namespaces

commit f4789364e524c164d88d94410a6c8d9b48ccadad
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Feb 11 10:57:29 2016 -0500

    amc: varlen: hide the field from C++. add accessor function

commit 30d2dece3408f68dd006be7028060fd6bb2d4eb7
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Feb 10 17:10:49 2016 -0500

    amc: count # access paths for GetOrCreate (more) correctly
    amc: implement correct_getorcreate flag

commit 9ecb0e72cda2c2aa9021a04c0a6bc1171f5c4c52
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Feb 10 17:11:26 2016 -0500

    amc: disallow casting between packed and unpacked types

commit 7b78c6c0fde5a6df646b732e59d7634c46ccdb8a
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Feb 10 11:11:51 2016 -0500

    amc: force LIFO lists (stacks), of which there are very few,

commit 04d2330aaeb9b5c2102e79bde208715b5f035744
Author: slokegow <shreejith.billenahalli@theice.com>
Date:   Wed Feb 10 13:12:49 2016 -0500

     - amc: Allocate huge pages only upto the huge_prc specified in modmem

commit a53d599e5a3947ce1e1f9eb551f749aae5554db8
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Feb 10 11:11:51 2016 -0500

    amc: generate cursors for thash on demand only

commit 94f44530fc5485bd4e9455b5d5c53db3f675b877
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Feb 9 12:11:26 2016 -0500

    amc: list all access paths (from same namespace!) with ctype:
    amc: disallow cascdel for a type that has more than 1 pool (ambiguous delete!)

commit 035dd4e3a3b278d302e1c73f8c5a3b6f27325d43
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Feb 8 17:36:59 2016 -0500

    amc: inline tiny bheap iter functions

commit f06c47bf2cdd163702bf44155315a633efd88369
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Feb 5 17:05:49 2016 -0500

    AB-9673: amc: don't generate pointer structs (-30K SLOC)

commit 8ddce446203a40be8cc24b5581ff2fb8d515fe51
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Feb 4 17:13:23 2016 -0500

    amc: make Castbase/Castdown functions of the Base field

commit 068c5640f0636aeaee43c5714e72a0945a1193bb
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Feb 2 19:22:08 2016 -0500

    amc: single LoadSsimfile per ns

commit c9d5d67c79c495760e615a4b2b267685c42cdf32
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Feb 2 12:42:27 2016 -0500

    amc: remove Exec.Reset
    amc: add a Wait call to Exec.Uninit

commit 3cc887b174537ec2da92ad2ca4c457c302ff9ff4
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Feb 1 09:42:50 2016 -0500

    amc: avoid generating unreachable code

commit 151493effa125a3dbf37c6667f9e88d5dc9af877
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Jan 30 14:55:46 2016 -0500

    amc: connect tcursor to tfunc

commit dc1de109068d057f97f09cbb6394ab755a9bcb94
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jan 29 19:33:01 2016 -0500

    amc: optimize comparison of single-field struct

commit 5311e51d15788c98e5cbafb3a81442aa66d28558
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jan 29 18:26:03 2016 -0500

    amc: use raw pointers internally -- no change to user code;
    amc: generate bheap unord_curs on demand only.

commit e2fb5d5d748867f0290b73c5787fc2b5637e0244
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jan 29 12:42:22 2016 -0500

    amc: fix transitive pack check: 242 inconsistencies fixed
    amc: add transitive no_dtor check: 30 inconsistencies fixed

commit d9cf12162e36437da0f17c54a282cc75793b7250
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jan 29 10:10:39 2016 -0500

    amc: dereference pointer in Ptrary.Find.
    amc: remove Ptrary.qFind -- all the places where it is being used are either unsound (cannot prove that index is within bounds)

commit 9685b930d764efee36d6f8cc6e005c54adbc15e2
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jan 28 17:14:17 2016 -0500

    amc: require Upptr to use p_ prefix
    amc: require p_ prefix to mean Upptr
    amc: require Ptrary to use c_ prefix

commit 606670f9d04ac63b7872658aa7fe35c525ab4f7f
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jan 28 13:39:10 2016 -0500

    amc: add nsalloc record -- restrict allocs.

commit ef9c5dd3d8985ab31dafa55d816fcb4eca01c579
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jan 28 09:16:21 2016 -0500

    amc: add Thash.FindRemove combined function
    amc: remove Thash cursor for zero-density hash

commit f7aa01a733c757e7154f16cbe6c4cdb53872fc41
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Jan 26 21:48:39 2016 -0500

    amc: thash: remove FindAt
    amc: tpool: remove defense against access-before-initialize (guaranteed not to happen)

commit c4213e6360915b5c739b3ec7a54a9d53dbd8f9e7
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Jan 25 19:38:05 2016 -0500

    amc: support fcompact

commit 39624cb5948159c47d00a45314c6ab7271ed0539
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Jan 24 12:59:57 2016 -0500

    amc: generate GetOrCreate only when it is guaranteed to succeed.
    amc: Shorten the name for _GetOrCreate,

commit f8d7f36e97cc76aaba0e7fa431be2fe08c052c55
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jan 22 16:04:26 2016 -0500

    amc: remove XrefX, SetDoubleX, speed up 20%

commit 99d207696109953fe0083f7c2f6d98d50c47edfd
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jan 21 16:59:03 2016 -0500

    amc: disable throwing version of extern ctype read

commit 2825b8b5d8099c4fb2eaef89b4cfb94d0472f607
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jan 21 12:48:51 2016 -0500

    amc: do not generate InsertX

commit c9bee0f9b4483529dec2cbcab7b656c411bfef01
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Jan 19 16:08:59 2016 -0500

    amc: add explicit targns link between target and ns

commit ab94cb27517a87cad949d059a06924ebaae199e7
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Jan 19 12:54:09 2016 -0500

    amc: prevent _InputMaybe from diverging if out of memory.

commit e07b84c88f64802e9e6b3b636f0687a45ed02b4f
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Jan 18 11:37:37 2016 -0500

    amc: fix possible corruption in sbrk_AllocMem

commit 0fc4eead4123cfcab1ab3848840e256319ab1330
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Jan 13 15:24:02 2016 -0500

    amc: remove support for problematic GetnumX, SetnumX functions.

commit 4bf56569e08b8a523eeb3a2144759bf5f4c8959e
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Jan 11 11:50:18 2016 -0500

    amc: fix Blkpool reserve to take Max(requested bytes, default blk size)

commit 843d99903358733a76874b411eada8c867364333
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Jan 11 10:07:47 2016 -0500

    amc: fix null pointer dereference in SaveSsimfile generator

commit bbd8b79074c86e99950733d68c8e88e32c75375e
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jan 8 17:19:01 2016 -0500

    amc: complete nsx support
    amc: fix uninitialized memory read when extracting substring as int.
    amc: annotate all generated functions as throw / no throw

commit e4bfad2221d2ee6704ac706ec64f7b51d0d8a626
Author: slokegow <shreejith.billenahalli@theice.com>
Date:   Wed Dec 30 15:57:07 2015 -0500

     - amc: Order cleanup fields with other fields in _Uninit

commit b1086fa6d10806feea6d529bfded74e9d0487e75
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Jan 6 17:32:12 2016 -0500

    amc: use XrefMaybe by default

commit 476eecee68e0eb39f821a98382a807785658ba83
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Jan 6 15:42:41 2016 -0500

    amc: call fcleanup in reverse field order.

commit 65b5b54450924a716cde5bcad52991aeb8be51a0
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Jan 6 12:39:48 2016 -0500

    amc: add non-throwing function Thash.InsertMaybe.
    amc: Remove {Bheap,Llist,Ptr}.InsertMaybe

commit 143d1026ed27231349086011ae26f23e65b9cfb6
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Jan 4 18:35:01 2016 -0500

    amc: add nsx table. initial support (FindX)
    amc: Ptr: Add MaybeInsert
    amc: rename MaybeInsert ->InsertMaybe for consistency everywhere.

commit 94ae0f6d932b113b2439dc2f9831ae4d47455e5a
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Jan 4 18:35:01 2016 -0500

    amc: add nsx table. initial support (FindX)

commit 82bac0a3ba6c2eb8a378f160455f53fb6e56b2ff
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Jan 3 18:17:04 2016 -0500

    amc: add Maybe functions for alloc

commit 44457f70d7200d0707413c788c8afe320c6c6bdb
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Jan 2 17:56:23 2016 -0500

    amc: add MaybeReadStrptr function for some field types.
    amc: create non-exception throwing versions of InsertStrptr / LoadTuples

commit 22d709e83a0226793392ce7faaacc3464a1401d4
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Jan 2 14:43:06 2016 -0500

    amc: fcast: cleanup

commit 25a3855713d69f9f4d8678d9bd68a40d15586d00
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jan 1 18:38:39 2016 -0500

    amc: organize functions by tclass
    amc: move _CopyIn, _CopyOut to tclass:Base
    amc: add fcast tclass

commit 5d8e488db71a0458683c4a25c46e411057f735a6
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Dec 29 18:52:45 2015 -0500

    amc: cmp: remove redundant tests
    amc: generate static check for csize
    amc: get rid of cbase

commit 14c4825f1759770de2cfd75bda008c4845870dea
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Dec 29 18:52:45 2015 -0500

    amc: cmp: remove redundant tests
    amc: generate static check for csize

commit 21e0d8a5ee13203941f189f76cd8829ddd7203ee
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Dec 28 19:01:40 2015 -0500

    amc: remove _FromStrptrX function.

commit 0c245205f558d2789060623404c93030592f188c
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Dec 22 18:18:58 2015 -0500

    amc: gconst: keep original symbol name intact.

commit 3e242e69be8c2d54ffa9d7736f4003d0989ae845
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Dec 21 10:32:41 2015 -0500

    amc: initialize value before parsing (potentially empty) substr field
    amc: add c++ draft pdf for reference

commit 0765b199649f09189551de3f4d589b23f8d31b2c
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Dec 20 16:41:53 2015 -0500

    amc: do not generate llist InsertX

commit 3e4a60a89957f8bca9a162dd6c8ea4bd78625bbb
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Dec 17 09:56:33 2015 -0500

    amc: define proper field.ReadStrptr
    amc: reorganize functions

commit f8b992967e56abc2edf79434ef88c919a3e1189c
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Dec 2 18:30:54 2015 -0500

    amc: don't follow Upptr during toposort

commit 939cc2362912d778d09af5b737a1252770a4f8e2
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Dec 1 14:35:15 2015 -0500

    amc: let client call _UpdateCycles on Callback steps
    amc: move trace counters for steps inside _UpdateCycles

commit 652e039a56ebf0a3fbed613d8f45eabc84f7d6a6
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Nov 30 14:45:57 2015 -0500

    amc: add Start/Wait for Exec reftype

commit 8c5de4ff2700d55fc36d45ee58ff25db2ad29d18
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Nov 11 11:40:01 2015 -0500

    amc: get rid of fldfunc table

commit 211965c768883a750b2f981683f9b25290542a33
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Nov 10 18:46:27 2015 -0500

    amc: add exception records for missing xrefs and make it an error to lack an xref
    amc: add exception records for missing cascdel and make it an error to lack a cascdel if xref is given

commit 85b78d173d6c4414320a5b292b7b264e9ee3f10f
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Nov 9 11:20:40 2015 -0500

    amc: add Exec tclass, implement redirects without intervening shell

commit 38bfcd7f44aa6c08fdb8e24986773f7518149c6d
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Nov 7 17:27:53 2015 -0500

    amc: simplify fbuf configuration

commit 4db55edbbf05d4f6dcda15251a3d6a55ec01a334
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Nov 4 17:59:04 2015 -0500

    AB-7786: amc: print bitfields in tuple form

commit 27e34a31b6bb02313591e30c5674f7b71e46ce3e
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Nov 4 15:34:15 2015 -0500

    AB-7785: amc: check that bitfields don't overlap

commit cc3e01b4ee054ecd809c7a725f39c4f6176957cb
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Nov 4 09:51:42 2015 -0500

    amc: cross check separator length and # of fields

commit b755151c308bb2857bc5dda4f8cb476a725fdf2c
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Oct 26 18:29:36 2015 -0400

    amc: let ccmp generate comparison ops for Smallstr

commit bcd87b4130680c98793d2027dcbbbca9f1b9d36d
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Oct 26 15:01:01 2015 -0400

    amc: remove gstatic.filter feature

commit 637a4c3967db8a6c68b669dd34641ec4a40a1fb4
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Oct 25 11:56:36 2015 -0400

    amc: rename reftype:Extern ->Global

commit bfc8cf78bbeb7447e84d82d28ce311149adbd0f4
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Oct 23 14:33:56 2015 -0400

    amc: fbuf: for byte buffer, return NULL if no data (consistency).

commit 079dca8962f589eb96a4044aed385eb634e2a504
Author: Jeffrey Wang <zwang@ckd-dv-aeac-01.ckd>
Date:   Fri Sep 25 11:53:13 2015 -0400

    AB-6512: target:amc: Do not print field if pmask bit not set

commit c3006bf2ccf17009405bfa92df7848a33761c67e
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Oct 17 17:59:37 2015 -0400

    amc: do not generate Print or Read for Inlary
    amc: separate print for Dec into a function

commit 11a726d98f677f2df777981b558930195fb5b7ec
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Oct 15 21:51:49 2015 -0400

    amc: Tpool: initialize next pointer later to avoid coverity warning

commit c785d689d6a53b0be31fd0363aeb512af1f1299d
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Oct 14 18:30:19 2015 -0400

    amc: parse fdec types as themselves (not as double)

commit 6f4d029ccbae30a1f25684b5dc88ed0d7d973845
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Oct 6 12:37:41 2015 -0400

    amc: add Stream reftype

commit 5442ec9dc7b8543b6ff453338b84d45dcdc753c8
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Oct 6 18:59:48 2015 -0400

    amc: add dmmeta.Disptrace.cycle attribute

commit fbd05a57e1ae1b09d0422b48fca5ccec781cd0c2
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Oct 5 18:52:33 2015 -0400

    AB-7210: amc: detect non-unique ptrary + cascdel (1 error found)

commit a1f9796cdeeaf6e037e0d4be45c7dcb6e5e05ae5
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Oct 5 17:49:57 2015 -0400

    amc: print verbose-level warning if cascdel required
    amc: uninit order: cascdel, unref fields (reverse order), user-defined cleanup fields (reverse order), uninit (reverse order)

commit 77fa629222af12e8829606d5033ec65dd27ad9dc
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Oct 5 17:14:35 2015 -0400

    amc: make it an error to not have needed ptrarys

commit 274c61331d992ded1dcedc5bd9f460cc9aabc564
Author: Ezequiel Fernandes <Ezequiel.Fernandes@theice.com>
Date:   Thu Oct 1 11:33:04 2015 -0400

    amc: Add qOrBitVal function to Bitset

commit aa0b42caa97ae1b12abc38cfe7c0ccd311334523
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Oct 1 18:19:06 2015 -0400

    amc: inlary: don't generate _ReadStrptr if cfmt not specified for parent ctype (-3K lines)

commit 2729270436fe847148bba36f654c26ae6dc0c67d
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Oct 1 11:00:07 2015 -0400

    amc: better basepool warning

commit 7f0fe35d4c55b6007de7ec6f7e68abd2344e85a8
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Sep 30 16:09:59 2015 -0400

    amc: add BitsEmptyQ function for bitset

commit 69a1d2f35447962ff8c3e8d81fec90192eb8cbea
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Sep 28 11:55:29 2015 -0400

    amc: implement ptrary membership flag
    amc: mark ~380 x-referenced Ptrarys as unique (only 4 remain without check)

commit 2b67916248fc21f077311422e35c55e92cbd870a
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Sep 18 17:40:32 2015 -0400

    amc: do not automatically generate cast-to-enum operator whenever enum is defined for a field

commit 25558c8f886471d8dfe21f934af3d9aff668af7d
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Sep 16 18:24:56 2015 -0400

    amc: verbose-level warning if Thash,Bheap, or Llist comes without xref.

commit 2d3d353986c21b7f0c002306b9a915c2a1f73de0
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Sep 15 13:13:47 2015 -0400

    amc: add tclass Ibendpt + fnotify.

commit ff742a74677dd9609f4fd5f1c507bff2df2d8df1
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Sep 14 12:10:17 2015 -0400

    amc: add cbase restrictions: disallow construct with msgtype in child & parent.

commit 51d10236bafcdd198be376eb2754d5119dca16e4
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Sep 8 21:49:48 2015 -0400

    amc: generate traces / cycle counts for timehooks

commit 5a1054738785cf51b43268c840572acdfdd5888e
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Sep 2 11:47:04 2015 -0400

    AB-6652: amc: tpool: mandatory check for double free

commit 43452b6d36bf2f8757bdbf7e6cb5d45bf749e4c0
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Tue Sep 1 14:58:41 2015 -0400

    amc: generate Cmp function for field/ctype
    amc: add tclass/tfunc for Ctype generators

commit 77c11bf96ee5b651235caede4ee42b057a6b3081
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Tue Sep 1 14:58:41 2015 -0400

    amc: print struct-wide functions near their ctypes
    amc: optimize comparison of multi-field structs
    amc: add tclass/tfunc for Ctype generators

commit cc46363a2c0d6a8f72590e35c8bf0b199a5f9b55
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Aug 31 11:51:12 2015 -0400

    amc: disallow copy if any x-references present in ctype.

commit 635fbc6d6f6ac848dc3cde47ff9c8b160d85ef14
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Aug 30 12:45:43 2015 -0400

    amc: bheap: do not require cheapcopy for key; use _Lt function; use dmmeta.fcmp.
    amc: fcmp: eliminate _Gt. Code must use _Eq or _Lt
    amc: .Lt: optimize comparison of last field.
    amc: gsymbol: delete; always generate compile-time symbols for Inlary + gstatic combination.

commit 61aff13d78ea0600c5a9b311123d964f45cc9900
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Aug 25 17:53:01 2015 -0400

    amc: fix unused variable warnings (no changes in generated code)

commit aea4b7776bc7de6c5643c49f029a4aebdbbdc326
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Aug 24 11:58:28 2015 -0400

    amc: add 49 tfuncs

commit aa71a1b8f6d12ff92565cc7cd6d53e460e66d6c0
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Aug 20 12:32:08 2015 -0400

    amc: make Main function per-namespace.
        amc::Main(algo_lib::_db.argc,algo_lib::_db.argv); // dmmeta.main:amc

commit 4eb16a1300b6162be38fa7b36bcf48d07351d014
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Aug 17 14:12:21 2015 -0400

    amc: move syntax/help strings to own namespace (they are never needed from the outside).

commit 67d5c0c8c5acac69848d36ae167e89e234ab60a6
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Aug 9 17:40:08 2015 -0400

    amc: disallow copying of ctypes that contain non-copiable fields.

commit 538538e11951dc01752268fd7d8e751c6972e5ad
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Aug 6 22:25:56 2015 -0400

    amc: speed up query mode by >10x.
    amc: speed up gen mode by 30%

commit 0a8829bcd1d07edc49d99faebb6f94c3822295f7
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Aug 3 15:18:05 2015 -0400

    amc: dispfilter: generate combined dispatch/filter function

commit e5f25cef0d13710e550104ea86c1c2a6849d3473
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Aug 1 11:48:22 2015 -0400

    amc: extend pmask support to RegxSql field. support partial regx (dmmeta.fregx)

commit 54ff4cb6b4c2b731ed9ec6846823599d96a267e5
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jul 31 17:37:12 2015 -0400

    amc: add _GetInt function, _GetScale function for dec types.

commit b710d94325cd2e691c9058ec601fb7e61cabbf62
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Jul 28 11:18:57 2015 -0400

    amc: fix initialization order problem in algo_lib

commit 1091c69ebf80baba9f5b8cffbd0e54660f176cfb
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Jul 27 17:52:27 2015 -0400

    amc: add RegxSql type.
        atf_amc::TestRegx1 test_regx1;
        atf_amc::TestRegx1_ReadStrptr(test_regx1, expr);

commit d5ea7b478339f2b77bb697c400d544d76dcf4543
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Jul 27 17:17:28 2015 -0400

    amc: Sort order of functions generated by Smallstr.

commit 5a8f363bd3ae2fce97af9fc1809e1c5edc3f8ac4
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Jul 26 15:27:07 2015 -0400

    amc: enumerate generators using amcdb.gen, amcdb.nsgen tables

commit 81489c7d74b29f6654683797da30cbd86eefbdf2
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jul 23 15:35:34 2015 -0400

    amc: fix crash on missing thash record

commit e6bc06bb296df47384f15e985128fe5a74a93817
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Jul 21 15:29:58 2015 -0400

    amc: add MsgbufOut fbuftype.
    amc: add output fbuf epoll support -- doesn't work with VMA due to lack of dup.
    amc: add optional input fbuf support. disabling epoll at CG_AX to see how that works.

commit 78a09698cb0720c37a8696c306b9f6e79830ad02
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jul 17 15:51:38 2015 -0400

    amc: allow substr with empty expression

commit a716177e9b860cddfea547db58146f5ff4ce93d7
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Jul 5 21:22:35 2015 -0400

    amc: generate more compact code for printing.

commit a699432425deea7bc51daf5b7933188cd214f966
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Jul 4 15:34:31 2015 -0400

    amc: align #include blocks in output files

commit 5978020b5932216fa4340c260e8ef87d2758f049
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Jul 4 13:42:56 2015 -0400

    amc: _N for varlen field: do not return negative number.

commit 9ce27241d704612be2c6df5f81d55589ccfc295e
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jul 3 13:04:36 2015 -0400

    amc: fix dmmeta entries for various built-in types; fix some inconsistent declarations in include/*.h
    amc: generate global namespace (include/gen/_gen.h, cpp/gen/_gen.cpp)
    amc: normalize function key: it is ctype.field.func. Where necessary, insert extra dots

commit 9afd523998bc023928ee9d07f6a9a87815c52c80
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jul 2 19:06:33 2015 -0400

    amc: no functional change, sort alphabetically generated functions

commit e10c4d38b26e32b820e72dadefd6ed52336bfbcb
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Jul 2 16:02:40 2015 -0400

    amc: no functional changes, generated functions for

commit 015d5615967d2bff74c19ae4c5ebcec030edb56f
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Jul 1 18:36:47 2015 -0400

    amc: No functional changes -- generated functions are now sorted alphabetically

commit 237617aeb1e5c9deadebcd8d014e817c44f1bec0
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Jul 1 10:58:33 2015 -0400

    amc: collect reftype funcs in a table

commit 28a599b5c98dcfcbaaf3776ae027143e77c1da72
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Jun 30 16:04:45 2015 -0400

    amc: trim unnecessary forward-declarations.
    amc: add ctype comments to forward-decl section

commit 0bb5f9f0e9c333caf707aee6de2e5531930c5b24
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Jun 30 14:33:47 2015 -0400

    amc: generate cursor for varlen field

commit 7c2a1a248df94307bc077c1075c132c7d3500978
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Jun 29 15:11:41 2015 -0400

    amc: avoid generating case type where unnecessary.

commit 3876a4b3f890ff6ab14937fe5f9651139eac8309
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Jun 29 13:02:37 2015 -0400

    amc: reading dispatch: return bool.

commit 32e01fa5972ddb93677df172fe975d1b1dd0962f
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Jun 22 16:34:57 2015 -0400

    amc: Tuple _ReadStrptr now returns bool if head was found.
    amc: print warning if short form tuple is read.

commit 1ebe45ced50d6a0c640881ddd2497145d451b0d7
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Jun 17 10:19:20 2015 -0400

    amc: add field order check for basepools

commit a46d35006451f2f9719692fbe8860873265cc0cc
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Jun 15 18:13:04 2015 -0400

    amc: add regex filter for gstatic

commit 12bafcac258ded151d7afb0039ae49fff1dcdf36
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jun 12 10:12:54 2015 -0400

    amc: rewrite Pkey into Val fields as a preprocessing step
    amc: detect Pkey chains pointing to Tary or some other unlikely type. Found 1 bug.
    amc: attach steps to Reftype code generation, catch assignment of tempstr -> strptr error.

commit 07fef2cbf765edd025135da5542235c2d283f0f7
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Jun 9 20:39:09 2015 -0400

    amc: stricter check for Upptr xreference, update error message

commit c7ef94a086f268b46e74bdaccc28d5aa9a660fa5
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jun 5 19:18:31 2015 -0400

    amc: remove random access from bheap cursor
    amc: add unordered cursor

commit abe1cf65c3989237e127736b3682c281a155058a
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue May 26 12:47:20 2015 -0400

    amc: throw exception on bad read of Opt field

commit 1b262c44aac1b5777c252eb8038d5c4485667e42
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat May 23 18:53:37 2015 -0400

    amc: add xrefvia consistency check. remove 131 redundant xrefvia records

commit 8e3dde9ba3f76002898d7e3becf60d40a0c47676
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu May 21 12:43:13 2015 -0400

    amc: generate dispread,dispprint for every message header. support recursive read,print.

commit f89337e66e874d99219ea2d598f6bccc17e794b3
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue May 19 17:12:14 2015 -0400

    amc: do not crash if missing msgtype for ctype in dispatch_msg

commit 6620477dcf31a2a08e0ac01ecbebf0188e3dbd1b
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon May 18 13:57:54 2015 -0400

    AB-4717: amc: support char fldfunc

commit b17c07c3b8cfc0ce7f7c618d5fb2aa6a502adfa3
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri May 15 22:54:19 2015 -0400

    amc: standardize on one symbol name designating message type,

commit ba6f1639258a16dbecd3f116e54258e01387e8ef
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri May 15 10:46:19 2015 -0400

    amc: don't generate unused function

commit a0a01df5d86cfc5a57558914606f4e7ccf40354c
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon May 11 14:59:25 2015 -0400

    amc: upgrade traces to 64 bits

commit 4fa2f0a7894aa4b6e28781bf96203222a19f473d
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat May 9 15:08:16 2015 -0400

    amc: emit comment with -help:

commit 651b8b2de13fcca90b27f8defb4dad394c9cab22
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri May 8 17:00:24 2015 -0400

    amc: Tpool: over-allocate less memory

commit b560672cbfefd2180be86d6cdc4c5cd6675ad3fa
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed May 6 10:44:07 2015 -0400

    amc: allow Upptr to be hashed
    amc: skip destruction of Inlary in FDb Uninit
    amc: cppfunc lenfld: use _Get, _Set; fix compile break on _Get

commit 550d7b1c421600572c90f974b3c0c7c929095f3f
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun May 3 10:35:29 2015 -0400

    amc: if both cascdel and cleanup specified, call cleanup first, cascdel after.
    amc: move global initialization step into main().

commit d30496b9d6f868dfb5655651db61556753a7bbe0
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Apr 30 15:27:49 2015 -0400

    amc: generate c_RowidFind for global Tary
    amc: Rename UnTimePrint -> UnTimePrintSpec where necessary to remove overload.

commit c420046709d3fb0fae6379957bd6e60d1d0b9c26
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Apr 21 11:24:25 2015 -0400

    amc: allow Upptr to another namespace.

commit 8fd771529c3b1aa3e2c832f268eda4f608c982d4
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Apr 14 12:01:06 2015 -0400

    amc: don't write blank lines when generating field initializers

commit b14c4cd9b4e8d71bcae17b8f63250cbe8cec8713
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Apr 12 16:53:52 2015 -0400

    amc: Ptr Insert: ignore insertion of same element.
    amc: Ptr Remove: ignore remove for unrelated element.
    amc: Bheap Insert: ignore insertion if element already in heap.

commit 283da86d1f6eb3c9b4b52bf241e9a3a24b6f0e84
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Apr 11 17:20:25 2015 -0400

    amc: allow use of fldfunc field with gconst

commit 4cb5e92ec8ff75a957e7cc4ec576f2d60882affd
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Apr 1 17:10:04 2015 -0400

    amc: add gstatic.isstep

commit a91dd9c7a22050b9b22595477fc25d31dd9d4444
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Mar 27 11:05:31 2015 -0400

    amc: add exact check for integer fields.

commit a56247c229bc787c3b70dff7399a9bfbfc28ab05
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Mar 25 13:24:52 2015 -0400

    amc: don't read msgtype/length when reading from from Tuple
    amc: some fixes to make dispfilters work.

commit 45a2158eee81352c401adf1a4e059d019bc50186
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Mar 26 17:05:51 2015 -0400

    amc: merge fcmp,csort -> fsort.

commit d49c6cc0a6090bef2bc2baa02cee939e0057c0de
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Mar 26 11:43:45 2015 -0400

    amc: implement presence mask via bitset
    amc: fbitset: support bitset over a single field.

commit b4782bc129d115d3200b00e76259636b1de5404c
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Mar 25 12:56:12 2015 -0400

    amc: unify cread,cprint -> cfmt.

commit ffc08c74b0e191a6d6636e361bc5dca620be4036
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Mar 23 19:24:18 2015 -0400

    amc: explicit initialization of cursors with _Reset.
    amc: use Attr_curs to read tuples

commit c927e5753d56a33ece9a47e3da1e67ac79eca3fb
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Mar 23 13:48:16 2015 -0400

    amc: refactor of Lary cursors. Do not implement sparse mode

commit d64b7756848f5bd4ebfb0df8d2a97a80885dd622
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Mar 22 11:38:03 2015 -0400

    amc: add usertracefld table, to allow specifying user-defined trace fields.

commit 82fe77ae9689613ff6ddd3e4e1069132b9b4279e
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Mar 20 13:49:01 2015 -0400

    amc: add RemoveAll for Fbuf

commit 440a0c4d8ec588d81944ccc8ab89047224d722f7
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Mar 18 00:33:53 2015 -0400

    amc: Blkpool: cache-line alignment doesn't help, go back to 16-byte alignment
    amc: fix corruption bug in Lpool realloc when reducing size (was never used)

commit 0d3b76a7be0222620d9a52913d3428f34c596940
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Mar 17 17:57:25 2015 -0400

      amc: Blkpool: aligned returned blocks to 64-byte boundary.

commit 3057f157732b4d7dc35f882d91ede49ab032999b
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Mar 16 14:59:17 2015 -0400

    amc: remove gconst from signature check -- fconst already captures it
    amc: do not comput nssig, ctypesig -- not interesting. only compute dispsig,

commit 3eb289235814ba274a351cd2774b9b30f72d6b5b
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Mar 16 12:59:55 2015 -0400

    amc: fix 128-bit bit field handling.

commit b2bb17bb812194de33bc5c8817d848cfc7310097
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Mar 15 21:34:21 2015 -0400

    amc: 128-bit bitfield support

commit a25ea5c2cd9b9d1ce260920112fa9c51aefd8ca7
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Mar 15 10:39:46 2015 -0400

    amc: disallow using ctype as its own base.
    amc: set next pointer for Thash elements to -1 when not-in-hash -- avoid corruption.

commit 8c440a69c335faedd6272462dd526d668882e5f0
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Mar 14 15:00:16 2015 -0400

    amc: require that all fields of a type which is marked cheapcopy are

commit ed79ced1c1fb00039bea98896eb9335f49a181aa
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Mar 12 18:03:27 2015 -0400

    amc: add Count xref type

commit 9c870c910289eaccbea77787fb6ae2bbcecfd78d
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Mar 12 11:39:08 2015 -0400

    amc: stricter reference validity check
    amc: use field search when computing reference.

commit 85bc155031d6fee1d6f979724878b4d38b80fd5c
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sun Mar 8 12:10:43 2015 -0400

    amc: print readable error message when base/derived ctypes have a name collision.
    amc: print/generate a more readable error message when thash key collision occurs

commit 27164fa55424106a3858b1939915649df280a38e
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Mar 6 10:50:16 2015 -0500

    amc: initialize new elements in Tary with field.dflt.

commit 96cdad20da873b797ed7de4478aa76ab757a0926
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Feb 28 17:09:19 2015 -0500

    amc: Add function to set Blkpool buffer size.

commit 32b0c6f2259e8cf942c92c414e796406dc8fd058
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Feb 25 01:45:32 2015 -0500

    amc: add cprint attribute 'fmtarg'.

commit f458a1c8b079f2b408857eb577acc5ea4ceb9260
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sun Feb 15 21:30:40 2015 -0500

    amc: do not generate traces for non-global Tary, Inlary (not interesting).

commit f953d9d01f1d629ba8c7daa0884eefe0302bf6c9
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Feb 13 15:59:21 2015 -0500

    amc: increment correct alloc/del traces for Lary, Tary (2100 lines added...)

commit 09e21da5ce322ce7a10a69beca69488f28b21f9f
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Thu Feb 12 16:52:57 2015 -0500

    amc: generate code for cstring (Tary of char)
    amc: add traces for malloc, free. These appear as
    amc: fully qualify calls to _AllocMem, _FreeMem.

commit 4534ec554370bfac3a571cc145cc427d6bc05aa6
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Tue Feb 10 12:19:32 2015 -0500

        amc: .testgen -> temp/gen -- not done

commit e7623a9bfc1124d3db5e13b8a2a8b95550cd4467
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Feb 6 18:39:20 2015 -0500

    amc: generate own main() for each target.

commit ed45036c27ff1ac25ebea55f6e0231b5949f1bab
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Feb 6 18:03:02 2015 -0500

    wip: amc: cleaner header with fewer sections

commit 83bcb5c6c431cb258715ce3ae18222d391f586c6
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Mon Feb 2 11:58:41 2015 -0500

    amc: throw error if pmask is too small for the fields.
    amc: support 128-bit pmask.

commit 9a47ed7e0df4a0d42551f84ef65369ae8463ddc1
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sun Feb 1 23:15:21 2015 -0500

    5. See cpp/atf/atf_amc.cpp, unit test amc::TestPmask for the various cases.
        inline bool value_PresentQ(atf_amc::PmaskU32& parent) {
        inline void value_Set(atf_amc::PmaskU32& parent, u32 rhs) {

commit 3d6e7cf8a7fa60ab2030c3f83439093904ed8bc2
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Jan 30 23:16:29 2015 -0500

    amc: rename _qSetdouble -> _qSetDouble
    amc: add _SetDoubleX, which does a range check:

commit 88ed34d05d1de33a08edfa242f1f89d623795b81
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Tue Jan 27 16:26:49 2015 -0500

    amc: mark buffer readable on error

commit 00a3748faa7da82f46ca834902922da87bcf711f
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sun Jan 25 20:20:37 2015 -0500

    amc: output tracerec, tracefld records

commit 7b7b7c4ad8c119b6f7be15476344e262c4212b3a
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Jan 23 18:04:37 2015 -0500

    amc: generate correct print statement for FDec types

commit 170e0575f39d5ed71474d312616f8cb7ad2e06d5
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Tue Jan 20 21:03:12 2015 -0500

    amc: dispatch read: do not do unnecessary work; add unit test.

commit 7d0be43bedad5826142007190df0575e05d9d467
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Jan 17 19:24:59 2015 -0500

    amc: do not skip inline functions in query -proto mode

commit 5d4b535eae8d1b30b61db770632a7e1ff4838b7e
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Jan 17 18:37:52 2015 -0500

    amc: convert all Fixary instances to Inlary
    amc: remove Fixary table.

commit bbd72bdbdd1749896056e5275c3dfba50e1b9e8e
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Thu Jan 15 09:20:20 2015 -0500

    amc: rename amc_index.cpp -> amc_xref.cpp, organize functions. no functional changes

commit b00f586d7c56b3894b00fa15d96cfa6618fad491
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sun Jan 11 19:29:20 2015 -0500

    amc: partially inherit -verbose, -debug flags

commit 98d72f21083a71137e5620d25c7e9d6ade135703
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Tue Jan 6 18:42:41 2015 -0500

    amc: allow fldfunc for Upptr.
    amc: disallow dflt for fldfunc.

commit 4b14508e435f7e4e4882795715b159d9bf27b0c2
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Mon Jan 5 14:26:36 2015 -0500

    amc: use ffunc for dispatches

commit 7731ac7f5c14fa4279f282060aa5c8bef9522255
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Dec 20 15:01:07 2014 -0500

    amc: hide Opt field to make it inaccessible.

commit 79fecec0c4cc16fe1ffe448f4cb16011dec55656
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Dec 19 18:10:09 2014 -0500

    amc: fix uninit order bug for global FDb.

commit c8610f9492f4dde94e4a4871ba72d97b773021bf
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Dec 10 17:31:21 2014 -0500

    amc: add '_elems' to varlenfld, to make it more obvious that the name of

commit f40a5fedd2b3c46f8144c2a845132cb37503a8ae
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Dec 10 09:43:27 2014 -0500

    amc: more u64s, fewer bcks in Lary logic

commit 0517f287eb6e1d85879c95456bbae15e037ec82d
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Nov 29 23:00:40 2014 -0500

    amc: add cmd_ToCmdline shortcut function, which returns a fully functioning command line:
    amc: make all trace structs packed
    amc: generate imdb reflection.
    amc: Val,Opt,Varlen are considered pools, and participate in reflection.

commit afd875984bb3b97466156c4cd1fa111f4acc342b
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Nov 29 18:48:58 2014 -0500

    amc: automatically calculate necessary set of includes for each file

commit 230d82d0d8de3a6887a874131799e8d9114498c6
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Nov 28 18:52:56 2014 -0500

    amc: don't generate trivial _Get functions -- only big-endian, bitfields, substrings

commit 81d401a64d6e3037e35a9c20246bf08dbeedacce
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Nov 28 18:35:55 2014 -0500

    amc: use dev.targdep to figure out which databases to instantiate in each executable

commit 0124a96d195cb333e4ec4b9af2b951f957d156a0
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Nov 26 16:25:01 2014 -0500

    amc: generate correct gconst for char types

commit bdd4a97dd5fdf82c5980e82e02833212d367a082
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Tue Nov 25 15:57:23 2014 -0500

    amc: generate Dispsigcheck for namespaces.
    amc: load dmmeta.dispsigcheck on input.

commit b5d9257cf2f0677223579401023137d0391218c1
Author: Alexey Polovinkin <Alexey.Polovinkin@theice.com>
Date:   Tue Nov 25 08:40:31 2014 -0500

    AB-2186: amc: first workable version of dispsig check

commit 236eabbb141d4e507bb85655d9f9329e5adcf1db
Author: Alexey Polovinkin <Alexey.Polovinkin@theice.com>
Date:   Fri Nov 21 05:56:46 2014 -0500

    AB-2186: amc: include stats for saved ssim files into n_filemod

commit 048fe0e78e6e6a78c9c0add1436b2e33dc9378fd
Author: Alexey Polovinkin <Alexey.Polovinkin@theice.com>
Date:   Thu Nov 20 06:43:55 2014 -0500

    AB-2186: amc: generate nssig, ctypesig, dispsig

commit ec3c4f1de0ac7f3906df3ce2b64395eb16646561
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Mon Nov 10 12:16:36 2014 -0500

    amc: add disptrace table to select which dispatches to trace.

commit d485710bd28618a55e28e447573436bcc24e3092
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sun Nov 9 18:08:46 2014 -0500

    amc: generate explanatory comments for Steps

commit 5de2f1473af6cd3e9d9766d3a232266b66a2cee3
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Nov 1 10:44:30 2014 -0400

    amc: automatically create ccast record for a single-field non-string ctype.

commit 7e9470e02c17b3b2791936984684cb224796da88
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Oct 24 15:28:48 2014 -0400

    amc: generate reflection entries for ctypes from another namespace.

commit 6bf8dead559979e02bcc5aaf3ea225479f26a2b3
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Thu Oct 23 16:15:25 2014 -0400

    amc: if tuple finput is non-external, inline the one-liner for readability (saves ~6k SLOC)

commit ac8f03ec8b09dea2846f5f0e36b6dd6945d41c77
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Oct 22 17:00:56 2014 -0400

    amc: do not print type/length fields

commit 693a39c631f8b8024d24447718c6d9236d20387b
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Tue Oct 14 15:23:21 2014 -0400

    amc: generate per-field Step function and aggregate into ns.Step.

commit fc55e8c4ae48ac250b392ca6ea0b8931f2743bf6
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Mon Oct 13 21:21:59 2014 -0400

    amc: fix null dereference (when printing from empty namespace); query mode: do not print functions marked internal

commit 2da2b3949adabe6fe38978e2337dffa79feb49a0
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Oct 11 19:39:21 2014 -0400

    amc: Require buffering type to be specified explicitly: Msgbuf,Linebuf,Bytebuf

commit 532a0191966bc66754059887b35d21c7f38e52bd
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Mon Oct 6 11:49:00 2014 -0400

    amc: add listtype table

commit 194fd1e316f64e67cf2c89f1eeeca3b8ab964742
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Sep 12 15:35:10 2014 -0400

    amc: _Print: always include type tag if nfields>1.
    amc: report # functions,ctypes,funcs generated in query mode.

commit 2f9c7b5e68c8dbae8844f784acca5ca86fe72822
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Sep 6 23:28:50 2014 -0400

    amc: simplify query mode; print enums (e.g. amc dmmeta.ReftypeId); add prototypes (-proto); acr_ed: add -bigend,-anon

commit 1be08869d938b57ef0e926b279f6a3dcc45506b4
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Aug 30 11:50:17 2014 -0400

    amc: fix string limits in numstr reflection.
    amc: add malloc-less conversion string->Ctype (ReadStrptrX, InsertStrptrX).

commit 5c02f55b447040a498e50502ad252cd50be4ecd1
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Aug 27 20:19:39 2014 -0400

    amc: don't pad strings if ==1

commit 0f84fca16f72d1224f8ef77f2b0a985dd3fe6534
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Aug 20 21:47:18 2014 -0400

    amc: some hacks for pnew to work around linking problems

commit 5de3723b4b9f764dee41462512a0f448088a71b3
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Thu Jul 31 16:36:56 2014 -0400

    amc: allow reading tuple using ctype's name

commit 02f62b2bb9afc5a8c3f607528d540661c0b9cb92
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Jul 16 23:34:35 2014 -0400

    amc: allow numeric substr (e.g. acmdb.Processor.core)

commit b3f820440e665e180fead3bc57aa803e71ab4e3f
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Jul 16 22:29:39 2014 -0400

    amc: allow signed big-endian types; rename fendian -> fbigend

commit 9d41879fc45a89f1317fc6ee5570398d11315de3
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Jun 25 12:59:15 2014 -0400

    amc: add FmtStream for messages

commit 0d37196ffb19d78d010295c587f5527597db8a33
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Jun 20 23:25:05 2014 -0400

    amc: edge-trigger reads.

commit 69451466b802b87b0ec098aea7a392a1e5cc985d
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Jun 7 20:02:59 2014 -0400

    amc: change gconst key to allow more than one enum

commit 9a18c119d14e4f1e6609ed8b1d379e92c42f8c17
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Tue Jun 3 11:10:07 2014 -0400

    amc: remove global zs_cprint / zs_cread lists, use Ctype.zs_cprint / zs_read.

commit 052abc75ec6a360d0b3929ecae4805a224db06e8
Author: Alexey Polovinkin <polov@algoengineering.com>
Date:   Mon Jun 2 10:08:25 2014 -0400

    amc: migration to cread/cprint - done

commit 860c7bc47119378ca4ac3d66a09c3b63aebf3237
Author: Alexey Polovinkin <polov@algoengineering.com>
Date:   Mon Jun 2 07:53:24 2014 -0400

    amc: migration to cread/cprint - continued

commit 9034787086fcfc8aaee227380e22ba7579fd7451
Author: Alexey Polovinkin <polov@algoengineering.com>
Date:   Mon Jun 2 07:44:38 2014 -0400

    amc: migration to cread/cprint - continued

commit 920c5e6232d8dd06fa82af36e2c9268e6ad5b50a
Author: Alexey Polovinkin <polov@algoengineering.com>
Date:   Mon Jun 2 06:03:03 2014 -0400

    amc: migration to cread/cprint - continued

commit ec9688060e93869b96c9a343cacc28379552057e
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri May 30 18:03:25 2014 -0400

    amc: main loop won't become infininte loop if db.clock modified from outside.

commit 44fda321244f1b8020f57fd7c373fb3b90f62347
Author: Alexey Polovinkin <polov@algoengineering.com>
Date:   Fri May 30 11:49:24 2014 -0400

    amc: migration to cread/cprint - continued

commit 221428cb62de497f982890cc5d2956fa5eea037c
Author: Alexey Polovinkin <polov@algoengineering.com>
Date:   Fri May 30 11:25:26 2014 -0400

    amc: migration to cread/cprint - continued

commit 13244e0df5acaf39e357dc24ad3ffd1948780585
Author: Alexey Polovinkin <polov@algoengineering.com>
Date:   Fri May 30 08:58:37 2014 -0400

    amc: migration to cread/cprint - started

commit 2cdeb8fa32c249909fba46aa7271dfd8d73fdf98
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Thu May 22 16:37:18 2014 -0400

    amc: not an error if LoadSsimfile fails.

commit f609280dc407f368c3517b149841356f50f052a6
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed May 21 20:18:19 2014 -0400

    amc: fix off by one in testcase generator

commit 225f1582d955790140a601e3a76090e92a1001dd
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat May 17 19:53:15 2014 -0400

    amc: add foutput support

commit 2b9f756ed190da92fb62dedbbaa6c01d04768313
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Thu May 8 16:14:11 2014 -0400

    amc: add -diff mode.

commit 58afc199726cc909967a0ebc58d052284ae04f62
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed May 7 22:30:18 2014 -0400

    amc: fix dispatch generation with big-endian message type

commit 2d1cd181c226e6e2be101a0866a36abdc5991f61
Author: Alexey Polovinkin <polov@algoengineering.com>
Date:   Fri Apr 25 06:00:40 2014 -0400

    amc: fix big-endian msgtype for pnew

commit 71cb1a335d203dda35ad96b6c88b0fa74eab9ce4
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Tue Apr 22 11:27:43 2014 -0400

    amc: fix cascdel of Ptrary.

commit ccca13ad749244fddac269953e27cc5e98b30f5d
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Apr 12 18:00:14 2014 -0400

    amc: generate compile-time string constants for string-valued enums.
    amc: align each enum block separately to prevent global changes when width of one enum changes

commit d1d0984fdc989341a63b627aa282d168c3e83102
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Apr 5 11:59:38 2014 -0400

    amc: generate thread-safe code
    amc: better comments

commit 158a7d4160498d33c08f5428f9658b466dd0a3b0
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Thu Feb 27 18:55:18 2014 -0500

    amc: detect Pkey applied to bltin type

commit 70626355713e1d25d1080e300987255043289d87
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Feb 22 11:25:39 2014 -0500

    amc: fix "unused variable" warning in FreeMem in release.
    amc: fix "dereferencing of type-punned pointer will break strict aliasing rules"

