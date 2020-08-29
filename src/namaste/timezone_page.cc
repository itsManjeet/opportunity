#include <namaste/namaste.hh>

using namespace namaste;

bool
__compare_func__(fs::obj a, fs::obj b) { return a.name() < b.name();}

page::timezone::timezone()
{
    heading.set_text("Timezone");

    __timezone_listbox.set_vexpand();
    std::vector<fs::obj> __conts = fs::list("/usr/share/zoneinfo");
    std::sort(__conts.begin(), __conts.end(), __compare_func__);


    for(auto c : __conts ) {
        
        // Skip non-dirs
        if (!c.isdir() || c.name() == "Etc") continue;

        std::vector<fs::obj> __states = fs::list("/usr/share/zoneinfo/" + c.name() + "/");
        std::sort(__states.begin(), __states.end(), __compare_func__);

        for(auto s : __states  ) {
            
            if (s.isdir()) continue;

            Gtk::Label* __zone_label = new Gtk::Label();
            __zone_label->set_text(io::sprint(c.name(),"/",s.name()));
            __zone_label->override_font(Pango::FontDescription("Sans Regular 12"));
            __zone_label->set_margin_top(8);
            __zone_label->set_margin_bottom(8);
            __zone_label->show();

            __timezone_listbox.add(*__zone_label);
        }

    }

    __scroll_window.add(__timezone_listbox);

    __back_btn.set_image_from_icon_name("back",Gtk::ICON_SIZE_BUTTON);
    __back_btn.set_halign(Gtk::ALIGN_CENTER);

    __next_btn.set_image_from_icon_name("next",Gtk::ICON_SIZE_BUTTON);
    __next_btn.set_halign(Gtk::ALIGN_CENTER);

    bbox.set_layout(Gtk::BUTTONBOX_CENTER);

    bbox.pack_start(__back_btn);
    bbox.pack_start(__next_btn);
    
    content.add(__scroll_window);
    content.set_spacing(10);
    content.set_vexpand();

}

void
window::__setup_timezone_page()
{
    __timezone_page.show_all();
    __timezone_page.__next_btn.signal_clicked().connect(sigc::mem_fun(*this,&window::on_timezone_next_click));
    __stack.add(__timezone_page,"timezone_page","timezone_page");
}