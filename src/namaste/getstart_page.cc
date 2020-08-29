#include <namaste/namaste.hh>

using namespace namaste;

page::getstart::getstart()
{


    __releax_label.set_text("🎉 releax is ready");
    __releax_label.override_font(Pango::FontDescription("Sans Regular 24"));
    __releax_label.set_margin_bottom(15);

    __get_start.set_label("Let's start");
    __get_start.set_halign(Gtk::ALIGN_CENTER);

    content.pack_start(__releax_label);
    content.pack_start(__get_start);
    content.set_spacing(5);

    content.set_valign(Gtk::ALIGN_CENTER);
    content.set_vexpand();
}

void
window::__setup_getstart_page()
{
    __getstart_page.show_all();
    __stack.add(__getstart_page,"getstart_page", "getstart_page");
    __getstart_page.__get_start.signal_clicked().connect(sigc::mem_fun(*this, &window::on_get_start_click));
}