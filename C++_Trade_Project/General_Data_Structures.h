#pragma once
#include "stdafx.h"
//#include "../libcurl-vc15-x64-release-static-ipv6-sspi-winssl/include/curl/curl.h"
#include <curl/curl.h>
#include <string>
#include "time.h"
#include <map>

using namespace std;

class Error
{
public:
	int error_type=-1;
	int error_code=0;
	string error_message;
};
class ReturnInfo
{
public:
	ReturnInfo()
	{
		error = new Error();
	}
	ReturnInfo(const ReturnInfo& returninfo)
	{
		this->error = new Error;
		*this->error = *returninfo.error;
	}
	~ReturnInfo()
	{
		delete error;
	}
	bool if_ok = false;
	string raw_reutrn;
	Error* error;
};

//***Market_Information_Response_DataStructure***

//TIcker
class Ticker
{
public:
	INT64 server_timestamp;
	double last_price;
	ReturnInfo returninfo;
};
//Depth
class Depth_Unit
{
private:
	int asks_num = 0;
	int bids_num = 0;
public:
	Depth_Unit()
	{}
	Depth_Unit(int asks_num, int bids_num)
	{
		this->asks_num = asks_num;
		this->bids_num = bids_num;
		asks = new double*[asks_num];
		for (int i = 0; i < asks_num; i++)
		{
			asks[i] = new double[2];
		}
		bids = new double*[bids_num];
		for (int i = 0; i < bids_num; i++)
		{
			bids[i] = new double[2];
		}
	}
	Depth_Unit(const Depth_Unit& depth_unit)
	{
		asks_num = depth_unit.asks_num;
		bids_num = depth_unit.bids_num;
		asks = new double*[asks_num];
		for (int i = 0; i < asks_num; i++)
		{
			asks[i] = new double[2];
			asks[i][0] = depth_unit.asks[i][0];
			asks[i][1] = depth_unit.asks[i][1];
		}
		bids = new double*[bids_num];
		for (int i = 0; i < bids_num; i++)
		{
			bids[i] = new double[2];
			bids[i][0] = depth_unit.bids[i][0];
			bids[i][1] = depth_unit.bids[i][1];
		}
	}
	~Depth_Unit()
	{
		for (int i = 0; i < asks_num; i++)
		{
			delete[] asks[i];
		}
		delete[] asks;
		for (int i = 0; i < bids_num; i++)
		{
			delete[] bids[i];
		}
		delete[] bids;
	}
	void set_scale(int asks_num, int bids_num)
	{
		if (asks != nullptr)
		{
			for (int i = 0; i < asks_num; i++)
			{
				delete[] asks[i];
			}
			delete[] asks;
		}
		if (bids != nullptr)
		{
			for (int i = 0; i < bids_num; i++)
			{
				delete[] bids[i];
			}
			delete[] bids;
		}
		this->asks_num = asks_num;
		this->bids_num = bids_num;
		asks = new double*[asks_num];
		for (int i = 0; i < asks_num; i++)
		{
			asks[i] = new double[2];
		}
		bids = new double*[bids_num];
		for (int i = 0; i < bids_num; i++)
		{
			bids[i] = new double[2];
		}
	}
	void set_asks_num(int asks_num)
	{
		if (asks != nullptr)
		{
			for (int i = 0; i < asks_num; i++)
			{
				delete[] asks[i];
			}
			delete[] asks;
		}
		this->asks_num = asks_num;
		asks = new double*[asks_num];
		for (int i = 0; i < asks_num; i++)
		{
			asks[i] = new double[2];
		}
	}
	void set_bids_num(int bids_num)
	{
		if (bids != nullptr)
		{
			for (int i = 0; i < bids_num; i++)
			{
				delete[] bids[i];
			}
			delete[] bids;
		}
		this->bids_num = bids_num;
		bids = new double*[bids_num];
		for (int i = 0; i < bids_num; i++)
		{
			bids[i] = new double[2];
		}
	}
	int get_asks_num()
	{
		return asks_num;
	}
	int get_bids_num()
	{
		return bids_num;
	}
	double** asks = nullptr;
	double** bids = nullptr;
};
class Depth
{
public:
	Depth_Unit depth_data;
	ReturnInfo returninfo;
};
//TradesInfo
class TradeInfo_Unit
{
public:
	INT64 timestamp;
	double amount;
	double price;
};
class TradesInfo
{
private:
	int num = 0;
public:
	TradesInfo(){};
	TradesInfo(int num)
	{
		this->num = num;
		this->tradesinfo_data = new TradeInfo_Unit[num];
	}
	TradesInfo(const TradesInfo& tradesinfo)
	{
		num = tradesinfo.num;
		returninfo = tradesinfo.returninfo;
		tradesinfo_data = new TradeInfo_Unit[num];
		for (int i = 0; i < num; i++)
		{
			tradesinfo_data[i] = tradesinfo.tradesinfo_data[i];
		}
		this->returninfo = tradesinfo.returninfo;
	}
	~TradesInfo()
	{
		delete[] tradesinfo_data;
	}
	void set_scale(int num)
	{
		this->num = num;
		if (tradesinfo_data != nullptr)
		{
			delete[] tradesinfo_data;
		}
		tradesinfo_data = new TradeInfo_Unit[num];
	}
	int get_num()
	{
		return this->num;
	}
	TradeInfo_Unit* tradesinfo_data;
	ReturnInfo returninfo;
};
//Kline
class Kline_Unit
{
public:
	INT64 timestamp;
	double start;
	double high;
	double low;
	double end;
	double amount;
};
class Kline
{
private:
	int num;
public:
	Kline(){};
	Kline(int num)
	{
		this->num = num;
		this->kline_data = new Kline_Unit[num];
	}
	Kline(const Kline& kline)
	{
		num = kline.num;
		kline_data = new Kline_Unit[num];
		for (int i = 0; i < num; i++)
		{
			kline_data[i] = kline.kline_data[i];
		}
		this->returninfo = kline.returninfo;
	}
	~Kline()
	{
		delete[] kline_data;
	}
	void set_scale(int num)
	{
		this->num = num;
		if (kline_data != nullptr)
		{
			delete[] kline_data;
		}
		kline_data = new Kline_Unit[num];
	}
	int get_num()
	{
		return this->num;
	}
	Kline_Unit* kline_data;
	ReturnInfo returninfo;
};

