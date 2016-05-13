#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <ctime>

#include "ShapeManager.h"
#include "Shape.h"

ShapeManager::ShapeManager()
{
    srand(time(NULL));
}

ShapeManager::~ShapeManager()
{

}

//Load the block data into the all block vector
bool ShapeManager::loadShapeData(std::string file_name)
{
    std::ifstream data;

    try{
        data.open(file_name);
    }
    catch(int e){
        std::cout << "Failed to parse shape data \n";
        return(false);
    }

    std::string line;

    //Skip first 7 example lines
    for(int i = 0; i < 7; ++i)
        std::getline(data,line);

    //Parse data file
    while(std::getline(data,line,','))
    {

        //Temp vectors for bound and attachment loading
        std::vector<float> x_bounds,y_bounds,x_attachments,y_attachments,angle_attachments;
        std::vector<int> thruster_attachment;
        //Block name. Mainly used to determine if thruster
        std::string shape_name = line;

        //Skip rest of line
        std::getline(data,line);


        for(int i = 0; i < 6; ++ i)
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
            new_attachment.is_thruster_attachment = thruster_attachment[i];

            attachments.push_back(new_attachment);
        }

        //If new faction, create new spot in map
        if(m_all_shapes.find(shape_name) == m_all_shapes.end()){
            m_all_shapes[shape_name];
        }

        //add to map
        m_all_shapes[shape_name] =  Shape(sf::Vector2f(0.0,0.0),bounds,attachments);
    }

    //return Success
    return(true);
}

Shape ShapeManager::getShape(std::string shape_name)
{
    if(m_all_shapes.find(shape_name) == m_all_shapes.end()){
        std::cout << "Shape not found \n";
    }
    else{
        return(m_all_shapes[shape_name]);
    }

    return(Shape());
}
