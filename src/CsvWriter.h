#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include <unordered_map>

#include "sierrachart.h"
#include "StrategyStats.h"


class CsvWriter
{

	SCStudyInterfaceRef sc_;

public:

	CsvWriter(SCStudyInterfaceRef sc);

	
	void write_table_to_csv(std::string filepath, std::string filename,
		const std::string& data);


	void write_table_to_csv(std::string filepath, std::string filename,
		std::vector<std::vector<double>> data_block, std::vector<std::string_view> header);
};


#endif
