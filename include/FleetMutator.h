#ifndef FLEETMUTATOR_H
#define FLEETMUTATOR_H

#include "Fleet.h"
#include "SettingsParser.h"
#include "ShipBuilder.h"
#include "Ship.h"
#include "TournamentManager.h"
#include "LogParser.h"
#include "FleetBuilder.h"

class FleetMutator
{
    public:
        FleetMutator();
        virtual ~FleetMutator();

        void init();

        void bruteForceFleetBeater();

    private:

        //Needed classes
        SettingsParser SP;
        FleetBuilder FB;
        TournamentManager TM;
        LogParser LP;

        //Path info
        std::string path_to_exe;
        std::string path_to_log;
        std::string path_to_ships;

        //Target Fleet info
        std::string target_fleet_file;
        std::string target_fleet_name;

        //Settings
        int fleet_p_value;
        int fleet_single_ship_p_value_max;
        int fleet_single_ship_p_value_min;
        int block_count_limit;
        int faction;
        int ship_symmetry;
        int mutation_blocks;
};

#endif // FLEETMUTATOR_H
