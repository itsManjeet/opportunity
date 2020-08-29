#include <namaste/namaste.hh>
#include <namaste/config.hh>

int
main(int ac, char** av)
{
    ui::obj __namaste(ac, av);
    __namaste.id(__APP_ID__)
             .ver(__APP_VER__)
             .about(__APP_ABOUT__);

    __namaste.add_window(new namaste::window);

    return __namaste.run().status();
}