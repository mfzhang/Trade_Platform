#include "stdafx.h"
#include <string>
#include <curl/curl.h>
#include "EXX_Connection.h"
#include "Basements.h"
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/schema.h"
#include "openssl/HMAC.h"
#include "ctime";
#include <vector>
#include <map>
#include <openssl/md5.h>
#include "General_Data_Structures.h"
#include <fstream>
#include <sstream>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Normaliz.lib")

using namespace std;
using namespace rapidjson;

void EXX_Rest_Connection::Deserialize_To_Ticker(char* raw_return, Ticker& ticker)
{
	ticker.returninfo.raw_reutrn = raw_return;
	ticker.returninfo.if_ok = false;
	Document document;
	if (!document.Parse(raw_return).HasParseError())
	{
		json_validators.ticker_validator.Reset();
		if (document.Accept(json_validators.ticker_validator))
		{
			ticker.returninfo.if_ok = true;
			if (document["date"].IsString())
			{
				ticker.server_timestamp = String_To_Num<INT64>(document["date"].GetString(), &sstream);
			}
			else
			{
				ticker.server_timestamp = document["date"].GetInt64();
			}

			if (document["ticker"]["last"].IsString())
			{
				ticker.last_price = String_To_Num<double>(document["ticker"]["last"].GetString(), &sstream);
			}
			else
			{
				ticker.last_price = document["ticker"]["last"].GetDouble();
			}
		}
		else
		{
			json_validators.error_validator.Reset();
			if (document.Accept(json_validators.error_validator))
			{
				ticker.returninfo.if_ok = false;
				ticker.returninfo.error->error_type = 1;
				ticker.returninfo.error->error_code = document["error_code"].GetInt();
			}
			else
			{
				ticker.returninfo.if_ok = false;
				ticker.returninfo.error->error_type = 0;
			}
		}
	}
	else
	{
		ticker.returninfo.if_ok = false;
		ticker.returninfo.error->error_type = 0;
	}
};

void EXX_Rest_Connection::Deserialize_To_Depth(char* raw_return, Depth& depth)
{
	depth.returninfo.raw_reutrn = raw_return;
	depth.returninfo.if_ok = false;
	Document document;
	if (!document.Parse(raw_return).HasParseError())
	{
		json_validators.depth_validator.Reset();
		if (document.Accept(json_validators.depth_validator))
		{
			depth.returninfo.if_ok = true;
			int asks_num = document["asks"].Size();
			depth.depth_data.set_asks_num(asks_num);
			for (int i = 0; i < asks_num; i++)
			{
				depth.depth_data.asks[i][0] = document["asks"][asks_num - i - 1][0].GetDouble();
				depth.depth_data.asks[i][1] = document["asks"][asks_num - i - 1][1].GetDouble();
			}
			int bids_num = document["bids"].Size();
			depth.depth_data.set_bids_num(bids_num);
			for (int i = 0; i < bids_num; i++)
			{
				depth.depth_data.bids[i][0] = document["bids"][i][0].GetDouble();
				depth.depth_data.bids[i][1] = document["bids"][i][1].GetDouble();
			}
		}
		else
		{
			json_validators.error_validator.Reset();
			if (document.Accept(json_validators.error_validator))
			{
				depth.returninfo.if_ok = false;
				depth.returninfo.error->error_type = 1;
				depth.returninfo.error->error_code = document["error_code"].GetInt();
			}
			else
			{
				depth.returninfo.if_ok = false;
				depth.returninfo.error->error_type = 0;
			}
		}
	}
	else
	{
		depth.returninfo.if_ok = false;
		depth.returninfo.error->error_type = 0;
	}
};

void EXX_Rest_Connection::Deserialize_To_TradesInfo(char* raw_return, TradesInfo& tradesinfo)
{
	tradesinfo.returninfo.raw_reutrn = raw_return;
	tradesinfo.returninfo.if_ok = false;
	Document document;
	if (!document.Parse(raw_return).HasParseError())
	{
		json_validators.tradesinfo_validator.Reset();
		if (document.Accept(json_validators.tradesinfo_validator))
		{
			tradesinfo.returninfo.if_ok = true;
			int num = document.Size();
			tradesinfo.set_scale(num);
			for (int i = 0; i < num; i++)
			{
				tradesinfo.tradesinfo_data[i].timestamp = document[i]["date"].GetInt64() * 1000;
				tradesinfo.tradesinfo_data[i].amount = document[i]["amount"].GetDouble();
				tradesinfo.tradesinfo_data[i].price = document[i]["price"].GetDouble();
			}
		}
		else
		{
			json_validators.error_validator.Reset();
			if (document.Accept(json_validators.error_validator))
			{
				tradesinfo.returninfo.if_ok = false;
				tradesinfo.returninfo.error->error_type = 1;
				tradesinfo.returninfo.error->error_code = document["error_code"].GetInt();
			}
			else
			{
				tradesinfo.returninfo.if_ok = false;
				tradesinfo.returninfo.error->error_type = 0;
			}
		}
	}
	else
	{
		tradesinfo.returninfo.if_ok = false;
		tradesinfo.returninfo.error->error_type = 0;
	}
};

