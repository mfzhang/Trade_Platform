#pragma once
#include "stdafx.h"
#include<iostream>
//#include "../libcurl-vc15-x64-release-static-ipv6-sspi-winssl/include/curl/curl.h"
#include <curl/curl.h>
#include<string>
#include<sstream>
#include<vector>
#include<map>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/schema.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Normaliz.lib")

using namespace std;
using namespace rapidjson;

enum Http_Method {GET,POST,HEAD,PUT};

template<typename Type> Type String_To_Num(const string& str, stringstream* psstream)
{
	psstream->clear();
	*psstream << str;
	Type num;
	*psstream >> num;
	return num;
}
template<typename Type> string Num_To_String(const Type& num, stringstream* psstream)
{
	psstream->clear();
	*psstream << num;
	string a;
	*psstream >> a;
	return a;
}

struct MemoryStruct 
{
	char *memory;
	size_t size;
};

vector<string> split(string& s, char seperator);

char* Read_Whole_File(char* buffer,const char* file_path);

static class Basements
{
public:
	char* General_Request_Return(CURL*,const char*,Http_Method,MemoryStruct&);
	string& Map_To_OrderedString(string&,map<string, int>& map);
	string MD5_32Bit_Capital(string& to_be_hashed);
};

