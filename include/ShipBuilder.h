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

#include <SFML/Graphics.hpp>

class ShipBuilder
{
    public:

        ShipBuilder();
        virtual ~ShipBuilder();

        //Symmetric builder... not working yet
        Ship createShip(int32_t target_point_value, int32_t faction, int32_t block_limit, bool is_symmetric);

        //Adds a block to an existing ship. Debug mainly. Could be useful during evolve
        void addBlock(Ship& new_ship, int32_t faction, bool is_symmetric);

    private:
        //Uses these classes
        BlockManager bm;
        CollisionCheck cc;

        // fits a new random block onto the ship
        //May collide. Collision check is done after attempt
        bool tryNewBlock(Ship& new_ship, int32_t faction);

        bool tryNewBlock(Ship& new_ship, int32_t faction, bool is_symmetric);


};

#endif // SHIPBUILDER_H
