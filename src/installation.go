package main

import (
	"log"
	"os/exec"
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
	err := prepareDisk(installData.rootDisk.Path)
	checkErr(err)

	/* */
	log.Println("Installing releax system on disk")
	glib.IdleAdd(progressBar.SetText, "Installing releax os")
	updateProgress(0.5)
	err = unpackImage(installData.rootDisk.Path, workDir, squashfsLoc)
	checkErr(err)

	log.Println("post processing")
	glib.IdleAdd(progressBar.SetText, "post processes")
	updateProgress(0.7)
	err = exeCmd("releax-post-process", workDir, installData.language, installData.userData.userid, installData.userData.password, installData.efiDir)
	checkErr(err)

	log.Println("Success")
	updateProgress(1.0)
	glib.IdleAdd(stackPage.SetVisibleChildName, "successPage")
}
