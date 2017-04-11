#pragma once
#include "stdafx.h"
enum CurlDownloadMode
{

};
struct CurlDownloader
{
	char* info;
	bool finish;
	size_t infosize;
	size_t nowsize;
	UINT mode;
};
 class Curl {
	private:
		CURL* CCURL;
		CurlDownloader dl;
	public:
		CURL* getCURL();
		char* outputDownloadData();
		void initDownloadMode();
		void cleanup();
		void setopt(CURLoption options, ...);
		~Curl();
		Curl();
		CURLcode getinfo(CURLINFO info, ...);
		CURLcode perform();
};