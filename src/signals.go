package main

import (
	"log"
	"os"
	"os/exec"

	"github.com/gotk3/gotk3/gtk"
)

func onDestroy() {

}

func onStartBtnClick() {
	langCombo := getWidget(builder, "langCombo").(*gtk.ComboBoxText)
	installData.language = langCombo.GetActiveText()
	log.Println("set language " + installData.language)
	if len(installData.language) == 0 {
		d := dialogBox("Please Select your language", gtk.MESSAGE_ERROR)
		d.Run()
		d.Destroy()
	}
	stackPage.SetVisibleChildName("diskPage")
}

func setDiskInfo(disk BlockDevice) {
	nameLabel := getWidget(builder, "nameLabel").(*gtk.Label)
	nameLabel.SetText(disk.Name)

	pathLabel := getWidget(builder, "pathLabel").(*gtk.Label)
	pathLabel.SetText(disk.Path)

	sizeLabel := getWidget(builder, "sizeLabel").(*gtk.Label)
	if len(disk.Used) > 0 {
		sizeLabel.SetText(disk.Used + "/" + disk.Size)
	} else {
		sizeLabel.SetText(disk.Size)
	}

}

func onDiskSelect(iconView *gtk.IconView, treePath *gtk.TreePath) {
	iconView.GetSelectedItems()
	iter, _ := diskListStore.GetIter(treePath)
	sel, _ := diskListStore.GetValue(iter, 2)
	diskPath, _ := sel.GetString()
	//fmt.Println(diskPath)

	for _, a := range diskData.BlockDevices {
		if a.Path == diskPath {
			installData.rootDisk = a
			setDiskInfo(a)
			partListStore.Clear()
			loadPart(a, partListStore, a.Path)
		}
	}

}

func onPartSelect(iconView *gtk.IconView, treePath *gtk.TreePath) {
	iconView.GetSelectedItems()
	iter, _ := partListStore.GetIter(treePath)
	sel, _ := partListStore.GetValue(iter, 2)
	parentPathPtr, _ := partListStore.GetValue(iter, 3)
	parentPath, _ := parentPathPtr.GetString()

	partPath, _ := sel.GetString()

	for _, a := range diskData.BlockDevices {
		if a.Path == parentPath {
			for _, c := range a.Children {
				if c.Path == partPath {
					installData.rootDisk = c
					setDiskInfo(c)
				}
			}
		}
	}

}

func onEfiSelect() {
	efiCombo := getWidget(builder, "efiCombo").(*gtk.ComboBoxText)
	installData.efiDir = efiCombo.GetActiveText()
}
func onDiskNext() {
	if len(installData.rootDisk.Name) <= 0 {
		errbox := dialogBox("Please select disk or partition to proceed", gtk.MESSAGE_ERROR)
		errbox.Run()
		errbox.Destroy()

		if _, err := os.Stat("/sys/firmware/efi/efivars"); err == nil {

			efiBox := getWidget(builder, "efiBox").(*gtk.Window)

			efiCombo := getWidget(builder, "efiCombo").(*gtk.ComboBoxText)
			for _, a := range diskData.BlockDevices {
				for _, j := range a.Children {
					efiCombo.AppendText(j.Path)
				}
			}
			efiBox.ShowAll()
		}
	} else {
		confirmBox := dialogBox(installData.rootDisk.Name+" is going to wipeout, Please make sure you have not valueable data inside it", gtk.MESSAGE_QUESTION)
		if confirmBox.Run() == gtk.RESPONSE_OK {
			stackPage.SetVisibleChildName("userDetail")
		}
		confirmBox.Destroy()
	}

}

func onDiskBack() {
	stackPage.SetVisibleChildName("mainPage")
}

func onOpenPart() {
	err := exec.Command("exo-open", "gparted").Run()
	checkErr(err)

	loadDisk()
}

func onUserNext() {

	userIDEntry := getWidget(builder, "userIDEntry").(*gtk.Entry)
	installData.userData.userid, _ = userIDEntry.GetText()

	passEntry := getWidget(builder, "passEntry").(*gtk.Entry)
	installData.userData.password, _ = passEntry.GetText()
	rePassEntry := getWidget(builder, "passEntry").(*gtk.Entry)
	retext, _ := rePassEntry.GetText()

	errfound := false
	if installData.userData.password != retext {
		d := dialogBox("Password is not same, please recheck", gtk.MESSAGE_ERROR)
		d.Run()
		d.Destroy()
		errfound = true
	} else {
		err := checkString(installData.userData.userid)
		if err != nil {
			d := dialogBox("userID "+err.Error(), gtk.MESSAGE_ERROR)
			d.Run()
			d.Destroy()
			errfound = true
		} else {
			err := checkString(installData.userData.password)
			if err != nil {
				d := dialogBox("Password "+err.Error(), gtk.MESSAGE_ERROR)
				d.Run()
				d.Destroy()
				errfound = true
			}
		}
	}

	if !errfound {
		setupConfirmPage()
		stackPage.SetVisibleChildName("confirmPage")
	}

}

func onUserBack() {
	stackPage.SetVisibleChildName("diskPage")
}

func onConfirmNext() {
	stackPage.SetVisibleChildName("installPage")
	progressBar = getWidget(builder, "progressBar").(*gtk.ProgressBar)
	go performInstallation()
}

func onConfirmBack() {
	stackPage.SetVisibleChildName("userDetail")
}
