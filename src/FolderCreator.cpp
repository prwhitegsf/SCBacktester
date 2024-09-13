#include "FolderCreator.h"


FolderCreator::FolderCreator(SCStudyInterfaceRef sc, SCString strategy_name, SCString custom_output_folder)
	: sc_{ sc },
	sc_data_dir_{ sc.DataFilesFolder() }
{

	custom_output_folder_ = custom_output_folder.GetChars();

	// test custom output folder validity
	// if valid, set it as root, otherwise use sc_data_dir
	set_root_folder(custom_output_folder_);
	strategy_name_ = strategy_name.Append("\\").GetChars();

	make_SCBacktester_folder();
	make_symbol_folder();
	make_strategy_folder();
	make_date_range_folder();

}


void FolderCreator::set_root_folder(std::string& custom_output_folder) {

	// remove quotes from user input if they exist
	if (custom_output_folder.starts_with("\""))
		custom_output_folder.erase(0, 1);

	if (custom_output_folder.ends_with("\""))
		custom_output_folder.erase(custom_output_folder.length() - 1, 1);


	const std::string valid_dir{ "C:\\" };

	if (custom_output_folder.empty()
		|| custom_output_folder.compare(0, 3, valid_dir))
	{

		// set root based on sc_data_dir
		root_ = sc_data_dir_.GetChars();
		root_.append("SCBacktester\\");

	}
	else
	{

		root_ = custom_output_folder.append("\\SCBacktester\\");

	}

	sc_.AddMessageToLog(root_.c_str(), 1);

}


int FolderCreator::make_folder(std::string foldername) {

	std::filesystem::path folder = foldername;

	if (!std::filesystem::exists(folder)) {
		std::filesystem::create_directory(folder);
		return 0;
	}
	else {

		printer::message(sc_, "folder already exists, skipping write: ", foldername.c_str());
		return -1;

	}
}


int FolderCreator::make_SCBacktester_folder() {

	// sets the root folder 
	return make_folder(root_);

}


int FolderCreator::make_symbol_folder() {

	std::string symbol = sc_.GetChartSymbol(sc_.ChartNumber).GetChars();

	root_.append(symbol + "\\");
	
	// sets the root folder 
	return make_folder(root_);

}

int FolderCreator::make_strategy_folder() {

	
	return make_folder(root_ + strategy_name_);

}

int FolderCreator::make_date_range_folder() {

	std::string start_date = chart::get_date_string_from_bar_index(sc_, 0).GetChars();
	std::string end_date = chart::get_date_string_from_bar_index(sc_, sc_.ArraySize - 1).GetChars();

	date_folder_ = root_ + strategy_name_ + start_date + "_" + end_date + "__" + sysTimeToString() + "\\";

	return make_folder(date_folder_);

}

std::string FolderCreator::sysTimeToString() {

	// To do: compiler says std::localtime is deprecated

	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d%H%M%S"); 
	return ss.str();

}


int FolderCreator::make_trade_list_folder() {

	return make_folder(date_folder_ + "TradeList\\");

}


int FolderCreator::make_stats_folder() {

	return make_folder(date_folder_ + "Stats\\");

}


std::string FolderCreator::get_output_folder() {
		return date_folder_;
	}