#include <cmath>
#include <iostream>
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
    SP.get("mutation_blocks",mutation_blocks);
    SP.get("core_count",core_count);

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
}

Mutator::~Mutator()
{

}

void Mutator::poolMutator(int generations){

	//Seed initial population
	std::cout << "\nSeeding initial population \n\n";
	for (int i = 0; i < number_of_ships; ++i){
		//Seed the population
		population.push_back(SB.createShip(p_value_target, faction, block_count_limit, thruster_value_target, ship_symmetry, names[i]));
		std::string file_name = "ships/";
		file_name.append(names[i]);
		file_name.append(".lua");
		//Write the ships to file
		population[i].writeShip(file_name, names[i], names[i]);
	}

    //The index of the winning ship
    int winner_index = 0;

    //Do this style of fighting until the target generations is reached
    for(int generation = 0; generation < generations; ++ generation){

        std::vector<int> wins(population.size());

        for(int i = 0; i < population.size(); ++i){
            for(int j = i + 1; j < population.size();++j){
                std::cout << "Arena " << population[i].getShipName() << " and " << population[j].getShipName() << "...";
                std::cout.flush();

                //Wooooo ifdefs
                int result = 0;
                #ifdef WIN_32
                TM.startArena(population[i],population[j]);
                //0 means first ship wins, 1 means second, and 2 means something went wrong
                result = LP.getWinner(population[i], population[j]);
                #endif // WIN_32
                #ifdef __linux__
                result = rand() % 2;
                #endif // __linux__

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
                    #ifdef WIN_32
					_sleep(1000);
					#endif // WIN_32
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
                int result = 0;
                #ifdef WIN_32
                TM.startArena(population[winner_index],population[i]);
                result = LP.getWinner(population[winner_index],population[i]);
                #endif // WIN_32
                #ifdef __linux__
                result = rand() % 2;
                #endif // __linux__
                if(result == 0){
                    //Winner index stays the same
                    population[winner_index].wins();
					wins[winner_index]++;
                }
                else if(result == 1){
                    //New winner, which will go settle more ties
                    population[i].wins();
                    winner_index = i;
					wins[winner_index]++;
                }
            }
        }

        //Select mate
        int win_diff = 1000;
        int mate_index = 0;
        for(int i = 0; i < wins.size(); ++i){
            if(wins[winner_index] - wins[i] < win_diff && i != winner_index){
                win_diff = wins[winner_index] - wins[i];
                mate_index = i;
            }
        }

        std::cout << "\nGeneration " << generation << " over. \n\n" <<
        population[winner_index].getShipName() <<" Has won with " << number_of_wins << " wins and " << population[winner_index].getLifetimeWins() << " life time wins.\n" <<
        "The winner has chosen a mate, Ship_" << mate_index << ".\nTheir children will be in the next generation\n\n";

        std::cout << "--Overall Results--\n";
        for(int i = 0; i < number_of_ships; ++i){
                if(i != winner_index)
                    std::cout << population[i].getShipName() << " had " << wins[i] << " wins \n";
                else
                    std::cout << population[i].getShipName() << " had " << wins[i] << " wins **** \n";
        }

        //Save the winner and mate

        winner = population[winner_index];
        Ship mate = population[mate_index];

        std::string winner_file_name = "ships/Winner_";
        std::string faction_name = "Winner";
        faction_name.append(std::to_string(winner_index));
        winner_file_name.append(winner.getShipName());
        winner_file_name.append(".lua");

        std::string num_wins = "NumWins";
        num_wins.append(std::to_string(winner.getLifetimeWins()));
        winner.writeShip(winner_file_name,num_wins,faction_name);

        //Seed next population
        population.clear();
        std::cout << "\nSeeding next population \n";
        for(int i = 0; i < number_of_ships; ++i){
            //Seed the population and keep winner, mate, and breed children
            if(i == winner_index){
            population.push_back(winner);
            population.back().setName(names[i]);
            }
            else if(i == mate_index){
                population.push_back(mate);
                population.back().setName(names[i]);
            }
            else if(i <= number_of_ships/2)
                population.push_back(SB.createShip(p_value_target,faction,block_count_limit,thruster_value_target,ship_symmetry,names[i]));
            else if(i >number_of_ships/2){
                population.push_back(SB.breedShips(winner,mate,ship_symmetry,mutation_blocks,p_value_target));
                population.back().setName(names[i]);
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

void Mutator::threadedPoolMutator(int generations)
{
    //Set index values
    int winner_index = -1;
    int mate_index = -1;

    population.resize(number_of_ships);
    for(int generation = 0; generation < generations; ++ generation){
        //Spawn ships initial if no winner
        if(winner_index == -1){
            for(int i = 0; i < number_of_ships; ++i){
                population[i] = SB.createShip(p_value_target, faction, block_count_limit, thruster_value_target, ship_symmetry, names[i]);
                std::string file_name = "ships/";
                file_name.append(names[i]);
                file_name.append(".lua");
                //Write the ships to file
                population[i].writeShip(file_name, names[i], names[i]);
            }
        }
        else{
            for(int i = 0; i < number_of_ships; ++i){
                //Breeding loop because we have winner and mate
                if(i == winner_index){
                    //Save winner
                }
                else if(i == mate_index){
                    //Save mate
                }
                else if(i <= number_of_ships/2)
                    population[i] = SB.createShip(p_value_target,faction,block_count_limit,thruster_value_target,ship_symmetry,names[i]);
                else if(i >number_of_ships/2){
                    population[i] = SB.breedShips(population[winner_index],population[mate_index],ship_symmetry,mutation_blocks,p_value_target);
                    population[i].setName(names[i]);
                }
                std::string file_name = "ships/";
                file_name.append(names[i]);
                file_name.append(".lua");
                //Write the ships to file
                population[i].writeShip(file_name,names[i],names[i]);
            }
        }
        //Start battles
        std::vector<std::thread> battles(core_count);
        int thread_index = 0;
        for(int i = 0; i < number_of_ships; ++i){
            for(int j = i + 1; j < number_of_ships; ++j){
                if(thread_index < core_count){
                    //Launch battle
                    battles[thread_index] = std::thread(&TournamentManager::startString, TM, names[i], names[j]);
                    thread_index++;
                }
                else{
                    j--;
                    //Wait for threads to finish
                    for(auto& t : battles)
                        t.join();
                    thread_index = 0;
                }
            }
        }
        //Catch lingering threads
        for(auto& t : battles)
            if(t.joinable())
                t.join();

        //Parse Result files
        std::vector<int> wins = LP.getWinner(names);

        //Add wins to lifetime
        for(int i = 0; i < wins.size();++i){
            population[i].addWins(wins[i]);
        }

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
                int result = 0;
                #ifdef WIN_32
                TM.startArena(population[winner_index],population[i]);
                result = LP.getWinner(population[winner_index],population[i]);
                #endif // WIN_32
                #ifdef __linux__
                result = rand() % 2;
                #endif // __linux__
                if(result == 0){
                    //Winner index stays the same
                    population[winner_index].wins();
					wins[winner_index]++;
                }
                else if(result == 1){
                    //New winner, which will go settle more ties
                    population[i].wins();
                    winner_index = i;
					wins[winner_index]++;
                }
            }
        }

        //Select mate
        int win_diff = 1000;
        mate_index = 0;
        for(int i = 0; i < wins.size(); ++i){
            if(wins[winner_index] - wins[i] < win_diff && i != winner_index){
                win_diff = wins[winner_index] - wins[i];
                mate_index = i;
            }
        }

        std::cout << "\nGeneration " << generation << " over. \n\n" <<
        population[winner_index].getShipName() <<" Has won with " << number_of_wins << " wins and " << population[winner_index].getLifetimeWins() << " life time wins.\n" <<
        "The winner has chosen a mate, Ship_" << mate_index << ".\nTheir children will be in the next generation\n\n";

        std::cout << "--Overall Results--\n";
        for(int i = 0; i < number_of_ships; ++i){
                if(i != winner_index)
                    std::cout << population[i].getShipName() << " had " << wins[i] << " wins \n";
                else
                    std::cout << population[i].getShipName() << " had " << wins[i] << " wins **** \n";
        }

        //Save the winner and mate
        winner = population[winner_index];
        Ship mate = population[mate_index];

        std::string winner_file_name = "ships/Winner_";
        std::string faction_name = "Winner";
        faction_name.append(std::to_string(winner_index));
        winner_file_name.append(winner.getShipName());
        winner_file_name.append(".lua");

        std::string num_wins = "NumWins";
        num_wins.append(std::to_string(winner.getLifetimeWins()));
        winner.writeShip(winner_file_name,num_wins,faction_name);
    }
}

void Mutator::bracketMutator(int generations)
{

	//Seed initial population
	std::cout << "\nSeeding initial population \n\n";
	for (int i = 0; i < number_of_ships; ++i){
		//Seed the population
		population.push_back(SB.createShip(p_value_target, faction, block_count_limit, thruster_value_target, ship_symmetry, names[i]));
		std::string file_name = "ships/";
		file_name.append(names[i]);
		file_name.append(".lua");
		//Write the ships to file
		population[i].writeShip(file_name, names[i], names[i]);
	}

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
                        int result = 0;
                        #ifdef WIN_32
                        TM.startArena(population[i],population[j]);
                        //0 means first ship wins, 1 means second, and 2 means something went wrong
                        result = LP.getWinner(population[i], population[j]);
                        #endif // WIN_32
                        #ifdef __linux__
                        result =  rand() % 2;
                        #endif // __linux__
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
                            int result = 0;
                            #ifdef WIN_32
                            TM.startArena(population[k],population[w]);
                            //0 means first ship wins, 1 means second, and 2 means something went wrong
                            result = LP.getWinner(population[k], population[w]);
                            #endif // WIN_32
                            #ifdef __linux__
                            result = rand() % 2;
                            #endif // __linux__

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
							else if (result == 2){
								std::cout << "One of your path values is incorrect\n";
								#ifdef WIN_32
                                _sleep(1000);
                                #endif // WIN_32
								exit(-1);
								//return(-1);
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

        //Select mate
        int win_diff = 1000;
        int mate_index = 0;
        for(int i = 0; i < wins.size(); ++i){
            if(wins[winner_index] - wins[i] < win_diff && i != winner_index){
                win_diff = wins[winner_index] - wins[i];
                mate_index = i;
            }
        }

        std::cout << "\nGeneration " << generation << " over. \n\n" <<
        population[winner_index].getShipName() <<" Has won with " << number_of_wins << " wins and " << population[winner_index].getLifetimeWins() << " life time wins.\n" <<
        "The winner has chosen a mate, Ship_" << mate_index << ".\nTheir children will be in the next generation\n\n";

        //Save the winner
        winner = population[winner_index];
        Ship mate = population[mate_index];
        winner.setName("Winner");
        std::string winner_file_name = "ships/Winner_";
        std::string faction_name = "Winner";
        faction_name.append(std::to_string(winner_index));
        winner_file_name.append(winner.getShipName());
        winner_file_name.append(".lua");

        std::string num_wins = "NumWins";
        num_wins.append(std::to_string(winner.getLifetimeWins()));
        winner.writeShip(winner_file_name,num_wins,faction_name);
        //Seed next population
        population.clear();
        std::cout << "\nSeeding next population \n";
        for(int i = 0; i < number_of_ships; ++i){
            //Make children, keep mate and winner, seed new population
            if(i == winner_index){
            population.push_back(winner);
            population.back().setName(names[i]);
            }
            else if(i == mate_index){
                population.push_back(mate);
                population.back().setName(names[i]);
            }
            else if(i <= number_of_ships/2)
                population.push_back(SB.createShip(p_value_target,faction,block_count_limit,thruster_value_target,ship_symmetry,names[i]));
            else if(i >number_of_ships/2){
                population.push_back(SB.breedShips(winner,mate,ship_symmetry,mutation_blocks,p_value_target));
                population.back().setName(names[i]);
            }

            std::string file_name = "ships/";
            file_name.append(names[i]);
            file_name.append(".lua");

            //Write the ships to file
            population[i].writeShip(file_name,names[i],names[i]);
        }
    }//End of all generations
}

void Mutator::singleTargetMutator(std::string target_name, std::string target_file_name){

	//Core number for thread creation
	int core_count = 8;

	//Make vector
    population.resize(core_count);

    //win tracking
    std::vector<int> wins(core_count,0);
    int winner_index = 0;

	for (bool did_win = false; !did_win;){
		//thread vector
		std::vector<std::thread> battles(core_count);
		//name vector
		std::vector<std::string> search_names(core_count);
		//Population spawn
		for (int i = 0; i < core_count; ++i){
            if(wins[i] == 0)
                population[i] = (SB.createShip(p_value_target, faction, block_count_limit, thruster_value_target, ship_symmetry, names[i]));
            else if(wins[i] >= 2){
                    did_win = true;
                    winner_index = i;
            }

            //Write the ships to file
            std::string file_name = "ships/";
            file_name.append(names[i]);
            file_name.append(".lua");
            population[i].setName(names[i]);
            population[i].writeShip(file_name,names[i],names[i]);

			battles[i] = std::thread(&TournamentManager::startString, TM, names[i], target_name);
			search_names[i] =names[i];
		}


        //wait for the battles to finish
		for (auto& t : battles){
            t.join();
		}

        //Get results from files and delete logs
        std::vector<int> results(core_count,0);
        #ifdef WIN_32
		std::vector<int> results = LP.getWinner(search_names);
        #endif // WIN_32
        #ifdef __linux__
        for(int i = 0; i < core_count;++i){
            rand() % 100 > 90 ? results[i] = 1 : results[i] = 0;
            if(results[i] == 1)
                std::cout << "WINNNN\n";
        }
        #endif // __linux__
		for (int i = 0; i < results.size(); ++i){
                        //See result, add to wins if won, kill loser if lost
            			if(results[i])
                            wins[i]++;
                        else
                            wins[i]=0;
		}
	}
	//Write the ships to file
	std::string new_file_name = "ships/Winner_";
	new_file_name.append(names[winner_index]);

    population[winner_index].writeShip(new_file_name,names[winner_index],names[winner_index]);
}
