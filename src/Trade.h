#ifndef TRADE_H
#define TRADE_H

#include <array>

#include "sierrachart.h"
#include "utilities.h"

class Trade
{
public:

	private:
		size_t id_;

		SCDateTime entry_time_, exit_time_;

		int entry_price_{}, exit_price_{};

		int trade_side_{};

		int max_open_contracts_{}, num_contracts_at_close_{};

		int contracts_closed_{};

		int pnl_in_ticks_{};

		int trade_high_price_{}, trade_low_price_{};

		SCStudyInterfaceRef sc_;

		std::array<std::string, 14> trade_list_header_{
		 "TradeID","EntryDate","EntryTime","ExitDate","ExitTime","Duration",
		 "TradeSide","MaxOpenContracts","ContractsClosed",
		 "EntryPrice","ExitPrice","ProfitLoss","Mae","Mfe" };

		std::array <std::string, 14 > trade_record_;

		void initializeTradeMembers(SCStudyInterfaceRef sc,
			SCFloatArrayRef OpenContracts,
			int use_close_for_entry_exit,
			float entry_price, float exit_price);

	public:
		
		Trade(SCStudyInterfaceRef sc, 
			SCFloatArrayRef OpenContracts, 
			float entry_price, float exit_price,
			size_t id, int use_close_for_entry_exit);

		
		
		int get_pnl_in_ticks() const; 
		int get_mae() const;
		int get_mfe() const;

		float get_trade_pnl() const;
		
		int get_trade_side() const;
		

		int get_trade_duration_in_seconds() const;
		SCDateTime get_entry_datetime() const;

		
		// These return a ref to an array of strings b/c that's 
		// easier to work with in the sierra chart native spreadsheet
		std::array<std::string, 14>& get_trade_list_header();
		std::array< std::string, 14>& get_trade_record();


};


#endif