// License: GNU Public License
// Author: Robert Viren
// Date:   2016
// ----------------------------

//Main genetic class. Calls all other major classes. Generates ships, breeds, battles, parses results, loads all data, and provides display data for the display class.

#ifndef MUTATOR_H
#define MUTATOR_H

#include <random>

#include "SettingsParser.h"
#include "ShipBuilder.h"
#include "Ship.h"
#include "TournamentManager.h"
#include "LogParser.h"

class Mutator
{
    public:

        Mutator();

        virtual ~Mutator();

        Ship poolMutator(int generations);

    private:

        SettingsParser SP;
        ShipBuilder SB;
        TournamentManager TM;
        LogParser LP;

        std::string path_to_exe;
        std::string path_to_log;
        std::string path_to_ships;

        int number_of_ships;
        int p_value_target;
        int thruster_value_target;
        int block_count_limit;
        int faction;
        int ship_symmetry;

        std::vector<Ship> population;
        std::vector<std::string> names;

};

#endif // MUTATOR_H
