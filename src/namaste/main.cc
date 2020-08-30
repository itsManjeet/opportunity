#include <namaste/namaste.hh>
#include <namaste/config.hh>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

//#define __LOGGING__

int
main(int ac, char** av)
{

    ui::obj __namaste(ac, av);
    __namaste.id(__APP_ID__)
             .ver(__APP_VER__)
             .about(__APP_ABOUT__);

    #ifdef __LOGGING__
        int log_out = open(
            io::sprint("/tmp/",__APP_ID__,".log").c_str(),
            O_RDWR | O_CREAT | O_APPEND, 0600
        );

        int save_out = dup(fileno(stdout));
        int save_err = dup(fileno(stderr));

        dup2(log_out, fileno(stdout));
        dup2(log_out, fileno(stderr));

    #endif

    __namaste.add_window(new namaste::window);

    io::log("initializing '",__APP_ID__,"'");

    int r = __namaste.run().status();

    #ifdef __LOGGING__
        fflush(stdout); close(log_out);
        fflush(stderr);
        dup2(save_err, fileno(stderr));
        dup2(save_out, fileno(stdout));
        close(save_out);
        close(save_err);

    #endif

    return r;
}