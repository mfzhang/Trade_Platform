// CTradeProject.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Normaliz.lib")
#pragma comment(lib,"libeay32.lib")
#pragma comment(lib,"ssleay32.lib")

//#include "../libcurl-vc15-x64-release-static-ipv6-sspi-winssl/include/curl/curl.h"
#include <curl/curl.h>
#include <string>
#include <iostream>
#include "Basements.h"
#include "TTEX_Connection.h"
#include "OKEx_Connection.h"
#include <vector>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
	
	CURL *curl = nullptr;
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	
	if (curl)
	{
		OKEx_Rest_Connection okex_rest_connection(curl, "", "");
		cout << (-5) % 3 << endl;
	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();
	
	return 0;
}

