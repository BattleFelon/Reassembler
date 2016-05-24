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

int main()
{

    //ShipBuilder SB;
    //Fleet f1;
    //f1.readFleet("blarg.lua");


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
        //Mutate class init
        Mutator mutate;

        switch(mutator_type){
        case 1:
            mutate.threadedPoolMutator(num_generations);
            break;

        case 2:
            mutate.bracketMutator(num_generations);
            break;
		case 3:
			mutate.singleTargetMutator(target_name,target_file_name);
			break;
        default:
            std::cout << "Check settings file \n";

        }

    }


    return EXIT_SUCCESS;
}
