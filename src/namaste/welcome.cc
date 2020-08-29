#include <namaste/namaste.hh>
#include <namaste/config.hh>
using namespace namaste;

page::welcome::welcome()
{
    set_vexpand();

    content.set_valign(Gtk::ALIGN_CENTER);
    content.set_vexpand();
    __logo_img.set(__DATA_DIR__ "/logo.png");
    content.pack_start(__logo_img, true, true);

    __releax_label.set_text("releax os");
    __releax_label.override_font(Pango::FontDescription("Sans Light 20"));

    __sub_label.set_text(_T("Feel the power of opensource"));
    __sub_label.override_font(Pango::FontDescription("Sans Light 12"));

    __next_btn.set_margin_top(30);
    __next_btn.set_image_from_icon_name("next",Gtk::ICON_SIZE_LARGE_TOOLBAR);
    __next_btn.set_halign(Gtk::ALIGN_CENTER);


    content.pack_start(__releax_label, false, false);
    content.pack_start(__sub_label, false, false);
    content.pack_start(__next_btn,false, false);

    content.show_all();
}

void 
window::__setup_welcome_page()
{
    __welcome_page.show();
    __stack.add(__welcome_page,"welcome_page","welcome_page");
    __welcome_page.__next_btn.signal_clicked().connect(sigc::mem_fun(*this, &window::on_wel_next_click));
}