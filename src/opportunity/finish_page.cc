#include <opportunity.hh>

using namespace opportunity;

page::finish::finish()
{
    finish_label.set_text(_T("Installation Success, click 'reboot' to restart into the system"));
    finish_label.override_font(Pango::FontDescription("Open Sans Light 18"));

    reboot.set_label(_T("Reboot"));
    reboot.set_halign(Gtk::ALIGN_CENTER);
    content.set_valign(Gtk::ALIGN_CENTER);
    content.set_spacing(20);
    content.add(finish_label);
    content.add(reboot);

    content.show_all();
}

void
window::setup_finish_page()
{
    finish_page.show_all();

    stack.add(finish_page, "finish_page","finish_page");
}