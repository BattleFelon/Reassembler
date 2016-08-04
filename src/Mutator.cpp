#include <cmath>
#include <iostream>
#include <thread>

#include "Mutator.h"

//Null block creation
Mutator::Mutator()
{

}

Mutator::~Mutator()
{

}

void Mutator::init()
{
    //Load settings file
    SP.loadFromFile("config.txt");

	//Settings get
	SP.get("number_of_ships", number_of_ships);
	SP.get("mutation_blocks", mutation_blocks);
	SP.get("p_value_target", p_value_target);
	SP.get("thruster_value_target", thruster_value_target);
	SP.get("block_count_limit", block_count_limit);
	SP.get("faction", faction);
	SP.get("path_to_reassembly_exe", path_to_exe);
	SP.get("path_to_log_folder", path_to_log);
	SP.get("path_to_ships_folder", path_to_ships);
	SP.get("ship_symmetry", ship_symmetry);
	SP.get("target_name", target_name);
	SP.get("target_file_name", target_file_name);
	SP.get("seed_file_name", seed_file_name);

     //Output the settings
    std::cout << "\nP Value Target= " <<
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
	for (int i = 0; i < number_of_ships; ++i){
		std::string name = "Ship_";
		name.append(std::to_string(i));
		names.push_back(name);
	}

	//Set the posssible factions
	factions[0] = 2;
	factions[1] = 8;
	factions[2] = 12;

	if (faction == 0){
		is_random_faction = true;
		faction = factions[rand() % 3];
	}
	else{
		is_random_faction = false;
	}
}

void Mutator::singleTargetMutator(){

	//total wins by target design;
	int total_wins = 0;

	//Naming
	std::string ship_name = "rival_of_";
	ship_name.append(target_name);

	//Init ship 
	if (is_random_faction)
		faction = factions[rand() % 3];

	Ship rival = SB.createShip(p_value_target, faction, block_count_limit, thruster_value_target, ship_symmetry, "rival");

	std::cout << "Starting Battle \n";

	//main loop
	for (int wins = 0; wins < 3;){

		//Make sure it is a loosing ship design
		if (wins == 0){
			//Try new design
			if (is_random_faction)
				faction = factions[rand() % 3];

			rival = SB.createShip(p_value_target, faction, block_count_limit, thruster_value_target, ship_symmetry, "rival");
		}

		//Write the ships to file
		std::string file_name = "ships/";
		file_name.append(ship_name);
		file_name.append(".lua");
		rival.setName(ship_name);
		rival.writeShip(file_name, ship_name, ship_name);

		//Start battle
		TM.startString(ship_name, target_file_name);
		
		//Parse Log
		int result = 0;
		std::cout << ".";
		result = LP.getWinner(rival, target_name);

		if (result == 0){
			wins++;
			std::cout << ship_name << " Wins! " << wins << "\n" << std::endl;
		}
		else if (result == 1)
		{

			if(total_wins%100 == 0)
				std::cout << target_name << " Wins... again. " << total_wins << " total wins so far\n" << std::endl;

			wins = 0;
			total_wins++;
		}
		else if (result == 2){
			std::cout << "One of your path values is incorrect\n";
			std::cin >> wins;
			exit(-1);
		}

		//Keep it if it wins two to save on processing time
		if (wins == 2){
			std::string two_win = "ships/";
			two_win.append("two_win.lua");
			rival.writeShip(two_win, ship_name, ship_name);
		}

		//Keep it if it wins two to save on processing time
		if (wins == 1){
			std::string one_win = "ships/";
			one_win.append("one_win.lua");
			rival.writeShip(one_win, ship_name, ship_name);
		}
	}

	int a;
	std::cout << "\n\n Target has been beaten! \n\n";
	_beep(523, 500);
	_sleep(500);
	_beep(523, 500);
	_sleep(500);
	_beep(523, 500);
	_sleep(500);
	std::cin >> a;
}

