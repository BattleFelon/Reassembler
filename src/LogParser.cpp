#include <iostream>
#include <sstream>
#include <fstream>

#include "LogParser.h"
#include "Ship.h"

LogParser::LogParser()
{

}

LogParser::~LogParser()
{

}

//False if ship 1, true if ship 2
bool LogParser::getWinner(Ship& ship_1, Ship& ship_2)
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

    return(one_win_count < two_win_count);
    //Close it up
    logFile.close();
}
