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

#include <thread>

#include "ShipBuilder.h"
#include "Mutator.h"

class Display
{
    public:
        Display();
        virtual ~Display();

        void run();

    private:
        //Display the ship selected
        void displayShip(Ship& display_ship, float scale, sf::Vector2f position);
        void displayShip(Ship* display_ship, float scale, sf::Vector2f position);

        //View Moving code
        void moveView();

        //Event handler
        void eventHandle();

        //Setup Gui Enviroment
        void createGui();

        void createNewShip();

        void addBlock(){int is_symm  = m_ship_symmetry; if(m_ship_symmetry == 2) is_symm = rand() %2 ;SB.addBlock(ships.back(),8,is_symm);}

        void addThrust(){int is_symm  = m_ship_symmetry; if(m_ship_symmetry == 2) is_symm = rand() %2 ;SB.addThrust(ships.back(),8,is_symm);}

        void saveShip(){ships.back().writeShip("ships/test.lua","Debug","Debug"); std::cout << "Ship Saved \n";}

        void debugBlock(){ships.back().getBlocks().back().debugBlock();}

        void symmSelect(){if (m_symm_1->IsActive()){m_ship_symmetry = 0;}else if(m_symm_2->IsActive()){m_ship_symmetry = 1;}else if(m_symm_3->IsActive()){m_ship_symmetry = 2;}}

        void startPool(){}//std::thread t1 (&Mutator::poolMutator,&mutate,10); t1.detach();

        void breedLastTwo(){if(ships.size() >= 2) ships.push_back(SB.breedShips(ships.back(),ships[ships.size()-2],m_ship_symmetry,5));}

        //Mutator object
        //Mutator mutate;

        //Gui Object
        sfg::SFGUI m_sfgui;

        //SFGui varibles
        sfg::Desktop m_desktop;
        sfg::Window::Ptr m_window;

        //Entry boxes
        sfg::Entry::Ptr m_point_entry;
        sfg::Entry::Ptr m_block_entry;
        sfg::Entry::Ptr m_thrust_entry;

        //Symm radio buttons
        sfg::RadioButton::Ptr m_symm_1;
        sfg::RadioButton::Ptr m_symm_2;
        sfg::RadioButton::Ptr m_symm_3;

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

        int m_p_value_target;
        int m_faction;
        int m_block_count_limit;
        int m_ship_symmetry;


        ShipBuilder SB;

};

#endif // DISPLAY_H
