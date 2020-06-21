package main

import (
	"os"
	"os/exec"
)

func prepareDisk(disk string) error {
	c := exec.Command("mkfs.ext4", "-F", disk)
	return c.Run()
}

func unpackImage(disk, wdir, squaloc string) error {
	os.MkdirAll(wdir, 0755)
	err := exec.Command("mount", disk, wdir).Run()
	if err != nil {
		return err
	}

	for _, a := range []string{"iso", "yreleax"} {
		err := exec.Command("unsquashfs", "-f", "-d", wdir, squaloc+"/"+a+".squa").Run()
		if err != nil {
			return err
		}
	}
	return nil
}
