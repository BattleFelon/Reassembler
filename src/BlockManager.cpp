#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <ctime>

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
    std::ifstream data;

    try{
        data.open(file_name);
    }
    catch(int e){
        std::cout << "Failed to parse block data \n";
        return(false);
    }

    std::string line;

    //Skip first 7 example lines
    for(int i = 0; i < 7; ++i)
        std::getline(data,line);

    //Parse data file
    while(std::getline(data,line,','))
    {
        //Check for thruster
        bool is_thruster = false;

        //Temp vectors for bound and attachment loading
        std::vector<float> x_bounds,y_bounds,x_attachments,y_attachments,angle_attachments;
        std::vector<int> thruster_attachment;
        //Block name. Mainyl used to determine if thruster
        std::string block_name = line;
        if(block_name == "Thruster"){
            is_thruster = true;
        }

        //Load block number
        std::getline(data,line,',');
        int32_t block_number = std::stoi(line);

        //Load block points
        std::getline(data,line,',');
        int32_t block_points = std::stoi(line);

        //Load block points
        std::getline(data,line,',');
        int32_t block_faction = std::stoi(line);

        //Load block points
        std::getline(data,line);
        int32_t block_selection_weight = std::stoi(line);

        //Load bound and attachment information
        //If statement for thruster logic
        int limit = 5;
        if(is_thruster)
            limit = 6;

        for(int i = 0; i < limit; ++ i)
        {
            std::getline(data,line);

            //Read line into stream for breakup
            //Gotta do this because of unknown line length
            std::stringstream ss;
            ss << line;
            std::string value;
            while(std::getline(ss,value,','))
            {
                //Check for empty value. Csv files format some null value ,,,, for some reason
                if((int)value[0] != 0){
                    // Sort the values into the proper vector
                    switch(i)
                    {
                        case 0:
                            x_bounds.push_back(std::stof(value));
                            break;
                        case 1:
                            y_bounds.push_back(std::stof(value));
                            break;
                        case 2:
                            x_attachments.push_back(std::stof(value));
                            break;
                        case 3:
                            y_attachments.push_back(std::stof(value));
                        break;
                        case 4:
                            angle_attachments.push_back(std::stof(value));
                            break;
                        case 5:
                            thruster_attachment.push_back(std::stof(value));
                            break;
                        default:
                            break;
                    }
                }
            }// End of the line
        }// End of block data
        //Create pair vectors
        std::vector<sf::Vector2f> bounds;
        //Attachment vector
        std::vector<Attachment> attachments;
        for(int i = 0; i < (int)x_bounds.size(); i++)
        {
            bounds.push_back(sf::Vector2f(x_bounds[i],y_bounds[i]));
        }
        for(int i = 0; i < (int)x_attachments.size(); i++)
        {
            Attachment new_attachment;
            new_attachment.position = sf::Vector2f(x_attachments[i],y_attachments[i]);
            new_attachment.angle = angle_attachments[i];
            if(is_thruster){
                new_attachment.is_thruster_attachment = thruster_attachment[i];
            }
            else{
                new_attachment.is_thruster_attachment = false;
            }
            attachments.push_back(new_attachment);
        }

        //If new faction, create new spot in map
        if(m_all_blocks.find(block_faction) == m_all_blocks.end())
            m_all_blocks[block_faction];

        //add to vector

        m_all_blocks[block_faction].push_back( Block(block_number,block_points,block_selection_weight,sf::Vector2f(0.0,0.0),bounds,attachments));
    }

    //return Success
    return(true);
}

Block BlockManager::getBlock(int32_t faction)
{
    if(m_all_blocks.find(faction) == m_all_blocks.end()){
        std::cout << "Block faction not found \n";
    }
    else{
        //Could be optimized....TODO
        while(true){
            //Generate random index that is not the command module
            int32_t random_block_index = rand() % (m_all_blocks[faction].size()-1) + 1;
            int32_t weight_compare = rand() % 100 + 1;
            if(m_all_blocks[faction][random_block_index].getSelectionWeight() >= weight_compare)
            {
                return(m_all_blocks[faction][random_block_index]);
            }
        }
    }

    return(null_block);
}

Block BlockManager::getBlock(int32_t faction, int32_t block_number)
{
    if(m_all_blocks.find(faction) == m_all_blocks.end()){
        std::cout << "Block faction not found \n";
    }
    else{
        for(int i = 0; i < (int)m_all_blocks[faction].size(); ++i){
            if(block_number == m_all_blocks[faction][i].getBlockNum())
                return(m_all_blocks[faction][i]);
        }
        std::cout << "Block number not found \n";
    }

    return(null_block);
}

Block BlockManager::getCommandBlock(int32_t faction)
{
    if(m_all_blocks.find(faction) == m_all_blocks.end()){
        std::cout << "Block faction not found \n";
    }
    else{
        return(m_all_blocks[faction][0]);
    }

    return(null_block);
}
