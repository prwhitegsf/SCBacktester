#include "TradeList.h"


void TradeList::record_trade(SCStudyInterfaceRef sc, SCFloatArrayRef OpenContracts, float entry_price, float exit_price, int use_close_for_entry_exit) {

	
	auto trade{ std::make_unique<Trade>(sc,OpenContracts, entry_price,exit_price, trade_list_.size()+1, use_close_for_entry_exit) };
	trade_list_.push_back(std::move(trade));

}


float TradeList::get_last_trade_pnl(SCStudyInterfaceRef sc) const {

	if (trade_list_.empty())
		return 0;
	else // returns the last trade's pnl in ticks
		return trade_list_.back()->get_trade_pnl();
}


int TradeList::get_last_trade_mae() const {

	if (trade_list_.empty())
		return 0;
	else // returns the last trade's mae in ticks
		return trade_list_.back()->get_mae();

}


int TradeList::get_last_trade_mfe() const {

	if (trade_list_.empty())
		return 0;
	else // returns the last trade's mfe in ticks
		return trade_list_.back()->get_mfe();

}


void TradeList::clear_trades() {

	if (!trade_list_.empty())
		trade_list_.clear();

}


std::string TradeList::get_trade_list_table() {

	if (trade_list_.empty()) {
		return "null";
	}

	std::string trade_list_table;

	// write the header row
	for (std::string& col_name : trade_list_.at(0)->get_trade_list_header()) {

		trade_list_table.append(col_name + ",");
	}

	trade_list_table.append("\n");

	// write the records
	for (std::unique_ptr<Trade>& trade : trade_list_) {

		for (std::string result : trade->get_trade_record()) {
			trade_list_table.append(result + ",");
		}

		trade_list_table.append("\n");

	}

	return trade_list_table;


}