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

        void init();

		void singleTargetMutator();

		void poolMutator();

		void seededMutator();


    private:

        //Needed classes
        SettingsParser SP;
        ShipBuilder SB;
        TournamentManager TM;
        LogParser LP;

        //Path info
        std::string path_to_exe;
        std::string path_to_log;
        std::string path_to_ships;

		//Target info
		std::string target_name;
		std::string target_file_name;

		//Seed info
		std::string seed_file_name;

        //Settings
        int number_of_ships;
        int p_value_target;
        int thruster_value_target;
        int block_count_limit;
        int faction;
        int ship_symmetry;
		int mutation_blocks;

		//Ship data
		std::vector<Ship> population;
		std::vector<std::string> names;
		Ship winner;

		//Possible Factions
		int factions[3];

		//Random Faction bool
		bool is_random_faction;
};

#endif // MUTATOR_H
