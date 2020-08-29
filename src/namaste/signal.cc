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
    __stack.set_visible_child("timezone_page");
}

void
window::on_timezone_next_click()
{
    __stack.set_visible_child("getstart_page");
}