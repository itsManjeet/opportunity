#include <opportunity.hh>

int main(int ac, char** av) {
	int status = 0;
	ui::obj opportunity_app (ac, av);

	opportunity_app.id(__APP_ID__)
				  .ver(__VERSION__)
				  .about(__APP_ABOUT__)
				  .with_locale(__LOCALE_DIR);

	opportunity_app.add_window(new opportunity::window());

	status = opportunity_app.run().status();	

	return status;
}