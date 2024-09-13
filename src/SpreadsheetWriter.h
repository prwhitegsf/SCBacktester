#ifndef SPREADSHEET_WRITER_H
#define SPREADSHEET_WRITER_H
#include "sierrachart.h"


#include "StrategyStats.h"
#include "TradeList.h"
#include "EquityCurve.h"
#include "utilities.h"


class SpreadsheetWriter
{


	SCStudyInterfaceRef sc_;
	StrategyStats& stats_;


	void* stat_sheet_{nullptr};
	void* trades_sheet_{ nullptr };

public:
	
	SpreadsheetWriter(SCStudyInterfaceRef sc, StrategyStats& strategy_stats, const char* spreadsheet_name);


	void write_chart_meta_data();

	void clear_cell_block(void* sheet, int start_row, int end_row, int start_col, int end_col) const;

	void clear_cells();

	void write_strategy_stats_block(std::string_view outcome, int col, int row);

	void write_strategy_stats();

	void write_equity_curve(EquityCurve* eq);

	void write_trades_by_day();

	void write_trades_by_time();

	void write_trades();
	

	

};

#endif