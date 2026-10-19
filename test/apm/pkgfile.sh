#!/bin/bash
# Fixture for comptest apm.Pkgfile.
# A package can ship a file of this tree at another path of its projection.  A
# dev.pkgfile row names the file and the path, and a package's readmefile is the
# same mapping for txt/README.md.  This script pushes a package with both into a
# fresh origin, then pushes it again without the mapped file, and prints what
# the origin holds after each push.  Then it gives the definitions three faults
# that apm -check reports and a push could not carry out, and prints the
# reports.
#
# Package base carries its own front page, txt/README.md.  Package pkg builds
# on base, carries txt/pkg/, makes txt/pkg/README.md its front page, and maps
# txt/pkg/AGENTS.md to AGENTS.md.  A push of pkg carries base too, so the
# origin ends up with pkg's front page at txt/README.md, where base's own would
# otherwise be, and with the mapped file at AGENTS.md.  Both sources still go
# out at their own paths, since pkg's key carries them.
#
# $1 is the checkout root and $2 the directory apm was built into.  The current
# directory is the test's own tempdir, and it stands in for a tree: data/ links
# every table of the checkout except the package tables and the file list, which
# come from test/apm/pkgfile.ssim and from the three files the script writes.
# The tables that name the checkout's own packages are left empty.
# The origin holds no schema of its own, so every acr the push runs reads the
# tempdir's through the acr in bin/.  Every write lands in the tempdir.
root="$1"
bindir="$2"
here="$PWD"

mkdir -p data/dev bin temp txt/pkg
for dir in "$root"/data/*/; do
    ns=$(basename "$dir")
    if [ "$ns" != dev ]; then ln -sfn "$root/data/$ns" "data/$ns"; fi
done
for file in "$root"/data/dev/*.ssim; do
    case $(basename "$file") in
        gitfile.ssim|package.ssim|pkgdep.ssim|pkgkey.ssim|pkgfile.ssim|pkgkeyword.ssim|pkgupstream.ssim|readmefile.ssim) ;;
        *) ln -sfn "$file" data/dev/;;
    esac
done
for table in package pkgdep pkgkey pkgfile; do
    grep "^dev.$table " "$root/test/apm/pkgfile.ssim" > "data/dev/$table.ssim"
done
touch data/dev/pkgkeyword.ssim data/dev/pkgupstream.ssim data/dev/readmefile.ssim
printf 'dev.gitfile  gitfile:%s\n' txt/README.md txt/pkg/AGENTS.md txt/pkg/README.md > data/dev/gitfile.ssim
echo "## Base" > txt/README.md
echo "## Package front page" > txt/pkg/README.md
echo "## Package guidance for agents" > txt/pkg/AGENTS.md

printf '#!/bin/bash\nexec %q -schema:%q "$@"\n' "$root/$bindir/acr" "$here/data" > bin/acr
chmod +x bin/acr
ln -sfn "$root/$bindir/acr_dm" bin/acr_dm
ln -sfn "$root/$bindir/apm" bin/apm
ln -sfn "$root/bin/update-gitfile" bin/update-gitfile
git init -q origin

# Print every file the origin tracks outside data/, with its first line.
show() {
    git -C origin ls-files | grep -v '^data/' | LC_ALL=C sort | while read -r f; do
        echo "apm.pkgfile_origin  file:$f  text:\"$(head -1 "origin/$f")\""
    done
}

bin/apm pkg -push -origin:origin > push1.log 2>&1
echo "apm.pkgfile_push  n:1  exit:$?"
show
git -C origin add -A
git -C origin -c user.name=apm -c user.email=apm@localhost commit -q -m one

# The second push drops the mapping, and the push deletes the file it mapped.
: > data/dev/pkgfile.ssim
bin/apm pkg -push -origin:origin > push2.log 2>&1
echo "apm.pkgfile_push  n:2  exit:$?"
show
git -C origin status --short

# A mapping whose source is not in this tree, a mapping onto a path the package
# carries from its own keys, and two carried packages mapping different files to
# one path.  Package top builds on pkg and maps nothing, so neither of the two
# mappings of AGENTS.md outranks the other when top is checked; checked alone,
# pkg's own mapping wins.
cat >> data/dev/pkgfile.ssim <<'ROWS'
dev.pkgfile  pkgfile:base/AGENTS.md  gitfile:txt/README.md  comment:""
dev.pkgfile  pkgfile:pkg/AGENTS.md  gitfile:txt/pkg/AGENTS.md  comment:""
dev.pkgfile  pkgfile:pkg/llms.txt  gitfile:txt/pkg/llms.txt  comment:""
dev.pkgfile  pkgfile:pkg/txt/pkg/AGENTS.md  gitfile:txt/pkg/README.md  comment:""
ROWS
echo 'dev.package  package:top  opensource:N  comment:""' >> data/dev/package.ssim
echo 'dev.pkgdep  pkgdep:top.pkg  soft:N  pkgdeptype:require  comment:""' >> data/dev/pkgdep.ssim
bin/apm '(pkg|top)' -check > check.log 2>&1
echo "apm.pkgfile_check  exit:$?"
grep '^apm\.pkgfile_' check.log
