#include <iostream>
#include <sstream>

#include "SettingsParser.h"
#include "Block.h"
#include "BlockManager.h"
#include "CollisionCheck.h"
#include "ShipBuilder.h"
#include "ShapeManager.h"
#include "TournamentManager.h"
#include "LogParser.h"

int main()
{
    //Settings Parser Class call
    SettingsParser SP;
    SP.loadFromFile("data/config.txt");

    //Settings to get
    int32_t number_of_ships, p_value_target, block_count_limit, faction;
    std::string path_to_exe;
    std::string path_to_log;
    std::string path_to_ships;

    //Settings get
    SP.get("number_of_ships",number_of_ships);
    SP.get("p_value_target",p_value_target);
    SP.get("block_count_limit",block_count_limit);
    SP.get("faction",faction);
    SP.get("path_to_reassembly_exe",path_to_exe);
    SP.get("path_to_log_folder",path_to_log);
    SP.get("path_to_ships_folder",path_to_ships);

    //Output the settings
    std::cout <<"Number of ships to create= " <<
        number_of_ships << "\nP Value Target= " <<
        p_value_target << "\nFaction= " <<
        faction <<
        "\nBlock Limit= " <<
        block_count_limit <<
        "\n\n";

    //vector to contain ships
    std::vector<Ship> new_ships;

    //fileName for the ship
    std::ostringstream file_name;

    //name for the ship
    std::ostringstream ship_name;

    //Ship Builder Class
    ShipBuilder SB;

    //Tournament manager
    TournamentManager TM(path_to_exe, path_to_ships);

    //Log parser
    LogParser LP(path_to_log);

    //Loop For building
    for(int i = 0; i < number_of_ships; ++i){
        //Create the ships
        new_ships.push_back(SB.createShip(p_value_target,faction,block_count_limit,0));

        //Make the names
        ship_name << "Ship_" << i;
        file_name << "ships/Ship_" << i << ".lua";

        //Write the ship to file
        new_ships[i].writeShip(file_name.str(),ship_name.str(),ship_name.str());

        //Clear the naming streams
        file_name.str("");
        file_name.clear();
        ship_name.str("");
        ship_name.clear();
    }

    for(;;){

        std::vector<int> wins(new_ships.size());

        for(int i = 0; i < new_ships.size(); ++i){
            for(int j = i + 1; j < new_ships.size();++j){

                TM.startArena(new_ships[i],new_ships[j]);
                int result = LP.getWinner(new_ships[i], new_ships[j]);
                if(result == 1){
                    std::cout << new_ships[j].getShipName() <<  " Wins!\n";
                    wins[j]++;
                }
                else if(result == 0)
                {
                    std::cout << new_ships[i].getShipName() <<  " Wins!\n";
                    wins[i]++;
                }
                else if (result == 2){
                    std::cout << "One of your path values is incorrect\n";
                    return(-1);
                }
            }
        }

        int winner_index = 0;
        int winning_kills = 0;
        for(int i = 0; i < wins.size(); ++i){
            if(wins[i] > winning_kills){
                winning_kills = wins[i];
                winner_index = i;
            }
        }

        std::cout << "\n Generation over. " << new_ships[winner_index].getShipName() <<" Has won with " << winning_kills << " victories, it shall live on!!!\n\n";

        //Loop For rebuilding
        Ship winning_ship = new_ships[winner_index];
        winning_ship.writeShip("ships/Winning_Ship.lua", "Winner", "Rob");
        new_ships.clear();

        for(int i = 0; i < number_of_ships; ++i){

            //Create the ships and keep last winner
            if(i != winner_index){
                new_ships.push_back(SB.createShip(p_value_target,faction,block_count_limit,0));
            }
            else{
                new_ships.push_back(winning_ship);
            }

            //Make the names
            ship_name << "Ship_" << i;
            file_name << "ships/Ship_" << i << ".lua";

            //Write the ship to file
            new_ships[i].writeShip(file_name.str(),ship_name.str(),ship_name.str());

            //Clear the naming streams
            file_name.str("");
            file_name.clear();
            ship_name.str("");
            ship_name.clear();
        }
    }
    return EXIT_SUCCESS;
}
