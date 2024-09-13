#include "StrategyStats.h"


StrategyStats::StrategyStats(SCStudyInterfaceRef sc,
	std::vector<std::unique_ptr<Trade>>& trade_list)
	: sc_{ sc }, trade_list_{ trade_list }
{

	add_trades();

	calculate_strategy_stats();

}


int StrategyStats::get_count(std::string_view trade_category) {
	return strategy_results_[trade_category].count_;
}


double StrategyStats::get_pnl(std::string_view trade_category) {
	return strategy_results_[trade_category].pnl_;
}


double StrategyStats::get_avg_pnl(std::string_view trade_category) {

	if (strategy_results_[trade_category].count_ != 0)
		return strategy_results_[trade_category].pnl_ / strategy_results_[trade_category].count_;
	else
		return 0;
}


double StrategyStats::get_win_percent(std::string_view trade_category) {

	if (strategy_results_[trade_category].count_ != 0)
		return strategy_results_[trade_category].win_count_ /
		static_cast<double>(strategy_results_[trade_category].count_);
	else
		return 0;

}


double StrategyStats::get_avg_mae(std::string_view trade_category) {

	if (strategy_results_[trade_category].count_ != 0)
		return strategy_results_[trade_category].mae_ /
		static_cast<double>(strategy_results_[trade_category].count_);
	else
		return 0;

}


double StrategyStats::get_avg_mfe(std::string_view trade_category) {

	if (strategy_results_[trade_category].count_ != 0)
		return strategy_results_[trade_category].mfe_ /
		static_cast<double>(strategy_results_[trade_category].count_);
	else
		return 0;

}


double StrategyStats::get_avg_duration_in_seconds(std::string_view trade_category) {
	if (strategy_results_[trade_category].count_ != 0)
		return strategy_results_[trade_category].duration_ /
		static_cast<double>(strategy_results_[trade_category].count_);
	else
		return 0;
}


double StrategyStats::get_profit_factor() {
	if (strategy_results_["loss"].pnl_ != 0)
		return strategy_results_["win"].pnl_ / (strategy_results_["loss"].pnl_ * -1);
	else
		return 0;
}


void StrategyStats::calculate_strategy_stats() {

	for (std::string_view cat : trade_categories_) {

		strategy_results_[cat].avg_pnl = get_avg_pnl(cat);
		strategy_results_[cat].win_percent = get_win_percent(cat);
		strategy_results_[cat].avg_mae = get_avg_mae(cat);
		strategy_results_[cat].avg_mfe = get_avg_mfe(cat);
		strategy_results_[cat].avg_dur = get_avg_duration_in_seconds(cat);

	}

	strategy_results_["all"].profit_factor = get_profit_factor();

}


void StrategyStats::set_outcomes_by_time(int entry_time, float pnl, int mae, int mfe, int duration) {

	const int hour_in_seconds{ 3600 };

	int idx{ entry_time / hour_in_seconds };

	results_by_time_.at(idx).count_ += 1;
	results_by_time_.at(idx).pnl_ += pnl;
	results_by_time_.at(idx).mae_ += mae;
	results_by_time_.at(idx).mfe_ += mfe;
	results_by_time_.at(idx).mfe_ += mfe;
	results_by_time_.at(idx).duration_ += duration;
}


void StrategyStats::set_outcomes_by_day(int day_of_week, float pnl, int mae, int mfe, int duration) {

	results_by_day_[day_of_week].count_ += 1;
	results_by_day_[day_of_week].pnl_ += pnl;
	results_by_day_[day_of_week].mae_ += mae;
	results_by_day_[day_of_week].mfe_ += mfe;
	results_by_day_[day_of_week].duration_ += duration;
}


void StrategyStats::set_trade_outcomes(std::string_view trade_category, float pnl, int mae, int mfe, int duration) {

	strategy_results_[trade_category].count_ += 1;
	strategy_results_[trade_category].pnl_ += pnl;
	strategy_results_[trade_category].mae_ += mae;
	strategy_results_[trade_category].mfe_ += mfe;
	strategy_results_[trade_category].duration_ += duration;

	if (pnl > 0)
		strategy_results_[trade_category].win_count_++;
}



