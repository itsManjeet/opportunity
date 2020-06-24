#!/bin/sh

[ -z $1 ] && exit 5
work_dir=$1
lang=$2
usid=$3
pass=$4
disk=$(mount | grep $work_dir | awk '{print $1}' | sed 's/[0-9]//g')
efidir=$5



function exec_chroot {
    chroot $work_dir $@
}

function clear_root {
    umount $work_dir/{dev,proc,sys,run} || true
}


setup_root

exec_chroot ldconfig
exec_chroot pwconv
exec_chroot grpconv
exec_chroot sys-usrgrp fix
exec_chroot glib-compile-schemas /usr/share/glib-2.0/schemas
exec_chroot gdk-pixbuf-query-loaders --update-cache

exec_chroot useradd -m -g user -G sudo,wheel,plugdev,adm,netdev $usid -p $(openssl passwd -1 $pass)
exec_chroot sys-local set "en_IN.UTF-8"
exec_chroot sys-initrd 

if [[ -n $efidir ]] ; then

    mount -t efivarfs none $work_dir/sys/firmware/efi/efivars/

    mkdir -pv $work_dir/boot/efi 
    mount $efidir $work_dir/boot/efi

    exec_chroot grub-install --recheck --bootloader-id="releax os"
else
    exec_chroot grub-install $disk --recheck
fi

exec_chroot grub-mkconfig -o /boot/grub/grub.cfg

clear_root