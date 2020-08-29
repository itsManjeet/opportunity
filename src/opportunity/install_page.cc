#include <opportunity.hh>

using namespace opportunity;


page::install::install()
{
    set_vexpand(true);

    content.set_valign(Gtk::ALIGN_CENTER);
    content.set_vexpand();

    mesg_label.set_text("click 'install' to install releax os on your system");
    mesg_label.override_font(Pango::FontDescription("Open Sans Light 12"));
    content.add(mesg_label);

    install_btn.set_label(_T(" Install"));
    install_btn.set_image_from_icon_name("download", Gtk::ICON_SIZE_BUTTON);
    install_btn.override_font(Pango::FontDescription("Open Sans Light 12"));
    install_btn.set_hexpand(false);
    install_btn.set_halign(Gtk::ALIGN_CENTER);
    content.add(install_btn);

    content.set_spacing(30);
    content.set_margin_start(20);
    content.set_margin_end(20);
    progress_bar.set_show_text();
    content.add(progress_bar);

    back_btn.set_label(_T("Back"));
    bbox.pack_end(back_btn, true, true);
    bbox.set_margin_bottom(40);
    content.show_all();

}

void
window::setup_install_page()
{
    install_page.show_all();
    stack.add(install_page,"install_page","install_page");
    install_page.install_btn.signal_clicked().connect(sigc::mem_fun(*this, &window::on_install_btn_click));
    install_page.back_btn.signal_clicked().connect(sigc::mem_fun(*this, &window::on_install_back_click));
}