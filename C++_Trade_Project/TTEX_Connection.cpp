#include "stdafx.h"
#include <string>
#include <curl/curl.h>
//#include "../libcurl-vc15-x64-release-static-ipv6-sspi-winssl/include/curl/curl.h"
#include "TTEX_Connection.h"
#include "Basements.h"
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "openssl/HMAC.h"
#include "ctime";

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Normaliz.lib")

using namespace std;
using namespace rapidjson;



TTEX_Rest_Connection::TTEX_Rest_Connection()
{
	this->sstream.precision(15);
}

TTEX_Rest_Connection::TTEX_Rest_Connection(CURL* curl, string api_key, string secret_key)
{
	this->curl = curl;
	this->api_key = api_key;
	this->secret_key = secret_key;
	this->sstream.precision(15);
}

TTEX_Rest_Connection::~TTEX_Rest_Connection()
{

}

string TTEX_Rest_Connection::Get_Ticker(string symbol)
{
	string base = "https://api.ttex.com/v1/market/tickers?symbol=";
	string url = base + symbol;
	string w_r_s = this->basements.General_Request_Return(this->curl,url.c_str(),GET,memorystruct);
	return w_r_s;
}

string TTEX_Rest_Connection::Get_Depth(string symbol, int size)
{
	string base = "https://api.ttex.com/v1/market/depth?symbol=";
	string size_string = Num_To_String<int>(size, &sstream);
	string url = base + symbol + "&size=" + size_string;
	string raw_return=this->basements.General_Request_Return(this->curl,url.c_str(), GET, memorystruct);
	return raw_return;
}

string TTEX_Rest_Connection::Send_Order(string symbol, string type, double price, double amount, bool if_use_market_price)
{
	if ((type == "buy") || (type == "sell"))
	{
		string base = "https://api.ttex.com/currency/trade/";
		if (if_use_market_price)
		{
			base = base + "market/";
		}
		base = base + type + "?";
		string to_be_signed = "accessKey=" + this->api_key;
		to_be_signed = to_be_signed + "&num=" + Num_To_String<double>(amount, &sstream);
		if (!if_use_market_price)
		{
			to_be_signed = to_be_signed + "&price=" + Num_To_String<double>(price, &sstream);
		}
		to_be_signed = to_be_signed + "&symbol=" + symbol;

		const char* s_k_0 = this->secret_key.c_str();
		unsigned char* s_k = (unsigned char*)(s_k_0);
		const char* data_to_be_signed_0 = to_be_signed.c_str();
		unsigned char* data_to_be_signed = (unsigned char*)(data_to_be_signed_0);
		unsigned char* result = HMAC(EVP_md5(), s_k, strlen(s_k_0), data_to_be_signed, strlen(data_to_be_signed_0), NULL, NULL);
		const char* result_0 = (const char*)result;
		char re[33];
		for (int i = 0; i < 16; i++)
		{
			int x = (int)result[i] / 16;
			int y = (int)result[i] % 16;
			if (x < 10)
			{
				re[i * 2] = (char)(x + 48);
			}
			else
			{
				re[i * 2] = (char)(x + 87);
			}
			if (y < 10)
			{
				re[i * 2 + 1] = (char)(y + 48);
			}
			else
			{
				re[i * 2 + 1] = (char)(y + 87);
			}
		}
		re[32] = '\0';
		string re_2 = "&sign=";
		string sign_re = re;
		string re_3 = "&reqTime=";
		time_t tre = time(NULL) * 1000;
		string tm_re = Num_To_String(tre, &sstream);
		string final_url = base + to_be_signed + re_2 + sign_re + re_3 + tm_re;

		string raw_return = this->basements.General_Request_Return(this->curl, final_url.c_str(),GET,memorystruct);
		return raw_return;
	}
}

string TTEX_Rest_Connection::Get_Entrust(string symbol, bool if_get_history_entrust)
{
	string base = "https://api.ttex.com/currency/trade/";
	if (if_get_history_entrust)
	{
		base = base + "findHistoryEntrust?";
	}
	else
	{
		base = base + "findEntrust?";
	}
	string to_be_signed = "accessKey=" + this->api_key;
	to_be_signed = to_be_signed + "&symbol=" + symbol;

	const char* s_k_0 = this->secret_key.c_str();
	unsigned char* s_k = (unsigned char*)(s_k_0);
	const char* data_to_be_signed_0 = to_be_signed.c_str();
	unsigned char* data_to_be_signed = (unsigned char*)(data_to_be_signed_0);
	unsigned char* result = HMAC(EVP_md5(), s_k, strlen(s_k_0), data_to_be_signed, strlen(data_to_be_signed_0), NULL, NULL);
	const char* result_0 = (const char*)result;
	char re[33];
	for (int i = 0; i < 16; i++)
	{
		int x = (int)result[i] / 16;
		int y = (int)result[i] % 16;
		if (x < 10)
		{
			re[i * 2] = (char)(x + 48);
		}
		else
		{
			re[i * 2] = (char)(x + 87);
		}
		if (y < 10)
		{
			re[i * 2 + 1] = (char)(y + 48);
		}
		else
		{
			re[i * 2 + 1] = (char)(y + 87);
		}
	}
	re[32] = '\0';
	string re_2 = "&sign=";
	string sign_re = re;
	string re_3 = "&reqTime=";
	time_t tre = time(NULL) * 1000;
	string tm_re = Num_To_String(tre, &sstream);
	string final_url = base + to_be_signed + re_2 + sign_re + re_3 + tm_re;

	string raw_return = this->basements.General_Request_Return(this->curl, final_url.c_str(),GET,memorystruct);
	return raw_return;
}

