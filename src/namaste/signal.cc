#include <namaste/namaste.hh>

using namespace namaste;

void
window::on_wel_next_click()
{
    __stack.set_visible_child("user_page");
}

void
window::on_user_next_click()
{
    int r = system(
        io::sprint(
            "useradd -g user -G sudo,wheel,netdev,storage,install,adm ",
            " -p ", __user_page.__get_enc_passwd(), " -m ", __user_page.__get_user_id()
        ).c_str()
    );

    if (WEXITSTATUS(r))
        __exit_error("failed to add new user '" + __user_page.__get_user_id());
    else
        __stack.set_visible_child("timezone_page");
}

void
window::on_timezone_next_click(Gtk::ListBoxRow *r)
{
    std::vector<Gtk::Widget*> l = r->get_children();
    std::string __time_zone = 
        static_cast<Gtk::Label*>(l[0])->get_label();
    
    io::info("Selected time zone: ",__time_zone);
    int x = system(
        io::sprint(
            "ln -srv ", __time_zone ," /etc/localtime"
        ).c_str()
    );

    if (WEXITSTATUS(x)) {
        __exit_error("Failed to setup time zone");
    }
    __stack.set_visible_child("getstart_page");
}

void
window::on_get_start_click()
{
    this->get_application()->quit();
}


err::obj
is_valid(const std::string &__text)
{
    if (__text.length() == 0)
        return err::obj(1, "input value");
    
    for(auto a : __text)
        if (!isalpha(a))
            return err::obj(2, "only alpha characters are allowed");
    
    return err::obj(0);
}

void
page::user::on_user_text_changed()
{
    std::string __text = __user_entry.get_text();
    err::obj __e = is_valid(__text);
    if (__e.status() != 0) {
        __user_entry.override_background_color(Gdk::RGBA("#ff0000"));
        is_user_valid = false;
    }
        
    else {
        is_user_valid = true;
        __user_entry.override_background_color(Gdk::RGBA("#ffffff"));
    }

    __next_btn.set_sensitive(is_pass_valid && is_user_valid);
        
}


void
page::user::on_pass_text_changed()
{
    std::string __text = __pass_entry.get_text();
    err::obj __e = is_valid(__text);
    if (__e.status() != 0) {
        __pass_entry.override_background_color(Gdk::RGBA("#ff0000"));
        is_pass_valid = false;
    }
        
    else {
        is_pass_valid = true;
        __pass_entry.override_background_color(Gdk::RGBA("#ffffff"));
    }

    __next_btn.set_sensitive(is_pass_valid && is_user_valid);
        
}