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
#include <mutex>

#include "ShipBuilder.h"
#include "FleetMutator.h"

class Display
{
    public:
        Display();
        virtual ~Display();

        void run();

		typedef struct {
			int mode;
			int faction;
			int symmetry;
			int mutation_blocks;
			int fleet_p_value;
			int fleet_single_ship_p_value_max;
			int fleet_single_ship_p_value_min;
			int fleet_population_size;
			int target_number_of_wins;
			int block_count_limit;

			//Single ship
			int p_value_target;

			std::string target_fleet_file;
			std::string target_fleet_name;
			std::string seed_file_name;


		} Config;

    private:
        //Display the ship selected
        void displayShip(Ship& display_ship, float scale, sf::Vector2f position);
        void displayShip(Ship* display_ship, float scale, sf::Vector2f position);

        //View Moving code
        void moveView();

        //Event handler
        void eventHandle();

        //Setup Gui Enviroment
		void initGUI();
        void createGui();

        void createNewShip();

        void addBlock(){int is_symm  = m_config.symmetry; if(m_config.symmetry == 2) is_symm = rand() %2 ;SB.addBlock(ships.back(),m_config.faction,is_symm);}

        void addThrust(){int is_symm  = m_config.symmetry; if(m_config.symmetry == 2) is_symm = rand() %2 ;SB.addThrust(ships.back(), m_config.faction,is_symm);}

        void saveShip(){ships.back().writeShip("ships/test.lua","Debug","Debug"); std::cout << "Ship Saved \n";}

        void debugBlock(){ships.back().getBlocks().back().debugBlock();}

        void symmSelect(){if (m_symm_1->IsActive()){ m_config.symmetry = 0;}else if(m_symm_2->IsActive()){ m_config.symmetry = 1;}else if(m_symm_3->IsActive()){ m_config.symmetry = 2;}}

		void startPool();

		void startPoolThreaded();

        void breedLastTwo(){if(ships.size() >= 2) ships.push_back(SB.breedShips(ships.back(),ships[ships.size()-2], m_config.symmetry, m_config.faction,ships.back().getTotalValue()));}

        //Gui Object
        sfg::SFGUI m_sfgui;

        //SFGui varibles
        sfg::Desktop m_desktop;
        sfg::Window::Ptr m_window;
		sfg::Window::Ptr m_program_window;

		//ComboBox
		sfg::ComboBox::Ptr m_program_mode;

        //Entry boxes
        sfg::Entry::Ptr m_point_entry;
        sfg::Entry::Ptr m_block_entry;
        sfg::Entry::Ptr m_thrust_entry;
		sfg::Entry::Ptr m_fleet_p_value;
		sfg::Entry::Ptr fleet_single_ship_p_value_max;
		sfg::Entry::Ptr fleet_single_ship_p_value_min;
		sfg::Entry::Ptr target_fleet_file;
		sfg::Entry::Ptr target_fleet_name;
		sfg::Entry::Ptr fleet_population_size;
		sfg::Entry::Ptr target_number_of_wins;
		sfg::Entry::Ptr seed_file_name;
		sfg::Entry::Ptr faction;
		sfg::Entry::Ptr mutation_blocks;
		sfg::Entry::Ptr block_count_limit;

        //Symm radio buttons
        sfg::RadioButton::Ptr m_symm_1;
        sfg::RadioButton::Ptr m_symm_2;
        sfg::RadioButton::Ptr m_symm_3;

        //SFML varibles
        sf::RenderWindow m_renderWindow;
        sf::Clock m_clock;
        sf::View m_view;
        sf::Event m_event;
        float m_zoom = 1.f;

        Ship debug_ship;

        //Attachment point circle
        sf::CircleShape attachment_point;

        std::vector<Ship> ships;

		//All config data
		Config m_config;

		//Class data
        ShipBuilder SB;
		SettingsParser SP;
		FleetMutator FM;

		//Thread Data
		std::vector<int> m_delays;
		std::vector<int> m_wins;

};

#endif // DISPLAY_H