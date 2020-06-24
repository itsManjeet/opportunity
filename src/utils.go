package main

import (
	"crypto/rand"
	"errors"
	"fmt"
	"log"
	"os"
	"os/exec"
	"strings"
	"unicode"

	"github.com/gotk3/gotk3/glib"
	"github.com/gotk3/gotk3/gtk"
)

func exists(path string) bool {
	_, err := os.Stat(path)
	if err == nil {
		return true
	}
	if os.IsNotExist(err) {
		return false
	}
	return false
}

func checkErr(e error) {
	if e != nil {
		d := dialogBox("err[software]: unexpected software side error - "+e.Error(), gtk.MESSAGE_ERROR)
		d.Run()
		d.Destroy()
		log.Panic(e.Error())
	}
}

func getWidget(builder *gtk.Builder, widgetName string) glib.IObject {
	obj, err := builder.GetObject(widgetName)
	checkErr(err)

	return obj
}

func dialogBox(msg string, dialogType gtk.MessageType) *gtk.MessageDialog {
	dialog := gtk.MessageDialogNew(
		nil,
		gtk.DIALOG_MODAL,
		dialogType,
		gtk.BUTTONS_OK_CANCEL,
		msg,
	)

	return dialog
}

func genOSID() string {
	b := make([]byte, 16)
	_, err := rand.Read(b)
	if err != nil {
		log.Fatalln("err[osID]: ", err)
	}

	uuid := fmt.Sprintf("%x-%x-%x-%x-%x", b[0:4], b[4:6], b[6:8], b[8:10], b[10:])
	return uuid
}

func setupConfirmPage() {
	langLabel := getWidget(builder, "langLabel").(*gtk.Label)
	langLabel.SetText(installData.language)

	deviceLabel := getWidget(builder, "deviceLabel").(*gtk.Label)
	deviceLabel.SetText(installData.rootDisk.Path)

	idLabel := getWidget(builder, "idLabel").(*gtk.Label)
	idLabel.SetText(installData.userData.userid)

	nickLabel := getWidget(builder, "nickLabel").(*gtk.Label)
	nickLabel.SetText(installData.userData.nick)

	installData.osID = genOSID()
	osIDLabel := getWidget(builder, "osIDLabel").(*gtk.Label)
	osIDLabel.SetText(installData.osID)
}

func isLower(s string) bool {
	for _, r := range s {
		if !unicode.IsLower(r) && unicode.IsLetter(r) {
			return false
		}
	}
	return true
}

func isSpace(s string) bool {
	for _, r := range s {
		if unicode.IsSpace(r) {
			return false
		}
	}
	return true
}

func checkString(s string) error {
	if len(s) == 0 {
		return errors.New("input value required")
	}
	if !isLower(s) {
		return errors.New("Must be lower case")
	}

	if !isSpace(s) {
		return errors.New("must not contain spaces")
	}

	if !unicode.IsLetter(rune(s[0])) {
		return errors.New("must start with letter")
	}

	return nil
}

func generatePasswordHash(password string) (string, error) {
	output, err := exec.Command("openssl", "passwd", "-6", "-salt", "xyz", password).Output()
	if err != nil {
		return "", err
	}

	return strings.TrimSuffix(string(output), "\n"), nil
}
