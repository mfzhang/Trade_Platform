#include "stdafx.h"
#include <string>
#include <curl/curl.h>
//#include "../libcurl-vc15-x64-release-static-ipv6-sspi-winssl/include/curl/curl.h"
#include "OKEx_Connection.h"
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

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Normaliz.lib")

using namespace std;
using namespace rapidjson;

void OKEx_Rest_Connection::Deserialize_To_Ticker(char* raw_return, Ticker& ticker)
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

void OKEx_Rest_Connection::Deserialize_To_Depth(char* raw_return, Depth& depth)
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

void OKEx_Rest_Connection::Deserialize_To_TradesInfo(char* raw_return, TradesInfo& tradesinfo)
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
				tradesinfo.tradesinfo_data[i].timestamp = document[i]["date_ms"].GetInt64();
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

void OKEx_Rest_Connection::Deserialize_To_Kline(char* raw_return, Kline& kline)
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
				kline.kline_data[i].timestamp = document[i][0].GetInt64();
				if (document[i][1].IsString())
				{
					kline.kline_data[i].start = String_To_Num<double>(document[i][1].GetString(), &sstream);
				}
				else
				{
					kline.kline_data[i].start = document[i][1].GetDouble();
				}

				if (document[i][2].IsString())
				{
					kline.kline_data[i].high = String_To_Num<double>(document[i][2].GetString(), &sstream);
				}
				else
				{
					kline.kline_data[i].high = document[i][2].GetDouble();
				}

				if (document[i][3].IsString())
				{
					kline.kline_data[i].low = String_To_Num<double>(document[i][3].GetString(), &sstream);
				}
				else
				{
					kline.kline_data[i].low = document[i][3].GetDouble();
				}

				if (document[i][4].IsString())
				{
					kline.kline_data[i].end = String_To_Num<double>(document[i][4].GetString(), &sstream);
				}
				else
				{
					kline.kline_data[i].end = document[i][4].GetDouble();
				}

				if (document[i][5].IsString())
				{
					kline.kline_data[i].amount = String_To_Num<double>(document[i][5].GetString(), &sstream);
				}
				else
				{
					kline.kline_data[i].amount = document[i][5].GetDouble();
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

void OKEx_Rest_Connection::Deserialize_To_AccountInfo(char* raw_return,string account_type, AccountInfo& accountinfo)
{
	accountinfo.returninfo.raw_reutrn = raw_return;
	accountinfo.returninfo.if_ok = false;
	if ((account_type == "Cash_Account")||(account_type=="Wallet_Account"))
	{
		Document document;
		if (!document.Parse(raw_return).HasParseError())
		{
			json_validators.cash_accountinfo_validator.Reset();
			if (document.Accept(json_validators.cash_accountinfo_validator))
			{
				accountinfo.returninfo.if_ok = true;
				if (account_type == "Cash")
				{
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
				}
				else
					if (account_type == "Wallet")
					{
						accountinfo.accountinfo_data.account_symbol = "Wallet_Account";
						if (accountinfo.accountinfo_data.wallet_account != NULL)
						{
							if (accountinfo.accountinfo_data.wallet_account->size() != 0)
							{
								delete accountinfo.accountinfo_data.wallet_account;
								accountinfo.accountinfo_data.wallet_account = new map<string, Cash_AccountInfo_Unit>;
							}
						}
						else
						{
							accountinfo.accountinfo_data.wallet_account = new map<string, Cash_AccountInfo_Unit>;
						}
					}

				Value::ConstMemberIterator itr = document["info"]["funds"].FindMember("holds");
				if (itr != document["info"]["funds"].MemberEnd())
				{
					map<string, Cash_AccountInfo_Unit>::iterator map_iter;
					for (Value::ConstMemberIterator itr_2 = itr->value.MemberBegin();
						itr_2 != itr->value.MemberEnd(); ++itr_2)
					{
						double number;
						if (itr_2->value.IsString())
						{
							number = String_To_Num<double>(itr_2->value.GetString(), &sstream);
						}
						else
						{
							number = itr_2->value.GetDouble();
						}
						if (account_type == "cash")
						{
							map_iter = accountinfo.accountinfo_data.cash_account->find(itr_2->name.GetString());
							if (map_iter != accountinfo.accountinfo_data.cash_account->end())
							{
								map_iter->second.total_hold = number;
							}
							else
							{
								Cash_AccountInfo_Unit cash_accountinfo_unit(itr_2->name.GetString(), number, 0, 0);
								accountinfo.accountinfo_data.cash_account->
									insert(pair<string, Cash_AccountInfo_Unit>(itr_2->name.GetString(), cash_accountinfo_unit));
							}

						}
						else
							if (account_type == "wallet")
							{
								map_iter = accountinfo.accountinfo_data.wallet_account->find(itr_2->name.GetString());
								if (map_iter != accountinfo.accountinfo_data.wallet_account->end())
								{
									map_iter->second.total_hold = number;
								}
								else
								{
									Cash_AccountInfo_Unit cash_accountinfo_unit(itr_2->name.GetString(), number, 0, 0);
									accountinfo.accountinfo_data.wallet_account->
										insert(pair<string, Cash_AccountInfo_Unit>(itr_2->name.GetString(), cash_accountinfo_unit));
								}
							}
					}
				}

				itr = document["info"]["funds"].FindMember("free");
				if (itr != document["info"]["funds"].MemberEnd())
				{
					map<string, Cash_AccountInfo_Unit>::iterator map_iter;
					for (Value::ConstMemberIterator itr_2 = itr->value.MemberBegin();
						itr_2 != itr->value.MemberEnd(); ++itr_2)
					{
						double number;
						if (itr_2->value.IsString())
						{
							number = String_To_Num<double>(itr_2->value.GetString(), &sstream);
						}
						else
						{
							number = itr_2->value.GetDouble();
						}
						if (account_type == "cash")
						{
							map_iter = accountinfo.accountinfo_data.cash_account->find(itr_2->name.GetString());
							if (map_iter != accountinfo.accountinfo_data.cash_account->end())
							{
								map_iter->second.free = number;
							}
							else
							{
								Cash_AccountInfo_Unit cash_accountinfo_unit(itr_2->name.GetString(), 0, number, 0);
								accountinfo.accountinfo_data.cash_account->
									insert(pair<string, Cash_AccountInfo_Unit>(itr_2->name.GetString(), cash_accountinfo_unit));
							}

						}
						else
							if (account_type == "wallet")
							{
								map_iter = accountinfo.accountinfo_data.wallet_account->find(itr_2->name.GetString());
								if (map_iter != accountinfo.accountinfo_data.wallet_account->end())
								{
									map_iter->second.free = number;
								}
								else
								{
									Cash_AccountInfo_Unit cash_accountinfo_unit(itr_2->name.GetString(), 0, number, 0);
									accountinfo.accountinfo_data.wallet_account->
										insert(pair<string, Cash_AccountInfo_Unit>(itr_2->name.GetString(), cash_accountinfo_unit));
								}
							}
					}
				}

				itr = document["info"]["funds"].FindMember("freezed");
				if (itr != document["info"]["funds"].MemberEnd())
				{
					map<string, Cash_AccountInfo_Unit>::iterator map_iter;
					for (Value::ConstMemberIterator itr_2 = itr->value.MemberBegin();
						itr_2 != itr->value.MemberEnd(); ++itr_2)
					{
						double number;
						if (itr_2->value.IsString())
						{
							number = String_To_Num<double>(itr_2->value.GetString(), &sstream);
						}
						else
						{
							number = itr_2->value.GetDouble();
						}
						if (account_type == "cash")
						{
							map_iter = accountinfo.accountinfo_data.cash_account->find(itr_2->name.GetString());
							if (map_iter != accountinfo.accountinfo_data.cash_account->end())
							{
								map_iter->second.freezed = number;
							}
							else
							{
								Cash_AccountInfo_Unit cash_accountinfo_unit(itr_2->name.GetString(), 0, 0, number);
								accountinfo.accountinfo_data.cash_account->
									insert(pair<string, Cash_AccountInfo_Unit>(itr_2->name.GetString(), cash_accountinfo_unit));
							}

						}
						else
							if (account_type == "wallet")
							{
								map_iter = accountinfo.accountinfo_data.wallet_account->find(itr_2->name.GetString());
								if (map_iter != accountinfo.accountinfo_data.wallet_account->end())
								{
									map_iter->second.freezed = number;
								}
								else
								{
									Cash_AccountInfo_Unit cash_accountinfo_unit(itr_2->name.GetString(), 0, 0, number);
									accountinfo.accountinfo_data.wallet_account->
										insert(pair<string, Cash_AccountInfo_Unit>(itr_2->name.GetString(), cash_accountinfo_unit));
								}
							}
					}
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
	}
	else
	if (account_type == "Futures_Account")
	{
		Document document;
		json_validators.futures_accountinfo_validator.Reset();
		if (!document.Parse(raw_return).HasParseError())
		{
			if (document.Accept(json_validators.futures_accountinfo_validator))
			{
				accountinfo.returninfo.if_ok = true;
				if (accountinfo.accountinfo_data.futures_account != NULL)
				{
					if (accountinfo.accountinfo_data.futures_account->size() != 0)
					{
						delete accountinfo.accountinfo_data.futures_account;
						accountinfo.accountinfo_data.futures_account = new map<string, Futures_AccountInfo_Unit>;
					}
				}
				else
				{
					accountinfo.accountinfo_data.futures_account = new map<string, Futures_AccountInfo_Unit>;
				}

				for (Value::ConstMemberIterator itr = document["info"].MemberBegin(); itr != document["info"].MemberEnd(); ++itr)
				{
					Futures_AccountInfo_Unit futures_account;
					futures_account.coin_symbol = itr->name.GetString();
					futures_account.account_rights = itr->value["account_rights"].GetDouble();
					futures_account.keep_deposit = itr->value["keep_deposit"].GetDouble();
					futures_account.profit_real = itr->value["profit_real"].GetDouble();
					futures_account.profit_unreal = itr->value["profit_unreal"].GetDouble();
					futures_account.risk_rate = itr->value["risk_rate"].GetDouble();
					accountinfo.accountinfo_data.futures_account->insert(pair<string, Futures_AccountInfo_Unit>(itr->name.GetString(),futures_account));
				}
			}
			else
				if (document.Accept(json_validators.error_validator))
				{
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
		else
		{
			accountinfo.returninfo.if_ok = false;
			accountinfo.returninfo.error->error_type = 0;
		}
	}
	else
	{
		accountinfo.returninfo.if_ok = false;
		accountinfo.returninfo.error->error_type = 0;
		cout << "unrecognized account_type" << endl;
	}
};

void OKEx_Rest_Connection::Deserialize_To_PositionsInfo(char* raw_return, PositionsInfo& positionsinfo)
{
	positionsinfo.returninfo.raw_reutrn = raw_return;
	positionsinfo.returninfo.if_ok = false;
	Document document;
	if (!document.Parse(raw_return).HasParseError())
	{
		json_validators.positionsinfo_validator.Reset();
		if (document.Accept(json_validators.positionsinfo_validator))
		{
			positionsinfo.returninfo.if_ok = true;
			if (document["force_liqu_price"].IsString())
			{
				positionsinfo.positionsinfo_data.force_liqu_price = String_To_Num<double>(document["force_liqu_price"].GetString(), &sstream);
			}
			else
			{
				positionsinfo.positionsinfo_data.force_liqu_price = document["force_liqu_price"].GetDouble();
			}
			int num = document["holding"].Size();
			positionsinfo.positionsinfo_data.set_scale(num);
			for (int i = 0; i < num; i++)
			{
				positionsinfo.positionsinfo_data.positioninfo_units[i].contract_id = document["holding"][i]["contract_id"].GetInt64();
				positionsinfo.positionsinfo_data.positioninfo_units[i].delivery_timestamp =
					contract_id_to_delivery_timestamp(positionsinfo.positionsinfo_data.positioninfo_units[i].contract_id);
				positionsinfo.positionsinfo_data.positioninfo_units[i].lever_rate = document["holding"][i]["lever_rate"].GetInt();
				positionsinfo.positionsinfo_data.positioninfo_units[i].create_timestamp = document["holding"][i]["create_date"].GetInt64();
				positionsinfo.positionsinfo_data.positioninfo_units[i].contract_symbol = document["holding"][i]["contract_type"].GetString();
				positionsinfo.positionsinfo_data.positioninfo_units[i].market_symbol = document["holding"][i]["symbol"].GetString();
				if (positionsinfo.positionsinfo_data.positioninfo_units[i].market_symbol == "btc_usd")
				{
					positionsinfo.positionsinfo_data.positioninfo_units[i].unit_describe = "100_usd";
				}
				else
				{
					positionsinfo.positionsinfo_data.positioninfo_units[i].unit_describe = "10_usd";
				}
				positionsinfo.positionsinfo_data.positioninfo_units[i].market_symbol = positionsinfo.positionsinfo_data.positioninfo_units[i].market_symbol
					                                                                   + "&" + positionsinfo.positionsinfo_data.positioninfo_units[i].contract_symbol;

				int buy_amount = document["holding"][i]["buy_amount"].GetInt();
				int sell_amount = document["holding"][i]["sell_amount"].GetInt();
				if ((buy_amount > 0) && (sell_amount == 0))
				{
					positionsinfo.positionsinfo_data.positioninfo_units[i].position_type = 1;
					positionsinfo.positionsinfo_data.positioninfo_units[i].amount = buy_amount;
					positionsinfo.positionsinfo_data.positioninfo_units[i].start_price_avg = document["holding"][i]["buy_price_avg"].GetDouble();
					positionsinfo.positionsinfo_data.positioninfo_units[i].counting_price_base = document["holding"][i]["buy_price_cost"].GetDouble();
					positionsinfo.positionsinfo_data.positioninfo_units[i].profit_real = document["holding"][i]["buy_profit_real"].GetDouble();
				}
				else
				if ((buy_amount == 0) && (sell_amount > 0))
				{
					positionsinfo.positionsinfo_data.positioninfo_units[i].position_type = 0;
					positionsinfo.positionsinfo_data.positioninfo_units[i].amount = sell_amount;
					positionsinfo.positionsinfo_data.positioninfo_units[i].start_price_avg = document["holding"][i]["sell_price_avg"].GetDouble();
					positionsinfo.positionsinfo_data.positioninfo_units[i].counting_price_base = document["holding"][i]["sell_price_cost"].GetDouble();
					positionsinfo.positionsinfo_data.positioninfo_units[i].profit_real = document["holding"][i]["sell_profit_real"].GetDouble();
				}
				else
				{
					cout << "!!!00" << endl;
				}
			}
		}
		else
		{
			json_validators.error_validator.Reset();
			if (document.Accept(json_validators.error_validator))
			{
				positionsinfo.returninfo.if_ok = false;
				positionsinfo.returninfo.error->error_type = 1;
				positionsinfo.returninfo.error->error_code = document["error_code"].GetInt();
			}
			else
			{
				positionsinfo.returninfo.if_ok = false;
				positionsinfo.returninfo.error->error_type = 0;
			}
		}
	}
	else
	{
		positionsinfo.returninfo.if_ok = false;
		positionsinfo.returninfo.error->error_type = 0;
	}
}

void OKEx_Rest_Connection::Deserialize_To_OrdersInfo(char* raw_return, string market_symbol,OrdersInfo& ordersinfo)
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
			int num = document["orders"].Size();
			ordersinfo.set_scale(num);
			for (int i = 0; i < num; i++)
			{
				ordersinfo.ordersinfo_data[i].market_symbol = market_symbol;
				ordersinfo.ordersinfo_data[i].create_timestamp = document["orders"][i]["create_date"].GetInt64();
				if (document["orders"][i].HasMember("lever_rate"))
				{
					ordersinfo.ordersinfo_data[i].if_futures = true;
					ordersinfo.ordersinfo_data[i].lever_rate = document["orders"][i]["lever_rate"].GetInt();
					ordersinfo.ordersinfo_data[i].order_type = document["orders"][i]["type"].GetInt();
					ordersinfo.ordersinfo_data[i].deal_price_avg = document["orders"][i]["price_avg"].GetDouble();
					ordersinfo.ordersinfo_data[i].delivery_timestamp =contract_name_to_delivery_timestamp(document["orders"][i]["contract_name"].GetString(),
						                                                                                  ordersinfo.ordersinfo_data[i].create_timestamp);
				}
				else
				{
					ordersinfo.ordersinfo_data[i].if_futures = false;
					string type = document["orders"][i]["type"].GetString();
					if ((type == "buy")||(type=="buy_market"))
					{
						ordersinfo.ordersinfo_data[i].order_type = 1;
					}
					else
					{
						ordersinfo.ordersinfo_data[i].order_type = 2;
					}
					ordersinfo.ordersinfo_data[i].deal_price_avg = document["orders"][i]["avg_price"].GetDouble();
				}
				ordersinfo.ordersinfo_data[i].order_id = document["orders"][i]["order_id"].GetInt64();
				ordersinfo.ordersinfo_data[i].total_amount = document["orders"][i]["amount"].GetDouble();
				ordersinfo.ordersinfo_data[i].deal_amount = document["orders"][i]["deal_amount"].GetDouble();
				ordersinfo.ordersinfo_data[i].set_price = document["orders"][i]["price"].GetDouble();
				int status = document["orders"][i]["status"].GetInt();
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

void OKEx_Rest_Connection::Deserialize_To_OrderInfo(char* raw_return, string market_symbol,OrderInfo& orderinfo)
{
	orderinfo.returninfo.raw_reutrn = raw_return;
	orderinfo.returninfo.if_ok = false;
	Document document;
	if (!document.Parse(raw_return).HasParseError())
	{
		json_validators.ordersinfo_validator.Reset();
		if (document.Accept(json_validators.ordersinfo_validator))
		{
			int num = document["orders"].Size();
			if (num > 0)
			{
				orderinfo.returninfo.if_ok = true;
				orderinfo.orderinfo_data.market_symbol = market_symbol;
				orderinfo.orderinfo_data.create_timestamp = document["orders"][0]["create_date"].GetInt64();
				if (document["orders"][0].HasMember("lever_rate"))
				{
					orderinfo.orderinfo_data.if_futures = true;
					orderinfo.orderinfo_data.lever_rate = document["orders"][0]["lever_rate"].GetInt();
					orderinfo.orderinfo_data.order_type = document["orders"][0]["type"].GetInt();
					orderinfo.orderinfo_data.deal_price_avg = document["orders"][0]["price_avg"].GetDouble();
					orderinfo.orderinfo_data.delivery_timestamp = contract_name_to_delivery_timestamp(document["orders"][0]["contract_name"].GetString(),
						orderinfo.orderinfo_data.create_timestamp);
				}
				else
				{
					orderinfo.orderinfo_data.if_futures = false;
					string type = document["orders"][0]["type"].GetString();
					if ((type == "buy") || (type == "buy_market"))
					{
						orderinfo.orderinfo_data.order_type = 1;
					}
					else
					{
						orderinfo.orderinfo_data.order_type = 2;
					}
					orderinfo.orderinfo_data.deal_price_avg = document["orders"][0]["avg_price"].GetDouble();
				}
				orderinfo.orderinfo_data.order_id = document["orders"][0]["order_id"].GetInt64();
				orderinfo.orderinfo_data.total_amount = document["orders"][0]["amount"].GetDouble();
				orderinfo.orderinfo_data.deal_amount = document["orders"][0]["deal_amount"].GetDouble();
				orderinfo.orderinfo_data.set_price = document["orders"][0]["price"].GetDouble();
				int status = document["orders"][0]["status"].GetInt();
				if (status > 2) status = 3;
				orderinfo.orderinfo_data.status = status;
			}
			else
			{
				ordersinfo.returninfo.if_ok = false;
				ordersinfo.returninfo.error->error_type = 0;
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

void OKEx_Rest_Connection::Deserialize_To_Manipulation_Response(char* raw_return, Manipulation_Response& manipulation_response)
{
	manipulation_response.returninfo.raw_reutrn = raw_return;
	manipulation_response.returninfo.if_ok = false;
	Document document;
	if (!document.Parse(raw_return).HasParseError())
	{
		json_validators.manipulation_response_validator.Reset();
		if (document.Accept(json_validators.manipulation_response_validator))
		{
			manipulation_response.returninfo.if_ok = document["result"].GetBool();
			if (document.HasMember("order_id"))
			{
				manipulation_response.if_order_id_valid = true;
				if (document["order_id"].IsString())
				{
					manipulation_response.order_id = String_To_Num<INT64>(document["order_id"].GetString(),&sstream);
				}
				else
				{
					manipulation_response.order_id = document["order_id"].GetInt64();
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

string& OKEx_Rest_Connection::get_parameters_string(map<string, int>& parameters, bool if_need_sign)
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

Ticker& OKEx_Rest_Connection::Get_Ticker(string market_symbol)
{
	string url = "";
	vector<string> splits = split(market_symbol, '&');
	map<string, int> parameters;
	int sp_c = splits.size();
	if (sp_c == 1)
	{
		url = base_url + "/ticker.do?";
		parameters.insert(pair<string, int>("symbol="+splits[0], 0));
		url += get_parameters_string(parameters, false);
	}
	else
	if (sp_c>1)
	{
		url = base_url + "/future_ticker.do?";
		parameters.insert(pair<string, int>("symbol="+splits[0], 0));
		parameters.insert(pair<string, int>("contract_type="+splits[1], 0));
		url += get_parameters_string(parameters, false);
	}
	Deserialize_To_Ticker(basements.General_Request_Return(this->curl,url.c_str() ,GET, memorystruct),ticker);
	return ticker;
}

Depth& OKEx_Rest_Connection::Get_Depth(string market_symbol, int size,int merge)
{
	string url = "";
	map<string, int> parameters;
	vector<string> splits = split(market_symbol, '&');
	int sp_c = splits.size();
	if (sp_c == 1)
	{
		url = base_url + "/depth.do?";
		parameters.insert(pair<string, int>("symbol=" + splits[0], 0));
		
	}
	else
	if (sp_c>1)
	{
		url = base_url + "/future_depth.do?";
		parameters.insert(pair<string, int>("symbol=" + splits[0], 0));
		parameters.insert(pair<string, int>("contract_type=" + splits[1], 0));
		if (merge != 0)
		{
			parameters.insert(pair<string, int>("merge=" + Num_To_String<int>(merge, &sstream), 0));
		}
	}
	if (size != 0)
	{
		parameters.insert(pair<string, int>("size=" + Num_To_String<int>(size, &sstream), 0));
	}
	url += basements.Map_To_OrderedString(parameters_string, parameters);
	Deserialize_To_Depth(basements.General_Request_Return(this->curl, url.c_str(),GET,memorystruct),depth);
	return depth;
}

TradesInfo& OKEx_Rest_Connection::Get_TradesInfo(string market_symbol,int size,INT64 since_timestamp)
{
	string url = "";
	map<string, int> parameters;
	vector<string> splits = split(market_symbol, '&');
	int sp_c = splits.size();
	if (sp_c == 1)
	{
		url = base_url + "/trades.do?";
		parameters.insert(pair<string, int>("symbol=" + splits[0], 0));
	}
	else
	if (sp_c>1)
	{
		url = base_url + "/future_trades.do?";
		parameters.insert(pair<string, int>("symbol=" + splits[0], 0));
		parameters.insert(pair<string, int>("contract_type=" + splits[1], 0));
	}
	url += basements.Map_To_OrderedString(parameters_string, parameters);
	Deserialize_To_TradesInfo(basements.General_Request_Return(this->curl, url.c_str(), GET, memorystruct), tradesinfo);
	return tradesinfo;
}

Kline& OKEx_Rest_Connection::Get_Kline(string market_symbol, string kline_type,int size,INT64 since_timestamp)
{
	string url = "";
	map<string, int> parameters;
	vector<string> splits = split(market_symbol, '&');
	int sp_c = splits.size();
	if (sp_c == 1)
	{
		url = base_url + "/kline.do?";
		parameters.insert(pair<string, int>("symbol=" + splits[0], 0));
	}
	else
	if (sp_c>1)
	{
		url = base_url + "/future_kline.do?";
		parameters.insert(pair<string, int>("symbol=" + splits[0], 0));
		parameters.insert(pair<string, int>("contract_type=" + splits[1], 0));
	}
	parameters.insert(pair<string, int>("type=" + kline_type,0));
	if (size != 0)
	{
		parameters.insert(pair<string, int>("size=" + Num_To_String<int>(size,&sstream), 0));
	}
	if (since_timestamp != 0)
	{
		parameters.insert(pair<string, int>("since=" + Num_To_String<INT64>(since_timestamp, &sstream), 0));
	}
	url += basements.Map_To_OrderedString(parameters_string, parameters);
	Deserialize_To_Kline(basements.General_Request_Return(this->curl, url.c_str(), GET, memorystruct), kline);
	return kline;
}

//Account_Information:

AccountInfo& OKEx_Rest_Connection::Get_AccountInfo(string account_symbol)
{
	string url;
	if (account_symbol == "Cash_Account")
	{
		url = base_url + "/userinfo.do?";
	}
	else
		if (account_symbol == "Wallet_Account")
		{
			url = base_url + "/wallet_info.do?";
		}
		else
		if (account_symbol == "Futures_Account")
		{
			url = base_url + "/future_userinfo.do?";
		}
		else
		{
			cout << "wrong account_symbol" << endl;
			return accountinfo;
		}
	map<string, int> parameters;
	parameters.insert(make_pair("api_key=" + apiKey, 0));
	get_parameters_string(parameters, true);
	url += parameters_string;
	basements.General_Request_Return(curl, parameters_string.c_str(), POST, memorystruct);
	Deserialize_To_AccountInfo(memorystruct.memory, account_symbol, accountinfo);
	return accountinfo;
}

PositionsInfo& OKEx_Rest_Connection::Get_PositionsInfo(string market_symbol)
{
	string url;
	vector<string> splits = split(market_symbol, '&');
	map<string, int> parameters;
	int sp_c = splits.size();
	if (sp_c > 1)
	{
		url = base_url + "/future_position.do?";
		parameters.insert(pair<string, int>("symbol=" + splits[0], 0));
		parameters.insert(pair<string, int>("contract_type=" + splits[1], 0));
		parameters.insert(pair<string, int>("api_key=" + apiKey, 0));
		url += get_parameters_string(parameters, true);
	}
	Deserialize_To_PositionsInfo(basements.General_Request_Return(this->curl, url.c_str(), POST, memorystruct), positionsinfo);
	return positionsinfo;
}

OrdersInfo& OKEx_Rest_Connection::Get_OrdersInfo(string market_symbol,bool type,int size,INT64 since_timestamp)
{
	string url;
	map<string, int> parameters;
	vector<string> splits = split(market_symbol, '&');
	int sp_c = splits.size();
	if (sp_c == 1)
	{
		url = base_url + "/order_history.do?";
		parameters.insert(pair<string, int>("symbol=" + splits[0], 0));
		if (type)
		{
			parameters.insert(pair<string, int>("status=1", 0));
		}
		else
		{
			parameters.insert(pair<string, int>("status=0", 0));
		}
	}
	else
		if (sp_c>1)
		{
			url = base_url + "/future_order_info.do?";
			parameters.insert(pair<string, int>("symbol=" + splits[0], 0));
			parameters.insert(pair<string, int>("contract_type=" + splits[1], 0));
			if (type)
			{
				parameters.insert(pair<string, int>("status=2", 0));
			}
			else
			{
				parameters.insert(pair<string, int>("status=1", 0));
			}
			parameters.insert(pair<string, int>("order_id=-1", 0));
		}
	parameters.insert(pair<string, int>("current_page=1", 0));
	parameters.insert(pair<string, int>("api_key="+apiKey, 0));
	if (size != 0)
	{
		parameters.insert(pair<string, int>("page_length=" + Num_To_String<int>(size, &sstream), 0));
	}
	else
	{
		parameters.insert(pair<string, int>("page_length=100", 0));
	}
	if (since_timestamp != 0)
	{
		parameters.insert(pair<string, int>("since=" + Num_To_String<INT64>(since_timestamp, &sstream), 0));
	}
	url += get_parameters_string(parameters, true);
	Deserialize_To_OrdersInfo(basements.General_Request_Return(this->curl, url.c_str(), POST, memorystruct),market_symbol,ordersinfo);
	return ordersinfo;
}

OrderInfo& OKEx_Rest_Connection::Get_OrderInfo(string market_symbol, INT64 order_id)
{
	string url;
	map<string, int> parameters;
	vector<string> splits = split(market_symbol, '&');
	int sp_c = splits.size();
	if (sp_c == 1)
	{
		url = base_url + "/order_info.do?";
		parameters.insert(pair<string, int>("symbol=" + splits[0], 0));
	}
	else
		if (sp_c>1)
		{
			url = base_url + "/future_order_info.do?";
			parameters.insert(pair<string, int>("symbol=" + splits[0], 0));
			parameters.insert(pair<string, int>("contract_type=" + splits[1], 0));
		}
	parameters.insert(pair<string, int>("order_id=" + Num_To_String<INT64>(order_id, &sstream), 0));
	parameters.insert(pair<string, int>("api_key=" + apiKey, 0));
	url += get_parameters_string(parameters, true);
	Deserialize_To_OrderInfo(basements.General_Request_Return(this->curl, url.c_str(), POST, memorystruct), market_symbol, orderinfo);
	return orderinfo;
};

//Manipulation

Manipulation_Response& OKEx_Rest_Connection::Send_Order(string market_symbol, int order_type, double price, double amount,int lever_rate) //1:���� 2:���� 3:ƽ�� 4:ƽ�� or 1,4��  2,3��
{
	string url;
	vector<string> splits = split(market_symbol, '&');
	map<string, int> parameters;
	int sp_c = splits.size();
	if (sp_c == 1)
	{
		url = base_url + "/trade.do?";
		parameters.insert(pair<string, int>("symbol="+splits[0], 0));
		if ((order_type == 1) || (order_type == 4))
		{
			parameters.insert(pair<string, int>("type=buy", 0));
		}
		else
		{
			parameters.insert(pair<string, int>("type=sell", 0));
		}
	}
	else
	if (sp_c > 1)
	{
		url = base_url + "/future_trade.do?";
		parameters.insert(pair<string, int>("symbol=" + splits[0], 0));
		parameters.insert(pair<string, int>("contract_type=" + splits[1], 0));
		parameters.insert(pair<string, int>("type=" + Num_To_String<int>(order_type,&sstream), 0));
		if (lever_rate != 0)
		{
			parameters.insert(pair<string, int>("lever_rate=" + Num_To_String<int>(lever_rate, &sstream), 0));
		}
	}
	parameters.insert(pair<string, int>("price=" + Num_To_String<double>(price, &sstream), 0));
	parameters.insert(pair<string, int>("amount=" + Num_To_String<double>(amount, &sstream), 0));
	parameters.insert(pair<string, int>("api_key=" + apiKey, 0));
	url += get_parameters_string(parameters, true);
	Deserialize_To_Manipulation_Response(basements.General_Request_Return(this->curl, url.c_str(),POST,memorystruct),manipulation_response);
	return manipulation_response;
}

Manipulation_Response& OKEx_Rest_Connection::Cancel_Order(string market_symbol,INT64 order_id)
{
	string url;
	vector<string> splits = split(market_symbol, '&');
	map<string, int> parameters;
	int sp_c = splits.size();
	if (sp_c == 1)
	{
		url = base_url + "/cancel_order.do?";
		parameters.insert(pair<string, int>("symbol=" + splits[0], 0));
	}
	else
		if (sp_c > 1)
		{
			url = base_url + "/future_cancel.do?";
			parameters.insert(pair<string, int>("symbol=" + splits[0], 0));
			parameters.insert(pair<string, int>("contract_type=" + splits[1], 0));
		}
	parameters.insert(pair<string, int>("order_id=" + Num_To_String<INT64>(order_id,&sstream), 0));
	parameters.insert(pair<string, int>("api_key=" + apiKey, 0));
	url += get_parameters_string(parameters, true);
	Deserialize_To_Manipulation_Response(basements.General_Request_Return(this->curl, url.c_str(), POST, memorystruct), manipulation_response);
	return manipulation_response;
};

Manipulation_Response& OKEx_Rest_Connection::Interaccount_Transfer(string source_account_symbol, string target_account_symbol, string coin_symbol, double amount)
{
	string url=base_url+"/funds_transfer.do?";
	map<string, int> parameters;
	int source_account_id=0;
	int target_account_id=0;
	if (source_account_symbol == "Cash_Account")
	{
		source_account_id = 1;
	}
	else
		if (source_account_symbol == "Wallet_Account")
		{
			source_account_id = 6;
		}
		else
	    if (source_account_symbol == "Futures_Account")
		{
			   source_account_id = 3;
		}
	if (target_account_symbol == "Cash_Account")
	{
		target_account_id = 1;
	}
	else
		if (target_account_symbol == "Wallet_Account")
		{
			target_account_id = 6;
		}
		else
			if (target_account_symbol == "Futures_Account")
			{
				target_account_id = 3;
			}
	if ((source_account_id != 0) && (target_account_id != 0))
	{
		parameters.insert(pair<string, int>("from=" + Num_To_String<int>(source_account_id, &sstream), 0));
		parameters.insert(pair<string, int>("to=" + Num_To_String<int>(target_account_id, &sstream), 0));
		parameters.insert(pair<string, int>("amount=" + Num_To_String<double>(amount, &sstream), 0));
		string e_symbol="";
		if ((coin_symbol == "btc")||(coin_symbol == "ltc")||(coin_symbol == "eth")||(coin_symbol == "etc")||(coin_symbol == "bch")
			||(coin_symbol == "xrp")||(coin_symbol == "btg")||(coin_symbol == "eos"))
		{
			e_symbol = coin_symbol + "_usd";
		}
		else
		{
			e_symbol = coin_symbol;
		}
		parameters.insert(pair<string, int>("symbol=" + e_symbol, 0));
		parameters.insert(pair<string, int>("api_key=" + apiKey, 0));
		url += get_parameters_string(parameters, true);
		Deserialize_To_Manipulation_Response(basements.General_Request_Return(this->curl, url.c_str(), POST, memorystruct), manipulation_response);
		return manipulation_response;
	}
	else
	{
		cout << "wrong account_symbol" << endl;
		return manipulation_response;
	}
};

Manipulation_Response& OKEx_Rest_Connection::Withdraw(string source_account_symbol, string coin_symbol, double amount, double fee,string target_address){};



