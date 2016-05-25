#include "FleetMutator.h"
#include "FleetBuilder.h"
#include "Fleet.h"
#include "SettingsParser.h"

FleetMutator::FleetMutator()
{
    //ctor
}

FleetMutator::~FleetMutator()
{
    //dtor
}

void FleetMutator::init()
{
    //Load settings file
    SP.loadFromFile("config.txt");

    //Settings get
    SP.get("fleet_p_value",fleet_p_value);
    SP.get("fleet_single_ship_p_value_max",fleet_single_ship_p_value_max);
    SP.get("fleet_single_ship_p_value_min",fleet_single_ship_p_value_min);
    SP.get("target_fleet_file",target_fleet_file);
    SP.get("target_fleet_name",target_fleet_name);
    SP.get("block_count_limit",block_count_limit);
    SP.get("faction",faction);
    SP.get("path_to_reassembly_exe",path_to_exe);
    SP.get("path_to_log_folder",path_to_log);
    SP.get("path_to_ships_folder",path_to_ships);
    SP.get("ship_symmetry",ship_symmetry);
    SP.get("mutation_blocks",mutation_blocks);

    //Output Settings
    std::cout << "Target Fleet Value= " << fleet_p_value << "\n"<<
        "Single Ship Value Max= " << fleet_single_ship_p_value_max << "\n"<<
        "File to Beat= " << target_fleet_file << "\n"<<
        "Name to Beat= " << target_fleet_name << "\n"<<
        "Block Limit= " << block_count_limit << "\n"<<
        "Faction of Fleet= " << faction << "\n"<<
        "Symmetry= " << ship_symmetry << "\n"<<
        "Mutation Block Count= " << mutation_blocks << "\n\n" <<
        "Launching Mutator..\n" << std::endl;

    //Init the Tournament and log parsing classes
    TM.init(path_to_exe, path_to_ships);
    LP.init(path_to_log);

}

void FleetMutator::bruteForceFleetBeater()
{
    //Keep track of new_fleet loses
    int loses = 0;

    //Win target, temp
    int target_num_of_wins = 2;

    //Fleet name
    std::string fleet_name = "Rival_of_";
    fleet_name.append(target_fleet_name);

    //Needs to win twice
    for(int did_win = 0; did_win != target_num_of_wins;){
        //Building
        Fleet new_fleet = FB.createFleet(fleet_p_value,fleet_single_ship_p_value_max,fleet_single_ship_p_value_min,block_count_limit,faction,ship_symmetry);

        //Naming
        new_fleet.setName(fleet_name);
        std::string file_name = "ships/";
        file_name.append(new_fleet.getName());
        file_name.append(".lua");

        //Writing
        new_fleet.writeFleet(file_name,"Reassembler",new_fleet.getName());

        //Fighting
        TM.startString(new_fleet.getName(),target_fleet_file);
        int result = LP.getWinner(new_fleet.getName(),target_fleet_name);

        //Resolving
        if(result == 1){
            loses++;
            std::cout << target_fleet_name << " wins again... that makes " << loses << " in a row\n";
            did_win = 0;
        }
        else if(result == 0)
        {
            did_win++;
            std::cout << "\n\n The new Fleet wins! It needs " << target_num_of_wins - did_win << " more in a row to claim victor\n\n";
        }
        else if (result == 2){
            std::cout << "One of your path values is incorrect\n";
            #ifdef _WIN32
            _sleep(1000);
            #endif // _WIN32
            exit(-1);
            //return(-1);
        }
    }
    //Raughing
    std::cout <<"\n\n\n YOU HAVE BEEN BEATEN \n\n\n";
    std::cout << "Rival Fleet is in folder";
}
