#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <locale>
#include <functional>
#include <typeinfo>

class ini_parser {
private:
	std::ifstream FILE;
	std::map<std::string, std::map<std::string, std::string>> sections;
public:
	ini_parser(std::string file_name) {
		if (file_name.find(".ini") == std::string::npos) {
			throw "Name of the file don't have .ini postfix";
		}
		else {
			FILE.open(file_name);
			if (FILE.is_open()) {
				std::string line;
				std::string current_section;
				int line_number = 0;
				while (std::getline(FILE, line)) {
					line_number++;
					std::string symbol = ";";
					std::string firstChar(1, line[0]);
					if (firstChar == symbol) {
						continue;
					}
					else {
						line.erase(std::remove_if(line.begin(), line.end(), std::bind(std::isspace < char >,
							std::placeholders::_1,
							std::locale::classic())),
							line.end());
						size_t bracket_pos_open = line.find("[");
						size_t bracket_pos_close = line.find("]");
						if (bracket_pos_open != std::string::npos && bracket_pos_close != std::string::npos) {
							current_section = line.substr(bracket_pos_open+1, bracket_pos_close - bracket_pos_open - 1);
						}
						else {
							std::string delimiter = "=";
							size_t delimiter_pos = line.find(delimiter);
							if (delimiter_pos != std::string::npos) {
								std::string value_name = line.substr(0, delimiter_pos);
								std::string value_score = "";
								size_t line_end = line.find(";");
								if (line_end != std::string::npos) {
									value_score = line.substr(delimiter_pos+1, line_end - delimiter_pos - 1);
								}
								else {
									value_score = line.substr(delimiter_pos+1, line.length() - delimiter_pos);
								}
								std::map<std::string, std::string>& tmp_set = sections[current_section];
								tmp_set[value_name] = value_score;
							} else {
								continue;
							}
						}
					}
				}
			}
			else {
				throw "File coudln't be open. Please check if file exist";
			}
		}
	}

	~ini_parser() {
		FILE.close();
	}

	template <typename T>
	T get_value(std::string value) {
		size_t dot_pos = value.find(".");
		std::string sec = value.substr(0, dot_pos);
		std::string val = value.substr(dot_pos+1, value.length() - dot_pos);
		for (auto section : sections) {
			if (section.first != sec) {
				continue;
			}
			else {
				std::string elements = "";
				for (auto value : section.second) {
					if (value.first != val) {
						elements += value.first + " ";
						continue;
					}
					else {
						std::stringstream buffer(value.second);
						T var;
						buffer >> var;
						return var;
					}
				}
				std::cout << "There is no value '" << val << "' in section '" << sec << "'" << std::endl;
				std::cout << "Section '" << sec << "' consist next values: " << elements << std::endl;
			}
		}
		return 0;
	}
};

int main(int argc, char argv[]) {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	ini_parser parser("filename.ini");
	auto value = parser.get_value<int>("Section1.var1");
	std::cout << "Your value is: " << value << std::endl;
	return 0;
}