#include <iostream>
#include <SFML/Graphics.hpp>
#include <numeric>

#include "Display.h"
#include "Block.h"
#include "BlockManager.h"
#include "CollisionCheck.h"
#include "ShipBuilder.h"

int main()
{
   // ShipBuilder SB;
   // std::vector<Ship> ships;
    //ships.push_back(SB.createShip(10000,8));

    //ships[0].writeShip("blarg.lua", "one", "Rob");

/*
    BlockManager bm;
    bm.loadBlockData("block_data.csv");

    Block b1 = bm.getBlock(8,802);
    Block b2 = bm.getBlock(8,801);
    b1.translate(sf::Vector2f(3.75,3.75));

    Ship new_ship(8);
    new_ship.addBlock(b2);

    CollisionCheck CC;
    CC.checkCollision(b1,new_ship);
*/
    Display reassem;
    reassem.run();


    return EXIT_SUCCESS;
}
