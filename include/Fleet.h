
#ifndef FLEET_H
#define FLEET_H

#include "Ship.h"


class Fleet
{
    public:
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
        int getFleetSize(){return m_ships.size();}

        Ship getShip(int index){if(index < m_ships.size()) return(m_ships[index]);}
        int getShipValue(int index){if (index < m_ships.size()) return(m_ships[index].getTotalValue());}

        void setName(std::string name){m_fleet_name = name;}
        std::string getName(){return(m_fleet_name);}

		int getScore(){ return(m_score);}
		void setScore(int new_score) { m_score = new_score; }

		int getWins(){ return m_wins; }
		void addWin(){ m_wins++; }

    private:

        int bracketCount(std::string line);

        int m_faction;
        std::string m_fleet_name;
        std::vector<Ship> m_ships;

		//Genetic fleet data
		int m_score;
		int m_wins;



};

#endif // FLEET_H
