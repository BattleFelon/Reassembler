#include <iostream>
#include <sstream>

#include "SettingsParser.h"
#include "Block.h"
#include "BlockManager.h"
#include "CollisionCheck.h"
#include "ShipBuilder.h"
#include "ShapeManager.h"

int main()
{
    //Settings Parser Class call
    SettingsParser SP;
    SP.loadFromFile("data/config.txt");

    //Settings to get
    int32_t number_of_ships, p_value_target, block_count_limit, faction;

    //Settings get
    SP.get("number_of_ships",number_of_ships);
    SP.get("p_value_target",p_value_target);
    SP.get("block_count_limit",block_count_limit);
    SP.get("faction",faction);

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

    return EXIT_SUCCESS;
}
