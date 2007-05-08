#!/bin/sh
# secure image handler for Mac OS X
#
# >>>                NOTICE                 <<<
#              ------------------
# >>> The first time you use this script it <<<
# >>> will create secure disk image for you <<<
#
# 1. Configure Settings
#
# mount_point:  where the image should be mounted
#    max_size:  the largest size an image can reach
#  compact_at:  compact on detach when this much % can be free'd[0|1]
mount_point=~/source
max_size=100m
compact_at=20
#
# 2. Setup Key Bindings
#
#
#

# >>> DO NOT TOUCH PAST THIS POINT <<<
image_file=`dirname $mount_point`/.`basename $mount_point`.sparseimage

attached() {
  [ 0 -eq `hdiutil info | grep -E "^image-path *: *$image_file$" | wc -l` ] && return 1
  return 0
}

create_image() {
  [ -f $image_file ] && exit
  hdiutil create -size $max_size $image_file        \
    -type SPARSE -fs HFS+ -layout NONE -fsargs -s   \
    -encryption -uid `id -u` -gid `id -g` -mode 700 \
    -volname "$USER's Secure Disk Image"
}

free_percentage() {
  disk=/dev/`stat -f %Sd $mount_point`
  data=`hdiutil hfsanalyze $disk | grep -E '^(volume size|current free space)'`
  total=`echo "$data" | grep '^volume size' | sed -E 's/.*(0x[0-9A-F]+).*/\1/'`
  free=`echo "$data" | grep '^current free space' | sed -E 's/.*(0x[0-9A-F]+).*/\1/'`
  size=0x`stat -f %Xz $image_file`
  let "one=$total/100"
  let "percentage=($free/$one)+($size/$one)-100"
  echo $percentage
}

case "$1" in
  attach)
    attached ; [ 0 -eq $? ] && exit
    [   -d $mount_point ] && exit
    [ ! -f $image_file  ] && create_image
    hdiutil attach $image_file -quiet -private -nobrowse -mountpoint $mount_point
    ;;
  detach)
    attached ; [ 1 -eq $? ] && exit
    can_free=`free_percentage`
    echo "can:$can_free threshold:$compact_at"
    hdiutil detach $mount_point -quiet -force
    [  $can_free -ge $compact_at ] && hdiutil compact $image_file -quiet
    ;;
  esac
