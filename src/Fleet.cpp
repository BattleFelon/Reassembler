#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

#include "Ship.h"
#include "Fleet.h"
#include "BlockManager.h"
#include "Block.h"

#define M_PI 3.14159265359

Fleet::Fleet()
{

}

Fleet::~Fleet()
{

}

//Reassembly corrects for the command block not actually being the center after it loads the file
//TODO - center of gravity finding and proper file writing.
void Fleet::writeFleet(std::string file_name,std::string author_name, std::string faction_name)
{
    std::ofstream file;
    m_fleet_name = faction_name;

    file.open(file_name);

    if(file.good()){
        //Set to 3 decimal place precision
        file.precision(3);
        file.setf(std::ios::fixed);
        //Fleet header
        file << "{\n  color0=0x410101,\n  color1=34182,\n  faction=" << m_faction << ",\n  name=\"" << faction_name << "\",\n  blueprints={\n    " ;

        //Loop for ships
        int last_ship = 0;
        for(auto& ship : m_ships){
            //counter for last ship
            last_ship++;

            //Ship header
            file << "{data={name=\""<<ship.getShipName()<<"\", author=\""<<author_name<<"\", color0=0xc96f20, color1=0x796bff,\n";

            //group line
            file << "    wgroup={0, 0, 2, 0}}, blocks={\n";

            //command block
            file << "    {" << ship.getBlocks()[0].getBlockNum() << ", {0.000, 0.000}, command={faction=" << m_faction << "}},\n";

            //All the other blocks
            for(int i = 1; i < (int)ship.getBlocks().size() -1 ;++i){
                file << "    {" << ship.getBlocks()[i].getBlockNum() << ", {" <<ship.getBlocks()[i].getPosition().x<< ", " << ship.getBlocks()[i].getPosition().y << "}";
                if(ship.getBlocks()[i].getRotation() != 0)
                    file << ", " << ship.getBlocks()[i].getRotation() << "},\n";
                else
                    file << "},\n";
            }

            //Write the closing line for ship
            file << "    {" << ship.getBlocks().back().getBlockNum() << ", {" <<ship.getBlocks().back().getPosition().x<< ", " << ship.getBlocks().back().getPosition().y << "}";
            if(last_ship != m_ships.size()){
                if(ship.getBlocks().back().getRotation() != 0)
                    file << ", " << ship.getBlocks().back().getRotation() << "}}},\n"; // Reason for this closing line
                else
                    file << "}}},\n"; //Other reason for the closing line
            }
            //If last ship in fleet, write closing for fleet
            else{
                if(ship.getBlocks().back().getRotation() != 0)
                    file << ", " << ship.getBlocks().back().getRotation() << "}}}"; // Reason for this closing line
                else
                    file << "}}}"; //Other reason for the closing line
            }

        }

        //Close the fleet brackets
        file << "}}";
    }
    else{
        std::cout << "Could not write file for some reason\n";
    }
    file.close();
}

void Fleet::readFleet(std::string file_name)
{
    //Block manager for block data access
    BlockManager BM;
    BM.loadBlockData("data/block_data.csv");

    //Input line
    std::string line;

    //Open her up
    std::ifstream input (file_name);
    if(input.is_open()){
        for(int i = 0; i < 3; ++i)
            std::getline(input,line); //Skip first three lines

        std::getline(input,line); //Line contains faction number
        //Search for faction number
        for(int i = 100; i > 0; i--){
            if(line.find(std::to_string(i)) != std::string::npos){
                m_faction = i;
                i = 0;
            }
        }

        //Faction name line
        std::getline(input,line);
        auto found = line.find("name=");
        auto end_find = line.find('"',found + 6);
        if(found != std::string::npos){
            m_fleet_name = line.substr(found+6,end_find - found - 6);
        }
        //Used to track command block, shift positions for local coords
        bool is_command = false;
        Vector2D origin_offset(0,0);
        float angle = 0;

        //Main processing loop
        while(input.good()){
            std::getline(input,line); //Ship Header

            //Count brackets to figure out what to do
            int bracket_count = bracketCount(line);
            //Process block data for ship
            if(bracket_count == 0){
                auto found = line.find('{');
                auto end_find = line.find(',',found);
                int block_number = std::atoi(line.substr(found+1,end_find-found-1).c_str());

                if(block_number != 0){
                    if(is_command){
                    m_ships.back().addBlock(BM.getCommandBlock(m_faction));
                    m_ships.back().setFaction(m_faction);
                    found = line.find('{',end_find);
                    end_find = line.find(',',found);
                    origin_offset.x  = std::stof(line.substr(found+1,end_find-found-1).c_str());
                    found = line.find('}',end_find);
                    origin_offset.y = std::stof(line.substr(end_find +2, found - end_find -2).c_str());
                    is_command = false;
                    }
                    else{
                        //Load unaltered block
                        Block new_block = BM.getBlock(m_faction,block_number);

                        //Solve for position
                        Vector2D new_position(0,0);
                        //new_position.x -= origin_offset.x;
                        //new_position.y -= origin_offset.y;

                        found = line.find('{',end_find);
                        end_find = line.find(',',found);
                        new_position.x += std::stof(line.substr(found+1,end_find-found-1).c_str());
                        found = line.find('}',end_find);
                        new_position.y += std::stof(line.substr(end_find +2, found - end_find -2).c_str());

                        //Rotation gathering
                        if(line.find("binding") == std::string::npos){ //No binding id in line
                            end_find = line.find('}',found+1);

                            if(end_find - found > 3){ //Must be rotation
                                angle = std::stof(line.substr(found+3,end_find-found-3).c_str());
                               // std::cout << angle << "\n";
                            }
                        }
                        else{ //Gotta handle binding id
                            int bracket_pos = line.find('}',found+2);
                            int comma_pos = line.find(',',found+2);

                            if(comma_pos < bracket_pos) // rotation and binding
                            {
                                angle = std::stof(line.substr(found+3,comma_pos - found - 3).c_str());
                            }
                        }

                        //Use gathered data
                        new_block.rotateBlock(angle * 180.0 / M_PI);
                        new_block.translate(new_position);

                        m_ships.back().addBlock(new_block);
                        angle = 0;
                        new_position.x = 0;
                        new_position.y = 0;
                    }
                }
                //Block new_block = BM.loadBlockData()
            }
            //New ship
            else if(bracket_count  == 2){
                m_ships.push_back(Ship());
                auto found = line.find("name=");
                auto end_find = line.find('"',found + 6);
                if(found != std::string::npos){
                std::string name = line.substr(found+6,end_find - found - 6);
                m_ships.back().setName(name);
                is_command = true;
            }
            //End of current ship
            else if(bracket_count == -2){

            }
            //End of file, save last block and exit
            else if(bracket_count == -4){
                    break;
                }
            }
        }
    }
    else{
        std::cout << "Could not find fleet to load, Check to ensure proper path and file name. Make sure it is not compressed\n";
    }

    m_ships.back().writeShip("Woop.lua","Woop","Woop");
}

int Fleet::bracketCount(std::string line)
{
    int counter = 0;
    for(int i = 0; i < line.size(); ++i){
        if(line[i] == '{')
            counter++;
        if(line[i] == '}')
            counter--;
    }

    return(counter);
}
