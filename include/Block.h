// License: GNU Public License
// Author: Robert Viren
// Date:   2016
// ----------------------------

//Block contains the information produced by block manager.It holds the bounding vertex information of the block
//The attachment points, and other information specific to each block. It also contains useful manipulators
//It is designed so the defined blocks should not be changed. Only copies of the defined blocks should be changed

#ifndef BLOCK_H
#define BLOCK_H

#include <random>

#include "Shape.h"
#include "Vector2D.h"

class Block
{
    public:

        Block(int number,
            int points,
            int selection_weight,
            int mirror_number,
            Shape block_shape
            );

        //For sending a null block as a return value
        Block(int block_number);

        virtual ~Block();

        //Moves all relevant points to a new location
        void translate(Vector2D new_position);

        //Rotate the block points and angle data.
        void rotateBlock(float angle);

        //Remove an attachment point. true if successful. false if outside of range
        bool removeAttachment(int index);

        std::vector<Vector2D>& getBounds(){return m_bounds;}

        //get normals and position of attachments
        std::vector<Attachment>& getAttachments(){return m_attachments;}

        int getBlockNum(){return m_number;}

        int getSelectionWeight(){return m_selection_weight;}

        int getPointValue(){return m_points;}

        float getRadius(){return m_collision_raduis;}

        float getRotation(){return m_block_rotation;}

        Vector2D getPosition(){return m_position;}

        //Needs to pass index so the builder knows which attachment it is using
        Attachment getRandomAttachment(int &index){index = rand() % m_attachments.size(); return(m_attachments[index]);}

        //get the mirror block number
        int getMirror(){return m_mirror_number;}

        bool hasThrustAttachment(){for(auto attachment : m_attachments) if(attachment.is_thruster_attachment) return(true); return(false);}

        //Output all block data to console
        void debugBlock();

    private:

        float distance(float x1, float y1, float x2, float y2){return(std::sqrt(std::pow(x1-x2,2.0) + std::pow(y1-y2,2.0)));}

        //Number of the block via the block.lua
        const int m_number;

        //Blocks value in P
        const int m_points;

        //The chance a block is selected. Normalized out of 100
        const int m_selection_weight;

        const int m_mirror_number;

        //Block center position
        Vector2D m_position;

        //vector containing the position of the vertex of the block. Used for collision
        std::vector<Vector2D> m_bounds;

        //Position of the attachment points
        std::vector<Attachment> m_attachments;

        //Used to filter collision checks
        float m_collision_raduis;

        //rotation to write to file. Kept in radians
        float m_block_rotation;
};

#endif // BLOCK_H
