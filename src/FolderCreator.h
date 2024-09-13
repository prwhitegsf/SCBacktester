#ifndef FOLDER_CREATOR_H
#define FOLDER_CREATOR_H

#include <filesystem>

#include "sierrachart.h"
#include "utilities.h"

class FolderCreator
{

	SCString sc_data_dir_{};
	SCStudyInterfaceRef sc_;

	std::string strategy_name_;
	std::string custom_output_folder_;

	std::string date_folder_;

	std::string root_;

public:

	FolderCreator(SCStudyInterfaceRef sc, SCString strategy_name, SCString custom_output_folder);

private:

	void set_root_folder(std::string& custom_output_folder);

	int make_folder(std::string foldername);

	int make_SCBacktester_folder();

	int make_symbol_folder();

	int make_strategy_folder();

	int make_date_range_folder();

	std::string sysTimeToString();

	int make_trade_list_folder();

	int make_stats_folder();
	
public:
	std::string get_output_folder();

};
#endif
