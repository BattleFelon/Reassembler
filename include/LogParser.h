#ifndef LOGPARSER_H
#define LOGPARSER_H

#include <iostream>
#include "Ship.h"

class LogParser
{
    public:
        LogParser();

        virtual ~LogParser();

        void init(std::string path_to_log){log_path = path_to_log;log_path.append("\\log.txt");}

        //false if first ship win, true second ship win
        int getWinner(Ship& ship_1, Ship& ship_2);
		int getWinner(Ship& ship_1, std::string ship_2);
		int getWinner(std::string fleet_1, std::string fleet_2);

		std::vector<int> getWinner(std::vector<std::string> ships);

    private:

        std::string log_path;
};

#endif // LOGPARSER_H