string TTEX_Rest_Connection::Get_OrderInfo(string txNo)
{
	string base = "https://api.ttex.com/currency/trade/findOrder?";
	string to_be_signed = "accessKey=" + this->api_key;
	to_be_signed = to_be_signed + "&txNo=" + txNo;

	const char* s_k_0 = this->secret_key.c_str();
	unsigned char* s_k = (unsigned char*)(s_k_0);
	const char* data_to_be_signed_0 = to_be_signed.c_str();
	unsigned char* data_to_be_signed = (unsigned char*)(data_to_be_signed_0);
	unsigned char* result = HMAC(EVP_md5(), s_k, strlen(s_k_0), data_to_be_signed, strlen(data_to_be_signed_0), NULL, NULL);
	const char* result_0 = (const char*)result;
	char re[33];
	for (int i = 0; i < 16; i++)
	{
		int x = (int)result[i] / 16;
		int y = (int)result[i] % 16;
		if (x < 10)
		{
			re[i * 2] = (char)(x + 48);
		}
		else
		{
			re[i * 2] = (char)(x + 87);
		}
		if (y < 10)
		{
			re[i * 2 + 1] = (char)(y + 48);
		}
		else
		{
			re[i * 2 + 1] = (char)(y + 87);
		}
	}
	re[32] = '\0';
	string re_2 = "&sign=";
	string sign_re = re;
	string re_3 = "&reqTime=";
	time_t tre = time(NULL) * 1000;
	string tm_re = Num_To_String(tre, &sstream);
	string final_url = base + to_be_signed + re_2 + sign_re + re_3 + tm_re;

	string raw_return = this->basements.General_Request_Return(this->curl, final_url.c_str(),GET,memorystruct);
	return raw_return;
}

string TTEX_Rest_Connection::Cancel_Order(string txNo)
{
	string base = "https://api.ttex.com/currency/trade/cancel?";
	string to_be_signed = "accessKey=" + this->api_key;
	to_be_signed = to_be_signed + "&txNo=" + txNo;

	const char* s_k_0 = this->secret_key.c_str();
	unsigned char* s_k = (unsigned char*)(s_k_0);
	const char* data_to_be_signed_0 = to_be_signed.c_str();
	unsigned char* data_to_be_signed = (unsigned char*)(data_to_be_signed_0);
	unsigned char* result = HMAC(EVP_md5(), s_k, strlen(s_k_0), data_to_be_signed, strlen(data_to_be_signed_0), NULL, NULL);
	const char* result_0 = (const char*)result;
	char re[33];
	for (int i = 0; i < 16; i++)
	{
		int x = (int)result[i] / 16;
		int y = (int)result[i] % 16;
		if (x < 10)
		{
			re[i * 2] = (char)(x + 48);
		}
		else
		{
			re[i * 2] = (char)(x + 87);
		}
		if (y < 10)
		{
			re[i * 2 + 1] = (char)(y + 48);
		}
		else
		{
			re[i * 2 + 1] = (char)(y + 87);
		}
	}
	re[32] = '\0';
	string re_2 = "&sign=";
	string sign_re = re;
	string re_3 = "&reqTime=";
	time_t tre = time(NULL) * 1000;
	string tm_re = Num_To_String(tre, &sstream);
	string final_url = base + to_be_signed + re_2 + sign_re + re_3 + tm_re;

	string raw_return = this->basements.General_Request_Return(this->curl, final_url.c_str(),GET,memorystruct);
	return raw_return;
}

string TTEX_Rest_Connection::Get_AccountInfo()
{
	string base = "https://api.ttex.com/member/getAccount?";
	const char* s_k_0 = this->secret_key.c_str();
	unsigned char* s_k = (unsigned char*)(s_k_0);
	string pre = "accessKey=";
	string data_pre = pre + this->api_key;
	const char* data_to_be_signed_0 = data_pre.c_str();
	unsigned char* data_to_be_signed = (unsigned char*)(data_to_be_signed_0);
	unsigned char* result = HMAC(EVP_md5(), s_k, strlen(s_k_0), data_to_be_signed, strlen(data_to_be_signed_0), NULL, NULL);
	const char* result_0 = (const char*)result;
	char re[33];
	for (int i = 0; i < 16; i++)
	{
		int x = (int)result[i] / 16;
		int y = (int)result[i] % 16;
		if (x < 10)
		{
			re[i * 2] = (char)(x + 48);
		}
		else
		{
			re[i * 2] = (char)(x + 87);
		}
		if (y < 10)
		{
			re[i * 2+1] = (char)(y + 48);
		}
		else
		{
			re[i * 2+1] = (char)(y + 87);
		}
	}
	re[32] = '\0';
	string re_2 = "&sign=";
	string sign_re = re;
	string re_3 = "&reqTime=";
	time_t tre = time(NULL)*1000;
	string tm_re = Num_To_String(tre, &sstream);
	string final_url = base + data_pre + re_2 + sign_re + re_3 + tm_re;
	string raw_return=this->basements.General_Request_Return(this->curl,final_url.c_str(),GET,memorystruct);
	return raw_return;
}