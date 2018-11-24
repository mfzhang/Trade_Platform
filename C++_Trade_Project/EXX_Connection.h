#pragma once
#include <string>
//#include "../libcurl-vc15-x64-release-static-ipv6-sspi-winssl/include/curl/curl.h"
#include <curl/curl.h>
#include "Basements.h"
#include <sstream>
#include "General_Data_Structures.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/schema.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Normaliz.lib")

#pragma warning(disable : 4996)

using namespace std;
using namespace rapidjson;

class EXX_JsonDocuments
{
public:
	EXX_JsonDocuments()
	{
		Document document;
		string path = folder_path + "ticker_schema.txt";
		char buffer[5000];
		if (ticker_document.Parse(Read_Whole_File(buffer, path.c_str())).HasParseError())
		{
			cout << "exx_ticker_schema parse error" << endl;
		}

		path = folder_path + "depth_schema.txt";
		if (depth_document.Parse(Read_Whole_File(buffer, path.c_str())).HasParseError())
		{
			cout << "exx_depth_schema parse error" << endl;
		}

		path = folder_path + "tradesinfo_schema.txt";
		if (tradesinfo_document.Parse(Read_Whole_File(buffer, path.c_str())).HasParseError())
		{
			cout << "exx_tradesinfo_schema parse error" << endl;
		}

		path = folder_path + "kline_schema.txt";
		if (kline_document.Parse(Read_Whole_File(buffer, path.c_str())).HasParseError())
		{
			cout << "exx_kline_schema parse error" << endl;
		}

		path = folder_path + "cash_accountinfo_schema.txt";
		if (cash_accountinfo_document.Parse(Read_Whole_File(buffer, path.c_str())).HasParseError())
		{
			cout << "exx_cash_accountinfo_chema parse error" << endl;
		}

		path = folder_path + "futures_accountinfo_schema.txt";
		if (futures_accountinfo_document.Parse(Read_Whole_File(buffer, path.c_str())).HasParseError())
		{
			cout << "exx_futures_accountinfo_chema parse error" << endl;
		}

		path = folder_path + "positionsinfo_schema.txt";
		if (positionsinfo_document.Parse(Read_Whole_File(buffer, path.c_str())).HasParseError())
		{
			cout << "exx_positionsinfo_schema parse error" << endl;
		}

		path = folder_path + "ordersinfo_schema.txt";
		if (ordersinfo_document.Parse(Read_Whole_File(buffer, path.c_str())).HasParseError())
		{
			cout << "exx_ordersinfo_schema parse error" << endl;
		}

		path = folder_path + "manipulation_response_schema.txt";
		if (manipulation_response_document.Parse(Read_Whole_File(buffer, path.c_str())).HasParseError())
		{
			cout << "exx_manipulation_response_schema parse error" << endl;
		}

		path = folder_path + "error_schema.txt";
		if (error_document.Parse(Read_Whole_File(buffer, path.c_str())).HasParseError())
		{
			cout << "error_schema parse error" << endl;
		}
	}
	string folder_path = "./resources/EXX_JsonSchemas/";
	Document ticker_document;
	Document depth_document;
	Document tradesinfo_document;
	Document kline_document;
	Document cash_accountinfo_document;
	Document futures_accountinfo_document;
	Document positionsinfo_document;
	Document ordersinfo_document;
	Document manipulation_response_document;
	Document error_document;
};

class EXX_JsonSchemaDocuments
{
public:
	EXX_JsonSchemaDocuments() :
		ticker_schemadocument(document.ticker_document),
		depth_schemadocument(document.depth_document),
		tradesinfo_schemadocument(document.tradesinfo_document),
		kline_schemadocument(document.kline_document),
		cash_accountinfo_schemadocument(document.cash_accountinfo_document),
		futures_accountinfo_schemadocument(document.futures_accountinfo_document),
		positionsinfo_schemadocument(document.positionsinfo_document),
		ordersinfo_schemadocument(document.ordersinfo_document),
		manipulation_response_schemadocument(document.manipulation_response_document),
		error_schemadocument(document.error_document) {};
	EXX_JsonDocuments document;
	SchemaDocument ticker_schemadocument;
	SchemaDocument depth_schemadocument;
	SchemaDocument tradesinfo_schemadocument;
	SchemaDocument kline_schemadocument;
	SchemaDocument cash_accountinfo_schemadocument;
	SchemaDocument futures_accountinfo_schemadocument;
	SchemaDocument positionsinfo_schemadocument;
	SchemaDocument ordersinfo_schemadocument;
	SchemaDocument manipulation_response_schemadocument;
	SchemaDocument error_schemadocument;
};

class EXX_JsonSchemaValidators
{
public:
	EXX_JsonSchemaValidators() :
		ticker_validator(schemadocument.ticker_schemadocument),
		depth_validator(schemadocument.depth_schemadocument),
		tradesinfo_validator(schemadocument.tradesinfo_schemadocument),
		kline_validator(schemadocument.kline_schemadocument),
		cash_accountinfo_validator(schemadocument.cash_accountinfo_schemadocument),
		futures_accountinfo_validator(schemadocument.futures_accountinfo_schemadocument),
		positionsinfo_validator(schemadocument.positionsinfo_schemadocument),
		ordersinfo_validator(schemadocument.ordersinfo_schemadocument),
		manipulation_response_validator(schemadocument.manipulation_response_schemadocument),
		error_validator(schemadocument.error_schemadocument) {};
	EXX_JsonSchemaDocuments schemadocument;
	SchemaValidator ticker_validator;
	SchemaValidator depth_validator;
	SchemaValidator tradesinfo_validator;
	SchemaValidator kline_validator;
	SchemaValidator cash_accountinfo_validator;
	SchemaValidator futures_accountinfo_validator;
	SchemaValidator positionsinfo_validator;
	SchemaValidator ordersinfo_validator;
	SchemaValidator manipulation_response_validator;
	SchemaValidator error_validator;
};

