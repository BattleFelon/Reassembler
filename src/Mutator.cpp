#include <cmath>
#include <iostream>
#include <unistd.h>
#include <thread>

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
        population.push_back(SB.createShip(p_value_target,faction,block_count_limit,thruster_value_target,ship_symmetry,names[i]));
        std::string file_name = "ships/";
        file_name.append(names[i]);
        file_name.append(".lua");
        //Write the ships to file
        population[i].writeShip(file_name,names[i],names[i]);
    }

}

Mutator::~Mutator()
{

}

void Mutator::poolMutator(int generations){
    //The index of the winning ship
    int winner_index = 0;

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
                    exit(-1);
                    //return(-1);
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
        mtx.lock();
        winner = population[winner_index];
        winner.setName("Winner");

        std::string winner_file_name = "ships/Winner_";
        std::string faction_name = "Winner";
        faction_name.append(std::to_string(winner_index));
        winner_file_name.append(winner.getShipName());
        winner_file_name.append(".lua");

        std::string num_wins = "NumWins";
        num_wins.append(std::to_string(winner.getLifetimeWins()));
        winner.writeShip(winner_file_name,num_wins,faction_name);
        mtx.unlock();
        //Seed next population
        population.clear();
        std::cout << "\nSeeding next population \n";
        for(int i = 0; i < number_of_ships; ++i){
            //Seed the population and keep winner
            if(i != winner_index)
            population.push_back(SB.createShip(p_value_target,faction,block_count_limit,thruster_value_target,ship_symmetry,names[i]));
            else{
            mtx.lock();
            population.push_back(winner);
            population.back().setName(names[i]);
            mtx.unlock();
            }

            std::string file_name = "ships/";
            file_name.append(names[i]);
            file_name.append(".lua");

            //Write the ships to file
            population[i].writeShip(file_name,names[i],names[i]);
        }

    }//End of all generations

   // return(winner);
}

void Mutator::bracketMutator(int generations)
{
    //The index of the winning ship
    int winner_index = 0;

    //Do this style of fighting until the target generations is reached
    for(int generation = 0; generation < generations; ++ generation){

        //win tracking and odd man out logic
        std::vector<int> wins;
        std::vector<bool> got_to_fight;
        std::vector<bool> did_lose;
        for(int i = 0; i < population.size(); ++i){
            got_to_fight.push_back(false);
            did_lose.push_back(false);
            wins.push_back(0);
        }

        //Check to see if battle happened. If not it is the end of the generation
        bool was_a_battle = true;

        for(int round = 0; was_a_battle ; ++round){
            //Reset end of bracket logic
            was_a_battle = false;

            std::cout << "\n----Round" << round << " started----\n\n";

            //Set whether or not they fought to zero
            for(int k = 0; k < got_to_fight.size(); ++k)
                got_to_fight[k] = false;

            for(int i = 0; i < population.size(); ++i){
                for(int j = i+1; j < population.size(); ++j){
                    if(wins[i] >= round && wins[i] == wins[j] && !did_lose[i] && !did_lose[j]){
                        //Fighter got to fight
                        got_to_fight[i] = true;
                        got_to_fight[j] = true;
                        //Arena happened
                        was_a_battle = true;

                        //Match begin
                        std::cout << "Arena " << population[i].getShipName() << " and " << population[j].getShipName() << "...";
                        std::cout.flush();

                        //TM.startArena(population[i],population[j]);
                        //0 means first ship wins, 1 means second, and 2 means something went wrong
                        int result = rand() % 2;//LP.getWinner(population[i], population[j]);
                        if(result == 1){
                            std::cout << population[j].getShipName() <<  " Wins!" << std::endl;
                            population[j].wins();
                            wins[j]++;
                            did_lose[i] = true;
                        }
                        else if(result == 0)
                        {
                            std::cout << population[i].getShipName() <<  " Wins!" << std::endl;
                            population[i].wins();
                            wins[i]++;
                            did_lose[j] = true;
                        }
                        else if (result == 2){
                            std::cout << "One of your path values is incorrect\n";
                            exit(-1);
                            //return(-1);
                        }

                        //Exit inner loop
                        j = population.size();
                        i++;
                    }
                }
            }//End of round

            //Check to see for odd man out, make him fight a random loser for a comeback!
            for(int k = 0; k < got_to_fight.size(); ++k){
                if(!got_to_fight[k] && wins[k] >= round && was_a_battle){
                    for(int w = 0; w < population.size();++w){
                        if(wins[k] == wins[w] && w != k){
                            std::cout<< "Ship_" << k << " has to fight loser Ship_" << w << " for a spot in the next round...";
                            //TM.startArena(population[i],population[j]);
                            //0 means first ship wins, 1 means second, and 2 means something went wrong
                            int result = rand() % 2;//LP.getWinner(population[k], population[w]);

                            if(result == 1){
                                std::cout<< " Ship_" << w  << " Wins \n";
                                wins[w]++;
                                w=population.size();
                                k=w;
                            }
                            else if(result == 0){
                                std::cout<< " Ship_" << k  << " Wins \n";
                                 wins[k]++;
                                 w=population.size();
                                 k=w;
                            }
                        }
                    }
                }
            }
        }

        //Find winners
        int number_of_wins = 0;
        for(int i = 0; i < wins.size(); ++i){
            if(wins[i] > number_of_wins){
                number_of_wins = wins[i];
                winner_index = i;
            }
        }

        std::cout << "\nGeneration " << generation << " over. \n\n" <<
        population[winner_index].getShipName() <<" Has won with " << number_of_wins << " and has " << population[winner_index].getLifetimeWins() << " life time wins." <<
        "\n\n    ******** It shall live on!!!  ********  \n\n";

        //Save the winner
        mtx.lock();
        winner = population[winner_index];
        winner.setName("Winner");
        std::string winner_file_name = "ships/Winner_";
        std::string faction_name = "Winner";
        faction_name.append(std::to_string(winner_index));
        winner_file_name.append(winner.getShipName());
        winner_file_name.append(".lua");

        std::string num_wins = "NumWins";
        num_wins.append(std::to_string(winner.getLifetimeWins()));
        winner.writeShip(winner_file_name,num_wins,faction_name);
        mtx.unlock();
        //Seed next population
        population.clear();
        std::cout << "\nSeeding next population \n";
        for(int i = 0; i < number_of_ships; ++i){
            //Seed the population and keep winner
            if(i != winner_index)
            population.push_back(SB.createShip(p_value_target,faction,block_count_limit,thruster_value_target,ship_symmetry,names[i]));
            else{
            mtx.lock();
            population.push_back(winner);
            mtx.unlock();
            }

            std::string file_name = "ships/";
            file_name.append(names[i]);
            file_name.append(".lua");

            //Write the ships to file
            population[i].writeShip(file_name,names[i],names[i]);
        }
    }//End of all generations
}
