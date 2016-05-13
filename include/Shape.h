// License: GNU Public License
// Author: Robert Viren
// Date:   2016
// ----------------------------

//Shape is a simple container for the shape data of the block. The data is copied to the block. This is so the data files can look nicer and shape data doesn't need to be replicated

#ifndef SHAPE_H
#define SHAPE_H

#include <SFML/Graphics.hpp>
#include <random>
#include <ctime>

//USed to keep position and normals
struct Attachment
{
    sf::Vector2f position;
    float angle;
    bool is_thruster_attachment;
};

class Shape
{
    public:

        Shape(
            sf::Vector2f position,
            std::vector<sf::Vector2f> bounds,
            std::vector<Attachment> attachments
            );

        //For sending a null shape as a return value
        Shape();

        virtual ~Shape();

        //Remove an attachment point. true if successful. false if outside of range
        bool removeAttachment(int32_t index);

        std::vector<sf::Vector2f> getBounds(){return m_bounds;}

        //get normals and position of attachments
        std::vector<Attachment> getAttachments(){return m_attachments;}

        float getRadius(){return m_collision_raduis;}

        float getRotation(){return m_block_rotation;}

        sf::Vector2f getPosition(){return m_position;}


    private:

        float distance(float x1, float y1, float x2, float y2){return(std::sqrt(std::pow(x1-x2,2.0) + std::pow(y1-y2,2.0)));}

        //Block center position
        sf::Vector2f m_position;

        //vector containing the position of the vertex of the block. Used for collision
        std::vector<sf::Vector2f> m_bounds;

        //Position of the attachment points
        std::vector<Attachment> m_attachments;

        //Used to filter collision checks
        float m_collision_raduis;

        //rotation to write to file. Kept in radians
        float m_block_rotation;

};

#endif // SHAPE_H
