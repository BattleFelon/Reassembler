// License: GNU Public License
// Author: Robert Viren
// Date:   2016
// ----------------------------

//Collision checking class. Really just a series of functions that determines if a block will collide with a ship

#ifndef COLLISIONCHECK_H
#define COLLISIONCHECK_H

#include "Block.h"
#include "Ship.h"
#include "Vector2D.h"

class CollisionCheck
{
    public:
        CollisionCheck();
        virtual ~CollisionCheck();

        //true means there was a collision. False means no collision
        bool checkCollision(Block& b1, Ship& new_ship);


    private:
        //Closer collision check. Contains line check and PIP
        bool blockCollisionCheck(Block& b1, Block& b2);

        //Checks for a point in a polygon collision
        bool pointInPolygon(Vector2D point, std::vector<Vector2D> polygon);

        //TODO - std::vector<Block> and std::vector<block> collision check for eventual chunk combining effort
};

#endif // COLLISIONCHECK_H
