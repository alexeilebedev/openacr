## ACR release notes
<a href="#acr-release-notes"></a>

commit fd4e3e9e92da625d46253a9d13f1886454163331
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Apr 26 08:51:30 2019 -0400

    acr: don't attempt to validate cppfunc + funique fields

commit e293e7c3108fce048789f48be273d2f3b986b425
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jul 27 16:49:26 2018 -0400

    acr: eliminate linear scan during record printout. up to 99% speed up in some cases
    acr: eliminate excessive calls to c_child_Insert (40M -> 200K for 200K records)

commit b60cc638fec68f165d4aeaa5f0aaf6757dc452fa
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed May 16 09:05:52 2018 -0400

    acr: 10% reduced memory usage on small queries; check length of comments / other fields

commit 53cebc9430ca6e9ef744eef8b697cbe14880bcf2
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Feb 27 13:23:33 2018 -0500

    target:acr: don't display cppfunc field

commit fd3b273d9101568b2fe28347d7f0bec7d90ced7f
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Dec 28 13:55:57 2017 -0500

    acr: don't attempt to print cppfunc fields (not valid anyway)
    acr: forgive most schema input errors

commit 86d3fe745ce88ae9cc906fe4d430d81ba57957b8
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Jul 15 12:21:16 2017 -0400

    acr: when calling -check -e, don't limit # bad records to 100

commit f7f2317448c61281e8276b04574ba0d6cb6ae615
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Jun 14 14:14:33 2017 -0400

    acr: support editing, checking of a single file with -in:x -e
    acr: increase coverage to 65%

commit 69a1c32feafed979aaa9dd8e44bdf79e925be230
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon May 1 16:28:15 2017 -0400

    acr: allow RegxSql fields (no auto x-ref on these)

commit 0d01fca2d5f500688ff780aa90a621f6cf732489
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Mar 13 11:07:42 2017 -0400

    target:acr: make -meta option less verbose

commit 98ba1144f350b1d6fd741586e924dfef2a0c676a
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Jan 6 17:08:05 2017 -0500

    acr: streamline FRec

commit fcd6bb217ff721a82730c7989e59c89fb9e9b6f0
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Dec 28 10:27:22 2016 -0500

    acr: support numeric sort

commit 95596f1388e3e9d610d170427b162e27d8f9f137
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Wed Nov 9 11:37:31 2016 -0500

    acr: 70% faster read-only queries

commit 738783927518580b2795750ccb2058f569e6812f
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Tue Nov 8 15:03:20 2016 -0500

    acr: use SafeStringToFile (less chance of corruption)

commit 02319cee6ad6aa82e4f0b5d9c4df48c0d2a6727c
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Nov 3 09:54:53 2016 -0400

    acr: add -regxof option

commit 7fc10ea858929ebd134f776056b7127179b6bc14
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Fri Feb 12 00:36:41 2016 -0500

    acr: -field implies -report:N

commit 31fd2929e344a3f8c62856ab316efc3b2cf30a16
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Sat Oct 17 14:51:34 2015 -0400

    acr: check non-primary columns for uniqueness

commit 538538e11951dc01752268fd7d8e751c6972e5ad
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Thu Aug 6 22:25:56 2015 -0400

    acr: add -merge (variant of -replace).

commit 32e01fa5972ddb93677df172fe975d1b1dd0962f
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Jun 22 16:34:57 2015 -0400

    acr: remove -s (short form) flag

commit 3f8566f2a3703e15181cde02131708693e3340ca
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Apr 27 18:07:36 2015 -0400

    acr: keep track of rowid per-Ctype (not per-File -- this gives the first record inserted on stdin a rowid of 0)

commit 1fc4607fdafbcee986f9c73aefceda75921c3539
Author: Alexei Lebedev <alexei.lebedev@theice.com>
Date:   Mon Mar 2 18:22:49 2015 -0500

    acr: add -field flag

commit 938da1cf94faa7357c533c606cec1e91da2b7a36
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Feb 13 19:35:53 2015 -0500

    acr: fix column widths on output

