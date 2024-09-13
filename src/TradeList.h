#ifndef TRADELIST_H
#define TRADELIST_H
#include <vector>
#include <memory>
#include "Trade.h"


class TradeList
{


public:
	
	std::vector<std::unique_ptr<Trade>> trade_list_;

	void record_trade(SCStudyInterfaceRef sc, SCFloatArrayRef OpenContracts, float entry_price, float exit_price, int use_close_for_entry_exit);

	float get_last_trade_pnl(SCStudyInterfaceRef sc) const;

	int get_last_trade_mae() const;

	int get_last_trade_mfe() const;

	void clear_trades();

	std::string get_trade_list_table();

};

#endif