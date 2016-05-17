#ifndef LOGPARSER_H
#define LOGPARSER_H

#include <iostream>
#include "Ship.h"

class LogParser
{
    public:
        LogParser(std::string path_to_log);
        virtual ~LogParser();

        //false if first ship win, true second ship win
        bool getWinner(Ship& ship_1, Ship& ship_2);

    private:

        std::string log_path;
};

#endif // LOGPARSER_H
