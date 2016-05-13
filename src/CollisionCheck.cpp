
#include "CollisionCheck.h"
#include <iostream>



CollisionCheck::CollisionCheck()
{

}

CollisionCheck::~CollisionCheck()
{

}

bool CollisionCheck::checkCollision(Block& b1, Ship& new_ship)
{
    for(auto& ship_block : new_ship.getBlocks()){

        //Same position check
        if(ship_block.getPosition() == b1.getPosition()){
            //std::cout << " Same Position ";
            return true;
            }
        //Simple bounding radius check
        float distance = std::sqrt(std::pow(b1.getPosition().x - ship_block.getPosition().x,2.0)+ std::pow(b1.getPosition().y - ship_block.getPosition().y,2.0f));

        if(distance  <= b1.getRadius() + ship_block.getRadius()){
            //std::cout << "In Range ";
            //More advanced collison check
            if(blockCollisionCheck(b1,ship_block)){
                return true;
            }
        }
        else{
            //std::cout << "Out of range ";
        }
    }
    //std::cout << "No collidey ";
    //No collidey
    return false;
}

bool CollisionCheck::blockCollisionCheck(Block& b1, Block& b2)
{
    std::vector<sf::Vector2f> vec1= b1.getBounds();
    std::vector<sf::Vector2f> vec2= b2.getBounds();

    //To complete the polygon
    vec1.push_back(vec1[0]);
    vec2.push_back(vec2[0]);
    //Check to see if center is in polygon, but not same pos
    if(pointInPolygon(b1.getPosition(),vec2)){
        //std::cout << "Center in polygon ";
        return true;
    }
    if(pointInPolygon(b2.getPosition(),vec1)){
        //std::cout << "Center2 in polygon \n ";
        return true;
    }

    //Check for point int polygon
    for(int i = 0; i < (int)vec1.size(); ++i){
        if(pointInPolygon(vec1[i],vec2)){
            //std::cout<< " Point in polygon ";
            return true;
            }
    }

    for(int i = 0; i < (int)vec2.size(); ++i){
        if(pointInPolygon(vec2[i],vec1)){
            //std::cout<< " Point in polygon ";
            return true;
            }
    }

    //Check for line intersection
    for(int i = 0; i < (int)vec1.size()-1; ++i){
        for(int j = 0; j < (int)vec1.size()-1; ++j){
            //Make values readable
            float XA1 = vec1[i].x;float YA1 = vec1[i].y;float XA2 = vec1[i+1].x;float YA2 = vec1[i+1].y;
            float XB1 = vec2[i].x;float YB1 = vec2[i].y;float XB2 = vec2[i+1].x;float YB2 = vec2[i+1].y;

            float denominator = (YB2 - YB1)*(XA2 - XA1) - (XB2 - XB1)*(YA2 - YA1);
            if(denominator == 0){
                //Parallel condition
            }
            else{
            float ua = ((XB2 - XB1)*(YA1 - YB1) - (YB2 - YB1)*(XA1 - XB1)) / denominator;
            float ub = ((XA2-XA1)*(YA1-YB1)-(YA2-YA1)*(XA1-XB1))/ denominator;

            if((ua < 1 && ua > 0) || (ub < 1 && ub > 0)){
                //std::cout << "Ua UB collision "<< ua << " " << ub << " ";
                return true;
                }
            }
        }
    }

    //No collidey
    return false;
}

bool CollisionCheck::pointInPolygon(sf::Vector2f point, std::vector<sf::Vector2f> polygon)
{
    for(int i = 0; i < (int)polygon.size(); ++i){
        if(polygon[i].x == point.x || polygon[i].y == point.y){
            polygon.erase(polygon.begin() + i);
            }
    }
    int i, j, nvert = polygon.size();
    bool c = false;

    for(i = 0, j = nvert - 1; i < nvert; j = i++) {
        if( ( (polygon[i].y >= point.y ) != (polygon[j].y >= point.y) ) &&
            (point.x <= (polygon[j].x - polygon[i].x) * (point.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x)
            )
            c = !c;
    }

  return c;
}


