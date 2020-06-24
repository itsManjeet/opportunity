#!/bin/bash

[ -z $1 ] && exit 5

work_dir=$1

install -d $work_dir/{dev,proc,sys,run}
mount --bind /dev $work_dir/dev
mount --bind /sys $work_dir/sys

mount -t proc proc $work_dir/proc
mount -t tmpfs tmpfs $work_dir/run