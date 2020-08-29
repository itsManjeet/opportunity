#include <opportunity.hh>

using namespace opportunity;

void
window::on_cont_btn_click()
{
    stack.set_visible_child("lang_page");
}

void 
window::on_lang_select(Gtk::ListBoxRow* r)
{
    try {
        std::vector<Gtk::Widget*> l =  r->get_children();
        std::string lang = static_cast<Gtk::Label*>(l[0])->get_label();
        auto __l = get_locale(lang);
        io::info("Selected ",lang, " ", __l);
        install_data.locale = __l;

    } catch (std::length_error e) {
        io::error(e.what());
    }
    
    
}

void
window::on_lang_next_click()
{
    disk_page.next_btn.set_sensitive(false);
    stack.set_visible_child("disk_page");
}

void
window::on_lang_back_click()
{
    stack.set_visible_child("welcome_page");
}



void
window::on_disk_next_click()
{
    install_page.install_btn.set_visible(true);
    install_page.progress_bar.set_visible(false);
    std::string __next_page = "install_page";
    if (fs::exist("/sys/firmware/efi/efivars")) {
        efi_page.next_btn.set_sensitive(false);
        __next_page = "efi_page";
        install_data.efi = true;
    }
        
    stack.set_visible_child(__next_page);
}

void
window::on_disk_back_click()
{
    stack.set_visible_child("lang_page");
}

void
window::on_efi_next_click()
{
    std::string __next_page = "install_page";
    stack.set_visible_child(__next_page);
}

void
window::on_efi_back_click()
{
    stack.set_visible_child("disk_page");
}

void
window::on_disk_select(Gtk::FlowBoxChild* c)
{
    Gtk::Box* __box = static_cast<Gtk::Box*>(c->get_children()[0]);
    Gtk::Box* __inbox = static_cast<Gtk::Box*>(__box->get_children()[0]);
    Gtk::Label* __label = static_cast<Gtk::Label*>(__inbox->get_children()[1]);

    std::string root_device = __label->get_text();
    io::info("Selected root_device: ",root_device);

    install_data.root_disk = "/dev/" + root_device;
    disk_page.next_btn.set_sensitive(true);

}

void
window::on_efi_select(Gtk::FlowBoxChild* c)
{
    Gtk::Box* __box = static_cast<Gtk::Box*>(c->get_children()[0]);
    Gtk::Box* __inbox = static_cast<Gtk::Box*>(__box->get_children()[0]);
    Gtk::Label* __label = static_cast<Gtk::Label*>(__inbox->get_children()[1]);

    std::string efi_device = __label->get_text();
    io::info("Selected efi device: ",efi_device);

    install_data.efi_disk = "/dev/" + efi_device;
    efi_page.next_btn.set_sensitive(true);

}

void
window::on_install_btn_click()
{
    
    install_page.install_btn.set_visible(false);
    install_page.back_btn.set_sensitive(false);
    install_page.progress_bar.set_visible(true);

    if (__work_thread) {
        io::error("installer thread already running");
    } else {
        __work_thread = new std::thread(
            [this] {
                __installer.start(this, &install_data);      
            }
        );
    }
}

void
window::on_install_back_click()
{
    stack.set_visible_child("disk_page");
}

void
window::on_notify_from_installer()
{

    if (__work_thread && __installer.__stop) {
        if (__work_thread->joinable())
            __work_thread->join();
        
        delete __work_thread;
        __work_thread = nullptr;

        stack.set_visible_child("finish_page");
    }

    update_progress();
}

void
window::notify()
{
    __dispatcher.emit();
}

void
window::update_progress()
{
    double frac;
    std::string mesg;

    __installer.get_data(&frac, &mesg);
    install_page.progress_bar.set_fraction(frac);
    install_page.progress_bar.set_text(mesg);
}