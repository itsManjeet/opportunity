#ifndef __NAMASTE__
#define __NAMASTE__

#include <releax/releax>
#include <releax/config>
#include <gtkmm.h>
#include <glibmm.h>
#include <pangomm.h>
#include <sigc++/sigc++.h>
#include <releax/ui>

using namespace releax;

namespace namaste {

    namespace page {
        class welcome : public ui::page {
            protected:
                Gtk::Label __releax_label, __sub_label;
                Gtk::Image __logo_img;

            public:
                welcome();
                Gtk::Button __next_btn;
        };

        class user : public ui::page {
            protected:
                Gtk::Entry __user_entry;
                Gtk::Entry __pass_entry;
                Gtk::Label __user_label, __pass_label;
                Gtk::Grid __user_grid;

            
            public:
                Gtk::Button __next_btn;
                user();

                std::string __get_user_id();
                std::string __get_enc_passwd();
        };

        class timezone : public ui::page {
            protected:
                Gtk::ScrolledWindow __scroll_window;
                // Gtk::Viewport __scroll_viewport;
                Gtk::ListBox __timezone_listbox;

            public:
                timezone();
                Gtk::Button __next_btn, __back_btn;
        };

        class getstart : public ui::page {
            protected:
                Gtk::Label __releax_label;

            public:
                getstart();
                Gtk::Button __get_start;
        };
    }

    class window : public ui::simple {
        protected:
            page::welcome __welcome_page;
            page::user __user_page;
            page::timezone __timezone_page;
            page::getstart __getstart_page;
            

            Gtk::Stack __stack;
            Gtk::Overlay __overlay;
            Gtk::Image __bg_img;

            void __setup_welcome_page();
            void __setup_user_page();
            void __setup_timezone_page();
            void __setup_getstart_page();

            void on_wel_next_click();
            void on_user_next_click();
            void on_timezone_next_click();
            void on_get_start_click();

            void __exit_error(const std::string& err);

        public:
            window();
    };
}


#endif