class EXX_Rest_Connection
{
protected:
	CURL* curl;
	string apiKey;
	string secretKey;
	string safe_pwd;
private:
	Basements basements;
	stringstream sstream;
	string base_url = "https://api.exx.com/data/v1";
	string parameters_string;
	MemoryStruct memorystruct;
	EXX_JsonSchemaValidators json_validators;
	INT64 contract_id_to_delivery_timestamp(INT64 contract_id)
	{
		string s_id = Num_To_String<INT64>(contract_id, &sstream);
		char ts[11];
		for (int i = 0; i < 4; i++)
		{
			ts[i] = s_id[i];
		}
		ts[4] = '-';
		for (int i = 0; i < 2; i++)
		{
			ts[5 + i] = s_id[4 + i];
		}
		ts[7] = '-';
		for (int i = 0; i < 2; i++)
		{
			ts[8 + i] = s_id[6 + i];
		}
		ts[10] = '\0';
		int year, month, day;
		sscanf_s(ts, "%d-%d-%d", &year, &month, &day);
		struct tm timestruct = { 0 };
		timestruct.tm_year = year - 1900;
		timestruct.tm_mon = month - 1;
		timestruct.tm_mday = day;
		timestruct.tm_hour = 16;
		return mktime(&timestruct) * 1000;
	}
	INT64 contract_name_to_delivery_timestamp(string contract_name, INT64 create_timestamp)
	{
		char ts[6];
		ts[2] = '-';
		int len = contract_name.length();
		for (int i = 0; i < 4; i++)
		{
			if (i < 2)
			{
				ts[i] = contract_name[len - 4 + i];
			}
			else
			{
				ts[i + 1] = contract_name[len - 4 + i];
			}
		}
		ts[5] = '\0';
		int year, month, day;
		sscanf_s(ts, "%d-%d", &month, &day);
		INT64 create_timestamp_s = create_timestamp / 1000;
		tm *ptm = localtime(&create_timestamp_s);
		if (month < (ptm->tm_mon + 1))
		{
			year = ptm->tm_year + 1;
		}
		else
		{
			year = ptm->tm_year;
		}
		struct tm timestruct = { 0 };
		timestruct.tm_year = year;
		timestruct.tm_mon = month - 1;
		timestruct.tm_mday = day;
		timestruct.tm_hour = 16;
		return mktime(&timestruct) * 1000;
	}
	string& get_parameters_string(map<string, int>& parameters, bool if_need_sign);
public:
	EXX_Rest_Connection()
	{
		this->sstream.precision(15);
	};
	EXX_Rest_Connection(CURL* curl, string apiKey, string secretKey, string safe_pwd)
	{
		this->curl = curl;
		this->apiKey = apiKey;
		this->secretKey = secretKey;
		this->safe_pwd = safe_pwd;
		this->sstream.precision(15);
	}
	~EXX_Rest_Connection() {};
	Ticker ticker;
	Depth depth;
	TradesInfo tradesinfo;
	Kline kline;
	AccountInfo accountinfo;
	PositionsInfo positionsinfo;
	OrdersInfo ordersinfo;
	OrderInfo orderinfo;
	Manipulation_Response manipulation_response;

	//Market_Information:
	Ticker& Get_Ticker(string market_symbol);
	Depth& Get_Depth(string market_symbol, int size = 0, int merge = 0);
	TradesInfo& Get_TradesInfo(string market_symbol, int size = 0, INT64 since_timestamp = 0);
	Kline& Get_Kline(string market_symbol, string kline_type, int size = 0, INT64 since_timestamp = 0);

	//Account_Information:
	AccountInfo& Get_AccountInfo(string account_symbol);
	PositionsInfo& Get_PositionsInfo(string market_symbol);
	OrdersInfo& Get_OrdersInfo(string market_symbol, bool type, int size = 0, INT64 since_timestamp = 0); //type: ture����ɶ���, falseδ��ɶ���
	OrderInfo& Get_OrderInfo(string market_symbol, INT64 order_id);

	//Manipulation:
	Manipulation_Response& Send_Order(string market_symbol, int order_type, double price, double amount, int lever_rate = 0); //order_type: 1 ���� 2 ���� 3 ƽ�� 4 ƽ�� or 1,4��  2,3��
	Manipulation_Response& Cancel_Order(string market_symbol, INT64 order_id);
	Manipulation_Response& Interaccount_Transfer(string source_account_symbol, string target_account_symbol, string coin_symbol, double amount);
	Manipulation_Response& Withdraw(string source_account_symbol, string coin_symbol, double amount, double fee, string target_address, string safe_pwd);

	void Deserialize_To_Ticker(char* raw_return, Ticker& ticker);
	void Deserialize_To_Depth(char* raw_return, Depth& depth);
	void Deserialize_To_TradesInfo(char* raw_return, TradesInfo& tradesinfo);
	void Deserialize_To_Kline(char* raw_return, Kline& kline);
	void Deserialize_To_AccountInfo(char* raw_return, string account_type, AccountInfo& accountinfo);
	void Deserialize_To_PositionsInfo(char* raw_return, PositionsInfo& positonsinfo);
	void Deserialize_To_OrdersInfo(char* raw_return, string market_symbol, OrdersInfo& ordersinfo);
	void Deserialize_To_OrderInfo(char* raw_return, string market_symbol, OrderInfo& orderinfo);
	void Deserialize_To_Manipulation_Response(char* raw_return, Manipulation_Response& manipulation_response);

};