commit f3509eea44528388101d32109f80a9d490567f11
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Feb 11 14:12:55 2015 -0500

    acr: allow loading, sorting and displaying of records that don't have ssimfiles defined.
    acr: check that char fields are 1 character exactly.

commit 61a38312ded90f0d22308fbafe72047a37e4d4b9
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Tue Feb 10 22:44:45 2015 -0500

    AB-3147: acr: always create input file row to avoid null pointer derefernece.

commit 4534ec554370bfac3a571cc145cc427d6bc05aa6
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Tue Feb 10 12:19:32 2015 -0500

        acr: acrtemp.ssim -> temp/acr.ssim -- done

commit 6c1280bac95305be803e01cca38ecb363ed78c3d
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sun Feb 8 22:23:01 2015 -0500

    acr: -in: support files and stdin (when -in:-).

commit b40e937a4c32d14e33e9c5404df66fb7ec4461a3
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Tue Jan 27 15:27:54 2015 -0500

    acr: print a warning if pkey is missing on a tuple.
    acr: change duplicate pkey warning to be compatible with errlist.

commit e1a421ec759d85f52a697b69cc814913c08a2183
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Jan 21 12:25:57 2015 -0500

    acr: Disallow a relation from include another relation by value (it is pretty much always an error)

commit c59bfc3c3e07fab9ad4f2d07b51f7060393353f0
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Jan 14 15:29:16 2015 -0500

    acr: do not skip lines after a blank line when loading ssim file

commit 3fed0c5005d86e838be518ea6726ece1eea2be87
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Thu Dec 18 11:24:36 2014 -0500

    acr: fix fldfunc propagation

commit 2240cea9f97f84602fbaa1b4560e1d13ac534af3
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Dec 17 22:22:38 2014 -0500

    acr: allow -rename of fldfunc

commit f4baa184961c81cf874cb4db1f56e83f27c6a479
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Thu Dec 4 22:44:46 2014 -0500

    acr: compute explicit list of ssimfiles to pass to ssim2db.
    acr: use command.{acr,ssim2db,db2ssim} and _ToCmdline

commit c057dc25cac3b87520eb342684830a7c36de786c
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Nov 22 21:41:29 2014 -0500

    acr: support acr.insert, acr.delete

commit 8ae91de3daaac0524d6b9071c40e93fed3618705
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Nov 15 13:42:49 2014 -0500

    acr: fix formatting bug caused by return of address of temporary variable

commit a2feb99d226c6baee4abb58dae3b3054bb9caf97
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Nov 15 10:29:44 2014 -0500

    acr: fix operation of -rename with ctype, ns
    acr: fix operation of -rename with field where field is a primary key.

commit 0059d220fd2fb8d06cc353aba3746bb6fcdbae92
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Oct 22 14:44:36 2014 -0400

    acr: add ctype comments to -cmt mode

commit dae2660bbd4964b9ef73999af069cae9b551d3a2
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Thu Oct 16 17:58:09 2014 -0400

    acr: roll back use of -s flag with -e. now -s must be specified explicitly.

commit 7f88675c0d73fb7e6cd68483ae3e4930da8a61eb
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Oct 11 17:38:14 2014 -0400

    acr: update documentation for acr_print; fix bug in EvalAttrDflt

commit dc22e5258c9c8381d342ece7028f55403980602c
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Oct 10 23:06:01 2014 -0400

    acr: create last-level directories on output

commit 6fc6ad9fa5e0e87a4700017d733fa59124154067
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Oct 8 10:05:42 2014 -0400

    acr: accept tuple short form on input. Example:

commit 5d54e04e0864b6d1b9158bee02568e5f0c06fc29
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Sep 27 14:35:45 2014 -0400

    acr: git interaction enabled by a flag

commit e7306565bc74767f4d017df87f2ddd6dd755673b
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Sep 26 19:59:43 2014 -0400

    acr: ignore gitfiles unless -write specified

