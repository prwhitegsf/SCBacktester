#include "SpreadsheetWriter.h"

SpreadsheetWriter::SpreadsheetWriter(SCStudyInterfaceRef sc, StrategyStats& strategy_stats, const char* spreadsheet_name)
	: sc_{ sc }, stats_{ strategy_stats }
{
	stat_sheet_ = sc.GetSpreadsheetSheetHandleByName(spreadsheet_name, "StrategyStats", false);
	trades_sheet_ = sc.GetSpreadsheetSheetHandleByName(spreadsheet_name, "Trades", false);

	// clear cells
	clear_cells();

	// write chart meta data: start/end dates, start/end session, bar period
	write_chart_meta_data();



}


void SpreadsheetWriter::write_chart_meta_data() {

	// set symbol
	sc_.SetSheetCellAsString(stat_sheet_, 0, 1, sc_.GetChartSymbol(sc_.ChartNumber));

	// start date
	sc_.SetSheetCellAsString(stat_sheet_, 1, 1, chart::get_date_string_from_bar_index(sc_, 0));
	
	// end date
	sc_.SetSheetCellAsString(stat_sheet_, 2, 1, chart::get_date_string_from_bar_index(sc_, sc_.ArraySize - 1));

	// session start time
	sc_.SetSheetCellAsString(stat_sheet_, 3, 1, chart::get_HHMM_string_from_time_in_seconds(sc_.StartTime1));
	
	// session end time
	sc_.SetSheetCellAsString(stat_sheet_, 4, 1, chart::get_HHMM_string_from_time_in_seconds(sc_.EndTime1));

	// bar period - returns bars period in minutes: 0 = non time based bar
	sc_.SetSheetCellAsDouble(stat_sheet_, 5, 1, chart::get_bar_period_in_minutes(sc_));

}

void SpreadsheetWriter::clear_cell_block(void* sheet, int start_row, int end_row, int start_col, int end_col) const {

	for (int i{ start_col }; i <= end_col; i++) {

		for (int j{ start_row }; j <= end_row; j++) {
			sc_.SetSheetCellAsString(sheet, i, j, " ");
		}
	}

}

void SpreadsheetWriter::clear_cells() {

	// Stat Block
	clear_cell_block(stat_sheet_, 4, 11, 1, 5);

	// equity curve block
	clear_cell_block(stat_sheet_, 14, 17, 1, 2);

	// day of week block
	clear_cell_block(stat_sheet_, 20, 26, 1, 2);

	// Time of day blocks
	// 0 - 7
	clear_cell_block(stat_sheet_, 29, 36, 1, 2);

	// 8 - 15
	clear_cell_block(stat_sheet_, 29, 36, 4, 5);

	// 16 - 23
	clear_cell_block(stat_sheet_, 29, 36, 7, 8);
}

void SpreadsheetWriter::write_strategy_stats_block(std::string_view outcome, int col, int row) {

	// all trades
	sc_.SetSheetCellAsDouble(stat_sheet_, col, row, stats_.get_count(outcome));
	sc_.SetSheetCellAsDouble(stat_sheet_, col, row + 1, stats_.get_pnl(outcome));
	sc_.SetSheetCellAsDouble(stat_sheet_, col, row + 2, stats_.get_avg_pnl(outcome));
	sc_.SetSheetCellAsDouble(stat_sheet_, col, row + 3, stats_.get_win_percent(outcome));
	sc_.SetSheetCellAsDouble(stat_sheet_, col, row + 4, stats_.get_avg_mae(outcome));
	sc_.SetSheetCellAsDouble(stat_sheet_, col, row + 5, stats_.get_avg_mfe(outcome));
	sc_.SetSheetCellAsDouble(stat_sheet_, col, row + 6, stats_.get_avg_duration_in_seconds(outcome) / 60);

}

void SpreadsheetWriter::write_strategy_stats() {

	sc_.SetSheetCellAsDouble(stat_sheet_, 1, 11, stats_.get_profit_factor());

	write_strategy_stats_block("all", 1, 4);
	write_strategy_stats_block("longs", 2, 4);
	write_strategy_stats_block("shorts", 3, 4);
	write_strategy_stats_block("win", 4, 4);
	write_strategy_stats_block("loss", 5, 4);

}


void SpreadsheetWriter::write_equity_curve(EquityCurve* eq) {

	sc_.SetSheetCellAsDouble(stat_sheet_, 1, 14, eq->get_equity_peak());
	sc_.SetSheetCellAsDouble(stat_sheet_, 1, 15, eq->get_equity_valley());
	sc_.SetSheetCellAsDouble(stat_sheet_, 1, 16, eq->get_max_drawdown());

	sc_.SetSheetCellAsDouble(stat_sheet_, 2, 14, eq->get_intratrade_equity_peak());
	sc_.SetSheetCellAsDouble(stat_sheet_, 2, 15, eq->get_intratrade_equity_valley());
	sc_.SetSheetCellAsDouble(stat_sheet_, 2, 16, eq->get_intratrade_max_drawdown());
}

void SpreadsheetWriter::write_trades_by_day() {

	// Sunday = 0, Monday = 1, etc
	for (int i{}; i < stats_.results_by_day_.size(); i++) {

		sc_.SetSheetCellAsDouble(stat_sheet_, 1, 20 + i, stats_.results_by_day_.at(i).count_);
		sc_.SetSheetCellAsDouble(stat_sheet_, 2, 20 + i, stats_.results_by_day_.at(i).pnl_);

	}
}


void SpreadsheetWriter::write_trades_by_time() {

	// To do: These arrays shouldn't overflow, but need to ensure they don't

	// array size = 24, one index for each 1H period of the day
	// on spread sheets these are laid out in groups of 8 hours

	// 00:00 - 07:59
	for (int i{}; i < 8; i++) {
		sc_.SetSheetCellAsDouble(stat_sheet_, 1, 29 + i, stats_.results_by_time_.at(i).count_);
		sc_.SetSheetCellAsDouble(stat_sheet_, 2, 29 + i, stats_.results_by_time_.at(i).pnl_);
	}

	// 08:00 - 15:59
	for (int i{}; i < 8; i++) {

		sc_.SetSheetCellAsDouble(stat_sheet_, 4, 29 + i, stats_.results_by_time_.at(i + 8).count_);
		sc_.SetSheetCellAsDouble(stat_sheet_, 5, 29 + i, stats_.results_by_time_.at(i + 8).pnl_);
	}

	// 16:00 - 23:59
	for (int i{}; i < 8; i++) {

		sc_.SetSheetCellAsDouble(stat_sheet_, 7, 29 + i, stats_.results_by_time_.at(i + 16).count_);
		sc_.SetSheetCellAsDouble(stat_sheet_, 8, 29 + i, stats_.results_by_time_.at(i + 16).pnl_);
	}

}


void SpreadsheetWriter::write_trades() {
	

	if (stats_.trade_list_.empty())
		return;

	int col{};

	for (std::string& col_name : stats_.trade_list_.at(0)->get_trade_list_header()) {

		sc_.SetSheetCellAsString(trades_sheet_, col, 0, col_name.c_str());

		col++;

	}


	int row{ 1 };

	for (auto& record : stats_.trade_list_) {

		col = 0;

		for (std::string& value : record->get_trade_record()) {

			sc_.SetSheetCellAsString(trades_sheet_, col, row, value.c_str());

			col++;
		}

		row++;
	}


}