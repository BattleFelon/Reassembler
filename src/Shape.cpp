#include "Shape.h"

#include <cmath>
#include <iostream>

#define ROUNDING_ERROR .00001
#include "Vector2D.h"

Shape::Shape(
    Vector2D position,
    std::vector<Vector2D> bounds,
    std::vector<Attachment> attachments):
    m_position(position),
    m_bounds(bounds),
    m_attachments(attachments),
    m_block_rotation(0.0f)
{

    //Set the collision radius
    m_collision_raduis = 0.0;
    for(auto& b: m_bounds)
    {
        float current_dist = distance(m_position.x,m_position.y,b.x,b.y);
        if(current_dist > m_collision_raduis)
            m_collision_raduis = current_dist;
    }
}

//Null block creation
Shape::Shape()
{

}

Shape::~Shape()
{

}

