#include <namaste/namaste.hh>

using namespace namaste;

void
window::__exit_error(const std::string& err)
{
    Gtk::MessageDialog __err_mesg(err);
    int r = __err_mesg.run();

    this->get_application()->quit();
}