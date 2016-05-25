#ifndef FLEETBUILDER_H
#define FLEETBUILDER_H

#include "Fleet.h"
#include "ShipBuilder.h"

class FleetBuilder
{
    public:
        FleetBuilder();
        virtual ~FleetBuilder();

        Fleet createFleet(int target_p_value, int max_ship_value, int min_ship_value, int block_limit, int faction, int ship_symm);

    private:

         //Ship building
        ShipBuilder SB;
};

#endif // FLEETBUILDER_H
