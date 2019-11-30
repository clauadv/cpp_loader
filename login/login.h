#pragma once
#include <string>
#include <iostream>
#include <algorithm>

#include "../utilities/utilities.h"
#include "../utilities/xor.h"

namespace login {
	extern std::string username, password, hwid, token;

	void get_credentials( );
	void check_data( );
	void fail_response( int code );
	void good_response( int code );
}