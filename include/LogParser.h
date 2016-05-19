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
        bool getWinner(Ship& ship_1, Ship& ship_2);

    private:

        std::string log_path;
};

#endif // LOGPARSER_H
