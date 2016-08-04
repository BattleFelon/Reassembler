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
	SP.get("fleet_population_size", fleet_population_size);
	SP.get("target_number_of_wins ", target_number_of_wins);

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
    int target_num_of_wins = 3;

    //Fleet name
    std::string fleet_name = "Rival_of_";
    fleet_name.append(target_fleet_name);

	//Spawn fleet 
	Fleet new_fleet;

    //Needs to win twice
    for(int did_win = 0; did_win != target_num_of_wins;){

		if (did_win == 0){
			//Building
			new_fleet = FB.createFleet(fleet_p_value, fleet_single_ship_p_value_max, fleet_single_ship_p_value_min, block_count_limit, faction, ship_symmetry);

			//Naming
			new_fleet.setName(fleet_name);
			std::string file_name = "ships/";
			file_name.append(new_fleet.getName());
			file_name.append(".lua");

			//Writing
			new_fleet.writeFleet(file_name, "Reassembler", fleet_name);
		}

        //Fighting
		//std::cout << "Starting Battle\n";
		//Timer
		clock_t timer = clock();
        TM.startString(new_fleet.getName(),target_fleet_file);
        int result = LP.getWinner(fleet_name,target_fleet_name);

		//End timer
		std::cout << "Time Duration Of Battle " << clock() - timer << "\n";
		int diff = clock() - timer;

        //Resolving
        if(result == 1){
            loses++;
            //std::cout << target_fleet_name << " wins again... that makes " << loses << " in a row\n";
            did_win = 0;
        }
        else if(result == 0)
        {
            did_win++;
            std::cout << "The new Fleet wins! It needs " << target_num_of_wins - did_win << " more in a row to claim victor\n\n";
        }
        else if (result == 2){
            std::cout << "One of your path values is incorrect\n";
			int a;
			std::cin >> a;
            exit(-1);
        }

		

		
		if (did_win == 1)
			new_fleet.writeFleet("ships/one_win_fleet.lua", "Reassembler", "one_win_fleet");

		if (did_win == 2)
			new_fleet.writeFleet("ships/two_win_fleet.lua", "Reassembler", "two_win_fleet");

    }
    //Raughing
    std::cout <<"\n\n\n YOU HAVE BEEN BEATEN \n\n\n";
    std::cout << "Rival Fleet is in folder";
	int a;
	std::cin >> a;

}

void FleetMutator::geneticFleetBeater()
{
	//Vectors for fleet storage
	std::vector<Fleet> population(fleet_population_size);
	std::vector<Fleet> top_performers(fleet_population_size);
	
	//Large lose tracker
	int loses = 0;

    //Fleet name
    std::string fleet_name = "Rival_of_";
    fleet_name.append(target_fleet_name);

	//Go until win condition met
	for (;;){

		//Building
		for (int i = 0; i < fleet_population_size; ++i){
			population[i] = FB.createFleet(fleet_p_value, fleet_single_ship_p_value_max, fleet_single_ship_p_value_min, block_count_limit, faction, ship_symmetry);

			//Naming
			std::string file_name = "ships/";
			file_name.append(fleet_name);
			file_name.append(std::to_string(i));
			std::string new_name = fleet_name;
			new_name.append(std::to_string(i));
			file_name.append(".lua");
			population[i].setName(new_name);

			//Writing
			population[i].writeFleet(file_name, "Reassembler", new_name);
		}

		//Fighting
		for (int i = 0; i < fleet_population_size; ++i){
			//Timer for score calc
			clock_t timer = clock();
			TM.startString(population[i].getName(), target_fleet_file);
			int result = LP.getWinner(population[i].getName(), target_fleet_name);

			//End timer
			std::cout << "Time Duration Of Battle " << clock() - timer << "\n";
			int diff = clock() - timer;

			//Put in score
			population[i].setScore(diff);

			//Resolving
			if (result == 1){
				loses++;
				if (loses % 100 == 0)
					std::cout << target_fleet_name << " wins again... that makes " << loses << " in a row\n";
				std::cout << ".";
			}
			else if (result == 0)
			{
				population[i].addWin();
				std::cout << "The new Fleet wins! \n";
			}
			else if (result == 2){
				std::cout << "One of your path values is incorrect.\nYour config file target name or file might not match the log file.\nCheck the Reassembly log file.\n";
				int a;
				std::cin >> a;
				exit(-1);
			}
		}//End fighting

		//Process results
		if (top_performers[0].getScore() == 0){ // Check for first loop
			std::vector<int> results(fleet_population_size);
			for (int i = 0; i < fleet_population_size; ++i){
				for (int j = 0; j < fleet_population_size; ++j){
					if (i != j){
						if (population[i].getWins() > 0){ // If did win at least once
							if (population[i].getWins() > population[j].getWins()){ //Winner Increment
								results[i]++;
							}
							else if (population[i].getWins() == population[j].getWins()){
								if (population[i].getScore() < population[j].getScore()){ //Winner Increment
									results[i]++;
								}
								else if (population[i].getScore() == population[j].getScore()){
									population[i].setScore(population[i].getScore() - 1);
									results[i]++;
								}
							}
						}
						else{ // If no wins
							if (population[i].getScore() > population[j].getScore() && population[j].getWins() == 0){ // Winner Increment
								results[i]++;
							}
							else if (population[i].getScore() == population[j].getScore() && population[j].getWins() == 0){
								population[i].setScore(population[i].getScore() + 1);
								results[i]++;
							}
						}
					}//End main if
				}//End for j
			}//End for i

			//Put first pop into top perfromers
			for (int i = 0; i < fleet_population_size; ++i){
				top_performers[results[i]] = population[i];
			}
		}
		else{ //Compare against top performers

			std::vector<int> results(fleet_population_size);
			for (int i = 0; i < fleet_population_size; ++i){
				for (int j = 0; j < fleet_population_size; ++j){
					if (i != j){
						if (population[i].getWins() > 0){ // If did win at least once
							if (population[i].getWins() > top_performers[j].getWins()){ //Winner Increment
								results[i]++;
							}
							else if (population[i].getWins() == top_performers[j].getWins()){
								if (population[i].getScore() < top_performers[j].getScore()){ //Winner Increment
									results[i]++;
								}
								else if (population[i].getScore() == top_performers[j].getScore()){
									population[i].setScore(population[i].getScore() - 1);
									results[i]++;
								}
							}
						}
						else{ // If no wins
							if (population[i].getScore() > top_performers[j].getScore() && top_performers[j].getWins() == 0){ // Winner Increment
								results[i]++;
							}
							else if (population[i].getScore() == top_performers[j].getScore() && top_performers[j].getWins() == 0){
								population[i].setScore(population[i].getScore() + 1);
								results[i]++;
							}
						}
					}//End main if
				}//End for j
			}//End for i

			//Swap the best performer into the top_perfromer vector
			for (int i = 0; i < fleet_population_size; ++i){
				if (results[i] > 0){
					for (int j = 0; j < results[i] - 1; ++j){
						top_performers[j] = top_performers[j + 1];
					}
					top_performers[results[i]] = population[i];
				}
			}
		}

		for (auto fl : top_performers){
			std::cout << fl.getWins() << " " << fl.getScore() << "\n";
		}

    }
    //Raughing
    std::cout <<"\n\n\n YOU HAVE BEEN BEATEN \n\n\n";
    std::cout << "Rival Fleet is in folder";
	int a;
	std::cin >> a;

}
