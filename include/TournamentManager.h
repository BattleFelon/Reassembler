#ifndef TOURNAMENTMANAGER_H
#define TOURNAMENTMANAGER_H

#include "Ship.h"
#include <iostream>

class TournamentManager
{
    public:
        TournamentManager();
        virtual ~TournamentManager();

        void init(std::string path_to_exe, std::string path_to_ships){exe_path = path_to_exe; ships_path = path_to_ships;}
        void startArena(Ship& ship_1, Ship& ship_2);
		void startString(std::string ship_1, std::string ship_2);
		void woop(Ship a, std::string name){std::cout << a.getShipName() << " " << name << "\n";}

    private:
        std::string exe_path;
        std::string ships_path;
};

#endif // TOURNAMENTMANAGER_H
