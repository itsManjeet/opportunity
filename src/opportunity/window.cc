#include <opportunity.hh>

using namespace opportunity;

window::window() {

    __header__.set_show_close_button(false);
    __header__.hide();

    setup_welcome_screen();
    setup_lang_page();
    setup_disk_page();
    setup_efi_page();
    setup_install_page();
    setup_finish_page();

    __dispatcher.connect(sigc::mem_fun(*this, &window::on_notify_from_installer));

    stack.set_margin_top(20);
    stack.set_margin_bottom(20);
    // stack.set_margin_start(50);
    // stack.set_margin_end(50);

    stack.show();
    //fullscreen();
    set_default_size(1000, 700);
    

    __bg_img.set(__DATA_DIR__ "bg.jpg");
    __bg_img.set_valign(Gtk::ALIGN_START);
    __overlay.add(__bg_img);
    __overlay.add_overlay(stack);
    __container__.add(__overlay);

    __overlay.show_all();

    stack.set_visible_child("welcome_page");
    stack.set_transition_duration(600);
    stack.set_transition_type(Gtk::STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
}

void
window::setup_welcome_screen()
{
    r_welc_box.set_vexpand(true);
    r_welc_box.set_hexpand(true);
    r_welc_box.set_halign(Gtk::ALIGN_CENTER);
    r_welc_box.set_valign(Gtk::ALIGN_CENTER);

    r_welc_box.set_orientation(Gtk::ORIENTATION_VERTICAL);

    r_welc_logo.set(Gdk::Pixbuf::create_from_file(io::sprint(__DATA_DIR__,"logo.png")));

    r_welc_label.set_text("releax os");
    r_welc_sub_label.set_text(_T("click 'continue' to enter the world of opensource"));

    r_welc_label.override_font(Pango::FontDescription("Sans Light 20"));
    r_welc_sub_label.override_font(Pango::FontDescription("Sans Light 12"));
    
    r_cont_btn.set_margin_top(50);
    r_cont_btn.set_image_from_icon_name("next",Gtk::ICON_SIZE_BUTTON);

    r_cont_btn.set_hexpand(false);
    r_cont_btn.set_halign(Gtk::ALIGN_CENTER);
    r_cont_btn.signal_clicked().connect(sigc::mem_fun(*this, &window::on_cont_btn_click));


    r_welc_box.add(r_welc_logo);
    r_welc_box.add(r_welc_label);
    r_welc_box.add(r_welc_sub_label);

    r_welc_box.add(r_cont_btn);

    r_welc_box.show_all();

    stack.add(r_welc_box, "welcome_page","welcome_page");
}