#include <opportunity.hh>

using namespace opportunity;

std::map<int, std::string> size_mesr = {
    {0, "Bytes"},
    {1, "KiB"},
    {2, "MiB"},
    {3, "GiB"},
    {4, "TiB"},
    {5, "PiB"},
};

std::string
util::humanify_size(long __size)
{
    int x = 0;
    double __c_size = (double) __size;

    __c_size = __c_size * 512;

    while (__c_size > 1024) {
        __c_size /= 1024;
        x++;
    }

    __c_size = std::ceil(__c_size * 100) / 100;
    return io::sprint(__c_size, " ", size_mesr[x]);
}


util::diskbtn::diskbtn(const std::string& head, const std::string& sub)
{
    __img = Gtk::Image("drive-harddisk",Gtk::ICON_SIZE_DIALOG);
    
    __title.set_text(head);
    __title.override_font(Pango::FontDescription("Open Sans Light 13"));
    __sub.set_text(sub);
    __sub.override_font(Pango::FontDescription("Open Sans Light 8"));
    

    __box.set_orientation(Gtk::ORIENTATION_VERTICAL);
    __box.set_spacing(3);

    add(__box);
    __box.add(__img);
    __box.add(__title);
    __box.add(__sub);

    __box.show_all();

}
