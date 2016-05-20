#include <iostream>
#include <numeric>

#include "ShipBuilder.h"
#include "BlockManager.h"
#include "Ship.h"
#include "Vector2D.h"

#define TRY_LIMIT 1000
#define M_PI 3.14159265359

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

Ship ShipBuilder::createShip(int target_point_value, int faction, int block_limit,int target_thruster_points, int is_symmetric, std::string ship_name)
{
    //Cast to random bool
    int is_symm = is_symmetric;
    if(is_symmetric == 2)
        is_symm = rand() % 2;

    //Init new ship
    Ship new_ship;
    new_ship.setFaction(faction);
    new_ship.setName(ship_name);

    //Check to ensure it can be found in the faction database
    if(bm.hasFaction(faction)){
        new_ship.addBlock(bm.getCommandBlock(faction));
        //Loop for main building
        for(int i = 0; i < TRY_LIMIT && new_ship.getTotalValue() <= (target_point_value - target_thruster_points) && (int)new_ship.getBlocks().size() <= block_limit; ++i){
            forceFitNewBlock(new_ship,faction,is_symm,0);
        }
        //Loop for thruster building
        for(int i = 0; i < TRY_LIMIT && new_ship.getTotalValue() <= target_point_value && (int)new_ship.getBlocks().size() <= block_limit; ++i){
            forceFitNewBlock(new_ship,faction,is_symm,1);
        }
        if(is_symm){
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

    //std::cout <<"Ship Value= " << new_ship.getTotalValue() << "    Number of Blocks= " << new_ship.getBlocks().size() << "\n";

    return(new_ship);
}

void ShipBuilder::addBlock(Ship& new_ship, int faction, int is_symmetric)
{

    //Cast to random bool
    int is_symm = is_symmetric;
    if(is_symmetric == 2)
        is_symm = rand() % 2;

    if(new_ship.getBlocks().size() == 0)
        new_ship.addBlock(bm.getCommandBlock(faction));
    else
        while(!tryNewBlock(new_ship,faction, false, is_symm));
}

void ShipBuilder::addThrust(Ship& new_ship, int faction, int is_symmetric)
{
    //Cast to random bool
    int is_symm = is_symmetric;
    if(is_symmetric == 2)
        is_symm = rand() % 2;

     if(new_ship.getBlocks().size() == 0)
        new_ship.addBlock(bm.getCommandBlock(faction));
    else
        while(!tryNewBlock(new_ship,faction, true , is_symm));
        //tryNewBlock(new_ship,faction, true , is_symmetric);
}

bool ShipBuilder::tryNewBlock(Ship& new_ship, int faction,bool is_thrust, bool is_symmetric)
{
    //New block for fitting
    Block new_block = bm.getBlock(faction);
    if(is_thrust)
        new_block = bm.getThruster(faction);

    //Indexs for the random attachment points
    int ship_block_index;
    int ship_block_attachment_index;
    int new_block_attachment_index;

    //Assume symmetry works
    bool symm_works = true;

    //Get random attachments
    Attachment new_block_attachment = new_block.getRandomAttachment(new_block_attachment_index);
    Attachment new_ship_attachment = new_ship.getAttachmentPoint(ship_block_index, ship_block_attachment_index);

    //std::cout << ship_block_index << " " << ship_block_attachment_index << " " << new_block_attachment_index << " " << new_block.getAttachments().size() << "\n";

    //Symmetric branch, make sure above 0
    if(is_symmetric && new_ship_attachment.position.y > 0.0f){

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

                    //If I don't have this logic no blocks will ever be in the center
                    if(is_symmetric){
                        //if the bound is below the line of symmetry then it cannot be used
                        for(auto bound : new_block.getBounds()){
                            if(bound.y < -5.0f )
                                symm_works = false;

                            //Check for a rare case where it crosses zero, but is also above 5. Leads to disconnected blocks
                            if(bound.y < 0.0f){
                                for(auto bound_2 : new_block.getBounds()){
                                    if(bound_2.y > 5.0f)
                                        symm_works = false;
                                }
                            }

                        }
                    }

                    //Check Symm and Collision if symm is good
                    if(symm_works && !cc.checkCollision(new_block,new_ship)){

                        //Remove ship attachment point
                        new_ship.getBlocks()[ship_block_index].removeAttachment(ship_block_attachment_index);
                        //remove new_block attachment point
                        new_block.removeAttachment(new_block_attachment_index);
                        //Add that shit
                        new_ship.addBlock(new_block);

                        //Add other block if non-center crossing bound
                        if(is_symmetric && symm_works){
                            //check to make sure the bound is above y = 0.0
                            for(auto bound : new_block.getBounds()){
                                if(bound.y <= 0.0 )
                                    symm_works = false;
                            }

                            if(symm_works){
                                //Capture and set target position
                                Vector2D mirror_position = new_block.getPosition();
                                mirror_position.y *= -1;

                                //Get angle and convert to deg
                                float mirror_angle = new_block.getRotation();
                                mirror_angle *= 180.0f / M_PI;

                                //Normalize angle
                                while(mirror_angle < 0.0f) mirror_angle+= 360.0f;
                                while(mirror_angle >= 360.0f) mirror_angle -= 360.0f;

                                //Set the target angle to its negative mirror version
                                mirror_angle *= -1;

                                //Get the mirror block already set to zero position and rotation
                                Block mirror_block = bm.getBlock(faction, new_block.getMirror());

                                //Special case section. Non-symmetrical blocks that have no mirror block num. They are themselves but rotated.
                                if(mirror_block.getBlockNum() == 802)
                                    mirror_angle -= 90;

                                mirror_block.rotateBlock(mirror_angle);
                                mirror_block.translate(mirror_position);

                                new_ship.addBlock(mirror_block);

                            }
                        }
                        return(true);
                    }
                    //Get new default block with no rotation or translation
                    new_block =  Block(bm.getBlock(faction,new_block.getBlockNum()));
                }
            }
        }
    }
    //Non-symmetric branch
    else if (!is_symmetric){

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

                    //Passed collision check. Add new block and return
                    if(!cc.checkCollision(new_block,new_ship)){
                        //Remove ship attachment point
                        new_ship.getBlocks()[ship_block_index].removeAttachment(ship_block_attachment_index);
                        //remove new_block attachment point
                        new_block.removeAttachment(new_block_attachment_index);
                        //Add that shit
                        new_ship.addBlock(new_block);

                        return(true);
                    }
                    new_block =  Block(bm.getBlock(faction,new_block.getBlockNum()));
                }
            }
        }
    }

    return(false);
}

