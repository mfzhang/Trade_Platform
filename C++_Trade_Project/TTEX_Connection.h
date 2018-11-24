#pragma once
#include<string>
//#include "../libcurl-vc15-x64-release-static-ipv6-sspi-winssl/include/curl/curl.h"
#include <curl/curl.h>
#include "Basements.h"
#include <sstream>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Normaliz.lib")


using namespace std;

class TTEX_Rest_Connection
{
protected:
	CURL* curl;
	string api_key;
	string secret_key;
private:
	Basements basements;
	stringstream sstream;
	MemoryStruct memorystruct;
public:
	TTEX_Rest_Connection();
	TTEX_Rest_Connection(CURL* curl,string api_key, string secret_key);
	~TTEX_Rest_Connection();
	string Get_Ticker(string symbol);
	string Get_Depth(string symbol, int size);
	string Send_Order(string symbol, string type, double price, double amount, bool if_use_market_price);
	string Get_Entrust(string symbol, bool if_get_history_entrust);
	string Get_OrderInfo(string txNo);
	string Cancel_Order(string txNo);
	string Get_AccountInfo();
};