void EXX_Rest_Connection::Deserialize_To_Kline(char* raw_return, Kline& kline)
{
	kline.returninfo.raw_reutrn = raw_return;
	kline.returninfo.if_ok = false;
	Document document;
	if (!document.Parse(raw_return).HasParseError())
	{
		json_validators.kline_validator.Reset();
		if (document.Accept(json_validators.kline_validator))
		{
			kline.returninfo.if_ok = true;
			int num = document.Size();
			kline.set_scale(num);
			for (int i = 0; i < num; i++)
			{
				kline.kline_data[i].timestamp = document[i]["datas"]["data"][0].GetInt64();
				if (document[i]["datas"]["data"][1].IsString())
				{
					kline.kline_data[i].start = String_To_Num<double>(document[i]["datas"]["data"][1].GetString(), &sstream);
				}
				else
				{
					kline.kline_data[i].start = document[i]["datas"]["data"][1].GetDouble();
				}

				if (document[i]["datas"]["data"][2].IsString())
				{
					kline.kline_data[i].high = String_To_Num<double>(document[i]["datas"]["data"][2].GetString(), &sstream);
				}
				else
				{
					kline.kline_data[i].high = document[i]["datas"]["data"][2].GetDouble();
				}

				if (document[i]["datas"]["data"][3].IsString())
				{
					kline.kline_data[i].low = String_To_Num<double>(document[i]["datas"]["data"][3].GetString(), &sstream);
				}
				else
				{
					kline.kline_data[i].low = document[i]["datas"]["data"][3].GetDouble();
				}

				if (document[i]["datas"]["data"][4].IsString())
				{
					kline.kline_data[i].end = String_To_Num<double>(document[i]["datas"]["data"][4].GetString(), &sstream);
				}
				else
				{
					kline.kline_data[i].end = document[i]["datas"]["data"][4].GetDouble();
				}

				if (document[i]["datas"]["data"][5].IsString())
				{
					kline.kline_data[i].amount = String_To_Num<double>(document[i]["datas"]["data"][5].GetString(), &sstream);
				}
				else
				{
					kline.kline_data[i].amount = document[i]["datas"]["data"][5].GetDouble();
				}
			}
		}
		else
		{
			json_validators.error_validator.Reset();
			if (document.Accept(json_validators.error_validator))
			{
				kline.returninfo.if_ok = false;
				kline.returninfo.error->error_type = 1;
				kline.returninfo.error->error_code = document["error_code"].GetInt();
			}
			else
			{
				kline.returninfo.if_ok = false;
				kline.returninfo.error->error_type = 0;
			}
		}
	}
	else
	{
		kline.returninfo.if_ok = false;
		kline.returninfo.error->error_type = 0;
	}
};

