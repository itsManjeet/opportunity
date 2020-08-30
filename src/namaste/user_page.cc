#include <namaste/namaste.hh>
#include <namaste/config.hh>
#include <sha512.hh>

using namespace namaste;

page::user::user()
{
    heading.set_label(_T("User details"));
    subheading.set_label(_T("click 'advance' for more options"));

    set_vexpand();

    __user_label.set_text("userid");
    __user_label.set_halign(Gtk::ALIGN_START);
    __pass_label.set_text("password");
    __user_entry.signal_changed().connect(sigc::mem_fun(*this, &user::on_user_text_changed));
    
    __pass_entry.set_visibility(false);
    __pass_entry.set_invisible_char('*');
    __pass_entry.signal_changed().connect(sigc::mem_fun(*this, &user::on_pass_text_changed));
    __pass_label.set_halign(Gtk::ALIGN_START);

    __next_btn.set_image_from_icon_name("dialog-ok-apply",Gtk::ICON_SIZE_BUTTON);
    __next_btn.set_halign(Gtk::ALIGN_CENTER);
    __next_btn.set_margin_top(20);

    __user_grid.add(__user_label);
    __user_grid.attach_next_to(__user_entry, __user_label, Gtk::POS_RIGHT);
    __user_grid.attach_next_to(__pass_label, __user_label, Gtk::POS_BOTTOM);
    __user_grid.attach_next_to(__pass_entry, __pass_label, Gtk::POS_RIGHT);
    __user_grid.attach_next_to(__next_btn, __pass_label, Gtk::POS_BOTTOM,2,1);

    __user_grid.set_row_spacing(10);
    __user_grid.set_column_spacing(30);

    __user_grid.set_vexpand();
    __user_grid.set_hexpand();
    __user_grid.set_valign(Gtk::ALIGN_CENTER);
    __user_grid.set_halign(Gtk::ALIGN_CENTER);


    content.add(__user_grid);
}

std::string
page::user::__get_user_id()
{
    return __user_entry.get_text();
}



std::string
page::user::__get_enc_passwd()
{
    return sw::sha512::calculate(__pass_entry.get_text());
}

void
window::__setup_user_page()
{
    __user_page.show_all();
    __stack.add(__user_page, "user_page","user_page");
    __user_page.__next_btn.signal_clicked().connect(sigc::mem_fun(*this, &window::on_user_next_click));
}