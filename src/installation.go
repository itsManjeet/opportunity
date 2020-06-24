package main

import (
	"errors"
	"log"
	"os/exec"
	"syscall"
	"time"

	"github.com/gotk3/gotk3/glib"
)

const (
	squashfsLoc = "/run/initramfs/medium/"
	workDir     = "/tmp/work_dir"
)

func exeCmd(cmd ...string) error {
	c := exec.Command(cmd[0], cmd[1:]...)
	return c.Run()
}

func updateProgress(value float64) {
	for i := progressBar.GetFraction(); i <= value; i = i + 0.005 {
		glib.IdleAdd(progressBar.SetFraction, i)
		time.Sleep(time.Millisecond * 2)
	}
}

func performInstallation() {

	log.Println("Formatting disk")
	glib.IdleAdd(progressBar.SetText, "Preparing disk")
	updateProgress(0.1)
	if err := prepareDisk(installData.rootDisk.Path); err != nil {
		glib.IdleAdd(checkErr, errors.New("Error while formatting disk "+err.Error()))
	}

	/* */
	log.Println("unpacking releax system on disk")
	glib.IdleAdd(progressBar.SetText, "Unpacking system image")
	updateProgress(0.5)
	if err := unpackImage(installData.rootDisk.Path, workDir, squashfsLoc); err != nil {
		glib.IdleAdd(checkErr, errors.New("Error while Unpacking system "+err.Error()))
	}

	/* Chroot Tasks */

	log.Println("post processing")
	glib.IdleAdd(progressBar.SetText, "post processes")
	updateProgress(0.55)
	if err := exeCmd("releax-chroot", workDir); err != nil {
		glib.IdleAdd(checkErr, errors.New("Error while setting up chroot "+err.Error()))
	}

	if err := exeCmd("cp", "/bin/refresh-system", workDir+"/bin/"); err != nil {
		glib.IdleAdd(checkErr, errors.New("Error while copying refresh toolkit "+err.Error()))
	}

	log.Println("chrooting into system")
	if err := syscall.Chroot(workDir); err != nil {
		glib.IdleAdd(checkErr, errors.New("Error while chrooting system "+err.Error()))
	}

	log.Println("refreshing system")
	glib.IdleAdd(progressBar.SetText, "refreshing system")
	updateProgress(0.6)
	if err := exeCmd("refresh-system"); err != nil {
		glib.IdleAdd(checkErr, errors.New("Error while refreshing system "+err.Error()))
	}

	log.Println("generating password hash")
	passHash, err := generatePasswordHash(installData.userData.password)
	if err != nil {
		glib.IdleAdd(checkErr, errors.New("Error while generating password hash "+err.Error()))
	}
	log.Println("adding user account")
	glib.IdleAdd(progressBar.SetText, "Adding User Account")
	updateProgress(0.7)
	if err := exeCmd("useradd", "-m", "-g", "user", "-G", "sudo,wheel,storage,adm,netdev", installData.userData.userid, "-p", passHash); err != nil {
		glib.IdleAdd(checkErr, errors.New("Error while creating user account "+err.Error()))
	}

	log.Println("Setting locale en_IN.UTF-8")
	glib.IdleAdd(progressBar.SetText, "Setting locale en_IN.UTF-8")
	updateProgress(0.74)
	if err := exeCmd("sys-locale", "set", "en_IN.UTF-8"); err != nil {
		glib.IdleAdd(checkErr, errors.New("Error while setting up locale "+err.Error()))
	}

	log.Println("Generatting Inital Ramdisk")
	glib.IdleAdd(progressBar.SetText, "Generatting Inital Ramdisk")
	updateProgress(0.8)
	if err := exeCmd("sys-initrd"); err != nil {
		glib.IdleAdd(checkErr, errors.New("Error while generatting initrd "+err.Error()))
	}

	if len(installData.efiDir) == 0 {
		log.Println("Installing Legacy Boot loader in ", installData.disk)
		glib.IdleAdd(progressBar.SetText, "Installing Legacy Boot loader in "+installData.disk)
		updateProgress(0.8)
		if err := exeCmd("grub-install", installData.disk); err != nil {
			glib.IdleAdd(checkErr, errors.New("Error while installing legacy bootloader "+err.Error()))
		}
	} else {
		log.Println("Preparing efiDir ", installData.efiDir)
		glib.IdleAdd(progressBar.SetText, "preparing efidir "+installData.efiDir)
		updateProgress(0.8)
		log.Println("mounting efivarfs")
		if err := exeCmd("mount", "-t", "efivarfs", "none", "/sys/firmware/efi/efivars"); err != nil {
			glib.IdleAdd(checkErr, errors.New("Error while mounting efivarfs "+err.Error()))
		}
		log.Println("creating efi dir")
		if err := exeCmd("mkdir", "-p", "/boot/efi"); err != nil {
			glib.IdleAdd(checkErr, errors.New("Error creating efi dir "+err.Error()))
		}
		log.Println("mounting efi device")
		if err := exeCmd("mount", installData.efiDir, "/boot/efi"); err != nil {
			glib.IdleAdd(checkErr, errors.New("Error while mounting efi device "+err.Error()))
		}

		log.Println("Installing EFI bootloader ", installData.efiDir)
		glib.IdleAdd(progressBar.SetText, "Installing EFI Bootloader "+installData.efiDir)
		updateProgress(0.85)
		if err := exeCmd("grub-install", "--recheck", "--bootloader-id='releax os'"); err != nil {
			glib.IdleAdd(checkErr, errors.New("Error while installing efi bootloader "+err.Error()))
		}
	}

	log.Println("Generating Bootloader Configurations")
	glib.IdleAdd(progressBar.SetText, "Generating Bootloader Configurations")
	updateProgress(0.9)
	if err := exeCmd("grub-mkconfig", "-o", "/boot/grub/grub.cfg"); err != nil {
		glib.IdleAdd(checkErr, errors.New("Error while generating bootloader configurations "+err.Error()))
	}

	log.Println("Success")
	updateProgress(1.0)
	glib.IdleAdd(stackPage.SetVisibleChildName, "successPage")
}
