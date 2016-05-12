#ifndef TOURNAMENTMANAGER_H
#define TOURNAMENTMANAGER_H

#include "Ship.h"

struct Result{
    int32_t winner;
    int32_t time_taken;
    int32_t score;
};

class TournamentManager
{
    public:
        TournamentManager();
        virtual ~TournamentManager();

        std::vector<Result> startTournament(std::vector<Ship>& ships);

    private:

        Result startFight(Ship& ship_1, Ship& ship_2);
};

#endif // TOURNAMENTMANAGER_H
