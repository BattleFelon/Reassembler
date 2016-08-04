#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <ctime>

#include "ShapeManager.h"
#include "BlockManager.h"
#include "Block.h"

BlockManager::BlockManager():
    null_block(0)
{
    srand(time(NULL));
}

BlockManager::~BlockManager()
{

}

//Load the block data into the all block vector
bool BlockManager::loadBlockData(std::string file_name)
{
    //Data file
    std::ifstream data;

    //Shape data passer
    ShapeManager SM;
    SM.loadShapeData("data/shape_data.csv");

    try{
        data.open(file_name);
    }
    catch(int e){
        std::cout << "Failed to parse block data \n";
        return(false);
    }

    std::string line;

    //Skip first example line
    std::getline(data,line);

    //Parse data file
    while(std::getline(data,line,','))
    {
        //Block name
        std::string block_name = line;

        //Load Shape name
        std::getline(data,line,',');
        std::string shape_name = line;

        //Load block number
        std::getline(data,line,',');
        int block_number = std::stoi(line);

        //Load block points
        std::getline(data,line,',');
        int block_points = std::stoi(line);

        //Load block faction
        std::getline(data,line,',');
        int block_faction = std::stoi(line);

        //Load block weight
        std::getline(data,line,',');
        int block_selection_weight = std::stoi(line);

        //Load block mirror number. Used for symmetric builds
        std::getline(data,line);

        int block_mirror_number = std::stoi(line);

        //If new faction, create new spot in map
        if(m_all_blocks.find(block_faction) == m_all_blocks.end()){
            m_all_blocks[block_faction];
        }

        //add to vector
        m_all_blocks[block_faction].push_back( Block(block_number,block_points,block_selection_weight,block_mirror_number,SM.getShape(shape_name)));
    }

    //return Success
    return(true);
}

Block BlockManager::getBlock(int faction)
{
    if(m_all_blocks.find(faction) == m_all_blocks.end()){
        std::cout << "Block faction not found \n";
		std::cout << " " << faction;
    }
    else{
        //Could be optimized....TODO
        while(true){
            //Generate random index that is not the command module
            int random_block_index = rand() % (m_all_blocks[faction].size()-1) + 1;
            int weight_compare = rand() % 1000 + 1;
            if(m_all_blocks[faction][random_block_index].getSelectionWeight() >= weight_compare && !m_all_blocks[faction][random_block_index].hasThrustAttachment())
            {
                return(m_all_blocks[faction][random_block_index]);
            }
        }
    }

    return(null_block);
}

Block BlockManager::getBlock(int faction, int block_number)
{
    if(m_all_blocks.find(faction) == m_all_blocks.end()){
        std::cout << "Block faction not found \n";
		std::cout << block_number << " " << faction;
    }
    else{
        for(int i = 0; i < (int)m_all_blocks[faction].size(); ++i){
            if(block_number == m_all_blocks[faction][i].getBlockNum())
                return(m_all_blocks[faction][i]);
        }
        std::cout << "Block number not found \n";
		std::cout << block_number << " " << faction;
    }

    return(null_block);
}

Block BlockManager::getCommandBlock(int faction)
{
    if(m_all_blocks.find(faction) == m_all_blocks.end()){
        std::cout << "Block faction not found \n";
		std::cout << " " << faction;
    }
    else{
        return(m_all_blocks[faction][0]);
    }

    return(null_block);
}

Block BlockManager::getThruster(int faction)
{

    if(m_all_blocks.find(faction) == m_all_blocks.end()){
        std::cout << "Block faction not found \n";
		std::cout << " " << faction;
    }
    else{
        //Could be optimized....TODO
        while(true){
            //Generate random index that is not the command module
            int random_block_index = rand() % (m_all_blocks[faction].size()-1) + 1;
            int weight_compare = rand() % 1000 + 1;
            if(m_all_blocks[faction][random_block_index].getSelectionWeight() >= weight_compare && m_all_blocks[faction][random_block_index].hasThrustAttachment())
            {
                return(m_all_blocks[faction][random_block_index]);
            }
        }
    }

    return(null_block);
}
