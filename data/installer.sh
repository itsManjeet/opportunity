#!/bin/sh

source /usr/lib/rsb/io.sh

workDir=$(mktemp -d /tmp/workDir.XXXXXXXXXX)

function log {
    process "$(date): $@"
}

function check {
    if [[ $1 -ne 0 ]] ; then
        exitWithMesg $1 "$2"
    fi
}

function exitWithMesg {
    err "$2"
    err "INSTALLATION FAILED WITH EXIT CODE - '$1'"
    exit $1
}

[[ -z $MODE ]] && exitWithMesg 11 "no method specified"
[[ -z $NAME ]] && exitWithMesg 12 "no name specified"
[[ -z $USER ]] && exitWithMesg 13 "no user specified"
[[ -z $PASS ]] && exitWithMesg 14 "no password specified"
#[[ -z $LOCA ]] && exitWithMesg 15 "no locale specified"
[[ -z $SQUA ]] && exitWithMesg 16 "no squa specified"

if [[ -d "/sys/firmware/efi/efivars" ]] ; then
    EFI=1
fi


ERR_UNKNOWN_MODE=101
ERR_DISK_NOT_FOUND=201
ERR_NO_ROOTPART_SPECIFIED=251
ERR_NO_ROOTPART_EXIST=252
ERR_EFIPART_NOT_AVALIABLE=253
ERR_NO_SQUADIR_EXIST=301
ERR_NO_SQUA_EXIST=302
function main {

    # log "performing pre configurations"
    # preConfig
    # check $? "error in preconfigurations"

    log "method specified - $MODE"
    case $MODE in
        full)

            [[ -z $DISK ]] && exitWithMesg 10 "no disk specified"

            log "perfroming full installation"
            fullInstall
            check $? "error in performing full installation"
            ;;

        manual)

            [[ -z $ROOTPART ]] && exitWithMesg $ERR_NO_ROOTPART_SPECIFIED "no disk specified"
    	
            log "performing manual installation"
            manualInstall
            check $? "performing manual installation"
            ;;
        
        *)
            check $ERR_UNKNOWN_MODE "unknown mode specified"
            ;;
    esac
	
    log "installing system"
    extractSqua
    check $? "error while installing system"

    log "updating database"
    updateDatabase
    check $? "error while updating database"

    log "adding user $USER"
    addUser
    check $? "error while adding user"

    log "setting up locale"
    setLocale
    check $? "error while setting up locale"


    log "installing bootloader"
    installBootloader
    check $? "error while installing bootloader"

    cleanRoots || true
    clear

    echo -e "\n\n\n"
    success Installation Success 
    echo -e "\n\n\n"
    notice "press any key to reboot"
    read -r
    log "rebooting"
    /sbin/openrc-shutdown -r now
}

# function preConfig {
# }

function setupRoots {
    install -d $workDir/{dev,proc,sys,run} || true

    mount --bind /dev $workDir/dev
    mount --bind /sys $workDir/sys

    mount -t proc proc $workDir/proc
    mount -t tmpfs tmpfs $workDir/run

}

function execInRoots {
    chroot $workDir $@
}

function cleanRoots {
    umount $workDir/{dev,proc,sys,run} || true
}

function fullInstall {
    if [[ ! -e $DISK ]] ; then
        return $ERR_DISK_NOT_FOUND
    fi

    if [[ $EFI ]] ; then
        parted --script $DISK \
            mklabel gpt \
            mkpart primary fat32 1MiB 100Mib  \
            set 1 esp on    \
            mkpart primary ext4 100Mib 100%

        ROOTPART=${DISK}2
	mkfs.ext4 -F $ROOTPART

        mount $ROOTPART $workDir

        install -d $ROOTPART/boot/efi
        mkfs.fat32 -F ${DISK}1
        mount ${DISK}1 $ROOTPART/boot/efi

    else
        parted --script $DISK \
            mklabel msdos \
            mkpart primary ext4 1Mib 100% \
            set 1 boot on

        ROOTPART=${DISK}1
	mkfs.ext4 -F $ROOTPART
        mount $ROOTPART $workDir
    fi

}

function manualInstall {

    if [[ ! -e $ROOTPART ]] ; then
        return $ERR_NO_ROOTPART_EXIST
    fi

    mkfs.ext4 -F $ROOTPART

    mount $ROOTPART $workDir

    if [[ $EFI ]] ; then
        if [[ -z $EFIPART ]] ; then
            EFIPART=$(lsblk -o path,fstype | grep vfat | awk '{print $1}')
            [[ -z $EFIPART ]] && return $ERR_EFIPART_NOT_AVALIABLE
        fi
	
		echo "efi-part: $EFIPART"
        install -dv -o root -g root $workDir/boot/efi
        mount $EFIPART $workDir/boot/efi -v
    fi
    
}


function extractSqua {

    if [[ ! -d $SQUA ]] ; then
        return $ERR_NO_SQUADIR_EXIST
    fi

    squa_count=$(ls $SQUA/*.squa | wc -l)

    if [[ $squa_count -eq 0 ]] ; then
        return $ERR_NO_SQUA_EXIST
    fi

    for squa in ${SQUA}/*.squa ; do
        sqImg=$(basename $squa | sed 's|.squa||g')
        case $sqImg in
            *installer*)
                continue
                ;;
            *)
                log "installing squa $sqImg"
                unsquashfs -f -d $workDir $squa
        esac
    done

}

function updateDatabase {
    setupRoots

    execInRoots ldconfig       || true
    execInRoots pwconv         || true
    execInRoots grpconv        || true
    execInRoots sys-usrgrp fix || true                                    
    execInRoots glib-compile-schemas /usr/share/glib-2.0/schemas || true
    execInRoots gdk-pixbuf-query-loaders --update-cache          || true

    
}


function addUser {
    execInRoots useradd -m -g user -G sudo,wheel,plugdev,adm,netdev $USER -p "$PASS"
}

function setLocale {
    execInRoots sys-locale set ${LOCA:-"en_IN.UTF-8"}
}

function installBootloader {
	if [[ $EFI ]] ; then
		execInRoots grub-install --bootloader-id "releax os" --recheck
	else
		if [[ -z $DISK ]] ; then
			DISK=$(echo $ROOTPART | sed 's|[0-9||g')
		fi
		execInRoots grub-install --recheck $DISK
	fi

	execInRoots grub-mkconfig -o /boot/grub/grub.cfg
}
main
