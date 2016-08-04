#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

#include "BlockManager.h"
#include "Ship.h"

#define M_PI 3.14159265359

Ship::Ship():
    m_lifetime_wins(0)
{

}

Ship::~Ship()
{

}

Attachment Ship::getAttachmentPoint(int &block_index,int &attachment_index)
{
    Attachment null_attachment;
    block_index = attachment_index = -1;

    if(m_blocks.size() == 0)
        return(null_attachment);

    block_index = rand() % m_blocks.size();

    if(m_blocks[block_index].getAttachments().size() == 0)
        return(null_attachment);

    attachment_index = rand() % m_blocks[block_index].getAttachments().size();
    return(m_blocks[block_index].getAttachments()[attachment_index]);

}

//Reassembly corrects for the command block not actually being the center after it loads the file
//TODO - center of gravity finding and proper file writing.
void Ship::writeShip(std::string file_name,std::string author_name, std::string faction_name)
{
    std::ofstream file;

    file.open(file_name);

    if(file.good()){
        //Set to 3 decimal place precision
        file.precision(3);
        file.setf(std::ios::fixed);
        //Fleet header
        file << "{\n  color0=0x410101,\n  color1=34182,\n  faction=" << m_faction << ",\n  name=\"" << faction_name.c_str() << "\",\n  blueprints={\n    " ;

        //Ship header
        file << "{data={name=\""<<m_ship_name.c_str()<<"\", author=\""<<author_name.c_str()<<"\", color0=0xc96f20, color1=0x796bff,\n";

        //group line
        file << "    wgroup={0, 0, 2, 0}}, blocks={\n";

        //command block
        file << "    {" << m_blocks[0].getBlockNum() << ", {0.000, 0.000}, command={faction=" << m_faction << "}},\n";

        //All the other blocks
        for(int i = 1; i < (int)m_blocks.size() -1 ;++i){
            file << "    {" << m_blocks[i].getBlockNum() << ", {" <<m_blocks[i].getPosition().x<< ", " << m_blocks[i].getPosition().y << "}";
            if(m_blocks[i].getRotation() != 0)
                file << ", " << m_blocks[i].getRotation() << "},\n";
            else
                file << "},\n";
        }

        //Write the closing line
        file << "    {" << m_blocks.back().getBlockNum() << ", {" <<m_blocks.back().getPosition().x<< ", " << m_blocks.back().getPosition().y << "}";
        if(m_blocks.back().getRotation() != 0)
            file << ", " << m_blocks.back().getRotation() << "}}}"; // Reason for this closing line
        else
            file << "}}}"; //Other reason for the closing line

            //Close the faction brackets
            file << "}}";
    }
    else{
        std::cout << "Could not write file for some reason\n";
    }

    file.close();
}

Ship Ship::readShip(std::string file_name)
{

	Ship new_ship;
	//Block manager for block data access
	BlockManager BM;
	BM.loadBlockData("data/block_data.csv");

	//Input line
	std::string line;

	std::string m_fleet_name;

	//Open her up
	std::ifstream input(file_name);
	if (input.is_open()){
		for (int i = 0; i < 3; ++i){
			std::getline(input, line); //Skip first three lines
		}

		std::getline(input, line); //Line contains faction number
		//Search for faction number
		for (int i = 100; i > 0; i--){
			if (line.find(std::to_string(i)) != std::string::npos){
				m_faction = i;
				i = 0;
			}
		}
		
		//Faction name line
		std::getline(input, line);
		auto found = line.find("name=");
		auto end_find = line.find('"', found + 6);
		if (found != std::string::npos){
			m_fleet_name = line.substr(found + 6, end_find - found - 6);
		}

		//Used to track command block, shift positions for local coords
		bool is_command = false;
		Vector2D origin_offset(0, 0);
		float angle = 0;

		//Main processing loop
		while (input.good()){
			std::cout << "there\n";
			std::getline(input, line); //Ship Header

			//Count brackets to figure out what to do
			int bracket_count = bracketCount(line);
			//Process block data for ship
			if (bracket_count == 0){
				auto found = line.find('{');
				auto end_find = line.find(',', found);
				int block_number = std::atoi(line.substr(found + 1, end_find - found - 1).c_str());

				if (block_number != 0){
					if (is_command){
						new_ship.addBlock(BM.getCommandBlock(m_faction));
						new_ship.setFaction(m_faction);
						found = line.find('{', end_find);
						end_find = line.find(',', found);
						origin_offset.x = std::stof(line.substr(found + 1, end_find - found - 1).c_str());
						found = line.find('}', end_find);
						origin_offset.y = std::stof(line.substr(end_find + 2, found - end_find - 2).c_str());
						is_command = false;
					}
					else{
						//Load unaltered block
						Block new_block = BM.getBlock(m_faction, block_number);

						//Solve for position
						Vector2D new_position(0, 0);
						//new_position.x -= origin_offset.x;
						//new_position.y -= origin_offset.y;

						found = line.find('{', end_find);
						end_find = line.find(',', found);
						new_position.x += std::stof(line.substr(found + 1, end_find - found - 1).c_str());
						found = line.find('}', end_find);
						new_position.y += std::stof(line.substr(end_find + 2, found - end_find - 2).c_str());

						//Rotation gathering
						if (line.find("binding") == std::string::npos){ //No binding id in line
							end_find = line.find('}', found + 1);

							if (end_find - found > 3){ //Must be rotation
								angle = std::stof(line.substr(found + 3, end_find - found - 3).c_str());
								// std::cout << angle << "\n";
							}
						}
						else{ //Gotta handle binding id
							int bracket_pos = line.find('}', found + 2);
							int comma_pos = line.find(',', found + 2);

							if (comma_pos < bracket_pos) // rotation and binding
							{
								angle = std::stof(line.substr(found + 3, comma_pos - found - 3).c_str());
							}
						}

						//Use gathered data
						new_block.rotateBlock(angle * 180.0 / M_PI);
						new_block.translate(new_position);

						new_ship.addBlock(new_block);
						angle = 0;
						new_position.x = 0;
						new_position.y = 0;
					}
				}
				//Block new_block = BM.loadBlockData()
			}
			//New ship
			else if (bracket_count == 2){
				std::cout << "here";
			}
		}
	}
	else{
		std::cout << "Could not find fleet to load, Check to ensure proper path and file name. Make sure it is not compressed\n";
	}

	
	input.close();
	return(new_ship);
}

int Ship::bracketCount(std::string line)
{
	int counter = 0;
	for (int i = 0; i < line.size(); ++i){
		if (line[i] == '{')
			counter++;
		if (line[i] == '}')
			counter--;
	}

	return(counter);
}

