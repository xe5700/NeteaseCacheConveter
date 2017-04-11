#include "stdafx.h"
#include "CPP_CURL.h"

size_t get(char *ptr, size_t size, size_t nmemb, void *userdata) {
	CurlDownloader* dl = (CurlDownloader*)userdata;
	size_t realsize = size*nmemb;
	while (realsize + dl->nowsize >= dl->infosize) {
		dl->infosize = dl->infosize * 2;
		dl->info = (char*)realloc(dl->info, dl->infosize);
	}
	strcat(dl->info, ptr);
	dl->nowsize += realsize;
	return realsize;
}

CURL* Curl::getCURL() {
	return CCURL;
}
char* Curl::outputDownloadData() {
	return dl.info;
}
void Curl::initDownloadMode() {
	if (dl.info != NULL)free(dl.info);
	dl.infosize = sizeof(char) * 1000;
	dl.info = (char*)malloc(dl.infosize);
	strcpy(dl.info, "");
	dl.nowsize = 0;
	curl_easy_setopt(CCURL, CURLOPT_WRITEFUNCTION, get);
	curl_easy_setopt(CCURL, CURLOPT_WRITEDATA, &dl);
}
void Curl::cleanup() {
	if (dl.info != NULL)free(dl.info);
	dl.nowsize = 0;
	dl.infosize = 0;
	curl_easy_cleanup(CCURL);
	CCURL = curl_easy_init();
}
void Curl::setopt(CURLoption options, ...) {
	curl_easy_setopt(CCURL, options);
}
Curl::~Curl() {
	if (dl.info != NULL)free(dl.info);
	curl_easy_cleanup(CCURL);
}
Curl::Curl() {
	CCURL = curl_easy_init();
}
CURLcode Curl::getinfo(CURLINFO info, ...) {
	return curl_easy_getinfo(CCURL, info);
}
CURLcode Curl::perform() {
	return curl_easy_perform(CCURL);
}
