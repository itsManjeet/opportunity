package main

import (
	"github.com/gotk3/gotk3/gtk"
)

// BlockDevices struct
type BlockDevices struct {
	BlockDevices []BlockDevice `json:"blockdevices"`
}

// BlockDevice struct
type BlockDevice struct {
	Name     string        `json:"name"`
	Size     string        `json:"size"`
	Type     string        `json:"type"`
	Path     string        `json:"path"`
	Used     string        `json:"fsused"`
	FSType   string        `json:"fstype"`
	Children []BlockDevice `json:"children"`
}

func loadPart(disk BlockDevice, prtlst *gtk.ListStore, parentPath string) {
	for _, a := range disk.Children {
		addDisk(a, "partition", prtlst, parentPath)
	}
}