void Mutator::poolMutator(){
	int generations;
	SP.get("num_generations", generations);

	//Seed initial population
	std::cout << "\nSeeding initial population \n\n";
	for (int i = 0; i < number_of_ships; ++i){
		//Seed the population

		if (is_random_faction)
			faction = factions[rand() % 3];

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
	for (int generation = 0; generation < generations; ++generation){

		std::vector<int> wins(population.size());

		for (int i = 0; i < population.size(); ++i){
			for (int j = i + 1; j < population.size(); ++j){
				std::cout << "Arena " << population[i].getShipName() << " and " << population[j].getShipName() << "...";
				std::cout.flush();

				//Wooooo ifdefs
				int result = 0;
#ifdef _WIN32
				TM.startArena(population[i], population[j]);
				//0 means first ship wins, 1 means second, and 2 means something went wrong
				result = LP.getWinner(population[i], population[j]);
#endif // _WIN32
#ifdef __linux__
				result = rand() % 2;
#endif // __linux__

				if (result == 1){
					std::cout << population[j].getShipName() << " Wins!" << std::endl;
					population[j].wins();
					wins[j]++;
				}
				else if (result == 0)
				{
					std::cout << population[i].getShipName() << " Wins!" << std::endl;
					population[i].wins();
					wins[i]++;
				}
				else if (result == 2){
					std::cout << "One of your path values is incorrect\n";
#ifdef _WIN32
					_sleep(1000);
#endif // _WIN32
					exit(-1);
				}
			}
		}//End of current generation

		//Find winners
		int number_of_wins = 0;
		for (int i = 0; i < wins.size(); ++i){
			if (wins[i] > number_of_wins){
				number_of_wins = wins[i];
				winner_index = i;
			}
		}

		//Settle Ties
		for (int i = 0; i < wins.size(); ++i){
			if (wins[i] == number_of_wins && i != winner_index){
				int result = 0;
#ifdef _WIN32
				TM.startArena(population[winner_index], population[i]);
				result = LP.getWinner(population[winner_index], population[i]);
#endif // _WIN32
#ifdef __linux__
				result = rand() % 2;
#endif // __linux__
				if (result == 0){
					//Winner index stays the same
					population[winner_index].wins();
					wins[winner_index]++;
				}
				else if (result == 1){
					//New winner, which will go settle more ties
					population[i].wins();
					winner_index = i;
					wins[winner_index]++;
				}
			}
		}

		int total = 0;
		for (auto win : wins)
			total += win;

		std::cout << "Total Wins " << total << "\n";

		//Select mate
		//Asexual by default
		int mate_index = winner_index;
		for (int attempts = 0; attempts < 1000; ++attempts)
		{
			int possible_index = rand() % population.size();
			if (population[possible_index].getFaction() == population[winner_index].getFaction()){
				mate_index = possible_index;
				attempts = 10000;
			}
		}

		std::cout << "\nGeneration " << generation << " over. \n\n" <<
			population[winner_index].getShipName() << " Has won with " << number_of_wins << " wins and " << population[winner_index].getLifetimeWins() << " life time wins.\n" <<
			"The winner has chosen a mate, Ship_" << mate_index << ".\nTheir children will be in the next generation\n\n";

		std::cout << "--Overall Results--\n";
		for (int i = 0; i < number_of_ships; ++i){
			if (i != winner_index)
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
		winner.writeShip(winner_file_name, num_wins, faction_name);

		//Seed next population
		population.clear();
		std::cout << "\nSeeding next population \n";
		for (int i = 0; i < number_of_ships; ++i){
			//Seed the population and keep winner, mate, and breed children
			if (i == winner_index){
				population.push_back(winner);
				population.back().setName(names[i]);
			}
			else if (i == mate_index){
				population.push_back(mate);
				population.back().setName(names[i]);
			}
			else if (i <= number_of_ships / 2){
				if (is_random_faction)
					faction = factions[rand() % 3];

				population.push_back(SB.createShip(p_value_target, faction, block_count_limit, thruster_value_target, ship_symmetry, names[i]));
			}
			else if (i >number_of_ships / 2){
				population.push_back(SB.breedShips(winner, mate, ship_symmetry, mutation_blocks, p_value_target));
				population.back().setName(names[i]);
			}


			std::string file_name = "ships/";
			file_name.append(names[i]);
			file_name.append(".lua");

			//Write the ships to file
			population[i].writeShip(file_name, names[i], names[i]);
		}

	}//End of all generations

	// return(winner);
}

void Mutator::seededMutator()
{
	int generations;
	SP.get("num_generations", generations);

	//Seed initial population
	std::cout << "\nSeeding initial population \n\n";
	for (int i = 0; i < number_of_ships; ++i){
		//Seed the population
		if (is_random_faction)
			faction = factions[rand() % 3];

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
	for (int generation = 0; generation < generations; ++generation){

		std::vector<int> wins(population.size());

		for (int i = 0; i < population.size(); ++i){
			for (int j = i + 1; j < population.size(); ++j){
				std::cout << "Arena " << population[i].getShipName() << " and " << population[j].getShipName() << "...";
				std::cout.flush();

				//Wooooo ifdefs
				int result = 0;
#ifdef _WIN32
				TM.startArena(population[i], population[j]);
				//0 means first ship wins, 1 means second, and 2 means something went wrong
				result = LP.getWinner(population[i], population[j]);
#endif // _WIN32
#ifdef __linux__
				result = rand() % 2;
#endif // __linux__

				if (result == 1){
					std::cout << population[j].getShipName() << " Wins!" << std::endl;
					population[j].wins();
					wins[j]++;
				}
				else if (result == 0)
				{
					std::cout << population[i].getShipName() << " Wins!" << std::endl;
					population[i].wins();
					wins[i]++;
				}
				else if (result == 2){
					std::cout << "One of your path values is incorrect\n";
#ifdef _WIN32
					_sleep(1000);
#endif // _WIN32
					exit(-1);
				}
			}
		}//End of current generation

		//Find winners
		int number_of_wins = 0;
		for (int i = 0; i < wins.size(); ++i){
			if (wins[i] > number_of_wins){
				number_of_wins = wins[i];
				winner_index = i;
			}
		}

		//Settle Ties
		for (int i = 0; i < wins.size(); ++i){
			if (wins[i] == number_of_wins && i != winner_index){
				int result = 0;
#ifdef _WIN32
				TM.startArena(population[winner_index], population[i]);
				result = LP.getWinner(population[winner_index], population[i]);
#endif // _WIN32
#ifdef __linux__
				result = rand() % 2;
#endif // __linux__
				if (result == 0){
					//Winner index stays the same
					population[winner_index].wins();
					wins[winner_index]++;
				}
				else if (result == 1){
					//New winner, which will go settle more ties
					population[i].wins();
					winner_index = i;
					wins[winner_index]++;
				}
			}
		}

		int total = 0;
		for (auto win : wins)
			total += win;

		std::cout << "Total Wins " << total << "\n";

		//Select mate
		int win_diff = 1000;
		int mate_index = 0;
		for (int i = 0; i < wins.size(); ++i){
			if (wins[winner_index] - wins[i] < win_diff && i != winner_index){
				win_diff = wins[winner_index] - wins[i];
				mate_index = i;
			}
		}

		std::cout << "\nGeneration " << generation << " over. \n\n" <<
			population[winner_index].getShipName() << " Has won with " << number_of_wins << " wins and " << population[winner_index].getLifetimeWins() << " life time wins.\n" <<
			"The winner has chosen a mate, Ship_" << mate_index << ".\nTheir children will be in the next generation\n\n";

		std::cout << "--Overall Results--\n";
		for (int i = 0; i < number_of_ships; ++i){
			if (i != winner_index)
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
		winner.writeShip(winner_file_name, num_wins, faction_name);

		//Seed next population
		population.clear();
		std::cout << "\nSeeding next population \n";
		for (int i = 0; i < number_of_ships; ++i){
			//Seed the population and keep winner, mate, and breed children
			if (i == winner_index){
				population.push_back(winner);
				population.back().setName(names[i]);
			}
			else if (i == mate_index){
				population.push_back(mate);
				population.back().setName(names[i]);
			}
			else if (i <= number_of_ships / 2){
				if (is_random_faction)
					faction = factions[rand() % 3];
				population.push_back(SB.createShip(p_value_target, faction, block_count_limit, thruster_value_target, ship_symmetry, names[i]));
			}
			else if (i >number_of_ships / 2){
				if (is_random_faction)
					faction = factions[rand() % 3];
				population.push_back(SB.breedShips(winner, mate, ship_symmetry, mutation_blocks, p_value_target));
				population.back().setName(names[i]);
			}


			std::string file_name = "ships/";
			file_name.append(names[i]);
			file_name.append(".lua");

			//Write the ships to file
			population[i].writeShip(file_name, names[i], names[i]);
		}

	}//End of all generations

	// return(winner);
}