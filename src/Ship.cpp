#include <fstream>
#include <iostream>
#include <iomanip>
#include "Ship.h"

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

Ship Ship::readShip()
{
	Ship new_ship;
	return(new_ship);
}
