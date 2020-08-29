#include <opportunity.hh>

using namespace opportunity;

std::map<std::string, std::string> language {
    {"English", "en_IN.UTF-8"},
    {"Hindi", "hi_IN.UTF-8"},
};


std::string 
opportunity::get_locale(const std::string& l)
{
    if (language.find(l) != language.end()) {
        return language[l];
    }
    return "en_IN.UTF-8";
}

void
window::setup_lang_page()
{
    lang_page.show_all();
    stack.add(lang_page, "lang_page","lang_page");
    lang_page.lang_box.signal_row_selected().connect(sigc::mem_fun(*this, &window::on_lang_select));
    lang_page.next_btn.signal_clicked().connect(sigc::mem_fun(*this, &window::on_lang_next_click));
    lang_page.back_btn.signal_clicked().connect(sigc::mem_fun(*this, &window::on_lang_back_click));

}

page::lang::lang()
{
    heading.set_text("Language");
    subheading.set_text("select your native language for better experience");
    set_spacing(10);
    
    for(auto a : language) {
        auto l = new Gtk::Label();  
        l->set_text(a.first);
        l->show();
        l->override_font(Pango::FontDescription("Open Sans Light 12"));
        l->set_margin_top(5);
        l->set_margin_bottom(5);
        lang_box.add(*l);
    }

    lang_box.set_margin_top(30);

    back_btn.set_label(_T("Back"));
    bbox.add(back_btn);

    next_btn.set_label(_T("Next"));
    bbox.add(next_btn);
    bbox.set_margin_bottom(40);
    bbox.set_layout(Gtk::BUTTONBOX_CENTER);
    next_btn.show();
    back_btn.show();

    lang_box.show();
    content.pack_start(lang_box, true, true);
}