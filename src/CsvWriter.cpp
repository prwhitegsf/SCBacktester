#include "CsvWriter.h"

CsvWriter::CsvWriter(SCStudyInterfaceRef sc)
	: sc_{ sc } {}

void CsvWriter::write_table_to_csv(std::string filepath, std::string filename, const std::string& data)
{

	std::ofstream file;
	file.open(filepath + filename);

	file << data;


}

void CsvWriter::write_table_to_csv(std::string filepath, std::string filename,
	std::vector<std::vector<double>> data_block, std::vector<std::string_view> header) {

	std::ofstream file;
	file.open(filepath + filename);

	// write header
	for (std::string_view col_name : header) {
		file << col_name;
		file << ",";
	}

	// next line
	file << "\n";


	// write data block
	for (std::vector<double>& row : data_block) {

		for (double value : row) {
			file << std::to_string(value);
			file << ",";
		}

		file << "\n";
	}

}