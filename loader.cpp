#include "login/login.h"

int main( ) {
	utilities::setup_console( xorstr_( "[cheat name]" ), 400, 200 );
	_beginthread( ( _beginthread_proc_type ) security::anti_program_thread, 0, nullptr );

	login::get_credentials( );
	login::check_data( );

	std::getchar( );
	std::getchar( );
	return 0;
}