//***Account_Information_Response_DataStructure***

//AccountInfo
class Cash_AccountInfo_Unit
{
public:
	Cash_AccountInfo_Unit(){};
	Cash_AccountInfo_Unit(string coin_symbol,double total_hold,double free,double freezed)
	{
		this->coin_symbol = coin_symbol;
		this->total_hold = total_hold;
		this->free = free;
		this->freezed = freezed;
	}
	string coin_symbol;
	double total_hold;
	double free;
	double freezed;
};
class Futures_AccountInfo_Unit
{
public:
	string coin_symbol;
	double account_rights;
	double keep_deposit;
	double profit_real;
	double profit_unreal;
	double risk_rate;
};

class AccountInfo_Data
{
public:
	AccountInfo_Data(){};
	AccountInfo_Data(const AccountInfo_Data& accountinfo_data)
	{
		account_symbol = accountinfo_data.account_symbol;
		wallet_account = new map<string, Cash_AccountInfo_Unit>;
		*wallet_account = *accountinfo_data.wallet_account;
		cash_account = new map<string, Cash_AccountInfo_Unit>;
		*cash_account = *accountinfo_data.cash_account;
		futures_account = new map<string, Futures_AccountInfo_Unit>;
		*futures_account = *accountinfo_data.futures_account;
	}
	~AccountInfo_Data()
	{
		delete wallet_account;
		delete cash_account;
		delete futures_account;
	}
	string account_symbol;
	map<string, Cash_AccountInfo_Unit>* wallet_account=nullptr;
	map<string, Cash_AccountInfo_Unit>* cash_account=nullptr;
	map<string, Futures_AccountInfo_Unit>* futures_account=nullptr;
};
class AccountInfo
{
public:
	AccountInfo_Data accountinfo_data;
	ReturnInfo returninfo;
};
//PositionsInfo
class PositionInfo_Unit
{
public:
	string market_symbol;
	string contract_symbol;
	INT64 contract_id;
	string unit_describe;
	int lever_rate;
	INT64 delivery_timestamp;
	INT64 create_timestamp;
	int position_type;  //0:空仓, 1:多仓 
	double amount;
	double start_price_avg; //开仓均价
	double counting_price_base; //结算基准价
	double profit_real; //已实现盈余
};
class PositionsInfo_Data
{
private:
	int num;
public:
	PositionsInfo_Data(){};
	PositionsInfo_Data(int num)
	{
		this->num = num;
		positioninfo_units = new PositionInfo_Unit[num];
	}
	PositionsInfo_Data(const PositionsInfo_Data& positionsinfo_data)
	{
		num = positionsinfo_data.num;
		positioninfo_units = new PositionInfo_Unit[num];
		for (int i = 0; i < num; i++)
		{
			positioninfo_units[i] = positionsinfo_data.positioninfo_units[i];
		}
		force_liqu_price = positionsinfo_data.force_liqu_price;
	}
	~PositionsInfo_Data()
	{
		delete[] positioninfo_units;
	}
	void set_scale(int num)
	{
		this->num = num;
		if (positioninfo_units != nullptr)
		{
			delete[] positioninfo_units;
		}
		positioninfo_units = new PositionInfo_Unit[num];
	}
	int get_num()
	{
		return num;
	}
	PositionInfo_Unit* positioninfo_units;
	double force_liqu_price;
};
class PositionsInfo
{
public:
	PositionsInfo_Data positionsinfo_data;
	ReturnInfo returninfo;
};
//OrdersInfo
class OrderInfo_Unit
{
public:
	string market_symbol;
	bool if_futures;
	
	int lever_rate;
	INT64 delivery_timestamp;
	
	INT64 create_timestamp;
	int order_type;
	INT64 order_id;
	double total_amount;
	double deal_amount;
	double set_price;
	double deal_price_avg;
	int status;
	
};
class OrdersInfo
{
private:
	int num = 0;
public:
	OrdersInfo() {};
	OrdersInfo(int num)
	{
		this->num = num;
		this->ordersinfo_data = new OrderInfo_Unit[num];
	}
	OrdersInfo(const OrdersInfo& ordersinfo)
	{
		num = ordersinfo.num;
		ordersinfo_data = new OrderInfo_Unit[num];
		for (int i = 0; i < num; i++)
		{
			ordersinfo_data[i] = ordersinfo.ordersinfo_data[i];
		}
		returninfo = ordersinfo.returninfo;
	}
	~OrdersInfo()
	{
		delete[] ordersinfo_data;
	}
	void set_scale(int num)
	{
		this->num = num;
		if (ordersinfo_data != nullptr)
		{
			delete[] ordersinfo_data;
		}
		ordersinfo_data = new OrderInfo_Unit[num];
	}
	int get_num()
	{
		return num;
	}
	OrderInfo_Unit* ordersinfo_data;
	ReturnInfo returninfo;
};
class OrderInfo
{
public:
	OrderInfo_Unit orderinfo_data;
	ReturnInfo returninfo;
};

//***Manipulation_Response_DataStructure***
class Manipulation_Response
{
public:
	INT64 order_id;
	bool if_order_id_valid;
	ReturnInfo returninfo;
};