bool ShipBuilder::forceFitNewBlock(Ship& new_ship, int faction, bool is_symmetric,bool is_thruster)
{

    //New block for fitting
    Block new_block = bm.getBlock(faction);
    if(is_thruster)
        new_block = bm.getThruster(faction);

    //std::cout << "\n NEW BLOCK # " << new_block.getBlockNum() << "\n";

    int i = 0;
    while(i < TRY_LIMIT){
        i++;

        //Indexs for the random attachment points
        int ship_block_index;
        int ship_block_attachment_index;
        int new_block_attachment_index;

        //Assume symmetry works
        bool symm_works = true;

        //Get random attachments
        Attachment new_block_attachment = new_block.getRandomAttachment(new_block_attachment_index);
        Attachment new_ship_attachment = new_ship.getAttachmentPoint(ship_block_index, ship_block_attachment_index);

        //std::cout << ship_block_index << " " << ship_block_attachment_index << " " << new_block_attachment_index << " " << new_block.getAttachments().size() << "\n";

        //Symmetric branch, make sure above 0
        if(is_symmetric && new_ship_attachment.position.y > 0.0f){

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

                        //If I don't have this logic no blocks will ever be in the center
                        if(is_symmetric){
                            //if the bound is below the line of symmetry then it cannot be used
                            for(auto bound : new_block.getBounds()){
                                if(bound.y < -5.0f )
                                    symm_works = false;

                                //Check for a rare case where it crosses zero, but is also above 5. Leads to disconnected blocks
                                if(bound.y < -0.5f){
                                    for(auto bound_2 : new_block.getBounds()){
                                        if(bound_2.y > 5.5f)
                                            symm_works = false;
                                    }
                                }

                            }
                        }

                        //Check Symm and Collision if symm is good
                        if(symm_works && !cc.checkCollision(new_block,new_ship)){

                            //Remove ship attachment point
                            new_ship.getBlocks()[ship_block_index].removeAttachment(ship_block_attachment_index);
                            //remove new_block attachment point
                            new_block.removeAttachment(new_block_attachment_index);
                            //Add that shit
                            new_ship.addBlock(new_block);

                            //Add other block if non-center crossing bound
                            if(is_symmetric && symm_works){
                                //check to make sure the bound is above y = 0.0
                                for(auto bound : new_block.getBounds()){
                                    if(bound.y <= -0.5 )
                                        symm_works = false;
                                }

                                if(symm_works){
                                    //Capture and set target position
                                    Vector2D mirror_position = new_block.getPosition();
                                    mirror_position.y *= -1;

                                    //Get angle and convert to deg
                                    float mirror_angle = new_block.getRotation();
                                    mirror_angle *= 180.0f / M_PI;

                                    //Normalize angle
                                    while(mirror_angle < 0.0f) mirror_angle+= 360.0f;
                                    while(mirror_angle >= 360.0f) mirror_angle -= 360.0f;

                                    //Set the target angle to its negative mirror version
                                    mirror_angle *= -1;

                                    //Get the mirror block already set to zero position and rotation
                                    Block mirror_block = bm.getBlock(faction, new_block.getMirror());

                                    //Special case section. Non-symmetrical blocks that have no mirror block num. They are themselves but rotated.
                                    if(mirror_block.getBlockNum() == 802)
                                        mirror_angle -= 90;

                                    mirror_block.rotateBlock(mirror_angle);
                                    mirror_block.translate(mirror_position);

                                    new_ship.addBlock(mirror_block);

                                }
                            }
                            return(true);
                        }
                        //Get new default block with no rotation or translation
                        new_block =  Block(bm.getBlock(faction,new_block.getBlockNum()));
                    }
                }
            }
        }
        //Non-symmetric branch
        else if (!is_symmetric){

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

                        //Passed collision check. Add new block and return
                        if(!cc.checkCollision(new_block,new_ship)){
                            //Remove ship attachment point
                            new_ship.getBlocks()[ship_block_index].removeAttachment(ship_block_attachment_index);
                            //remove new_block attachment point
                            new_block.removeAttachment(new_block_attachment_index);
                            //Add that shit
                            new_ship.addBlock(new_block);

                            return(true);
                        }
                        new_block =  Block(bm.getBlock(faction,new_block.getBlockNum()));
                    }
                }
            }
        }
    }//While

    return(false);
}
