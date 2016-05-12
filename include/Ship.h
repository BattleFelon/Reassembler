// License: GNU Public License
// Author: Robert Viren
// Date:   2016
// ----------------------------

//Container for ship data and manipulation. Fairly small and self explaining

#ifndef SHIP_H
#define SHIP_H

#include<SFML/Graphics.hpp>
#include "Block.h"

class Ship
{
    public:

        Ship(int32_t faction);
        virtual ~Ship();

        int32_t getTotalValue(){int32_t total = 0; for(auto& block : m_blocks) total += block.getPointValue(); return total;}

        Attachment getAttachmentPoint(int32_t &block_index,int32_t &attachment_index);

        void writeShip(std::string file_name,std::string ship_name,std::string author_name);

        Ship readShip();

        void addBlock(Block new_block){m_blocks.push_back(new_block);}

        bool removeBlock(int32_t index);

        std::vector<Block>& getBlocks(){return m_blocks;}

        //Gets rid of all touching attachments
        //Works really slow. Not in use
        //TODO optimize or get rid of
        void clearAttachments();

    private:
        int32_t m_faction;

        //The blocks that make up the ship
        std::vector<Block> m_blocks;
};

#endif // SHIP_H
