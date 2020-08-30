#include <namaste/namaste.hh>
#include <namaste/config.hh>

namaste::window::window()
{
    __header__.hide();
    __header__.set_show_close_button(false);
    __container__.add(__overlay);

    __container__.set_margin_start(0);
    __container__.set_margin_bottom(0);

    __bg_img.set(__DATA_DIR__ "/bg.jpg");
    __bg_img.set_valign(Gtk::ALIGN_START);
    __overlay.add(__bg_img);
    __overlay.add_overlay(__stack);


    __stack.set_transition_duration(500);
    __stack.set_transition_type(Gtk::STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    
    __stack.set_margin_top(10);
    __stack.set_margin_bottom(20);
    // __stack.set_margin_start(50);
    // __stack.set_margin_end(50);

    __setup_welcome_page();
    __setup_user_page();
    __setup_timezone_page();
    __setup_getstart_page();

    __stack.show();
    __overlay.show_all();

    set_default_size(1000, 700);

    set_skip_taskbar_hint();
    set_skip_pager_hint();

    //fullscreen();
}