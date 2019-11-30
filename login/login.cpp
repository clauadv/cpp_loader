#include "login.h"
#include <thread>
#include <string>

int status_code = -1;

void login::get_credentials( ) {
	bool empty_fields = false;
	while ( username.empty( ) || password.empty( ) ) {
		if ( empty_fields )
			std::cout << xorstr_( "\nfill all the empty fields.\n" ) << std::endl;

		std::cout << xorstr_( "[cheat name] username: " );
		std::getline( std::cin, username );

		std::cout << xorstr_( "[cheat name] password: " );
		utilities::toggle_text( );
		std::getline( std::cin, password );

		if ( !empty_fields )
			empty_fields = true;
	}
	std::string stored_hwid = std::to_string( ( int ) utilities::find_hwid() );
	hwid = stored_hwid;
}

void login::check_data( ) {
	std::string result = utilities::get_data( xorstr_( "http://localhost/check.php" ), xorstr_( "username=" ) + username + xorstr_( "&password=" ) + password + xorstr_( "&hwid=" ) + hwid );
	if ( result.length( ) > 3 ) {
		std::vector<std::string> data = utilities::result( result, ':' );
		try {
			status_code = std::stoi( data[ 0 ] );
		} catch ( std::invalid_argument & e ) {
		}

		good_response( status_code );
		token = data [ 1 ];
	} else {
		std::string data = result;
		try {
			status_code = std::stoi( data );
		} catch ( std::invalid_argument & e ) {
			std::abort( );
		}

		fail_response( status_code );
	}
}

void login::fail_response( int code ) {
	switch ( code ) {
	case 339:
		std::cout << xorstr_( "\n\nuser doesn't exist" ) << std::endl;
		break;
	case 215:
		std::cout << xorstr_( "\n\nincorrect password" ) << std::endl;
		break;
	case 253:
		std::cout << xorstr_( "\n\nhwid doesn't match" ) << std::endl;
		break;
	default:
		std::cout << xorstr_( "\n\nan unknown error occured" ) << std::endl;
		break;
	}
}

void login::good_response( int code ) {
	switch ( code ) {
	case 633:
		// first login
		break;
	case 951:
		std::cout << xorstr_( "\n\nwelcome back, " ) << username << xorstr_( "." ) << std::endl;
		std::cout << xorstr_( "your session expires in 1 minute." ) << std::endl;

		std::this_thread::sleep_for( std::chrono::minutes( 1 ) );
		std::abort( );
		break;
	default:
		std::cout << xorstr_( "\nan unknown error occured" ) << std::endl;
		break;
	}
}

namespace login {
	std::string username, password, hwid, token;
}
