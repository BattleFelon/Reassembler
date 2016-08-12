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
	//Set Score;
	best_delay = 0;
	best_wins = 0;

	//Thread Active
	thread_active = true;

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
	SP.get("target_number_of_wins", target_number_of_wins);

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

		//Timer for score
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

	//First Loop Tracker
	bool is_first = true;

    //Fleet name
    std::string fleet_name = "Rival_of_";
    fleet_name.append(target_fleet_name);

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

	//Go until win condition met
	bool win_condition = false;
	while(!win_condition){

		//Fighting
		for (int i = 0; i < fleet_population_size; ++i){
			//Timer for score calc
			clock_t timer = clock();
			TM.startString(population[i].getName(), target_fleet_file);
			int result = LP.getWinner(population[i].getName(), target_fleet_name);

			//End timer
			int diff = clock() - timer;

			//Put in score
			population[i].setScore(diff);

			//Resolving
			if (result == 1){
				loses++;
				if (loses % 100 == 0)
					std::cout << target_fleet_name << " wins again... that makes " << loses << " in a row\n";
				std::cout << ".";
				population[i].resetWin();
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
		if (is_first){ // Check for first loop
            is_first = false;
            std::vector<int> results = getFirstResults(population,population);
			//Put first pop into top performer
			for (int i = 0; i < fleet_population_size; ++i){
				top_performers[results[i]] = population[i];
			}
		}
		else{ //Compare against top performers

			//Swap the best performer into the performer vector
			for (int i = 0; i < fleet_population_size; ++i){

                std::vector<int> results = getResults(population,top_performers);

				if (results[i] > 0){
					top_performers.erase(top_performers.begin());
					top_performers.insert(top_performers.begin() + results[i], population[i]);
				}
			}

			//Sort top
			std::vector<int> results = getResults(top_performers,top_performers);
			//Put first pop into top performer
			for (int i = 0; i < fleet_population_size; ++i){
				top_performers[results[i]] = top_performers[i];
			}

		}

		best_wins = top_performers.back().getWins();
		best_delay = top_performers.back().getScore();

        //Print top performers
        std::cout << "\nTop Performers: Wins, Duration Score\n";
		for(auto fl : top_performers){
            std::cout << "\t\t " << fl.getWins() << ",\t" << fl.getScore() << "\n";
		}

		//Save top
		top_performers[top_performers.size()-1].writeFleet("ships/TopFleet.lua","Rob","TopFleet");
		top_performers[top_performers.size()-2].writeFleet("ships/SecondFleet.lua","Rob","SecondFleet");
		top_performers[top_performers.size()-3].writeFleet("ships/ThirdFleet.lua","Rob","ThirdFleet");

		//New Pop Creation
        for (int i = 0; i < fleet_population_size; ++i){
            //50% is brand new random fleet other half is bred from top performers pool
            if(i < fleet_population_size/2){
                //Keep those that win
                if(population[i].getWins() == 0)
                    population[i] = FB.createFleet(fleet_p_value, fleet_single_ship_p_value_max, fleet_single_ship_p_value_min, block_count_limit, faction, ship_symmetry);
            }
            else{
                int random_index = rand()% fleet_population_size;
                //Keep those that win
                if(population[i].getWins() == 0)
                    population[i] = FB.breedFleet(top_performers.back(),top_performers[random_index],fleet_p_value, ship_symmetry, mutation_blocks);
            }
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

        //Check Win condition
        for(auto fl : top_performers){
            if(fl.getWins() >= target_number_of_wins)
                win_condition = true;
        }
    }
    //Raughing
    std::cout <<"\n\n\n YOU HAVE BEEN BEATEN \n\n\n";
    std::cout << "Rival Fleet is in folder";
	int a;
	std::cin >> a;

}


std::vector<int> FleetMutator::getResults(std::vector<Fleet>& f1, std::vector<Fleet>& f2)
{

    std::vector<int> results(f1.size());
    for (int i = 0; i < f1.size(); ++i){
        for (int j = 0; j < f2.size(); ++j){
            if (f1[i].getWins() > 0){ // If did win at least once
                if (f1[i].getWins() > f2[j].getWins()){ //Winner Increment
                    results[i]++;
                }
                else if (f1[i].getWins() == f2[j].getWins()){
                    if (f1[i].getScore() < f2[j].getScore()){ //Winner Increment
                        results[i]++;
                    }
                    else if (f1[i].getScore() == f2[j].getScore()){
                        f1[i].setScore(f1[i].getScore() - 1);
                        results[i]++;
                    }
                }
            }
            else{ // If no wins
                if (f1[i].getScore() > f2[j].getScore() && f2[j].getWins() == 0){ // Winner Increment
                    results[i]++;
                }
                else if (f1[i].getScore() == f2[j].getScore() && f2[j].getWins() == 0){
                    f1[i].setScore(f1[i].getScore() + 1);
                    results[i]++;
                }
            }
        }//End for j
    }//End for i

    return(results);
}

std::vector<int> FleetMutator::getFirstResults(std::vector<Fleet>& f1, std::vector<Fleet>& f2)
{

	std::vector<int> results(f1.size());
	for (int i = 0; i < f1.size(); ++i) {
		for (int j = 0; j < f1.size(); ++j) {
			if (i != j) {
				if (f1[i].getWins() > 0) { // If did win at least once
					if (f1[i].getWins() > f2[j].getWins()) { //Winner Increment
						results[i]++;
					}
					else if (f1[i].getWins() == f2[j].getWins()) {
						if (f1[i].getScore() < f2[j].getScore()) { //Winner Increment
							results[i]++;
						}
						else if (f1[i].getScore() == f2[j].getScore()) {
							f1[i].setScore(f1[i].getScore() - 1);
							results[i]++;
						}
					}
				}
				else { // If no wins
					if (f1[i].getScore() > f2[j].getScore() && f2[j].getWins() == 0) { // Winner Increment
						results[i]++;
					}
					else if (f1[i].getScore() == f2[j].getScore() && f2[j].getWins() == 0) {
						f1[i].setScore(f1[i].getScore() + 1);
						results[i]++;
					}
				}
			}//End main if
		}//End for j
	}//End for i

	return(results);
}

void FleetMutator::threadedGeneticFleetBeater()
{
	int thread_count = std::thread::hardware_concurrency();
	
	//first loop bool
	bool is_first = true;

	//Winning Population
	std::vector<Fleet> top_performers(fleet_population_size);

	//Make vector
	std::vector<Fleet> population;
	population.resize(thread_count);


	//Go till win condition met
	for (;;) {
		//thread vector
		std::vector<std::thread> battles(thread_count);
		//Delay futures for score estimation
		std::vector<int> delays(thread_count);

		//name vector
		std::vector<std::string> search_names(thread_count);
		//Population spawn
		for (int i = 0; i < thread_count; ++i) {
			if (population[i].getWins() == 0) {
				population[i] = (FB.createFleet(fleet_p_value, fleet_single_ship_p_value_max, fleet_single_ship_p_value_min, block_count_limit, faction, ship_symmetry));
			}
			else if(i > thread_count /2){
				int random_index = rand() % fleet_population_size;
				//Keep those that win
				if (population[i].getWins() == 0)
					population[i] = FB.breedFleet(top_performers.back(), top_performers[random_index], fleet_p_value, ship_symmetry, mutation_blocks);
			}

			//Write the ships to file
			std::string file_name = "ships/";
			file_name.append("Rival_of_");
			file_name.append(target_fleet_name);
			file_name.append(std::to_string(i));
			file_name.append(".lua");

			std::string ship_name = "Rival_of_";
			ship_name.append(target_fleet_name);
			ship_name.append(std::to_string(i));

			population[i].setName(ship_name);
			population[i].writeFleet(file_name, "Reassembler", ship_name);

			std::cout << "Launching thread # " << i << std::endl;

			battles[i] = std::thread(&TournamentManager::startStringThreaded, TM, ship_name, target_fleet_file, &delays[i]);
			search_names[i] = ship_name;
		}

		//wait for the battles to finish
		for (int i = 0; i < thread_count; ++i) {
			std::cout << "Catching Thread # " << i << std::endl;
			battles[i].join();
		}

		//Get results from files and delete logs
		search_names.push_back(target_fleet_name);
		std::vector<int> battle_results = LP.getWinner(search_names);

		for (int i = 0; i < battle_results.size() - 1; ++i) {
			//Put in score
			population[i].setScore(delays[i]);
			//See result, add to wins if won, kill loser if lost
			if (battle_results[i] > 0) {
				population[i].addWin();
			}
			else {
				population[i].resetWin();
			}
		}

		//Process results
		if (is_first) { // Check for first loop
			is_first = false;
			//
			std::vector<int> results = getFirstResults(population, population);

			//Put first pop into top performer
			for (int i = 0; i < population.size(); ++i) {
				top_performers[results[i]] = population[i];
			}
		}
		else { //Compare against top performers

			   //Swap the best performer into the performer vector
			for (int i = 0; i < thread_count; ++i) {

				std::vector<int> results = getResults(population, top_performers);

				if (results[i] > 0) {
					top_performers.insert(top_performers.begin() + results[i], population[i]);
					top_performers.erase(top_performers.begin());
				}
			}
		}

		//Sort top
		std::vector<int> results = getFirstResults(top_performers, top_performers);
		//Put first pop into top performer
		for (int i = 0; i < fleet_population_size; ++i) {
			top_performers[results[i]] = top_performers[i];
		}

		best_wins = top_performers.back().getWins();
		best_delay = top_performers.back().getScore();

		//Print top performers
		std::cout << "\nTop Performers: Wins, Duration Score\n";
		for (auto fl : top_performers) {
			std::cout << "\t\t " << fl.getWins() << ",\t" << fl.getScore() << "\n";
		}

		//Save top
		top_performers[top_performers.size() - 1].writeFleet("ships/TopFleet.lua", "Rob", "TopFleet");
		top_performers[top_performers.size() - 2].writeFleet("ships/SecondFleet.lua", "Rob", "SecondFleet");
		top_performers[top_performers.size() - 3].writeFleet("ships/ThirdFleet.lua", "Rob", "ThirdFleet");

		if (top_performers.back().getWins() > target_number_of_wins) {
			std::cout << "\n\n Target Number of Wins Reached. You have been beaten!!!\n";
			int a;
			std::cin >> a;
			break;
		}
	}
}