void EXX_Rest_Connection::Deserialize_To_AccountInfo(char* raw_return, string account_type, AccountInfo& accountinfo)
{
	accountinfo.returninfo.raw_reutrn = raw_return;
	accountinfo.returninfo.if_ok = false;

	Document document;
	if(!document.Parse(raw_return).HasParseError())
	{
		json_validators.cash_accountinfo_validator.Reset();
		if (document.Accept(json_validators.cash_accountinfo_validator))
		{
			accountinfo.returninfo.if_ok = true;

			accountinfo.accountinfo_data.account_symbol = "Cash_Account";
			if (accountinfo.accountinfo_data.cash_account != NULL)
			{
				if (accountinfo.accountinfo_data.cash_account->size() != 0)
				{
					delete accountinfo.accountinfo_data.cash_account;
					accountinfo.accountinfo_data.cash_account = new map<string, Cash_AccountInfo_Unit>;
				}
			}
			else
			{
				accountinfo.accountinfo_data.cash_account = new map<string, Cash_AccountInfo_Unit>;
			}	

			Value::ConstMemberIterator itr = document.FindMember("funds");
			for(Value::ConstMemberIterator itr_2 = itr->value.MemberBegin();
				itr_2 != itr->value.MemberEnd(); ++itr_2)
			{
				string currency = itr_2->name.GetString();
				double total, freeze = 0.0, balance = 0.0;
				Value::ConstMemberIterator tmp = itr_2->value.FindMember("total");
				if (tmp != itr_2->value.MemberEnd())
					total = stod(tmp->value.GetString());
				tmp = itr_2->value.FindMember("freeze");
				if (tmp != itr_2->value.MemberEnd())
					freeze = stod(tmp->value.GetString());
				tmp = itr_2->value.FindMember("balance");
				if (tmp != itr_2->value.MemberEnd())
					balance = stod(tmp->value.GetString());

				Cash_AccountInfo_Unit cash_accountinfo_unit(currency, total, balance, freeze);
				accountinfo.accountinfo_data.cash_account->
					insert(pair<string, Cash_AccountInfo_Unit>(currency, cash_accountinfo_unit));
			}
		}
		else
		{
			json_validators.error_validator.Reset();
			if (document.Accept(json_validators.error_validator))
			{
				cout << 4 << endl;
				accountinfo.returninfo.if_ok = false;
				accountinfo.returninfo.error->error_type = 1;
				accountinfo.returninfo.error->error_code = document["error_code"].GetInt();
			}
			else
			{
				accountinfo.returninfo.if_ok = false;
				accountinfo.returninfo.error->error_type = 0;
			}
		}
	}
	else
	{
		accountinfo.returninfo.if_ok = false;
		accountinfo.returninfo.error->error_type = 0;
	}
};

void EXX_Rest_Connection::Deserialize_To_PositionsInfo(char* raw_return, PositionsInfo& positionsinfo)
{

}

void EXX_Rest_Connection::Deserialize_To_OrdersInfo(char* raw_return, string market_symbol, OrdersInfo& ordersinfo)
{
	ordersinfo.returninfo.raw_reutrn = raw_return;
	ordersinfo.returninfo.if_ok = false;
	Document document;
	if (!document.Parse(raw_return).HasParseError())
	{
		json_validators.ordersinfo_validator.Reset();
		if (document.Accept(json_validators.ordersinfo_validator))
		{
			ordersinfo.returninfo.if_ok = true;
			int num = document.Size();
			ordersinfo.set_scale(num);
			for (int i = 0; i < num; i++)
			{
				ordersinfo.ordersinfo_data[i].market_symbol = document[i]["currency"].GetString();
				ordersinfo.ordersinfo_data[i].create_timestamp = document[i]["trade_date"].GetInt64();
			
				ordersinfo.ordersinfo_data[i].if_futures = false;
				string type = document[i]["type"].GetString();
				if (type == "buy")
				{
					ordersinfo.ordersinfo_data[i].order_type = 1;
				}
				else
				{
					ordersinfo.ordersinfo_data[i].order_type = 2;
				}

				ordersinfo.ordersinfo_data[i].order_id = document[i]["id"].GetInt64();
				ordersinfo.ordersinfo_data[i].total_amount = document[i]["total_amount"].GetDouble();
				ordersinfo.ordersinfo_data[i].deal_amount = document[i]["trade_amount"].GetDouble();
				ordersinfo.ordersinfo_data[i].set_price = document[i]["price"].GetDouble(); // ?
				int status = document[i]["status"].GetInt();
				if (status > 2) status = 3;
				ordersinfo.ordersinfo_data[i].status = status;
			}
		}
		else
		{
			json_validators.error_validator.Reset();
			if (document.Accept(json_validators.error_validator))
			{
				ordersinfo.returninfo.if_ok = false;
				ordersinfo.returninfo.error->error_type = 1;
				ordersinfo.returninfo.error->error_code = document["error_code"].GetInt();
			}
			else
			{
				ordersinfo.returninfo.if_ok = false;
				ordersinfo.returninfo.error->error_type = 0;
			}
		}
	}
	else
	{
		ordersinfo.returninfo.if_ok = false;
		ordersinfo.returninfo.error->error_type = 0;
	}
};

