#include "Trade.h"

Trade::Trade(SCStudyInterfaceRef sc, SCFloatArrayRef OpenContracts, float entry_price, float exit_price,
	size_t id, int use_close_for_entry_exit) :
	sc_{ sc }, num_contracts_at_close_{ static_cast<int>(OpenContracts[sc.Index]) }, id_{ id }
{

	initializeTradeMembers(sc, OpenContracts, use_close_for_entry_exit, entry_price, exit_price);

}


void Trade::initializeTradeMembers(SCStudyInterfaceRef sc, SCFloatArrayRef OpenContracts, int use_close_for_entry_exit, float entry_price, float exit_price) {

	// To Do: break this function into its logical parts

	// establish if trade was long (1) or short (-1)
	trade_side_ = OpenContracts[sc.Index - 1] > 0 ? 1 : -1;


	// Get the number of contracts being closed at signal
	contracts_closed_ = static_cast<int>(abs(OpenContracts[sc.Index - 1] - num_contracts_at_close_));
	
	
	
	// How long ago was the entry? Loop back until the last bar with no open contracts
	// check the number of contracts along the way to determine the max number open at any one time
	// This allows some extensibility in the future - planned support for scaling in / out of positions
	int bars_since_open{};

	max_open_contracts_ = contracts_closed_;
	
	while (OpenContracts[sc.Index-1 - bars_since_open] != 0) {

		if (abs(OpenContracts[sc.Index - 1 - bars_since_open]) > max_open_contracts_)
			max_open_contracts_ = static_cast<int>(abs(OpenContracts[sc.Index - 1 - bars_since_open]));

		++bars_since_open;
	}

	// Set Trade Entry and Exit Times
	exit_time_ = sc.BaseDateTimeIn[sc.Index - 1];
	entry_time_ = sc.BaseDateTimeIn[sc.Index - bars_since_open];


	// check user input, and substitute the bar closes for entry and exit prices if enabled
	if (use_close_for_entry_exit == 1) {
		entry_price_ = static_cast<int>(sc.PriceValueToTicks(sc.Close[sc.Index-bars_since_open]));
		exit_price_ = static_cast<int>(sc.PriceValueToTicks(sc.Close[sc.Index-1]));
	}
	else {
		entry_price_ = static_cast<int>(sc.PriceValueToTicks(entry_price));
		exit_price_ = static_cast<int>(sc.PriceValueToTicks(exit_price));
	}


	// Calculate trade profit / loss
	pnl_in_ticks_ = trade_side_ > 0 ?
		(exit_price_ - entry_price_) * contracts_closed_ :
		(entry_price_ - exit_price_) * contracts_closed_;



	// Get high and low in ticks to calculate mae/mfe
	trade_high_price_ = static_cast<int>(sc.PriceValueToTicks(sc.GetHighest(sc.BaseDataIn[SC_HIGH], sc.Index - 1, bars_since_open)));
	trade_low_price_  = static_cast<int>(sc.PriceValueToTicks(sc.GetLowest(sc.BaseDataIn[SC_LOW], sc.Index-1, bars_since_open)));

	
}


int Trade::get_pnl_in_ticks() const  {
	return pnl_in_ticks_;
}


int Trade::get_mae() const {

	return trade_side_ > 0 ? entry_price_ - trade_low_price_ :
		trade_side_ < 0 ? trade_high_price_ - entry_price_ : 0;
}


int Trade::get_mfe() const {

	return	trade_side_ > 0 ? trade_high_price_ - entry_price_ :
		trade_side_ < 0 ? entry_price_ - trade_low_price_ : 0;
}


int Trade::get_trade_side() const {
	return trade_side_;
}


float Trade::get_trade_pnl() const{

	return (pnl_in_ticks_ * sc_.CurrencyValuePerTick) - sc_.RoundTurnCommission;

}



SCDateTime Trade::get_entry_datetime() const {
	return entry_time_;
}

int Trade::get_trade_duration_in_seconds() const{

	return (exit_time_.GetTimeInSeconds() - entry_time_.GetTimeInSeconds());

}

std::array<std::string, 14>& Trade::get_trade_list_header() {

	return trade_list_header_;

}

std::array< std::string, 14>& Trade::get_trade_record() {

	trade_record_.at(0) = std::to_string(id_);

	trade_record_.at(1) = chart::get_date_string_from_SCDateTime(entry_time_);
	trade_record_.at(2) = chart::get_HHMM_string_from_time_in_seconds(entry_time_.GetTimeInSeconds());
	trade_record_.at(3) = chart::get_date_string_from_SCDateTime(exit_time_);
	trade_record_.at(4) = chart::get_HHMM_string_from_time_in_seconds(exit_time_.GetTimeInSeconds());
	trade_record_.at(5) = std::to_string(get_trade_duration_in_seconds());

	trade_record_.at(6) = std::to_string(trade_side_);
	trade_record_.at(7) = std::to_string(max_open_contracts_);
	trade_record_.at(8) = std::to_string(contracts_closed_);

	trade_record_.at(9) = std::to_string(sc_.TicksToPriceValue(entry_price_));
	trade_record_.at(10) = std::to_string(sc_.TicksToPriceValue(exit_price_));
	trade_record_.at(11) = std::to_string(get_trade_pnl());

	trade_record_.at(12) = std::to_string(get_mae());
	trade_record_.at(13) = std::to_string(get_mfe());


	return trade_record_;
}