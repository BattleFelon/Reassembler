#include <iostream>
#include <sstream>
#include <unistd.h>
#include <mutex>

#include "Display.h"
#include "SettingsParser.h"
#include "ShipBuilder.h"
#include "Ship.h"
#include "TournamentManager.h"
#include "LogParser.h"
#include "Fleet.h"

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

    SP.get("debug_mode",debug_mode);
    SP.get("num_generations",num_generations);
    SP.get("mutator_type",mutator_type);

    if(debug_mode){
        Display disp;
        disp.run();
    }
    else{
        //Mutate class init
        Mutator mutate;

        switch(mutator_type){
        case 1:
            mutate.poolMutator(num_generations);
            break;

        case 2:
            mutate.bracketMutator(num_generations);
            break;

        default:
            std::cout << "Check settings file \n";

        }

    }


    return EXIT_SUCCESS;
}
