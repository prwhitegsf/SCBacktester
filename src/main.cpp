#include "sierrachart.h"
#include "pointer_manager.h"
#include "SpreadsheetWriter.h"
#include "CsvWriter.h"
#include "FolderCreator.h"
#include "utilities.h"



SCDLLName("SCBacktester")

SCSFExport scsf_SCBacktester(SCStudyInterfaceRef sc)
{

	//inputs
	SCInputRef OpenContractsRef = sc.Input[0];
	SCInputRef UseBarCloseForEntryExit = sc.Input[1];
	SCInputRef EntryPriceRef = sc.Input[2];
	SCInputRef ExitPriceRef = sc.Input[3];
	SCInputRef SCOutputSpreadsheet = sc.Input[4];
	SCInputRef WriteTradesButton = sc.Input[5];

	//outputs
	SCSubgraphRef OpenContracts = sc.Subgraph[0];
	SCSubgraphRef CumulativePnl = sc.Subgraph[1];
	SCSubgraphRef MaxDrawdown = sc.Subgraph[2];
	SCSubgraphRef EquityPeak = sc.Subgraph[3];
	SCSubgraphRef EquityValley = sc.Subgraph[4];
	SCSubgraphRef IntratradePeak = sc.Subgraph[5];
	SCSubgraphRef IntratradeValley = sc.Subgraph[6];
	SCSubgraphRef IntratradeMaxDrawdown = sc.Subgraph[7];

	//subArrays

	if (sc.SetDefaults)
	{
		sc.GraphName = "SCBacktester";
		sc.GraphRegion = 1;
		sc.AutoLoop = 1;

		//inputs
		OpenContractsRef.Name = "OpenContractsRef";
		UseBarCloseForEntryExit.Name = "UseBarCloseForEntryExit";
		EntryPriceRef.Name = "EntryPriceRef";
		ExitPriceRef.Name = "ExitPriceRef";
		SCOutputSpreadsheet.Name = "SCOutputSpreadsheet";
		WriteTradesButton.Name = "WriteTradesButton";

		OpenContractsRef.SetChartStudySubgraphValues(1, 8, 0);
		UseBarCloseForEntryExit.SetYesNo(1);
		EntryPriceRef.SetChartStudySubgraphValues(1, 8, 2);
		ExitPriceRef.SetChartStudySubgraphValues(1, 8, 35);
		SCOutputSpreadsheet.SetString("SCBacktesterSheet");
		WriteTradesButton.SetInt(1);

		//outputs
		OpenContracts.Name = "OpenContracts";
		CumulativePnl.Name = "CumulativePnl";
		MaxDrawdown.Name = "MaxDrawdown";
		EquityPeak.Name = "EquityPeak";
		EquityValley.Name = "EquityValley";
		IntratradePeak.Name = "IntratradePeak";
		IntratradeValley.Name = "IntratradeValley";
		IntratradeMaxDrawdown.Name = "IntratradeMaxDrawdown";

		OpenContracts.DrawStyle = DRAWSTYLE_BACKGROUND_TRANSPARENT;
		CumulativePnl.DrawStyle = DRAWSTYLE_LINE;
		MaxDrawdown.DrawStyle = DRAWSTYLE_IGNORE;
		EquityPeak.DrawStyle = DRAWSTYLE_IGNORE;
		EquityValley.DrawStyle = DRAWSTYLE_IGNORE;
		IntratradePeak.DrawStyle = DRAWSTYLE_IGNORE;
		IntratradeValley.DrawStyle = DRAWSTYLE_IGNORE;
		IntratradeMaxDrawdown.DrawStyle = DRAWSTYLE_IGNORE;
		OpenContracts.PrimaryColor = RGB(0, 255, 0);
		CumulativePnl.PrimaryColor = RGB(0, 255, 0);
		MaxDrawdown.PrimaryColor = RGB(255, 0, 255);
		EquityPeak.PrimaryColor = RGB(0, 255, 0);
		EquityValley.PrimaryColor = RGB(0, 255, 0);
		IntratradePeak.PrimaryColor = RGB(0, 255, 0);
		IntratradeValley.PrimaryColor = RGB(0, 255, 0);
		IntratradeMaxDrawdown.PrimaryColor = RGB(128, 0, 128);
		OpenContracts.AutoColoring = AUTOCOLOR_POSNEG;
		CumulativePnl.AutoColoring = AUTOCOLOR_POSNEG;
		EquityPeak.AutoColoring = AUTOCOLOR_POSNEG;
		EquityValley.AutoColoring = AUTOCOLOR_POSNEG;
		IntratradePeak.AutoColoring = AUTOCOLOR_POSNEG;
		IntratradeValley.AutoColoring = AUTOCOLOR_POSNEG;
		OpenContracts.SecondaryColor = RGB(255, 0, 0);
		CumulativePnl.SecondaryColor = RGB(255, 0, 0);
		EquityPeak.SecondaryColor = RGB(255, 0, 0);
		EquityValley.SecondaryColor = RGB(255, 0, 0);
		IntratradePeak.SecondaryColor = RGB(255, 0, 0);
		IntratradeValley.SecondaryColor = RGB(255, 0, 0);

		MaxDrawdown.DrawStyle = DRAWSTYLE_IGNORE;
		EquityPeak.DrawStyle = DRAWSTYLE_IGNORE;
		EquityValley.DrawStyle = DRAWSTYLE_IGNORE;
		IntratradePeak.DrawStyle = DRAWSTYLE_IGNORE;
		IntratradeValley.DrawStyle = DRAWSTYLE_IGNORE;
		IntratradeMaxDrawdown.DrawStyle = DRAWSTYLE_IGNORE;

		return;
	}




	// We use Sierra Chart's built in persistent pointer functionality to store and accumulate 
	// trade data across successive function calls
	TradeList* trade_list = reinterpret_cast<TradeList*>(sc.GetPersistentPointer(0));
	EquityCurve* eq = reinterpret_cast<EquityCurve*>(sc.GetPersistentPointer(1));
	
	pointer_manager ptr;

	// Validate Pointers, release pointers on last call to function
	if (ptr.process_pointers(sc, trade_list, eq) == -1) {
		return;
	}





	// Reset trade list and equity curve on chart reload
	if (sc.IsFullRecalculation && sc.Index == 0) {
		trade_list->clear_trades();
		eq->reset();
	}





	// get open contracts from strategy study
	OpenContracts[sc.Index] = subgraph::get_value_from_study(sc, OpenContractsRef.GetStudyID(), OpenContractsRef.GetSubgraphIndex(), 0);





	// Check to see if a we've closed a contract - if so process the closed trade and add it to the trade list
	if (OpenContracts[sc.Index - 1] != 0
	&&  OpenContracts[sc.Index - 1] != OpenContracts[sc.Index]) {
	
		// Get the entry and exit prices from strategy study
		float entry_price = 
			subgraph::get_value_from_study(sc,EntryPriceRef.GetStudyID(), EntryPriceRef.GetSubgraphIndex(), 1);

		float exit_price = 
			subgraph::get_value_from_study(sc,ExitPriceRef.GetStudyID(), ExitPriceRef.GetSubgraphIndex(), 1);

		
		// record the trade
		trade_list->record_trade(sc, OpenContracts, entry_price, exit_price, UseBarCloseForEntryExit.GetYesNo());


		// update equity curve with trade results
		eq->set_trade_info(sc, trade_list->get_last_trade_pnl(sc), 
							trade_list->get_last_trade_mae(), 
							trade_list->get_last_trade_mfe());
	
	}




	// write equity curve subgraphs
	CumulativePnl[sc.Index] = eq->get_cumulative_pnl();
	MaxDrawdown[sc.Index] = eq->get_max_drawdown() * -1.0;

	EquityPeak[sc.Index] = eq->get_equity_peak();
	EquityValley[sc.Index] = eq->get_equity_valley();

	IntratradePeak[sc.Index] = eq->get_intratrade_equity_peak();
	IntratradeValley[sc.Index] = eq->get_intratrade_equity_valley();
	IntratradeMaxDrawdown[sc.Index] = eq->get_intratrade_max_drawdown()*-1.0;

	



	// on the last closed bar of the chart, aggregate trades, calculate stats and output
	if (sc.Index == sc.ArraySize - 2) {

		
		// calculate aggregate stats
		StrategyStats strategy_stats{sc, trade_list->trade_list_ };


		// write to spreadsheet
		SCString temp_name = SCOutputSpreadsheet.GetString();
		const char* spreadsheet_name; 
		spreadsheet_name = temp_name.GetChars();

		SpreadsheetWriter sw(sc, strategy_stats, spreadsheet_name);
		sw.write_strategy_stats();
		sw.write_equity_curve(eq);
		sw.write_trades_by_day();
		sw.write_trades_by_time();
		sw.write_trades();

	
		// if write button enabled, write stats to csv files
		if (sc.GetCustomStudyControlBarButtonEnableState(WriteTradesButton.GetInt()))	
		{
			sc.SetCustomStudyControlBarButtonEnable(WriteTradesButton.GetInt(), 0);
			
			// Set Folders
			FolderCreator fc(sc, "strat", "");

			// CSV Writer
			CsvWriter csv(sc);

			csv.write_table_to_csv(fc.get_output_folder(), "TradeList.csv",
				trade_list->get_trade_list_table());

			csv.write_table_to_csv(fc.get_output_folder(), "Stats.csv",
				strategy_stats.get_stats_table());
		}

	}


	






}