commit 703dfae4d5b4d0ba21f66619b0f1df1c38a497b7
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Sep 26 19:07:36 2014 -0400

     acr: add git add/rm hook for gitfile records

commit 194fd1e316f64e67cf2c89f1eeeca3b8ab964742
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Sep 12 15:35:10 2014 -0400

    acr: add auto-complete of queries. example: acr device:smch<tab>
    acr: use lowercase ctypes in command namespace ot match real command name (i.e. ctype:command.abt, not command.Abt)

commit fb73141dc375876c9c291eb0edb3f5d226fbfc08
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Jun 6 16:48:03 2014 -0400

    acr: don't truncate ssimfile.ssim with -trunc -replace

commit 6cd8e27765fb1764b8645f42298165498abe7781
Author: Alexey Polovinkin <polov@algoengineering.com>
Date:   Tue May 27 07:07:57 2014 -0400

    acr: fix infinite loop on record self-join

commit 954ac107241af6b962a83832f2eac6e49b08ea6b
Author: Alexey Polovinkin <polov@algoengineering.com>
Date:   Mon May 5 08:12:08 2014 -0400

    acr: timestamp checking

commit 86380512c446e796be4a82f2bebf34cdfc8c4c57
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sun May 4 19:28:36 2014 -0400

    fix createexe (add pch, add missing cbase); acr: add Rec_DeselectAll, Rec_Deselect functions

commit 1cfdc5343fbb79eae6122c47709409af079bc1e4
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat May 3 20:40:53 2014 -0400

    acr: show record status (inserted/deleted/updated) in acr print step

commit 777e03b3f80c94a82252bad5abee396eb9f31288
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Thu May 1 10:35:07 2014 -0400

    acr: remove -changes, -q, -normalize options.

commit 9ff1cc02589a2c6955211ffb80eff2c6540935e2
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Apr 25 18:48:01 2014 -0400

    acr: allow rowid if ssimsort.sortfld is not the full key

commit 137a71b47da723d70cd8836a2fba6a21e57f0aa8
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Mon Apr 14 13:00:25 2014 -0400

    acr: don't include unrelated types in tree view.

commit 1b68e63574e3e2f15a1b097cb84212d212de6d44
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sat Apr 12 15:28:18 2014 -0400

    acr: fix crash when ctype has zero fields.
    acr: detect sql regex without % in them (example: acr 'ctype:amc.F(Db|Query)' now works.)

commit 026699b976bb24bf62ef0c2046f258564537c20c
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Thu Apr 10 14:10:40 2014 -0400

    acr: interpret missing namespace name as wildcard "%".

commit 8f72adf8739a9733ef5e1ff4d8eff3b0b39c0908
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Sun Mar 30 16:39:21 2014 -0400

    acr: better grouping of tuples in -t mode.

commit d4ffa6f9a6e14c6fe968e2e8aaf2ba179a4e2be2
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Mar 26 17:59:54 2014 -0400

    acr: do not print rowid if table is sorted.

commit 25a8f04ed2ac4321f549c5d41932dae8d2d9948e
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Wed Mar 5 13:36:52 2014 -0500

    acr: re-generate code for namespaces which now have ssimsort.
    acr: load ssimfiles as records in order to print (non-tree mode) correctly.

commit a701075616e31c2154f145c14137f8bf4db2d5e2
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Tue Mar 4 17:18:17 2014 -0500

    acr: add sorting of records by string and integer field.

commit 46c3bc52a061197b860ee88077225b333e02e61d
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Feb 28 15:10:48 2014 -0500

    acr: always output normalized data.
    acr: -fldfunc not written to disk
    acr: -fldfunc does not imply -myabort when using with mysql.
    acr: fldfunc is printed in its field position (not at the end of the line)

commit 61cbe8e8d2522a2e7b8bdc9efeba0e7dd20a54fb
Author: Alexei Lebedev <alebedev@nyx.com>
Date:   Fri Feb 21 23:38:05 2014 -0500

    acr: add option -cmt (turned on by default with -e) -- print comments

