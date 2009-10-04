#!/bin/sh
# finds, packs, and prunes all local git repo's
for repo in `find . -type d \( -name '.git' -o -name '*.git' \)` ; do
  dir=`echo $repo | sed 's:/*\.git::'`
  echo "*** Cleaning: $dir"
  GIT_DIR=$repo git gc --aggressive --prune=now
  echo
done
