import gi
gi.require_version('Gtk','3.0')
from gi.repository import Gtk, GLib, Vte, Gdk
from releax.lang import lang
from time import sleep
from logging import getLogger
import os, sys, json, threading

SQUA_LOC="/run/initramfs/medium"
INSTALLER_FILE="/usr/libexec/opportunity/installer.sh"
class errorDialog(Gtk.Dialog):

    def __init__(self, parent, msg_text):
        Gtk.Dialog.__init__(self, 'Error', parent, 0,
            (Gtk.STOCK_OK, Gtk.ResponseType.OK))
        
        self.set_default_size(150, 100)

        label = Gtk.Label(msg_text)
        box = self.get_content_area()
        box.add(label)
        self.show_all()

def loadWebsite(url):
    os.system('exo-open --launch WebBrowser %s' % url)

class Opportunity:
    def __init__(self, ui_file):
        self._ui_file = ui_file

    def load(self):
        self._builder = Gtk.Builder()
        self._builder.add_from_file(self._ui_file)
        self._builder.connect_signals(self)
        self._window = self._getWidget('mainWindow')

        self._logger = getLogger(__name__)

        self._already_disk_combo_set = False

        for l in lang:
            self._getWidget('langCombo').append_text(lang[l])

        self._getWidget('langCombo').set_active(0)
        

    def run(self):
        self._window.show_all()
        Gtk.main()

    def _errorDialog(self, msg_text, exit_func = None):
        dialog = errorDialog(self._window, msg_text)
        dialog.run()
        if exit_func is not None:
            exit_func()

        dialog.destroy()
        Gtk.main_quit()

    def _getWidget(self, widget_name):
        widget = self._builder.get_object(widget_name)
        if widget == None:
            self._errorDialog('Unable to get widget %s' % widget_name)
        else:
            return widget

    def _setPage(self, page_name):
        self._getWidget('stack').set_visible_child_name(page_name)

    def _listDisk(self):
        cmd_stream = os.popen('lsblk -O -J')
        return json.loads(cmd_stream.read())
    
    def _updateProgress(self, user_data):
        prog_val = float(user_data[0])      
        self._getWidget('processProgress').set_text(user_data[1])
        self._getWidget('processProgress').set_show_text(True)
        self._getWidget('processProgress').set_fraction(prog_val)


    def onDestroy(self, *args):
        Gtk.main_quit()

    def onStartBtnClick(self, *args):
        self._language = self._getWidget('langCombo').get_active_text()
        if self._language is None:
            dialog = errorDialog(self._window, 'Please select Language')
            dialog.run()
            dialog.destroy()
        else:
            print("Language Selected: %s" % self._language)
            self._setPage('instModePage')

        self._efi = False
        if os.path.exists('/sys/firmware/efi/efivars'):
            self._efi = True

    def onInsHelpBtnClick(self, *args):
        loadWebsite('https://wiki.releax.in')

    def onInstBackBtnClick(self, *args):
        self._setPage('introPage')

    
    def onDestInstBtnClick(self, *args):
        self._mode = 'destructive'
        self._disk = os.popen("lsblk | head -n2 | tail -n1 | awk '{print $1}'").readline()
        self._setPage('userPage')

    def onManInstBtnClick(self, *args):
        self._mode = 'manual'
        if not self._efi:
            self._getWidget('efiCombo').set_visible(False)
        
        if not self._already_disk_combo_set:
            for comboBox in ['efiCombo', 'rootCombo', 'homeCombo', 'varCombo']:
                if comboBox != 'rootCombo':
                    self._getWidget(comboBox).append_text('default')
                for disk in self._listDisk()['blockdevices']:
                    if 'children' in disk:
                        for part in disk['children']:
                            self._getWidget(comboBox).append_text('%s - %s' % (part['path'], part['size']))

                self._getWidget(comboBox).set_active(0)    

            self._already_disk_combo_set = True

        self._setPage('mountPage')

    def onMountNextBtnClick(self, *args):
        self._mount_data = {}
        for part in [ 'efi', 'root', 'home', 'var']:
            self._mount_data[part] = self._getWidget('%sCombo' % part).get_active_text()
        
        self._setPage('userPage')

    
    def onOpenPartBtnClick(self, *args):
        os.system('gparted')

    
    def onMountBackBtnClick(self, *args):
        self._setPage('instModePage')


    def onUsrNameEntryChanged(self, entryBox):
        txt = entryBox.get_text()
        err = ' '
        if len(txt) == 0:
            err = 'Username is must (otherwise login sucks)'
        elif ' ' in entryBox.get_text():
            err = 'No space is username'
        elif not txt.islower():
            err = 'No uppercase in username'
            

        self._getWidget('usrErrLbl').set_text(err)

    def onRPasswdEntryChanged(self, entryBox):
        if entryBox.get_text() != self._getWidget('passwdEntry').get_text():
            self._getWidget('usrErrLbl').set_text('Both password must be (otherwise you have to toss)')
        else:
            self._getWidget('usrErrLbl').set_text('')


    def onUserNextBtnClick(self, *args):
        self._user_data = {}
        for i in ['name', 'username', 'passwd']:
            self._user_data[i] = self._getWidget('%sEntry' % i).get_text()

        self._getWidget('confirmLangLbl').set_text(self._language)
        self._getWidget('confirmUserNameLbl').set_text(self._user_data['username'])
        self._getWidget('confirmInstallMethodLbl').set_text(self._mode)

        disk_data = ''
        if self._mode == 'manual':
            disk_data = 'root: %s' % self._mount_data['root']
            for i in ['efi', 'home', 'var']:
                if self._mount_data[i] != 'default':
                    disk_data += '\n%s: %s' % (i,self._mount_data[i])

        else:
            disk_data = 'root: %s' % self._disk

        self._getWidget('confirmDiskLbl').set_text(disk_data)
        self._setPage('confirmPage')


    def onUserBackBtnClick(self, *args):
        self._setPage('instModePage')

    
    def onConfirmBackBtnClick(self, *args):
        self._setPage('instModePage')

    def onConfirmNextBtnClick(self, *args):

        if self._mode == "destructive":
            os.environ['MODE'] = "full"
            os.environ['DISK'] = self._disk
        else:
            os.environ['MODE'] = 'manual'
            os.environ['ROOTPART'] = self._mount_data['root']
            if self._mount_data['efi'] != 'default':
                os.environ['EFIPART'] = self._mount_data['efi']

        
        os.environ['NAME'] = self._user_data['name']
        os.environ['USER'] = self._user_data['username']
        os.environ['PASS'] = self._user_data['passwd']
        os.environ['SQUA'] = SQUA_LOC
        
        self.terminal = Vte.Terminal()
        self.terminal.spawn_sync(
            Vte.PtyFlags.DEFAULT,
            '/',
            ['/bin/sh', INSTALLER_FILE],
            [],
            GLib.SpawnFlags.DO_NOT_REAP_CHILD,
            None,
            None
        )

        self.terminal.set_colors(
            Gdk.RGBA(red=0.0, green=0.0, blue=0.0),
            Gdk.RGBA(red=1.0, green=1.0, blue=1.0),
            None

        )
        self.terminal.show()
        self._getWidget('vteBox').add(self.terminal)
        self._setPage('instProcPage')

    def onRebootBtnClick(self, *args):
        Gtk.main_quit()


if __name__ == '__main__':
    app = Opportunity('data/ui.glade')
    app.load()
    app.run()