#pragma once
#include "../includes.h"

class web_class {
public:
	auto get_data(std::string url, unsigned long& sz, std::string communication = "")->std::string;
};

extern web_class* web;