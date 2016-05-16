#include <iostream>
#include <numeric>

#include "ShipBuilder.h"
#include "BlockManager.h"
#include "Ship.h"
#include "Vector2D.h"

#define TRY_LIMIT 10000000

ShipBuilder::ShipBuilder()
{
    if(bm.loadBlockData("data/block_data.csv")){
        std::cout << "Block data loaded \n";
    }
    else{
        std::cout << "Block data failed to load\n";
    }
}

ShipBuilder::~ShipBuilder()
{

}

Ship ShipBuilder::createShip(int target_point_value, int faction, int block_limit, bool is_symmetric)
{
    Ship new_ship(faction);

    if(bm.hasFaction(faction)){
        new_ship.addBlock(bm.getCommandBlock(faction));

        //Implement block limit
        for(int i = 0; i < TRY_LIMIT && new_ship.getTotalValue() <= target_point_value && (int)new_ship.getBlocks().size() <= block_limit; ++i){
            tryNewBlock(new_ship,faction,is_symmetric);
        }
        if(is_symmetric){
            //Remove last two blocks to keep it under target value
            new_ship.getBlocks().pop_back();
            new_ship.getBlocks().pop_back();
        }
        else{
            //Remove only one if totally random non-symmetry
            new_ship.getBlocks().pop_back();
        }
    }
    else{
        std::cout << "Faction not found. Please look at faction number \n";
    }
    std::cout <<"Ship Value= " << new_ship.getTotalValue() << "    Number of Blocks= " << new_ship.getBlocks().size() << "\n";
    return(new_ship);
}

void ShipBuilder::addBlock(Ship& new_ship, int faction, bool is_symmetric)
{
    if(new_ship.getBlocks().size() == 0)
        new_ship.addBlock(bm.getCommandBlock(faction));
    else
        while(!tryNewBlock(new_ship,faction, is_symmetric));
}

bool ShipBuilder::tryNewBlock(Ship& new_ship, int faction, bool is_symmetric)
{
    //New block for fitting
    Block new_block = bm.getBlock(faction);

    //Indexs for the random attachment points
    int ship_block_index;
    int ship_block_attachment_index;
    int new_block_attachment_index;

    //Get random attachments
    Attachment new_block_attachment = new_block.getRandomAttachment(new_block_attachment_index);
    Attachment new_ship_attachment = new_ship.getAttachmentPoint(ship_block_index, ship_block_attachment_index);

    //Basic Symmetric check. Attachment point must be above x axis or equal
    if(is_symmetric && new_ship_attachment.position.y < 0){
        return false;
    }

    //if the new block attachment is a thruster attachment it cannot fit anywhere
    if(!new_block_attachment.is_thruster_attachment){
        //if the ship attachment is a thruster attachment and the new block is a thruster then it will work. If the ship attachment is not a thruster attachment it wont matter
        if(new_ship_attachment.is_thruster_attachment && new_block.hasThrustAttachment() || !new_ship_attachment.is_thruster_attachment){
            //Check to see if no attachments on block. getAttachment returns -1 if no attachments
            if(ship_block_attachment_index != -1){

                //calculate the proper target angle and rotate the new_block
                float target_angle = new_ship_attachment.angle + 180.f;
                //Normalize angle
                while(target_angle < 0.0f) target_angle+= 360.0f;
                while(target_angle > 360.0f) target_angle -= 360.0f;

                new_block.rotateBlock(target_angle - new_block_attachment.angle);

                //attachment position difference
                Vector2D diff(new_ship_attachment.position - new_block.getAttachments()[new_block_attachment_index].position);

                new_block.translate(diff);

                if(is_symmetric){
                    //if the bound is below the line of symmetry then it cannot be used
                    for(auto bound : new_block.getBounds()){
                        if(bound.y < -5.0 )
                            return false;
                    }
                }

                //Passed collision check. Add new block and return
                if(!cc.checkCollision(new_block,new_ship)){
                    //Remove ship attachment point
                    new_ship.getBlocks()[ship_block_index].removeAttachment(ship_block_attachment_index);
                    //remove new_block attachment point
                    new_block.removeAttachment(new_block_attachment_index);

                    new_ship.addBlock(new_block);

                    //Add other block if symmetric
                    if(is_symmetric){
                        //check to make sure the bound is above y = 0.0
                        for(auto bound : new_block.getBounds()){
                            if(bound.y <= 0.0 )
                                return false;
                        }
                        //Symmetrical Block Adding TODO
                    }

                   // new_ship.clearAttachments(); Much faster without. Needs optimization or not TODO
                    return(true);
                }
            }
        }
    }

    return(false);
}
