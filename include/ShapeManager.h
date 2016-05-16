// License: GNU Public License
// Author: Robert Viren
// Date:   2016
// ----------------------------

//ShapeManager manages the loading and getting of shape data. It is closely ties to blocks

#ifndef SHAPEMANAGER_H
#define SHAPEMANAGER_H
#include <map>
#include "Shape.h"


class ShapeManager
{
    public:
        ShapeManager();
        virtual ~ShapeManager();

        bool loadShapeData(std::string file_name);

        //Random Block Getting
        Shape getShape(std::string shape_name);

    private:

        //All defined block information. First block in vector should be command
        //Make sure data file has command block first for each faction data
        std::map<std::string , Shape> m_all_shapes;

};

#endif // SHAPEMANAGER_H
