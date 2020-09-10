#!/bin/sh


[[ -z $3 ]] && {
    echo "software side error - no valid input provided"
    return -2
} 

__work_dir=$1
__bootloader_type=$2
__ddir=$3



function mount_fs {
    mount --bind /dev $__work_dir/dev
    mount -t devpts devpts $__work_dir/dev/pts -o gid=5,mode=620
    mount -t proc proc $__work_dir/proc
    mount -t sysfs sysfs $__work_dir/sys
    mount -t tmpfs tmpfs $__work_dir/run

    if [[ $__bootloader_type == "EFI" ]] ; then
        mount --bind /sys/firmware/efi/efivars $__work_dir/sys/firmware/efi/efivars
        mkdir -pv $__work_dir/boot/efi
        mount $__ddir $__work_dir/boot/efi
    fi
}

function umount_fs {
    umount $__work_dir/dev/pts
    umount $__work_dir/dev
    umount $__work_dir/run
    umount $__work_dir/proc
    if [[ $__bootloader_type == "EFI" ]] ; then
        umount $__work_dir/sys/firmware/efi/efivars
    fi
    umount $__work_dir/sys 
}



mount_fs
if [[ $__bootloader_type == "EFI" ]] ; then
    chroot $__work_dir grub-install --target=x86_64-efi --bootloader-id=releax --recheck &>/dev/null
else
    chroot $__work_dir grub-install "/dev/$(lsblk -no pkname $__ddir)" &>/dev/null
fi
__r=$?

if [[ $__r -ne 0 ]] ; then
    echo "failed to install bootloader"
    exit $__r
fi

chroot $__work_dir grub-mkconfig -o /boot/grub/grub.cfg &>/dev/null
__r=$?

umount_fs


exit $__r


