// License: GNU Public License
// Author: Robert Viren
// Date:   2016
// ----------------------------

//Container for ship data and manipulation. Fairly small and self explaining

#ifndef SHIP_H
#define SHIP_H

#include "Block.h"

class Ship
{
    public:

        Ship();
        virtual ~Ship();

        //Ship Information
        int getTotalValue(){int total = 0; for(auto& block : m_blocks) total += block.getPointValue(); return total;}
        int getLifetimeWins(){return m_lifetime_wins;}
        std::string getShipName(){return(m_ship_name);}
        int getFaction(){return m_faction;};

        //Used by mutator for lifetime win tracking
        void wins(){m_lifetime_wins++;}

        //Used by shipbuilder to get a random attachment point
        Attachment getAttachmentPoint(int &block_index,int &attachment_index);

        //Read and write
        void writeShip(std::string file_name,std::string author_name, std::string faction_name);
        Ship readShip();

        //Block functions
        void addBlock(Block new_block){m_blocks.push_back(new_block);}
        bool removeBlock(int index);
        std::vector<Block>& getBlocks(){return m_blocks;}

        //Setters
        void setFaction(int faction){m_faction = faction;}
        void setName(std::string ship_name){m_ship_name = ship_name;}

    private:

        int m_faction;
        int m_lifetime_wins;
        std::string m_ship_name;
        //The blocks that make up the ship
        std::vector<Block> m_blocks;
};

#endif // SHIP_H
