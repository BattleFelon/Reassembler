// License: GNU Public License
// Author: Robert Viren
// Date:   2016
// ----------------------------

//Block manager loads all the info from block_data.csv into a map of blocks specific to each faction
//These blocks are the defined blocks and should not be changed. BlockManager is like a block library

#ifndef BLOCKMANAGER_H
#define BLOCKMANAGER_H

#include <map>
#include "Block.h"

class BlockManager
{
    public:
        BlockManager();
        virtual ~BlockManager();

        bool loadBlockData(std::string file_name);

        //Random Block Getting
        Block getBlock(int32_t faction);

        //Specific Block getting
        Block getBlock(int32_t faction, int32_t block_number);

        Block getCommandBlock(int32_t faction);

        //Checks of faction is there
        bool hasFaction(int32_t faction){if(m_all_blocks.find(faction) == m_all_blocks.end()) return(false); else return(true);}

    private:

        //All defined block information. First block in vector should be command
        //Make sure data file has command block first for each faction data
        std::map<int32_t , std::vector<Block>> m_all_blocks;

        //Block to return if not found
        Block null_block;
};

#endif // BLOCKMANAGER_H
