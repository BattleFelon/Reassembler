
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


    //Check to see if center is in polygon, but not same pos
    if(pointInPolygon(b1.getPosition(),vec2)){
        //std::cout << "Center in polygon "<< std::endl;
        return true;
    }

    if(pointInPolygon(b2.getPosition(),vec1)){
        //std::cout << "Center2 in polygon  " << std::endl;
        return true;
    }

    //Imperfect collision checking. ohhhh welll
    //Check for point int polygon with slight offset towards center point because screw floating point math.
    //b1 points in b2
    for(int i = 0; i < (int)vec1.size(); ++i){
        Vector2D temp_vec(vec1[i]);

        vec1[i].x > b1.getPosition().x ? temp_vec.x -= 0.1 : temp_vec.x+=0.1 ;
        vec1[i].y > b1.getPosition().y ? temp_vec.y -= 0.1 : temp_vec.y+=0.1 ;

        if(pointInPolygon(temp_vec,vec2)){
           // std::cout<< " Point in polygon 2" << std::endl;
            return true;
        }

        float distance = std::sqrt(std::pow(temp_vec.x - b1.getPosition().x,2)+std::pow(temp_vec.y - b1.getPosition().y,2));
        vec1[i].x > b1.getPosition().x ? temp_vec.x -= 0.1 * distance : temp_vec.x+=0.1 * distance;
        vec1[i].y > b1.getPosition().y ? temp_vec.y -= 0.1 * distance : temp_vec.y+=0.1 * distance;
        if(pointInPolygon(temp_vec,vec2)){
           // std::cout<< " Point in polygon 2" << std::endl;
            return true;
        }
        vec1[i].x > b1.getPosition().x ? temp_vec.x -= 0.1 * distance : temp_vec.x+=0.1 * distance;
        vec1[i].y > b1.getPosition().y ? temp_vec.y -= 0.1 * distance : temp_vec.y+=0.1 * distance;
        if(pointInPolygon(temp_vec,vec2)){
           // std::cout<< " Point in polygon 2" << std::endl;
            return true;
        }
        vec1[i].x > b1.getPosition().x ? temp_vec.x -= 0.1 * distance : temp_vec.x+=0.1 * distance;
        vec1[i].y > b1.getPosition().y ? temp_vec.y -= 0.1 * distance : temp_vec.y+=0.1 * distance;
        if(pointInPolygon(temp_vec,vec2)){
           // std::cout<< " Point in polygon 2" << std::endl;
            return true;
        }
    }

    //b2 points in b1
    for(int i = 0; i < (int)vec2.size(); ++i){
        Vector2D temp_vec(vec2[i].x,vec2[i].y);

        vec1[i].x > b1.getPosition().x ? temp_vec.x -= 0.1 : temp_vec.x+=0.1 ;
        vec1[i].y > b1.getPosition().y ? temp_vec.y -= 0.1 : temp_vec.y+=0.1 ;

        if(pointInPolygon(temp_vec,vec1)){
            //std::cout<< " Point in polygon 1" << std::endl;
            return true;
        }

        float distance = std::sqrt(std::pow(temp_vec.x - b2.getPosition().x,2)+std::pow(temp_vec.y - b2.getPosition().y,2));
        vec2[i].x > b2.getPosition().x ? temp_vec.x -= 0.1 * distance : temp_vec.x+= 0.1 * distance;
        vec2[i].y > b2.getPosition().y ? temp_vec.y -= 0.1 * distance : temp_vec.y+= 0.1 * distance;
        if(pointInPolygon(temp_vec,vec1)){
            //std::cout<< " Point in polygon 1" << std::endl;
            return true;
        }

        vec2[i].x > b2.getPosition().x ? temp_vec.x -= 0.1 * distance : temp_vec.x+= 0.1 * distance;
        vec2[i].y > b2.getPosition().y ? temp_vec.y -= 0.1 * distance : temp_vec.y+= 0.1 * distance;
        if(pointInPolygon(temp_vec,vec1)){
            //std::cout<< " Point in polygon 1" << std::endl;
            return true;
        }

        vec2[i].x > b2.getPosition().x ? temp_vec.x -= 0.1 * distance : temp_vec.x+= 0.1 * distance;
        vec2[i].y > b2.getPosition().y ? temp_vec.y -= 0.1 * distance : temp_vec.y+= 0.1 * distance;
        if(pointInPolygon(temp_vec,vec1)){
            //std::cout<< " Point in polygon 1" << std::endl;
            return true;
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


