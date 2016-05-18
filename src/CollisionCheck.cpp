
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

        //Same position check
        if(ship_block.getPosition() == b1.getPosition()){
            //std::cout << " Same Position ";
            return true;
            }
        //Simple bounding radius check
        float distance = std::sqrt(std::pow(b1.getPosition().x - ship_block.getPosition().x,2.0)+ std::pow(b1.getPosition().y - ship_block.getPosition().y,2.0f));

        if(distance  <= b1.getRadius() + ship_block.getRadius() + 10.0){
            //std::cout << "\n In Range ";
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
    std::vector<Vector2D> vec1= b1.getBounds();
    std::vector<Vector2D> vec2= b2.getBounds();

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
            //std::cout<< " Point in polygon 1";
            return true;
            }
    }

    for(int i = 0; i < (int)vec2.size(); ++i){
        if(pointInPolygon(vec2[i],vec1)){
            //std::cout<< " Point in polygon 2";
            return true;
            }
    }

    //This doesnt really work right
    //Check for line intersection
    for(int i = 0; i < (int)vec1.size(); ++i){
        for(int j = 0; j < (int)vec2.size(); ++j){
            //Make values readable
            float X1 = vec1[i].x;float Y1 = vec1[i].y;float X2 = vec1[i+1].x;float Y2 = vec1[i+1].y;
            float X3 = vec2[j].x;float Y3 = vec2[j].y;float X4 = vec2[j+1].x;float Y4 = vec2[j+1].y;

            float denominator = (Y4 - Y3)*(X2 - X1) - (X4 - X3)*(Y2 - Y1);
            if(denominator <.001 && denominator > -.001){
                //Parallel condition
            }
            else{
            float ua = ((X4 - X3)*(Y1 - Y3) - (Y4 - Y3)*(X1 - X3)) / denominator;
            float ub = ((X2-X1)*(Y1-Y3)-(Y2-Y1)*(X1-X3)) / denominator;

            if(ua > .01 && ua < .99 && ub > 0.01 && ub < .99){
                //std::cout << "Ua UB collision "<< ua << " " << ub << " ";
                return true;
                }
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


