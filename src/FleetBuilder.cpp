#include "FleetBuilder.h"
#include "ShipBuilder.h"
#include <string>
#include <iostream>

FleetBuilder::FleetBuilder()
{
    //ctor
}

FleetBuilder::~FleetBuilder()
{
    //dtor
}


Fleet FleetBuilder::createFleet(int target_p_value, int max_ship_value, int min_ship_value, int block_limit, int faction, int ship_symm)
{
    //Fleet naming
    static int fleet_number = 0;
    fleet_number ++;
    std::string fleet_name = "Fleet_";
    fleet_name.append(std::to_string(fleet_number));

    //Fleet to create
    Fleet new_fleet;
    new_fleet.setFaction(faction);
    new_fleet.setName(fleet_name);
    //Ship naming
    int ship_name  = 0;

    //Building loop
    while(new_fleet.totalValue() < target_p_value){
        //make name
        std::string name = "Ship_";
        name.append(std::to_string(ship_name));

        //Some random p value
        int ship_p_value = rand() % (max_ship_value - min_ship_value) + min_ship_value;
        //Some random non-zero percent of p value
        int min_thruster_points = ((rand() %30 + 5) / 100.0) * ship_p_value;

        //Add the ship
        new_fleet.addShip(SB.createShip(ship_p_value,faction,block_limit,min_thruster_points,ship_symm,name));
        //Progress name
        ship_name++;
    }
    //Remove the closest valued ship to the diff
    int point_diff = new_fleet.totalValue() - target_p_value;
    int closest_ship_value = 1000000000;
    int closest_index = 0;
    for(int i = 0; i < new_fleet.getFleetSize(); ++i){
        if(new_fleet.getShipValue(i) > point_diff){
            if(new_fleet.getShipValue(i) - target_p_value < closest_ship_value){
                closest_ship_value = new_fleet.getShipValue(i) - target_p_value;
                closest_index = i;
            }
        }
    }

    new_fleet.removeShip(closest_index);
	int ship_p_value = target_p_value - new_fleet.totalValue();
	//Some random non-zero percent of p value
	int min_thruster_points = ((rand() %30 + 5) / 100.0) * ship_p_value;

	std::string name = "Ship_";
	name.append(std::to_string(ship_name));

	//Add the ship
	new_fleet.addShip(SB.createShip(ship_p_value, faction, block_limit, min_thruster_points, ship_symm, name));
    return(new_fleet);
}
