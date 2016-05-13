#include <iostream>
#include <numeric>

#include "ShipBuilder.h"
#include "BlockManager.h"
#include "Ship.h"

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

Ship ShipBuilder::createShip(int32_t target_point_value, int32_t faction, int32_t block_limit, bool is_symmetric)
{
    Ship new_ship(faction);

    if(bm.hasFaction(faction)){
        new_ship.addBlock(bm.getCommandBlock(faction));

        //Implement block limit
        for(int i = 0; i < TRY_LIMIT && new_ship.getTotalValue() < target_point_value && (int)new_ship.getBlocks().size() < block_limit; ++i){
            tryNewBlock(new_ship,faction,is_symmetric);
        }
    }
    else{
        std::cout << "Faction not found. Please look at faction number \n";
    }

    return(new_ship);
}

void ShipBuilder::addBlock(Ship& new_ship, int32_t faction, bool is_symmetric)
{
    if(new_ship.getBlocks().size() == 0)
        new_ship.addBlock(bm.getCommandBlock(faction));
    else
        tryNewBlock(new_ship,faction, is_symmetric);
}

bool ShipBuilder::tryNewBlock(Ship& new_ship, int32_t faction)
{


    //Attempted attachment points. Start with one point on new block, try to fit on ship
    //After that fails then select new point on new block

    for(int i = 0; i < TRY_LIMIT; ++i)
    {
        //New block for fitting
        Block new_block = bm.getBlock(faction);

        //Indexs for the random attachment points
        int32_t ship_block_index;
        int32_t ship_block_attachment_index;
        int32_t new_block_attachment_index;

        //Get random attachments
        Attachment new_block_attachment = new_block.getRandomAttachment(new_block_attachment_index);
        Attachment new_ship_attachment = new_ship.getAttachmentPoint(ship_block_index, ship_block_attachment_index);

        //if the new block is a thruster attachment it cannot fit anywhere
        if(!new_block_attachment.is_thruster_attachment){
            //Check to make sure if the ship attachment point is true, that the new block is a thruster. This should work....
            if(new_ship_attachment.is_thruster_attachment && new_block.hasThrustAttachment() || !new_ship_attachment.is_thruster_attachment){
                //Check to see if no attachments on block
                if(ship_block_attachment_index != -1){
                    //calculate the proper target angle and rotate the new_block
                    float target_angle = new_ship_attachment.angle + 180.f;
                    //Normalize angle
                    while(target_angle < 0.0f) target_angle+= 360.0f;
                    while(target_angle > 360.0f) target_angle -= 360.0f;

                    new_block.rotateBlock(target_angle - new_block_attachment.angle);

                    //attachment position difference
                    sf::Vector2f diff(new_ship_attachment.position - new_block.getAttachments()[new_block_attachment_index].position);

                    new_block.translate(diff);

                    //Passed collision check. Add new block and return
                    if(!cc.checkCollision(new_block,new_ship)){
                        //Remove ship attachment point
                        new_ship.getBlocks()[ship_block_index].removeAttachment(ship_block_attachment_index);
                        //remove new_block attachment point
                        new_block.removeAttachment(new_block_attachment_index);

                        new_ship.addBlock(new_block);
                       // new_ship.clearAttachments(); Much faster without. Needs optimization or not TODO
                        return(true);
                    }
                }
            }
        }
    }

    //Could not add more blocks within limit
    return(false);
}

bool ShipBuilder::tryNewBlock(Ship& new_ship, int32_t faction, bool is_symmetric)
{

    if(!is_symmetric)
        return(tryNewBlock(new_ship, faction));

    //Attempted attachment points. Start with one point on new block, try to fit on ship
    //After that fails then select new point on new block
    for(int i = 0; i < TRY_LIMIT; ++i)
    {
        //New block for fitting
        Block new_block = bm.getBlock(faction);

        //Indexs for the random attachment points
        int32_t ship_block_index;
        int32_t ship_block_attachment_index;
        int32_t new_block_attachment_index;

        //Get random attachments
        Attachment new_block_attachment = new_block.getRandomAttachment(new_block_attachment_index);
        Attachment new_ship_attachment = new_ship.getAttachmentPoint(ship_block_index, ship_block_attachment_index);

        //Make Symmetric
        if(is_symmetric && new_ship_attachment.position.y >= 0){

                bool bounds_above_y = true;
                for(auto bound : new_ship.getBlocks()[ship_block_index].getBounds()){
                    if(bound.y < -5.0f) //Below the axis
                        bounds_above_y = false;
                }
            if(bounds_above_y){
                //Check to see if no attachments on block
                if(ship_block_attachment_index != -1){
                    //calculate the proper target angle and rotate the new_block
                    float target_angle = new_ship_attachment.angle + 180.f;
                    //Normalize angle
                    while(target_angle < 0.0f) target_angle+= 360.0f;
                    while(target_angle > 360.0f) target_angle -= 360.0f;

                    new_block.rotateBlock(target_angle - new_block_attachment.angle);

                    //attachment position difference
                    sf::Vector2f diff(new_ship_attachment.position - new_block.getAttachments()[new_block_attachment_index].position);

                    new_block.translate(diff);

                    //Passed collision check. Add new block and return
                    if(!cc.checkCollision(new_block,new_ship)){
                        //Remove ship attachment point
                        new_ship.getBlocks()[ship_block_index].removeAttachment(ship_block_attachment_index);
                        //remove new_block attachment point
                        new_block.removeAttachment(new_block_attachment_index);

                        new_ship.addBlock(new_block);
                        //Stupid floating points. Would be zero, but hey, floating point
                        if(new_block.getPosition().y  > 5.0){
                            //Add symmetry block
                            new_block.mirror();
                            new_ship.addBlock(new_block);
                        }

                       // new_ship.clearAttachments(); Much faster without. Needs optimization or not TODO
                        return(true);
                    }
                }
            }
        }
    }

    //Could not add more blocks within limit
    return(false);
}

