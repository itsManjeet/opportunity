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
window::on_timezone_next_click()
{
    __stack.set_visible_child("getstart_page");
}

void
window::on_get_start_click()
{
    this->get_application()->quit();
}