#ifndef STRATEGY_STATS_H
#define STRATEGY_STATS_H


#include <vector>
#include<memory>
#include<unordered_map>
#include<array>
#include <type_traits>

#include "sierrachart.h"
#include "Trade.h"




class StrategyStats
{

private:
	class StrategyResults {

	public:
		
		int trade_side_{};

		double pnl_{}, avg_pnl{};
		
		int count_{}, win_count_{};
		double win_percent{};
		
		int mae_{}, mfe_{};
		double avg_mae{}, avg_mfe{};
		
		
		int duration_{};
		double avg_dur{};

		double profit_factor{};

	};

	SCStudyInterfaceRef sc_;


public:
	
	const std::vector<std::unique_ptr<Trade>>& trade_list_;
	
	const std::array<std::string_view, 5> trade_categories_{ "all","longs","shorts","win","loss" };

	
	// hold the StrategyResults for each trade category
	std::unordered_map<std::string_view, StrategyResults> strategy_results_;
	
	// holds a col header row and a values row for all stats
	std::string stats_table_;
	
	// 0 = Sunday, 1 = Monday, etc
	std::array<StrategyResults, 7> results_by_day_;

	// Splits day into 24 1Hr intervals
	// index[0] = 00:00, index[1] = 01:00, index[2] = 2:00, etc
	std::array<StrategyResults, 24> results_by_time_;
	


	StrategyStats(SCStudyInterfaceRef sc,std::vector<std::unique_ptr<Trade>>& trade_list);

	
	int get_count(std::string_view trade_category);

	double get_pnl(std::string_view trade_category);

	double get_avg_pnl(std::string_view trade_category);

	double get_win_percent(std::string_view trade_category);

	double get_avg_mae(std::string_view trade_category);

	double get_avg_mfe(std::string_view trade_category);

	double get_avg_duration_in_seconds(std::string_view trade_category);

	double get_profit_factor();


	const std::string& get_stats_table(
		const std::string& prepend_cols="", 
		const std::string& append_cols = "");


private:	
	
	void calculate_strategy_stats();
	
	void set_outcomes_by_time(int entry_time, float pnl, int mae, int mfe, int duration);

	void set_outcomes_by_day(int day_of_week, float pnl, int mae, int mfe, int duration);

	void set_trade_outcomes(std::string_view trade_category, float pnl, int mae, int mfe, int duration);

	void add_trades();


};

#endif
