#include <iostream>
#include <sstream>
#include <fstream>

#include "LogParser.h"
#include "Ship.h"

LogParser::LogParser(std::string path_to_log):
    log_path(path_to_log)
{
    log_path.append("\\log.txt");
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
    if(!logFile.good())
        std::cout << "Log File Path Not Correct !";

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
    int result = 0;
    if(one_win_count < two_win_count)
        result = 1;

    //check if paths are off
    if(one_win_count == two_win_count)
        result = 2;

    return(result);
    //Close it up
    logFile.close();
}
