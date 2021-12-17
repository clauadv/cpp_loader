#pragma once
#include "../includes.h"

namespace web {
	static std::size_t write_callback(void* contents, const std::size_t size, const std::size_t nmemb, void* userp) {
		static_cast<std::string*>(userp)->append(static_cast<char*>(contents), size * nmemb);
		return size * nmemb;
	}

	inline std::string get_data(std::string url, const std::string& communication = "") {
		std::string ret;

		url += "?" + communication;

		auto curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url.data());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);

			curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}

		return ret;
	}
}