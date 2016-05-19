#include <cmath>
#include <iostream>
#include <unistd.h>
#include "Mutator.h"

//Null block creation
Mutator::Mutator()
{
    //Load settings file
    SP.loadFromFile("config.txt");

    //Settings get
    SP.get("number_of_ships",number_of_ships);
    SP.get("p_value_target",p_value_target);
    SP.get("thruster_value_target",thruster_value_target);
    SP.get("block_count_limit",block_count_limit);
    SP.get("faction",faction);
    SP.get("path_to_reassembly_exe",path_to_exe);
    SP.get("path_to_log_folder",path_to_log);
    SP.get("path_to_ships_folder",path_to_ships);
    SP.get("ship_symmetry",ship_symmetry);

     //Output the settings
    std::cout <<"Number of ships to create= " <<
        number_of_ships << "\nP Value Target= " <<
        p_value_target << "\nThruster Value= " <<
        thruster_value_target << "\nFaction= " <<
        faction <<
        "\nBlock Limit= " <<
        block_count_limit <<
        "\n\n";

    //Init the Tournament and log parsing classes
    TM.init(path_to_exe, path_to_ships);
    LP.init(path_to_log);

    //Set the names
    for(int i = 0; i < number_of_ships; ++i){
        std::string name = "Ship_";
        name.append(std::to_string(i));
        names.push_back(name);
    }

    //Seed initial population
    std::cout << "\nSeeding initial population \n\n";
    for(int i = 0; i < number_of_ships; ++i){
        //Seed the population
        population.push_back(SB.createShip(p_value_target,faction,block_count_limit,thruster_value_target,ship_symmetry));
        std::string file_name = "ships/";
        file_name.append(names[i]);
        file_name.append(".lua");
        //Write the ships to file
        population[i].writeShip(file_name,names[i],names[i],names[i]);
    }

}

Mutator::~Mutator()
{

}

Ship Mutator::poolMutator(int generations){

    //The index of the winning ship
    int winner_index = 0;

    //Null ship
    Ship winning_ship = SB.createShip(12,8,100,0,0);

    //Do this style of fighting until the target generations is reached
    for(int generation = 0; generation < generations; ++ generation){

        std::vector<int> wins(population.size());

        for(int i = 0; i < population.size(); ++i){
            for(int j = i + 1; j < population.size();++j){
                std::cout << "Arena " << population[i].getShipName() << " and " << population[j].getShipName() << "...";
                std::cout.flush();
                //TM.startArena(population[i],population[j]);
                //0 means first ship wins, 1 means second, and 2 means something went wrong
                int result = rand() % 2;//LP.getWinner(population[i], population[j]);
                if(result == 1){
                    std::cout << population[j].getShipName() <<  " Wins!" << std::endl;
                    population[j].wins();
                    wins[j]++;
                }
                else if(result == 0)
                {
                    std::cout << population[i].getShipName() <<  " Wins!" << std::endl;
                    population[i].wins();
                    wins[i]++;
                }
                else if (result == 2){
                    std::cout << "One of your path values is incorrect\n";
                    return(-1);
                }
            }
        }//End of current generation

        //Find winners
        int number_of_wins = 0;
        for(int i = 0; i < wins.size(); ++i){
            if(wins[i] > number_of_wins){
                number_of_wins = wins[i];
                winner_index = i;
            }
        }

        //Settle Ties
        for(int i = 0; i < wins.size(); ++i){
            if(wins[i] == number_of_wins && i != winner_index){
                //TM.startArena(population[winner_index],population[i]);
                int result = rand() % 2;//LP.getWinner(population[winner_index],population[i]);
                if(result == 0){
                    //Winner index stays the same
                    population[winner_index].wins();
                }
                else if(result == 1){
                    //New winner, which will go settle more ties
                    population[i].wins();
                    winner_index = i;
                }
            }
        }

        std::cout << "\nGeneration " << generation << " over. \n\n" <<
        population[winner_index].getShipName() <<" Has won with " << number_of_wins << " wins and " << population[winner_index].getLifetimeWins() << " life time wins." <<
        "\n\n    ******** It shall live on!!!  ********  \n\n";

        std::cout << "--Overall Results--\n";
        for(int i = 0; i < number_of_ships; ++i){
                if(i != winner_index)
                    std::cout << population[i].getShipName() << " had " << wins[i] << " wins \n";
                else
                    std::cout << population[i].getShipName() << " had " << wins[i] << " wins **** \n";
        }

        //Save the winner
        winning_ship = population[winner_index];
        std::string winner = "ships/Winner_";
        std::string faction_name = "Winner";
        faction_name.append(std::to_string(winner_index));
        winner.append(winning_ship.getShipName());
        winner.append(".lua");

        std::string num_wins = "NumWins";
        num_wins.append(std::to_string(winning_ship.getLifetimeWins()));
        winning_ship.writeShip(winner,"Winner",num_wins,faction_name);

        //Seed next population
        population.clear();
        std::cout << "\nSeeding next population \n";
        for(int i = 0; i < number_of_ships; ++i){
            //Seed the population and keep winner
            if(i != winner_index)
            population.push_back(SB.createShip(p_value_target,faction,block_count_limit,thruster_value_target,ship_symmetry));
            else
            population.push_back(winning_ship);

            std::string file_name = "ships/";
            file_name.append(names[i]);
            file_name.append(".lua");

            //Write the ships to file
            population[i].writeShip(file_name,names[i],names[i],names[i]);
        }

    }//End of all generations

    return(winning_ship);
}