void EXX_Rest_Connection::Deserialize_To_OrderInfo(char* raw_return, string market_symbol, OrderInfo& orderinfo)
{
	orderinfo.returninfo.raw_reutrn = raw_return;
	orderinfo.returninfo.if_ok = false;
	Document document;
	if (!document.Parse(raw_return).HasParseError())
	{
		json_validators.ordersinfo_validator.Reset();
		if (document.Accept(json_validators.ordersinfo_validator))
		{
			int num = document.Size();
			if (num > 0)
			{
				orderinfo.returninfo.if_ok = true;
				orderinfo.orderinfo_data.market_symbol = document["currency"].GetString();
				orderinfo.orderinfo_data.create_timestamp = document["trade_date"].GetInt64();
			
				orderinfo.orderinfo_data.if_futures = false;
				string type = document["type"].GetString();
				if (type == "buy")
				{
					orderinfo.orderinfo_data.order_type = 1;
				}
				else
				{
					orderinfo.orderinfo_data.order_type = 2;
				}
							
				orderinfo.orderinfo_data.order_id = document["id"].GetInt64();
				orderinfo.orderinfo_data.total_amount = document["total_amount"].GetDouble();
				orderinfo.orderinfo_data.deal_amount = document["trade_amount"].GetDouble();
				orderinfo.orderinfo_data.set_price = document["price"].GetDouble(); // ?
				int status = document["status"].GetInt();
				if (status > 2) status = 3;
				orderinfo.orderinfo_data.status = status;
			}
			else
			{
				orderinfo.returninfo.if_ok = false;
				orderinfo.returninfo.error->error_type = 0;
			}
		}
		else
		{
			json_validators.error_validator.Reset();
			if (document.Accept(json_validators.error_validator))
			{
				orderinfo.returninfo.if_ok = false;
				orderinfo.returninfo.error->error_type = 1;
				orderinfo.returninfo.error->error_code = document["error_code"].GetInt();
			}
			else
			{
				orderinfo.returninfo.if_ok = false;
				orderinfo.returninfo.error->error_type = 0;
			}
		}
	}
	else
	{
		orderinfo.returninfo.if_ok = false;
		orderinfo.returninfo.error->error_type = 0;
	}
}

void EXX_Rest_Connection::Deserialize_To_Manipulation_Response(char* raw_return, Manipulation_Response& manipulation_response)
{
	manipulation_response.returninfo.raw_reutrn = raw_return;
	manipulation_response.returninfo.if_ok = false;
	Document document;
	if (!document.Parse(raw_return).HasParseError())
	{
		json_validators.manipulation_response_validator.Reset();
		if (document.Accept(json_validators.manipulation_response_validator))
		{
			manipulation_response.returninfo.if_ok = document["result"].GetInt() == 100 ? true : false;
			if (document.HasMember("id"))
			{
				manipulation_response.if_order_id_valid = true;
				if (document["id"].IsString())
				{
					manipulation_response.order_id = String_To_Num<INT64>(document["id"].GetString(), &sstream);
				}
				else
				{
					manipulation_response.order_id = document["id"].GetInt64();
				}
			}
			else
			{
				if (document.Accept(json_validators.error_validator))
				{
					manipulation_response.returninfo.error->error_type = 1;
					manipulation_response.returninfo.error->error_code = document["error_code"].GetInt();
				}
				else
				{
					manipulation_response.if_order_id_valid = false;
				}
			}
		}
		else
		{
			json_validators.error_validator.Reset();
			if (document.Accept(json_validators.error_validator))
			{
				manipulation_response.returninfo.if_ok = false;
				manipulation_response.returninfo.error->error_type = 1;
				manipulation_response.returninfo.error->error_code = document["error_code"].GetInt();
			}
			else
			{
				manipulation_response.returninfo.if_ok = false;
				manipulation_response.returninfo.error->error_type = 0;
			}
		}
	}
	else
	{
		manipulation_response.returninfo.if_ok = false;
		manipulation_response.returninfo.error->error_type = 0;
	}
};



//Market_Information:

string& EXX_Rest_Connection::get_parameters_string(map<string, int>& parameters, bool if_need_sign)
{
	this->basements.Map_To_OrderedString(parameters_string, parameters);
	if (if_need_sign)
	{
		string to_be_signed = parameters_string + "&secret_key=" + this->secretKey;
		string sign = this->basements.MD5_32Bit_Capital(to_be_signed);
		parameters_string = parameters_string + "&sign=" + sign;
	}
	return parameters_string;
}

