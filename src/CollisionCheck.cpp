
#include "CollisionCheck.h"
#include "Vector2D.h"
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

        //Simple bounding radius check
        float distance = std::sqrt(std::pow(b1.getPosition().x - ship_block.getPosition().x,2.0)+ std::pow(b1.getPosition().y - ship_block.getPosition().y,2.0f));

        if(distance  <= b1.getRadius() + ship_block.getRadius() + 10.0){
                //std::cout << "in range";
            //More advanced collison check
            if(blockCollisionCheck(b1,ship_block)){
                //std::cout << "Collided \n";
                return true;
            }
        }
        else{
            //std::cout << "Out of range ";
        }
    }
    //std::cout << "\n No collidey \n";
    //No collidey
    return false;
}

bool CollisionCheck::blockCollisionCheck(Block& b1, Block& b2)
{
    //std::cout << "Doing collision check \n";
    //std::cout << b2.getBlockNum() << " " << b2.getPosition().x << " " << b2.getPosition().y << "\n";

    std::vector<Vector2D> vec1= b1.getBounds();
    std::vector<Vector2D> vec2= b2.getBounds();

    //To complete the polygon
    vec1.push_back(vec1[0]);
    vec2.push_back(vec2[0]);

    //Imperfect collision checking. ohhhh welll
    //Check for point int polygon with slight offset towards center point because screw floating point math.
    //b1 points in b2
    for(int i = 0; i < (int)vec1.size(); ++i){
        Vector2D temp_vec(vec1[i].x,vec1[i].y);

        float diff_x = temp_vec.x - b1.getPosition().x;
        float diff_y = temp_vec.y - b1.getPosition().y;
        float distance = std::sqrt(std::pow(diff_x,2)+ std::pow(diff_y,2));
        float angle = std::atan2(diff_y,diff_x);
        float x_change = std::cos(angle) * distance * .05;
        float y_change = std::sin(angle) * distance * .05;

        for(int j = 0; j < 10; ++j){
            temp_vec.x -= x_change;
            temp_vec.y -= y_change;
            if(pointInPolygon(temp_vec,vec2)){
                //std::cout<< " Point in polygon 2" << std::endl;
                return true;
            }
        }
    }

    //b2 points in b1
    for(int i = 0; i < (int)vec2.size(); ++i){
        Vector2D temp_vec(vec2[i].x,vec2[i].y);

        float diff_x = temp_vec.x - b2.getPosition().x;
        float diff_y = temp_vec.y - b2.getPosition().y;
        float distance = std::sqrt(std::pow(diff_x,2)+ std::pow(diff_y,2));
        float angle = std::atan2(diff_y,diff_x);
        float x_change = std::cos(angle) * distance * .05;
        float y_change = std::sin(angle) * distance * .05;

        for(int j = 0; j < 10; ++j){
            temp_vec.x -= x_change;
            temp_vec.y -= y_change;
            if(pointInPolygon(temp_vec,vec1)){
                //std::cout<< " Point in polygon 1" << std::endl;
                return true;
            }
        }
    }

    //No collidey
    return false;
}

bool CollisionCheck::pointInPolygon(Vector2D point, std::vector<Vector2D> polygon)
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


