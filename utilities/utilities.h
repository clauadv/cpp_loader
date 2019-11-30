#pragma once
#include <string>
#include <curl.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <TlHelp32.h>

#pragma comment (lib, "C:\\libcurl\\libcurl_a.lib" )

namespace utilities {
	std::string get_data( std::string url, std::string data );
	std::vector<std::string> result( std::string & string, char split );
	void toggle_text( );
	void setup_console( std::string name, int w, int h );
	
	static size_t write_callback( void * contents, size_t size, size_t nmemb, void * userp ) {
		( ( std::string * ) userp )->append( ( char * ) contents, size * nmemb );
		return size * nmemb;
	}

	inline unsigned long find_hwid( void ) {
		unsigned long hwid = 0;
		GetVolumeInformationA( "C:\\", nullptr, 255, &hwid, nullptr, nullptr, nullptr, 255 );
		return hwid;
	}
}