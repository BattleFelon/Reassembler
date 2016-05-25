#include <iostream>
#include <sstream>
#include <mutex>

#include "SettingsParser.h"
#include "ShipBuilder.h"
#include "Ship.h"
#include "TournamentManager.h"
#include "LogParser.h"
#include "Fleet.h"
#include "Mutator.h"
#include "FleetBuilder.h"
#include "FleetMutator.h"

int main()
{
   // FleetBuilder FB;
   // Fleet f1 = FB.createFleet(16000,1000,100,1000,8,1);
    //f1.writeFleet("New_fleet.lua","ME","name");

    SettingsParser SP;
    SP.loadFromFile("config.txt");
    int debug_mode;
    int num_generations;
    int mutator_type;
	std::string target_name;
	std::string target_file_name;

    SP.get("debug_mode",debug_mode);
    SP.get("num_generations",num_generations);
    SP.get("mutator_type",mutator_type);
	SP.get("target_name", target_name);
	SP.get("target_file_name", target_file_name);

    if(debug_mode){
        //Display disp;
       // disp.run();
    }
    else{

        Mutator M;
        FleetMutator FM;

        switch(mutator_type){

        case 1:
            M.init();
            M.poolMutator(num_generations);
            break;

        case 2:
            M.init();
            M.bracketMutator(num_generations);
            break;
		case 3:
		    FM.init();
		    FM.bruteForceFleetBeater();
			break;
        default:
            std::cout << "Check settings file \n";

        }

    }

    return EXIT_SUCCESS;
}
