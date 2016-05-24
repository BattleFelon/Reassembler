//Borrowed from terathon.com

#ifndef FLEET_H
#define FLEET_H

#include "Ship.h"

class Fleet
{
    public:
        float   x;
        float   y;

        Fleet();
        virtual ~Fleet();

        void writeFleet(std::string file_name,std::string author_name, std::string faction_name);
        void readFleet(std::string file_name);

        int numberOfShips(){return m_ships.size();}
        int totalValue(){int value = 0; for(auto ship : m_ships) value += ship.getTotalValue(); return value;}

        void setFaction(int faction){m_faction = faction;}
        int getFaction(){return m_faction;}

        void addShip(Ship new_ship){m_ships.push_back(new_ship);}
        void removeShip(int index){if(index < m_ships.size()) m_ships.erase(m_ships.begin() + index);}

        Ship getShip(int index){if(index < m_ships.size()) return(m_ships[index]);}
    private:

        int bracketCount(std::string line);

        int m_faction;
        std::string m_fleet_name;
        std::vector<Ship> m_ships;

};

#endif // FLEET_H
