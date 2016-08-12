#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif // _WIN32


#include "LogParser.h"
#include "Ship.h"

LogParser::LogParser()
{

}

LogParser::~LogParser()
{

}

//False if ship 1, true if ship 2
int LogParser::getWinner(Ship& ship_1, Ship& ship_2)
{
    std::string ship_1_wins = "(";
    ship_1_wins.append(ship_1.getShipName());
    ship_1_wins.append(") wins");

    std::string ship_2_wins = "(";
    ship_2_wins.append(ship_2.getShipName());
    ship_2_wins.append(") wins");

    std::string line;
    std::ifstream logFile (log_path);

    int one_win_count = 0;
    int two_win_count = 0;
    for(unsigned int curLine = 0; getline(logFile, line); curLine++) {
        if (line.find(ship_1_wins) != std::string::npos) {
            one_win_count++;
        }
        if (line.find(ship_2_wins) != std::string::npos) {
            two_win_count++;
        }
    }

	//Close it up
	logFile.close();

	if (one_win_count == two_win_count)
		return(2);
	else
		return(one_win_count < two_win_count);
}

int LogParser::getWinner(Ship& ship_1, std::string ship_2)
{
	std::string ship_1_wins = "(";
	ship_1_wins.append(ship_1.getShipName());
	ship_1_wins.append(") wins");

	std::string ship_2_wins = "(";
	ship_2_wins.append(ship_2);
	ship_2_wins.append(") wins");

	std::string line;
	std::ifstream logFile(log_path);

	int one_win_count = 0;
	int two_win_count = 0;
	for (unsigned int curLine = 0; getline(logFile, line); curLine++) {
		if (line.find(ship_1_wins) != std::string::npos) {
			one_win_count++;
		}
		if (line.find(ship_2_wins) != std::string::npos) {
			two_win_count++;
		}
	}

	//Close it up
	logFile.close();

	if (one_win_count == two_win_count)
		return(2);
	else
		return(one_win_count < two_win_count);
}

int LogParser::getWinner(std::string fleet_1, std::string fleet_2)
{
    //Start with error condition
    int win = 2;

    //Make search strings
	std::string search_1 = "\'";
	std::string search_2 = "\'";

    search_1.append(fleet_1);
    search_2.append(fleet_2);

    //Read it
    std::string line;
    std::ifstream logFile (log_path);

    for (unsigned int curLine = 0; getline(logFile, line); curLine++) {
        if (line.find(search_1) != std::string::npos) {
            win = 0;
        }
        if (line.find(search_2) != std::string::npos) {
            win = 1;
        }
    }

    //Close it up
	logFile.close();

	return(win);
}

std::vector<int> LogParser::getWinner(std::vector<std::string> ships)
{

	#ifdef _WIN32
	//Vector to hold string names
	std::vector<std::string> file_names;
	//make search strings
	std::vector<std::string> serach_strings;
	for (auto ship : ships){
		std::string search = ship;
		search.append(" - ");

		serach_strings.push_back(search);
	}
	//Bool vector as int. too lazy to change
	std::vector<int> did_win(ships.size(), 0);

	HANDLE hFind;
	WIN32_FIND_DATA data;
	std::string path = log_path;
	for (int i = 0; i < 7; ++i)
		path.pop_back();
	path.append("*");

	//do the damn windows thing
	hFind = FindFirstFile(path.c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			file_names.push_back(data.cFileName);
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}

	//remove *
	path.pop_back();

	//Search for result
	for (int i = 0; i < file_names.size(); ++i){
		std::string new_file_name = path;
		new_file_name.append(file_names[i]);

		std::ifstream logFile(new_file_name);

		//File reading
		std::string line;

		//bool for getting rid of file
		bool did_find = false;
		for (unsigned int curLine = 0; getline(logFile, line) && !did_find; curLine++) {
			for (int i = 0; i < serach_strings.size() && !did_find; ++i){
				if (line.find(serach_strings[i]) != std::string::npos) {
					did_win[i]++;
					did_find = true;
				}
			}
		}
		if (did_find){
			logFile.close();
			std::remove(new_file_name.c_str());
		}
	}

	return(did_win);
	#endif // _WIN32
	#ifdef __linux__
	std::vector<int> results(ships.size());
	for(auto& result : results)
        result = rand() % 10;
    return(results);
	#endif // __linux__
}