Ticker& EXX_Rest_Connection::Get_Ticker(string market_symbol)
{
	string base = "https://api.exx.com/data/v1/ticker?currency=";
	string url = base + market_symbol;
	
	Deserialize_To_Ticker(basements.General_Request_Return(this->curl, url.c_str(), GET, memorystruct), ticker);
	return ticker;
}

Depth& EXX_Rest_Connection::Get_Depth(string market_symbol, int size, int merge)
{
	string base = "https://api.exx.com/data/v1/depth?currency=";
	string size_string = Num_To_String<int>(size, &sstream);
	string url = base + market_symbol + "&size=" + size_string;
	
	Deserialize_To_Depth(basements.General_Request_Return(this->curl, url.c_str(), GET, memorystruct), depth);
	return depth;
}

TradesInfo& EXX_Rest_Connection::Get_TradesInfo(string market_symbol, int size, INT64 since_timestamp)
{
	string url = base_url + "/trade?currency=" + market_symbol;

	Deserialize_To_TradesInfo(basements.General_Request_Return(this->curl, url.c_str(), GET, memorystruct), tradesinfo);
	return tradesinfo;
}

Kline& EXX_Rest_Connection::Get_Kline(string market_symbol, string kline_type, int size, INT64 since_timestamp)
{
	string url = base_url + "/klines?market=" + market_symbol + "&type=" + kline_type + "&size=" + to_string(size) + "&assist=none";

	Deserialize_To_Kline(basements.General_Request_Return(this->curl, url.c_str(), GET, memorystruct), kline);
	return kline;
}

//Account_Information:

long Get_Current_Time()  
{  
    std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
        std::chrono::system_clock::now().time_since_epoch()
    );
	return ms.count();
} 

AccountInfo& EXX_Rest_Connection::Get_AccountInfo(string account_symbol)
{
	string base = "https://trade.exx.com/api/getBalance?";
	string parameter = "accesskey=" + apiKey + "&nonce=" + to_string(Get_Current_Time());

	const char* data_to_be_signed_0 = parameter.c_str();
	unsigned char* data_to_be_signed = (unsigned char*)(data_to_be_signed_0);
	const char* s_k_0 = secretKey.c_str();
	unsigned char* s_k = (unsigned char*)(s_k_0);

	unsigned char* signture = HMAC(EVP_sha512(), s_k, strlen(s_k_0), data_to_be_signed, strlen(data_to_be_signed_0), NULL, NULL);
	const char* signture_0 = (const char*)signture;

	string url = base + parameter + "&signature=" + signture_0;

	basements.General_Request_Return(curl, url.c_str(), GET, memorystruct);
	Deserialize_To_AccountInfo(memorystruct.memory, account_symbol, accountinfo);
	return accountinfo;
}

PositionsInfo& EXX_Rest_Connection::Get_PositionsInfo(string market_symbol)
{
	return positionsinfo;
}

OrdersInfo& EXX_Rest_Connection::Get_OrdersInfo(string market_symbol, bool type, int size, INT64 since_timestamp)
{
	string base = "https://trade.exx.com/api/getOpenOrders?";
	string parameter = "accesskey=" + apiKey + "&currency=" + market_symbol + "&nonce=" + to_string(Get_Current_Time()) + "&pageIndex=1&type=buy";

	const char* data_to_be_signed_0 = parameter.c_str();
	unsigned char* data_to_be_signed = (unsigned char*)(data_to_be_signed_0);
	const char* s_k_0 = this->secretKey.c_str();
	unsigned char* s_k = (unsigned char*)(s_k_0);

	unsigned char* signture = HMAC(EVP_sha512(), s_k, strlen(s_k_0), data_to_be_signed, strlen(data_to_be_signed_0), NULL, NULL); 
	const char* signture_0 = (const char*)signture;

	string url = base + parameter + "&signature=" + signture_0;

	Deserialize_To_OrdersInfo(basements.General_Request_Return(this->curl, url.c_str(), GET, memorystruct), market_symbol, ordersinfo);
	return ordersinfo;
}

