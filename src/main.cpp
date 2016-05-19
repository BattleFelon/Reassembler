#include <iostream>
#include <sstream>
#include <unistd.h>
#include <mutex>

#include "SettingsParser.h"
#include "Mutator.h"

int main()
{
    SettingsParser SP;
    SP.loadFromFile("config.txt");
    int debug_mode;
    int num_generations;
    int mutator_type;

    SP.get("debug_mode",debug_mode);
    SP.get("num_generations",num_generations);
    SP.get("mutator_type",mutator_type);

    if(debug_mode){

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
