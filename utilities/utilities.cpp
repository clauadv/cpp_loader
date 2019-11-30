#include "utilities.h"

std::string utilities::get_data( std::string url, std::string data ) {
	CURL * curl;
	CURLcode res;
	std::string ret;

	curl = curl_easy_init( );
	url = url + "?" + data;

	if ( curl ) {
		curl_easy_setopt( curl, CURLOPT_URL, url.c_str( ) );
		curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_callback );
		curl_easy_setopt( curl, CURLOPT_WRITEDATA, &ret );
		res = curl_easy_perform( curl );
		curl_easy_cleanup( curl );
	}

	return ret;
}

std::vector<std::string> utilities::result( std::string & string, char split ) {
	std::vector<std::string> res;
	std::istringstream iss( string );

	for ( std::string token; getline( iss, token, split ); )
		res.push_back( move( token ) );

	return res;
}

void utilities::toggle_text( ) {
	void * _stdin = GetStdHandle( STD_INPUT_HANDLE );
	DWORD mode = 0;

	GetConsoleMode( _stdin, &mode );
	SetConsoleMode( _stdin, mode & ( ~ENABLE_ECHO_INPUT ) );
}

void utilities::setup_console( std::string name, int w, int h ) {
	SetConsoleTitleA( name.c_str( ) );
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 12 );

	HWND console = GetConsoleWindow( );
	RECT r;
	GetWindowRect( console, &r );
	MoveWindow( console, r.left, r.top, w, h, TRUE );
}