OrderInfo& EXX_Rest_Connection::Get_OrderInfo(string market_symbol, INT64 order_id)
{
	string base = "https://trade.exx.com/api/getOrder?";
	string parameter = "accesskey=" + apiKey + "&currency=" + market_symbol + "&id=" + to_string(order_id) + "&nonce=" + to_string(Get_Current_Time());
	
	const char* data_to_be_signed_0 = parameter.c_str();
	unsigned char* data_to_be_signed = (unsigned char*)(data_to_be_signed_0);
	const char* s_k_0 = secretKey.c_str();
	unsigned char* s_k = (unsigned char*)(s_k_0);

	unsigned char* signture = HMAC(EVP_sha512(), s_k, strlen(s_k_0), data_to_be_signed, strlen(data_to_be_signed_0), NULL, NULL);  
	const char* signture_0 = (const char*)signture;

	string url = base + parameter + "&signature=" + signture_0;

	Deserialize_To_OrderInfo(basements.General_Request_Return(this->curl, url.c_str(), GET, memorystruct), market_symbol, orderinfo);
	return orderinfo;
};

//Manipulation

Manipulation_Response& EXX_Rest_Connection::Send_Order(string market_symbol, int order_type, double price, double amount, int lever_rate) //1:���� 2:���� 3:ƽ�� 4:ƽ�� or 1,4��  2,3��
{
	string base = "https://trade.exx.com/api/order?";
	string type_info = order_type == 1 ? "buy" : "sell";
	string parameter = "accesskey=" + apiKey + "&amount=" + to_string(amount) + "&currency=" + market_symbol + "&nonce=" + to_string(Get_Current_Time()) + "&price=" + to_string(price) + "&type=" + type_info;

	const char* data_to_be_signed_0 = parameter.c_str();
	unsigned char* data_to_be_signed = (unsigned char*)(data_to_be_signed_0);
	const char* s_k_0 = secretKey.c_str();
	unsigned char* s_k = (unsigned char*)(s_k_0);

	unsigned char* signture = HMAC(EVP_sha512(), s_k, strlen(s_k_0), data_to_be_signed, strlen(data_to_be_signed_0), NULL, NULL); 
	const char* signture_0 = (const char*)signture;

	string url = base + parameter + "&signature=" + signture_0;

	Deserialize_To_Manipulation_Response(basements.General_Request_Return(this->curl, url.c_str(), GET, memorystruct), manipulation_response);
	return manipulation_response;
}

Manipulation_Response& EXX_Rest_Connection::Cancel_Order(string market_symbol, INT64 order_id)
{
	string base = "https://trade.exx.com/api/cancel?";
	string parameter = "accesskey=" + apiKey  + "&currency=" + market_symbol + "&id=" + to_string(order_id) + "&nonce=" + to_string(Get_Current_Time());

	const char* data_to_be_signed_0 = parameter.c_str();
	unsigned char* data_to_be_signed = (unsigned char*)(data_to_be_signed_0);
	const char* s_k_0 = secretKey.c_str();
	unsigned char* s_k = (unsigned char*)(s_k_0);

	unsigned char* signture = HMAC(EVP_sha512(), s_k, strlen(s_k_0), data_to_be_signed, strlen(data_to_be_signed_0), NULL, NULL);  
	const char* signture_0 = (const char*)signture;

	string url = base + parameter + "&signature=" + signture_0;

	Deserialize_To_Manipulation_Response(basements.General_Request_Return(this->curl, url.c_str(), GET, memorystruct), manipulation_response);
	return manipulation_response;
};

Manipulation_Response& EXX_Rest_Connection::Interaccount_Transfer(string source_account_symbol, string target_account_symbol, string coin_symbol, double amount)
{		
	return manipulation_response;
};

Manipulation_Response& EXX_Rest_Connection::Withdraw(string source_account_symbol, string coin_symbol, double amount, double fee, string target_address, string safe_pwd)
{
	string base = "https://trade.exx.com/api/withdraw?";
	string parameter = "accesskey=" + apiKey  + "&amount=" + to_string(amount) + "&currency=" + source_account_symbol + "&nonce=" + to_string(Get_Current_Time()) + "&receiveAddr=" + target_address + "&safePwd=" + "";

	const char* data_to_be_signed_0 = parameter.c_str();
	unsigned char* data_to_be_signed = (unsigned char*)(data_to_be_signed_0);
	const char* s_k_0 = secretKey.c_str();
	unsigned char* s_k = (unsigned char*)(s_k_0);

	unsigned char* signture = HMAC(EVP_sha512(), s_k, strlen(s_k_0), data_to_be_signed, strlen(data_to_be_signed_0), NULL, NULL);  
	const char* signture_0 = (const char*)signture;

	string url = base + parameter + "&signature=" + signture_0;

	Deserialize_To_Manipulation_Response(basements.General_Request_Return(this->curl, url.c_str(), GET, memorystruct), manipulation_response);
	return manipulation_response;	
};



