#include <fstream>
#include <iostream>
#include <iomanip>
#include "Ship.h"

Ship::Ship(int faction):
    m_faction(faction)
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

void Ship::clearAttachments()
{
    //Remove attachment that share the same position
    for(int i = 0; i < (int)m_blocks.size(); ++i){
        for(int j = 0; j < (int)m_blocks.size(); ++j){
            if(i != j){ // Not the same block
                //See if they are even close
                float distance =
                std::sqrt(std::pow(m_blocks[i].getPosition().x - m_blocks[j].getPosition().x,2.0)+
                          std::pow(m_blocks[i].getPosition().y - m_blocks[j].getPosition().y,2.0f));
                if(distance <= m_blocks[i].getRadius() + m_blocks[j].getRadius() + 5.0) {// 5 just to be conservative
                    //This means they are near each other
                    for(int w = 0; w < (int)m_blocks[i].getAttachments().size(); ++w){
                        for(int k = 0; k < (int)m_blocks[j].getAttachments().size(); ++k){
                            if(m_blocks[i].getAttachments()[w].position == m_blocks[j].getAttachments()[k].position){
                                m_blocks[i].removeAttachment(w);
                                m_blocks[j].removeAttachment(k);
                            }
                        }
                    }
                }
            }
        }
    }
}

//Reassembly corrects for the command block not actually being the center after it loads the file
//TODO - center of gravity finding and proper file writing.
void Ship::writeShip(std::string file_name,std::string ship_name,std::string author_name)
{
    std::ofstream file;
    file.open(file_name);

    if(file.good()){
        //Set to 3 decimal place precision
        file.precision(3);
        file.setf(std::ios::fixed);
        //File header
        file << "{data={name=\""<<ship_name<<"\", author=\""<<author_name<<"\", color0=0xc96f20, color1=0x796bff,\n";

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
    }
    else{
        std::cout << "Could not write file for some reason\n";
    }
    file.close();
}

Ship Ship::readShip()
{
 //TODO
}
