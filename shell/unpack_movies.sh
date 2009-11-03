#!/bin/bash

# set up our root dir and current dir
BASE=$PWD
if [ -d /$1 ] ; then
  ROOT=$1
else
  ROOT=$PWD/$1
fi

for movie_dir in `find $ROOT -type d -depth 1` ; do
  movie_release_name=`basename $movie_dir`
  
  if [ -f "$movie_release_name.mkv" ] ; then
    printf "Skipping $movie_release_name... Already Exists\n"
    continue
  fi

  # variables we need
  rar_file=`ls $movie_dir/*.part001.rar 2>/dev/null || ls $movie_dir/*.part01.rar 2>/dev/null || ls $movie_dir/*.rar`
  movie_file=`unrar lb $rar_file | grep '\.mkv$'`

  if [ -z "$movie_file" ] ; then
    printf "Movie not found for $movie_dir\n"
    continue
  fi

  # now we unpack the movie
  printf "Unpacking $movie_release_name... "
  echo $movie_file | unrar x -inul -tk -n@ $rar_file >/dev/null 2>&1
  if [ ! -f "$movie_file" ] ; then
    printf "Failed!\n"
    continue
  fi

  mv $movie_file $BASE/$movie_release_name.mkv
  printf "Success!\n"
done
