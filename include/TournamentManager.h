#ifndef TOURNAMENTMANAGER_H
#define TOURNAMENTMANAGER_H

#include "Ship.h"


class TournamentManager
{
    public:
        TournamentManager(std::string path_to_exe, std::string path_to_ships);
        virtual ~TournamentManager();

        void startArena(Ship& ship_1, Ship& ship_2);

    private:
        std::string exe_path;
        std::string ships_path;
};

#endif // TOURNAMENTMANAGER_H
