#ifndef __OPPORTUNITY__
#define __OPPORTUNITY__

#include <releax/releax>
#include <releax/config>
#include <glibmm.h>
#include <gtkmm.h>
#include <gdkmm.h>
#include <mutex>
#include <thread>

#include <releax/ui>

#include "config.hh"


using namespace releax;

namespace opportunity {
    struct data {
        std::string locale;
        std::string root_disk;
        std::string efi_disk;
        bool efi = false;
    };

    class window;

    std::string get_locale(const std::string& l);
    namespace util {
        class diskbtn : public Gtk::Box {
            protected:
                Gtk::Image __img;
                Gtk::Label __title;
                Gtk::Label __sub;
                Gtk::Box __box;

            public:
                diskbtn(const std::string& head, const std::string& sub);
        };

        std::string humanify_size(long __s);

        class efi_combo : Gtk::Dialog {
            public:
                efi_combo();
        };

        class installer {
            private:
                mutable std::mutex __mutex;
                
                data* __d;
            
            public:
                installer() : __mutex(),
                    __mesg(), __progress(0.0) {}

                void set_data(data* __d) { __d = this->__d;}                
                void start(window*, data*);
                void stop();
                bool __stop = false;
                bool __error = true;
                double __progress;
                std::string __mesg;

                void get_data(double*, std::string*) const;
        };
    }
    namespace page {
        class lang : public ui::page {
            public:
                Gtk::Button next_btn, back_btn;
                Gtk::ListBox lang_box;
                lang();
        };


        class disk : public ui::page {
            public:
                Gtk::Button next_btn, back_btn;
                Gtk::FlowBox part_box;
                disk();
        };

        class install : public ui::page {
            public:
                Gtk::Button install_btn, back_btn;
                Gtk::ProgressBar progress_bar;
                Gtk::Label mesg_label;
                install();
        };

        class finish : public ui::page {
            public:
                Gtk::Button reboot;
                Gtk::Label finish_label;
                finish();
        };
 
    }



    class window : public ui::simple {
        protected:
            data install_data;
            Gtk::Image r_welc_logo;
            Gtk::Label r_welc_label, r_welc_sub_label;
            Gtk::Button r_cont_btn;


            Gtk::Box r_welc_box;

            Gtk::Overlay __overlay;
            Gtk::Image __bg_img;
            Gtk::Stack stack;

            page::lang lang_page;
            page::disk disk_page, efi_page;
            page::install install_page;
            page::finish finish_page;


            util::installer __installer;
            std::thread* __work_thread = nullptr;
            Glib::Dispatcher __dispatcher;

            void setup_welcome_screen();
            void setup_lang_page();
            void setup_disk_page();
            void setup_efi_page();
            void setup_install_page();
            void setup_finish_page();


            void on_cont_btn_click();
            void on_lang_select(Gtk::ListBoxRow* r);

            void on_lang_next_click();
            void on_lang_back_click();

            void on_disk_next_click();
            void on_disk_back_click();

            void on_efi_next_click();
            void on_efi_back_click();

            void on_disk_select(Gtk::FlowBoxChild*);
            void on_efi_select(Gtk::FlowBoxChild*);

            void on_install_btn_click();
            void on_install_back_click();

            void on_notify_from_installer();
            void update_progress();

        public:
            window();

            void notify();

    };
}


#endif