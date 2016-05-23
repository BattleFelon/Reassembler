// License: GNU Public License
// Author: Robert Viren
// Date:   2016
// ----------------------------

//Class used to build the ships according to inputs
//TODO - Block count limiter, thruster attachment control. block vector adding.

#ifndef SHIPBUILDER_H
#define SHIPBUILDER_H

#include "Block.h"
#include "BlockManager.h"
#include "Ship.h"
#include "CollisionCheck.h"

class ShipBuilder
{
    public:

        ShipBuilder();
        virtual ~ShipBuilder();

        //General ship building function
        Ship createShip(int target_point_value, int faction, int block_limit,int min_thruster_points, int is_symmetric, std::string name);

        //Actual genetic code!!!
        Ship breedShips(Ship& father, Ship& mother, int ship_symmetry, int mutation_blocks);


        //Adds a block to an existing ship. Debug mainly. Could be useful during evolve
        void addBlock(Ship& new_ship, int faction, int is_symmetric);
        void addThrust(Ship& new_ship, int faction, int is_symmetric);

    private:
        //Uses these classes
        BlockManager bm;
        CollisionCheck cc;

        //fits a new random block onto the ship
        //May collide. Collision check is done after attempt
        bool tryNewBlock(Ship& new_ship, int faction,bool is_thrust, bool is_symmetric);

        //Force the fits to ensure weights are correct
        bool forceFitNewBlock(Ship& new_ship, int faction, bool is_symmetric, bool is_thruster);
        //Specific block
        bool forceFitNewBlock(Ship& new_ship,Block new_block,bool is_symmetric);



};

#endif // SHIPBUILDER_H