void StrategyStats::add_trades() {

	if (trade_list_.empty())
		return;

	// loop through all trades and
	// aggregate trade outcomes
	for (const std::unique_ptr<Trade>& trade : trade_list_) {

		// pull info for trade
		float pnl = trade->get_trade_pnl();

		int mae = trade->get_mae();
		int mfe = trade->get_mfe();
		int duration = trade->get_trade_duration_in_seconds();
		int entry_time = trade->get_entry_datetime().GetTimeInSeconds();
		int day_of_week = trade->get_entry_datetime().GetDayOfWeek();


		set_trade_outcomes("all", pnl, mae, mfe, duration);



		// filter for longs/shorts
		if (trade->get_trade_side() > 0)
			set_trade_outcomes("longs", pnl, mae, mfe, duration);
		else
			set_trade_outcomes("shorts", pnl, mae, mfe, duration);

		// filter for winners/losers
		if (pnl > 0)
			set_trade_outcomes("win", pnl, mae, mfe, duration);
		else
			set_trade_outcomes("loss", pnl, mae, mfe, duration);

		// sort by time of day
		set_outcomes_by_time(entry_time, pnl, mae, mfe, duration);

		// sort by day of week
		set_outcomes_by_day(day_of_week, pnl, mae, mfe, duration);

	}
}






const std::string& StrategyStats::get_stats_table(
	const std::string& prepend_cols, const std::string& append_cols) {

	
	std::string stat_header;
	std::string stat_data;

	// split prepend cols, if any at line break, and place them at front
	if(!prepend_cols.empty())

	// stats
	for (std::string_view cat : trade_categories_) {

		std::string col_name{ cat };

		stat_header.append(col_name + "_entries" + ",");
		stat_data.append(std::to_string(strategy_results_[cat].count_) + ",");

		stat_header.append(col_name + "_pnl" + ",");
		stat_data.append(std::to_string(strategy_results_[cat].pnl_) + ",");

		stat_header.append(col_name + "_avg_pnl" + ",");
		stat_data.append(std::to_string(strategy_results_[cat].avg_pnl) + ",");

		stat_header.append(col_name + "_win_percent" + ",");
		stat_data.append(std::to_string(strategy_results_[cat].win_percent) + ",");

		stat_header.append(col_name + "_avg_mae" + ",");
		stat_data.append(std::to_string(strategy_results_[cat].avg_mae) + ",");

		stat_header.append(col_name + "_avg_mfe" + ",");
		stat_data.append(std::to_string(strategy_results_[cat].avg_mfe) + ",");

		stat_header.append(col_name + "_avg_duration" + ",");
		stat_data.append(std::to_string(strategy_results_[cat].avg_dur) + ",");

		stat_header.append(col_name + "_profit_factor" + ",");
		stat_data.append(std::to_string(strategy_results_[cat].profit_factor) + ",");

	}

	std::string days_of_week[]{ "Sunday", "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday" };

	// Write By Day
	for (int i{}; i < results_by_day_.size(); i++) {

		stat_header.append(days_of_week[i] + "_TradeCount" + ",");
		stat_data.append(std::to_string(results_by_day_[i].count_) + ",");

		stat_header.append(days_of_week[i] + "_Pnl" + ",");
		stat_data.append(std::to_string(results_by_day_[i].pnl_) + ",");


	}

	// Write By Time
	for (int i{}; i < results_by_time_.size(); i++) {

		int period = i * 60;
		int hour{ period / 60 };

		SCString h;
		h.Format("%02d:00", hour);

		std::string time = h.GetChars();

		stat_header.append(time + "_TradeCount" + ",");
		stat_data.append(std::to_string(results_by_time_[i].count_) + ",");

		stat_header.append(time + "_Pnl" + ",");
		stat_data.append(std::to_string(results_by_time_[i].pnl_) + ",");

	}

	stats_table_ = stat_header + "\n" + stat_data;


	return stats_table_;

}


