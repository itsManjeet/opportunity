#!/bin/env python
import gi, os
import json
gi.require_version('Gtk','3.0')
from gi.repository import Gtk, Gio, Gdk
from releax.ui import show_err_dialog, ui
from releax.lang import lang

SQUA_LOC = '/run/initramfs/medium'
INSTALLER_FILE = '/usr/libexec/opportunity/installer.sh'

HELP_INSTALL_TYPE_URL = 'https://wiki.releax.in/'

def list_disk():
    cmd_stream = os.popen('lsblk -O -J')
    return json.loads(cmd_stream.read())


def load_website(url):
    os.system('exo-open --launch WebBrowser %s' % url)

def validate_input(txt: str) -> (bool,str):
    if len(txt) == 0:
        return (False, 'must not empty')
    elif ' ' in txt:
        return (False, 'no space allowed')
    elif not txt.islower():
        return (False, 'no uppercase allowed')
    
    return (True,None)

class Opportunity(ui):
    def __init__(self, ui_file):
        ui.__init__(self, ui_file)

    def load(self):
        self.logger.debug('loading opportunity')
        self.window = self.get_widget('main_window')
        self.stack_page = self.get_widget('stack_page')

        self.exec_thread(self.thread_load_lang, None)

    def on_start_btn_click(self, *args):
        self.lang = self.get_widget('language_combo').get_active_text()

        self.logger.debug('selected {0}'.format(self.lang))
        self.stack_page.set_visible_child_name('installation_type')
        self.is_efi = os.path.exists('/sys/firmware/efi/efivars')

    def on_instype_back_btn_click(self, *args):
        self.stack_page.set_visible_child_name('intro_page')

    def on_instype_help_btn_click(self, *args):
        load_website(HELP_INSTALL_TYPE_URL)

    def on_destuctive_install_btn_click(self, *args):
        self.mode = 'destructive'
        self.disk = os.popen('lsblk  | head -n2 | tail -n1 | awk "{print $1}"').readline()[:-2]
        self.logger.debug('selected disk {self.disk}')
        self.stack_page.set_visible_child_name('user_page')

    
    def on_manual_install_btn_click(self, *args):
        self.mode = 'manual'

        if not self.is_efi:
            self.get_widget('efi_combo').set_visible(False)

        self.exec_thread(self.thread_load_disks, None)
        self.stack_page.set_visible_child_name('mount_page')

    def on_mount_next_btn_click(self, *args):
        self.mount_data = {}
        for part in  ['efi', 'root', 'home', 'var']:
            self.mount_data[part] = self.get_widget('{0}_combo'.format(part)).get_active_text()

        self.stack_page.set_visible_child_name('user_page')

    def on_mount_back_btn_click(self, *args):
        self.stack_page.set_visible_child_name('installation_type')

    def on_mount_open_part_btn_click(self, *args):
        os.system('gparted')
        self.exec_thread(self.thread_load_disks)

    def on_user_back_btn_click(self, *args):
        self.stack_page.set_visible_child_name('installation_type')

    def on_user_next_btn_click(self, *args):
        self.user_data = {}
        for i in ['user', 'pass']:
            self.user_data[i] = self.get_widget('%s_entry' % i).get_text()

        self.get_widget('confirm_lang_label').set_text(self.lang)
        self.get_widget('confirm_user_label').set_text(self.user_data['user'])
        self.get_widget('confirm_install_method_label').set_text(self.mode)

        disk_data = ''

        if self.mode == 'manual':
            disk_data = 'root: %s' % self.mount_data['root']
            for i in ['efi', 'home', 'var']:
                if self.mount_data[i] != 'default':
                    disk_data += '\n%s: %s' % (i, self.mount_data[i])

        else:
            disk_data = 'root: %s' % self.disk

        self.get_widget('confirm_disk_label').set_text(disk_data)
        self.stack_page.set_visible_child_name('confirm_page')

    def on_user_entry_changed(self, *args):
        isvalid, errmsg = validate_input(args[0].get_text())
        if not isvalid:
            self.get_widget('user_error_label').set_text('[username]: ' + errmsg)
        else:
            self.get_widget('user_error_label').set_text('')

    def on_pass_entry_changed(self, *args):
        isvalid, errmsg = validate_input(args[0].get_text())
        if not isvalid:
            self.get_widget('user_error_label').set_text('[password]: ' + errmsg)
        else:
            self.get_widget('user_error_label').set_text('')

    def on_r_pass_entry_changed(self, *args):
        if args[0].get_text() != self.get_widget('pass_entry').get_text():
            self.get_widget('user_error_label').set_text('[repeat password]: password is not same')
        else:
            self.get_widget('user_error_label').set_text('')


    def on_confirm_back_btn_click(self, *args):
        self.stack_page.set_visible_child_name('user_page')

    def on_confirm_next_btn_click(self, *args):
        self.install_vte, self.install_vte_pty = self.setup_vte_terminal(self.vte_installation_finished)

        self.install_vte.set_colors(
            Gdk.RGBA(red=0.0, green = 0.0, blue = 0.0),
            Gdk.RGBA(red=1.0, green = 1.0, blue = 1.0),
            None
        )
        self.get_widget('vte_box').add(self.install_vte)
        self.install_vte.show()

        if self.mode == 'manual':
            os.environ['MODE'] = 'manual'
            os.environ['ROOTPART'] = self.mount_data['root'][:self.mount_data['root'].find(' ')]
            if self.mount_data['efi'] != 'default':
                os.environ['EFIPART'] = self.mount_data['efi'][:self._mount_data['efi'].find(' ')]
        else:
            os.environ['MODE'] = 'full'
            os.environ['DISK'] = self.disk

        os.environ['USER'] = self.user_data['user']
        os.environ['PASS'] = os.popen('openssl passwd -1 %s' % self.user_data['pass']).readline().replace('\n','')
        os.environ['SQUA'] = SQUA_LOC


        self.exec_vte(self.install_vte, self.install_vte_pty, ['/bin/sh', INSTALLER_FILE])
        self.stack_page.set_visible_child_name('install_page')

    def on_reboot_btn_click(self, *args):
        os.system('xfce4-session-logout --reboot')

    def vte_installation_finished(self, *args):
        self.resetup_vte(args[0], args[1])

        self.stack_page.set_visible_child_name('done_page')

    def thread_load_disks(self, *args):
        for combo in  ['efi_combo', 'root_combo', 'home_combo', 'var_combo']:
            model = self.get_widget(combo).get_model()
            model.clear()
        
        for combo in  ['efi_combo', 'root_combo', 'home_combo', 'var_combo']:
            if combo != 'root_combo':
                self.get_widget(combo).append_text('default')
                self.get_widget(combo).set_active(0)
            for disk in list_disk()['blockdevices']:
                if 'children' in disk:
                    for part in disk['children']:
                        self.get_widget(combo).append_text('%s - %s' % (part['path'], part['size']))

    def on_destroy(self, *args):
        Gtk.main_quit()
    
    def thread_load_lang(self, *args):
        lang_combo = self.get_widget('language_combo')
        for l in lang:
            lang_combo.append_text(lang[l])
        
        lang_combo.set_active(0)

if __name__ == '__main__':   
    app = Opportunity('data/ui.glade')
    app.load()
    app.run()
