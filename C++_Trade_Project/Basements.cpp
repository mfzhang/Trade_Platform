#pragma once
#include "stdafx.h"
#include<iostream>
//#include "../libcurl-vc15-x64-release-static-ipv6-sspi-winssl/include/curl/curl.h"
#include <curl/curl.h>
#include<string>
#include "Basements.h"
#include<vector>
#include "../openssl/include/openssl/md5.h"
#include <fstream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Normaliz.lib")

using namespace std;

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;
	if (realsize > mem->size)
	{
		free(mem->memory);
		mem->memory = (char*)malloc(realsize + 1);
		mem->size = realsize;
	}
	if (mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}
	memcpy(mem->memory, contents, realsize);
	mem->memory[realsize] = 0;
	return realsize;
}

char* Basements::General_Request_Return(CURL *curl,const char* url,Http_Method http_method,MemoryStruct& memorystruct)
{
	if (curl != nullptr)
	{
		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, true);
		if (http_method == POST)
		{
			curl_easy_setopt(curl, CURLOPT_POST, 1L);
		}
		else
			if (http_method == GET)
			{
				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
			}
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&memorystruct);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		return memorystruct.memory;
	}
	else
	{
		return "curl_null";
	}
}

string& Basements::Map_To_OrderedString(string& the_string,map<string, int>& the_map)
{
	the_string = "";
	for (map<string, int>::iterator iter = the_map.begin(); iter != the_map.end(); ++iter)
	{
		if (the_string == "")
		{
			the_string = the_string + iter->first;
		}
		else
		{
			the_string = the_string + "&" + iter->first;
		}
	}
	return the_string;
}

string Basements::MD5_32Bit_Capital(string& to_be_hased)
{
	const char* to_be_hased_1 = to_be_hased.c_str();
	unsigned char* to_be_hased_2 = (unsigned char*)to_be_hased_1;
	MD5_CTX ctx;
	unsigned char md5[16] = { 0 };
	MD5_Init(&ctx);
	MD5_Update(&ctx, to_be_hased_2, strlen(to_be_hased_1));
	MD5_Final(md5, &ctx);
	char re[33];
	for (int i = 0; i < 16; i++)
	{
		int x = (int)md5[i] / 16;
		int y = (int)md5[i] % 16;
		if (x < 10)
		{
			re[i * 2] = (char)(x + 48);
		}
		else
		{
			re[i * 2] = (char)(x + 55);
		}
		if (y < 10)
		{
			re[i * 2 + 1] = (char)(y + 48);
		}
		else
		{
			re[i * 2 + 1] = (char)(y + 55);
		}
	}
	re[32] = '\0';
	string result = re;
	return result;
}

vector<string> split(string& s, char seperator)
{
	vector<string> result;
	int position_1=0;
	int position_2;
	int len = s.length();
	do
	{
		while ((position_1 < len) && (s[position_1] == seperator)) position_1++;
		position_2 = position_1 + 1;
		while ((position_2 < len) && (s[position_2] != seperator))
		{
			position_2++;
		}
		if (position_1 < len)
		{
			result.push_back(s.substr(position_1, position_2 - position_1));
			position_1 = position_2 + 1;
		}
	} while (position_1 < len);
    return result;
}

char* Read_Whole_File(char* buffer,const char* file_path)
{
	ifstream fin(file_path);
	if (!fin)
	{
		cout << "open error int \"Read_Whole_File\":"<<file_path << endl;
		return buffer;
	}
	int i = 0;
	char c;
	while (fin >> c)
	{
		buffer[i] = c;
		i++;
	}
	buffer[i] = '\0';
	return buffer;
}