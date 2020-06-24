package main

import (
	"encoding/json"
	"log"
	"os"
	"os/exec"

	"github.com/gotk3/gotk3/gdk"
	"github.com/gotk3/gotk3/glib"
	"github.com/gotk3/gotk3/gtk"
)

const appID = "in.releax.sys.opportunity"

var lang = []string{"English"}

var testDisk = []string{"sda1", "sda2", "sda3"}

var (
	builder       *gtk.Builder
	stackPage     *gtk.Stack
	diskListStore *gtk.ListStore
	partListStore *gtk.ListStore
	diskData      BlockDevices
	progressBar   *gtk.ProgressBar
)

type iuserdata struct {
	nick     string
	userid   string
	password string
}

type idata struct {
	language string
	rootDisk BlockDevice
	disk     string
	userData iuserdata
	osID     string
	efiDir   string
}

var installData idata

func main() {

	application, err := gtk.ApplicationNew(appID, glib.APPLICATION_FLAGS_NONE)
	checkErr(err)

	application.Connect("startup", func() {
		log.Println("application started")
	})

	application.Connect("activate", func() {
		log.Println("application activate")

		builder, err = gtk.BuilderNewFromFile("/usr/share/opportunity/ui.glade")
		checkErr(err)

		signals := map[string]interface{}{
			"onDestroy":       onDestroy,
			"onStartBtnClick": onStartBtnClick,
			"onDiskSelect":    onDiskSelect,
			"onPartSelect":    onPartSelect,
			"onDiskNext":      onDiskNext,
			"onDiskBack":      onDiskBack,
			"onOpenPart":      onOpenPart,
			"onUserNext":      onUserNext,
			"onUserBack":      onUserBack,
			"onConfirmNext":   onConfirmNext,
			"onConfirmBack":   onConfirmBack,
			"onEfiSelect":     onEfiSelect,
		}

		builder.ConnectSignals(signals)

		window := getWidget(builder, "mainWindow").(*gtk.Window)

		langCombo := getWidget(builder, "langCombo").(*gtk.ComboBoxText)
		setupLangCombo(langCombo)

		/* Disk Page */

		stackPage = getWidget(builder, "stackPage").(*gtk.Stack)
		diskIconView := getWidget(builder, "diskIconView").(*gtk.IconView)
		partIconView := getWidget(builder, "partIconView").(*gtk.IconView)

		diskListStore, err = gtk.ListStoreNew(gdk.PixbufGetType(), glib.TYPE_STRING, glib.TYPE_STRING, glib.TYPE_STRING)
		checkErr(err)

		partListStore, err = gtk.ListStoreNew(gdk.PixbufGetType(), glib.TYPE_STRING, glib.TYPE_STRING, glib.TYPE_STRING)
		checkErr(err)

		diskIconView.SetModel(diskListStore)
		diskIconView.SetPixbufColumn(0)
		diskIconView.SetTextColumn(1)

		partIconView.SetModel(partListStore)
		partIconView.SetPixbufColumn(0)
		partIconView.SetTextColumn(1)

		loadDisk()

		window.Show()
		application.AddWindow(window)
	})

	application.Connect("shutdown", func() {
		log.Println("application shutdown")
	})

	os.Exit(application.Run(os.Args))
}

func loadDisk() {
	diskListStore.Clear()
	diskData = getDisk()
	for _, a := range diskData.BlockDevices {
		addDisk(a, "Disk", diskListStore, "null")
	}
}

func setupLangCombo(langCombo *gtk.ComboBoxText) {
	for _, a := range lang {
		langCombo.AppendText(a)
	}
}

func addDisk(disk BlockDevice, ty string, listStore *gtk.ListStore, parentPath string) {
	icontheme, err := gtk.IconThemeGetDefault()
	checkErr(err)

	pixbuf, err := icontheme.LoadIcon("drive-harddisk", 64, 0)
	checkErr(err)
	iter := listStore.Append()
	listStore.Set(
		iter,
		[]int{0, 1, 2, 3},
		[]interface{}{pixbuf, disk.Name, disk.Path, parentPath},
	)
}

func getDisk() BlockDevices {
	cmd := exec.Command("lsblk", "-O", "-J")
	out, err := cmd.CombinedOutput()
	if err != nil {
		log.Fatalf("cmd.Run() failed with %s\n", err)
	}

	var allDevices BlockDevices
	err = json.Unmarshal(out, &allDevices)
	checkErr(err)

	return allDevices
}
