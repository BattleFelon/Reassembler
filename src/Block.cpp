#include "Block.h"

#include <cmath>
#include <iostream>
#include "Vector2D.h"

#define ROUNDING_ERROR .00001
#define M_PI 3.14159265359

Block::Block(int number,
    int points,
    int selection_weight,
    int mirror_number,
    Shape block_shape):
    m_number(number),
    m_points(points),
    m_selection_weight(selection_weight),
    m_mirror_number(mirror_number),
    m_block_rotation(0.0f)
{
    //Pass shape data
    m_bounds = block_shape.getBounds();
    m_attachments = block_shape.getAttachments();
    m_position = block_shape.getPosition();
    m_collision_raduis = block_shape.getRadius();
    m_block_rotation = block_shape.getRotation();

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
Block::Block(int block_number):
    m_number(block_number),
    m_points(0),
    m_selection_weight(0),
    m_mirror_number(0)
{

}

Block::~Block()
{

}

//translate all block data to new position
void Block::translate(Vector2D new_position)
{
    m_position += new_position;

    for(auto& bound : m_bounds)
    {
        bound += new_position;
    }

    for(auto& attachment : m_attachments)
    {
        attachment.position += new_position;
    }
}

//Rotate the block points and angle data.
void Block::rotateBlock(float angle)
{
    //check for zero. No reason to rotate zero
    if(angle != 0.0f)
    {
        //Normalize angle
        while(angle < 0.0f) angle+= 360.0f;
        while(angle >= 360.0f) angle -= 360.0f;

        //Hold degree angle for "angle" change
        float angle_deg = angle;

        //convert to radian
        angle *= M_PI /180.0f;

        //Change private rotation
        //The .0001 is because floating point angle is showing up as higher than M_PI even though it is not
        if(angle-.0001 > M_PI)
            m_block_rotation = (angle - 2 * M_PI);
        else
            m_block_rotation = angle;

        //Rotate bounds
        for(int i = 0; i < (int)m_bounds.size(); ++i)
        {
            float old_x = m_bounds[i].x;
            float old_y = m_bounds[i].y;

            //Rotation
            m_bounds[i].x = old_x * cos(angle) - old_y * sin(angle);
            m_bounds[i].y = old_y * cos(angle) + old_x * sin(angle);

            //Rounding down because of trig floating point errors
            if(ROUNDING_ERROR > m_bounds[i].x && m_bounds[i].x  > -ROUNDING_ERROR)
                m_bounds[i].x  = 0.0f;
            if(ROUNDING_ERROR > m_bounds[i].y && m_bounds[i].y > -ROUNDING_ERROR)
                m_bounds[i].y = 0.0f;
        }

        //Rotate the attachments and "angles"
        for(int i = 0; i < (int)m_attachments.size(); ++i)
        {
            float old_x = m_attachments[i].position.x;
            float old_y = m_attachments[i].position.y;

            //Rotation
            m_attachments[i].position.x = old_x * cos(angle) - old_y * sin(angle);
            m_attachments[i].position.y = old_y * cos(angle) + old_x * sin(angle);

            //Round because trig suckssssss
            if(ROUNDING_ERROR > m_attachments[i].position.x && m_attachments[i].position.x  > -ROUNDING_ERROR)
                m_attachments[i].position.x  = 0.0f;
            if(ROUNDING_ERROR > m_attachments[i].position.y && m_attachments[i].position.y > -ROUNDING_ERROR)
                m_attachments[i].position.y = 0.0f;

            //Change angle
            m_attachments[i].angle += angle_deg;

            //Normalize
            while(m_attachments[i].angle < 0.0f) m_attachments[i].angle += 360.0f;
            while(m_attachments[i].angle >= 360.0f) m_attachments[i].angle -= 360.0f;
        }
    }
}

//Remove an attachment point. true if successful. false if outside of range
bool Block::removeAttachment(int index)
{
    if(index < (int)m_attachments.size())
    {
        m_attachments.erase(m_attachments.begin() + index);
        return(true);
    }
    return(false);
}

void Block::debugBlock(){
    std::cout << "\nPosition= " << m_position.x << " " << m_position.y << "\n";

    for(auto bound : m_bounds)
        std::cout <<"X=" << bound.x << " Y=" << bound.y << " ";
    std::cout << "\n Attachments ";

    for(auto attachments : m_attachments)
        std::cout <<" X=" << attachments.position.x << " Y=" << attachments.position.y  << " angle=" << attachments.angle << " ";
    std::cout << "\n";

}
