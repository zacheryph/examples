#!/bin/sh
# svn-hg.sh <svn-repo> <dir-in-repo> <new-hg-repo> [base]
#
# requires:
#   direct access to the repo (uses svnlook)
# supports:
#   simple single directory pull
#   revision date memory
if [ $# -gt 4 ] ; then
  echo "usage: svn-hg.sh svn-repo repo-dir new-hg-repo"
  exit
fi
svn=$PWD/$1
dir=$2
hg=$3
base=$4 ; [ -z "$base" ] && base=0

if [ -d $hg ] ; then
  echo "error: new hg repo dir: $hg exists."
  exit
fi

# create new hg repo
hg init $hg
cd $hg
touch .hgignore
echo "syntax: glob" >> .hgignore
echo ".hgignore" >> .hgignore
echo ".svn" >> .hgignore
echo "**/.svn" >> .hgignore

echo "svn:$svn hg:$hg dir:$dir"

# loop through revs
for rev in `svnlook history ${svn} ${dir} | awk '{print $1}' | grep -vE '[^0-9]' | sort -n` ; do
  [ $rev -lt $base ] && continue
  echo "importing revision: ${rev}"
  svn co --ignore-externals -r${rev} file://${svn}/${dir} . >/dev/null 2>&1
  hg addremove >/dev/null 2>&1
  msg="`svnlook log -r${rev} $svn | sed 's/\* *//'`"
  date="`svnlook date -r${rev} $svn | sed 's/ (.*)//'`"
  hg ci -d "$date" -m "$msg"
done

# cleanup
find . -type d -name .svn -exec rm -rf {} \;
rm .hgignore
