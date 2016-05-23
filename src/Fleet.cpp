#include <fstream>
#include <iostream>
#include <iomanip>

#include "Ship.h"
#include "Fleet.h"

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
    std::string line;
    std::ifstream input (file_name);
    if(input.is_open()){
        for(int i = 0; i < 3; ++i)
            std::getline(input,line); //Skip first three lines

        std::getline(input,line); //Line contains faction number
        //Search for faction number
        for(int i = 20; i > 0; i--){
            if(line.find(std::to_string(i)) != std::string::npos){
                m_faction = i;
                i = 0;
            }
        }

    }
    else{
        std::cout << "Could not find fleet to load, Check to ensure proper path and file name. Make sure it is not compressed\n";
    }
}
