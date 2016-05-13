// License: GNU Public License
// Author: Robert Viren
// Date:   2016
// ----------------------------

//SFML visual display of ships being built. Not needed for program, but extremely useful during debug phase
// Borders are lines, attachments are dots. TODO -- More GUI and colors!!!

#ifndef DISPLAY_H
#define DISPLAY_H

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <SFML/Graphics.hpp>

#include "ShipBuilder.h"

class Display
{
    public:
        Display();
        virtual ~Display();

        void run();

    private:
        //Display the ship selected
        void displayShip(Ship& display_ship, float scale, sf::Vector2f position);

        //View Moving code
        void moveView();

        //Event handler
        void eventHandle();

        //Setup Gui Enviroment
        void createGui();

        void createNewShip(){ships.push_back(Ship(8));}
        void addBlock(){SB.addBlock(ships.back(),8,false);}
        void saveShip(){ships.back().writeShip("8_Ship.lua","RandomShip","Me");}
        void debugBlock(){ships.back().getBlocks().back().debugBlock();}

        //Gui Object
        sfg::SFGUI m_sfgui;

        //SFGui varibles
        sfg::Desktop m_desktop;
        sfg::Window::Ptr m_window;

        //SFML varibles
        sf::RenderWindow m_renderWindow;
        sf::Clock m_clock;
        sf::View m_view;
        sf::Event m_event;
        float m_zoom = 1.0f;

        Ship debug_ship;

        //Attachment point circle
        sf::CircleShape attachment_point;

        std::vector<Ship> ships;

        ShipBuilder SB;
};

#endif // DISPLAY_H
