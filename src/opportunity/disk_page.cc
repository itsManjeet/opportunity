#include <opportunity.hh>

using namespace opportunity;

page::disk::disk() {


    for(auto a : fs::list("/sys/class/block/")) {
        if (a.isdir()) {
            
            // Skip disk devices
            if (!isdigit(a.name()[a.name().length() - 1])) continue;

            auto __size = std::strtol(fs::read(io::sprint("/sys/class/block/",a.name(),"/size")).c_str(),0,10);
            auto d = new util::diskbtn(a.name(), util::humanify_size(__size));
            d->show_all();
            d->set_vexpand(true);
            d->set_hexpand(true);
            d->set_halign(Gtk::ALIGN_CENTER);
            d->set_valign(Gtk::ALIGN_CENTER);
            part_box.add(*d);
        }

        

    }
    set_margin_start(50);
    set_margin_end(50);
    set_vexpand();
    set_hexpand();

    part_box.set_selection_mode(Gtk::SELECTION_SINGLE);
    part_box.set_homogeneous(false);
    part_box.show_all();
    part_box.set_hexpand();
    part_box.set_vexpand();
    part_box.set_valign(Gtk::ALIGN_START);
    
    next_btn.set_label("Next");
    back_btn.set_label("Back");

    bbox.set_layout(Gtk::BUTTONBOX_CENTER);
    bbox.pack_start(back_btn, false, false);
    bbox.pack_start(next_btn, false, false);
    bbox.set_margin_bottom(40);

    bbox.show_all();

    pack_start(part_box,true, true);
}


void
window::setup_disk_page()
{
    disk_page.heading.set_text("Select root partition");
    disk_page.subheading.set_text("selected parititon will be used to install releax os");
    disk_page.show_all();
    stack.add(disk_page, "disk_page","disk_page");
    disk_page.part_box.signal_child_activated().connect(sigc::mem_fun(*this, &window::on_disk_select));
    disk_page.next_btn.signal_clicked().connect(sigc::mem_fun(*this, &window::on_disk_next_click));
    disk_page.back_btn.signal_clicked().connect(sigc::mem_fun(*this, &window::on_disk_back_click));
}

void
window::setup_efi_page()
{
    efi_page.heading.set_text("Select efi partition");
    efi_page.subheading.set_text("selected partition will be used to install bootloader");
    efi_page.show_all();
    stack.add(efi_page, "efi_page", "efi_page");
    efi_page.part_box.signal_child_activated().connect(sigc::mem_fun(*this, &window::on_efi_select));
    efi_page.next_btn.signal_clicked().connect(sigc::mem_fun(*this, &window::on_efi_next_click));
    efi_page.back_btn.signal_clicked().connect(sigc::mem_fun(*this, &window::on_efi_back_click));
}