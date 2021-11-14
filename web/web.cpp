#include "web.h"

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
	((std::string*) userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

auto web_class::get_data(std::string url, unsigned long& sz, std::string communication) -> std::string {
	std::string ret;
	CURL* curl;
	CURLcode res;

	url += "?" + communication;

	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.data());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);
		res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);
	}

	sz = ret.size();

	return ret;
}

web_class* web;