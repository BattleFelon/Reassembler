#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Display.h"
#include "Vector2D.h"
#include "SettingsParser.h"

Display::Display()
{

    //Set Window and View varibles
	m_renderWindow.create(sf::VideoMode(800, 600), "Reassembler");
	m_view = m_renderWindow.getDefaultView();
	m_renderWindow.setFramerateLimit(60);

	
	SP.loadFromFile("config.txt");

    SP.get("p_value_target",m_config.p_value_target);
    SP.get("faction",m_config.faction);
    SP.get("block_count_limit",m_config.block_count_limit);
    SP.get("ship_symmetry",m_config.symmetry);
	SP.get("mutation_blocks", m_config.mutation_blocks);
	SP.get("fleet_p_value", m_config.fleet_p_value);
	SP.get("fleet_single_ship_p_value_max", m_config.fleet_single_ship_p_value_max);
	SP.get("fleet_single_ship_p_value_min", m_config.fleet_single_ship_p_value_min);
	SP.get("fleet_population_size", m_config.fleet_population_size);
	SP.get("target_number_of_wins", m_config.target_number_of_wins);
	SP.get("block_count_limit", m_config.block_count_limit);
	SP.get("target_fleet_file", m_config.target_fleet_file);
	SP.get("target_fleet_name", m_config.target_fleet_name);
	SP.get("seed_file_name", m_config.seed_file_name);

    //Cast to bool
    ships.push_back(SB.createShip(m_config.p_value_target, m_config.faction, m_config.block_count_limit,0, m_config.symmetry,"Debug"));
	initGUI();

	attachment_point.setOrigin(5,5);

	//Move zoom
	m_view.zoom(3.f);

}

Display::~Display()
{

}

void Display::run()
{
	sf::CircleShape delay(25);
	sf::CircleShape wins(25);
	delay.setFillColor(sf::Color::Blue);
	wins.setFillColor(sf::Color::Green);

	while (m_renderWindow.isOpen())
	{
		moveView();
		eventHandle();

		//Update GUI
        m_desktop.Update( 0.f );

		//Set view
		m_renderWindow.setView(m_view);

		//Clear previous frame
		m_renderWindow.clear();

        //draw the sip to screen
        displayShip(ships.back(),10, sf::Vector2f(400,300));
        
		if (FM.isThreadActive()) {
			if (m_wins.size() == 0) {
				m_wins.push_back(FM.getBestWins());
				m_delays.push_back(FM.getBestDelay());
			}
			else {
				if (m_wins.back() != FM.getBestWins() || m_delays.back() != FM.getBestDelay()) {
					m_wins.push_back(FM.getBestWins());
					m_delays.push_back(FM.getBestDelay());
				}
			}

			for (int i = 0; i < m_wins.size(); ++i) {
				wins.setPosition(sf::Vector2f(i * 100 + 1200, -m_wins[i] * 100));
				delay.setPosition(sf::Vector2f(i * 100 + 1250, -m_delays[i]/50));

				m_renderWindow.draw(wins);
				m_renderWindow.draw(delay);
			}
		}


		//Show render windows
		m_sfgui.Display(m_renderWindow);
		m_renderWindow.display();
	}

	std::cout << "Program Terminated \n";
}

void Display::displayShip(Ship& display_ship, float scale, sf::Vector2f position)
{
    if(display_ship.getBlocks().size() > 0){
        for(auto block : display_ship.getBlocks()){

            sf::VertexArray draw_block(sf::LinesStrip, block.getBounds().size() + 1);

            //Draw the bounds
            int i = 0;
            for(auto bound : block.getBounds()){
                draw_block[i].position = sf::Vector2f(bound.x * scale + position.x,bound.y * -scale + position.y);
                i++;
            }
            //Complete the shape
            draw_block[block.getBounds().size()].position = sf::Vector2f(block.getBounds()[0].x * scale + position.x,block.getBounds()[0].y * -scale + position.y);

            //Draw it
            m_renderWindow.draw(draw_block);

            //Draw the attachments
            for(auto attachment : block.getAttachments()){
                attachment_point.setPosition(sf::Vector2f(attachment.position.x * scale + position.x, attachment.position.y * -scale + position.y));
                m_renderWindow.draw(attachment_point);
            }
        }
    }
}

void Display::displayShip(Ship* display_ship, float scale, sf::Vector2f position)
{
    if(display_ship->getBlocks().size() > 0){
        for(auto block : display_ship->getBlocks()){

            sf::VertexArray draw_block(sf::LinesStrip, block.getBounds().size() + 1);

            //Draw the bounds
            int i = 0;
            for(auto bound : block.getBounds()){
                draw_block[i].position = sf::Vector2f(bound.x * scale + position.x,bound.y * -scale + position.y);
                i++;
            }
            //Complete the shape
            draw_block[block.getBounds().size()].position = sf::Vector2f(block.getBounds()[0].x * scale + position.x,block.getBounds()[0].y * -scale + position.y);

            //Draw it
            m_renderWindow.draw(draw_block);

            //Draw the attachments
            for(auto attachment : block.getAttachments()){
                attachment_point.setPosition(sf::Vector2f(attachment.position.x * scale + position.x, attachment.position.y * -scale + position.y));
                m_renderWindow.draw(attachment_point);
            }
        }
    }
}

void Display::moveView()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		m_view.move(sf::Vector2f(0.f, -20.0f*m_zoom));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		m_view.move(sf::Vector2f(0.f, 20.0f * m_zoom));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		m_view.move(sf::Vector2f(-20.f * m_zoom, 0.f));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		m_view.move(sf::Vector2f(20.f * m_zoom, 0.f));
	}
}

void Display::eventHandle()
{
	while (m_renderWindow.pollEvent(m_event))
	{
		//Gui Event Handler
		m_desktop.HandleEvent( m_event );

		// "close requested" event: we close the m_renderWindow
		if (m_event.type == sf::Event::Closed)
			m_renderWindow.close();

		// catch the resize events
		if (m_event.type == sf::Event::Resized)
		{
			// update the view to the new size of the window
            m_zoom = 1.f;
            sf::FloatRect visibleArea(0, 0, (float)m_event.size.width, (float)m_event.size.height);
            m_renderWindow.setView(sf::View(visibleArea));
            m_view = m_renderWindow.getView();
		}

		//Wheel Moving Listener
		if (m_event.type == sf::Event::MouseWheelMoved)
		{

			if (m_event.mouseWheel.delta > 0)
			{
				m_zoom *= .90f;
				m_view.zoom(.9f);
			}
			else if (m_event.mouseWheel.delta < 0)
			{
				m_zoom /= .90f;
				m_view.zoom(1.f / .9f);
			}
		}
	}
}

void Display::initGUI()
{
	m_renderWindow.resetGLStates();

	//General debug buttons
	m_desktop.SetProperty("Button#Button", "FontSize", 18.f);

	m_program_window = sfg::Window::Create();
	m_program_window->SetTitle("Program Mode");

	m_program_mode = sfg::ComboBox::Create();
	m_program_mode->AppendItem("Genetic Fleet Beater");
	m_program_mode->AppendItem("Threaded Fleet Beater");
	m_program_mode->AppendItem("Debug Mode");
	m_program_mode->GetSignal(sfg::ComboBox::OnSelect).Connect(std::bind(&Display::createGui, this));


	//Box
	auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL,1.f);
	auto label = sfg::Label::Create("Program Mode");
	box->Pack(label);
	box->Pack(m_program_mode);
	m_program_window->Add(box);
	m_desktop.Add(m_program_window);
	
}

void Display::createGui()
{
	
	//Create Settings Window
	m_window = sfg::Window::Create();
	m_window->SetTitle("Reassembler");
	int mode = m_program_mode->GetSelectedItem();

	if (mode == 0) {

		//Widgets
		auto start_button = sfg::Button::Create("Start");
		m_fleet_p_value = sfg::Entry::Create(std::to_string(m_config.fleet_p_value));
		fleet_single_ship_p_value_max = sfg::Entry::Create(std::to_string(m_config.fleet_single_ship_p_value_max));
		fleet_single_ship_p_value_min = sfg::Entry::Create(std::to_string(m_config.fleet_single_ship_p_value_min));
		target_fleet_file = sfg::Entry::Create(m_config.target_fleet_file);
		target_fleet_name = sfg::Entry::Create(m_config.target_fleet_name);
		seed_file_name = sfg::Entry::Create(m_config.seed_file_name);
		fleet_population_size = sfg::Entry::Create(std::to_string(m_config.fleet_population_size));
		target_number_of_wins = sfg::Entry::Create(std::to_string(m_config.target_number_of_wins));
		faction = sfg::Entry::Create(std::to_string(m_config.faction));
		mutation_blocks = sfg::Entry::Create(std::to_string(m_config.mutation_blocks));
		block_count_limit = sfg::Entry::Create(std::to_string(m_config.block_count_limit));

		//Labels
		auto faction_label = sfg::Label::Create("Faction");
		auto m_fleet_p_value_label = sfg::Label::Create("Fleet P Value");
		auto fleet_single_ship_p_value_max_label = sfg::Label::Create("Max Ship Value");
		auto fleet_single_ship_p_value_min_label = sfg::Label::Create("Min Ship Value");
		auto fleet_population_size_label = sfg::Label::Create("Population Size");
		auto target_number_of_wins_label = sfg::Label::Create("Target Number Of Wins");
		auto mutation_blocks_label = sfg::Label::Create("Mutation Blocks");
		auto target_fleet_file_label = sfg::Label::Create("Target Fleet File");
		auto target_fleet_name_label = sfg::Label::Create("Target Fleet Name");
		auto block_count_limit_label = sfg::Label::Create("Block Limit");
		//auto seed_file_name_label = sfg::Label::Create("Seed File Name");

		//Radio
		m_symm_1 = sfg::RadioButton::Create("No Symmetry");
		m_symm_2 = sfg::RadioButton::Create("Symmetric", m_symm_1->GetGroup());
		m_symm_3 = sfg::RadioButton::Create("Random", m_symm_1->GetGroup());

		m_symm_2->SetActive(true);

		//Bind functions
		m_symm_1->GetSignal(sfg::ToggleButton::OnToggle).Connect(std::bind(&Display::symmSelect, this));
		m_symm_2->GetSignal(sfg::ToggleButton::OnToggle).Connect(std::bind(&Display::symmSelect, this));
		m_symm_3->GetSignal(sfg::ToggleButton::OnToggle).Connect(std::bind(&Display::symmSelect, this));
		start_button->GetSignal(sfg::ToggleButton::OnLeftClick).Connect(std::bind(&Display::startPool, this));

		//Table Building
		auto table = sfg::Table::Create();

		float x_spacing = 5.f;
		float y_spacing = 5.f;

		table->Attach(faction_label, sf::Rect<sf::Uint32>(1, 0, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(faction, sf::Rect<sf::Uint32>(2, 0, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(m_fleet_p_value_label, sf::Rect<sf::Uint32>(1, 1, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(m_fleet_p_value, sf::Rect<sf::Uint32>(2, 1, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(fleet_single_ship_p_value_max_label, sf::Rect<sf::Uint32>(1, 2, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(fleet_single_ship_p_value_max, sf::Rect<sf::Uint32>(2, 2, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(fleet_single_ship_p_value_min_label, sf::Rect<sf::Uint32>(1, 3, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(fleet_single_ship_p_value_min, sf::Rect<sf::Uint32>(2, 3, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(fleet_population_size_label, sf::Rect<sf::Uint32>(1, 4, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(fleet_population_size, sf::Rect<sf::Uint32>(2, 4, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(target_number_of_wins_label, sf::Rect<sf::Uint32>(1, 5, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(target_number_of_wins, sf::Rect<sf::Uint32>(2, 5, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(target_fleet_file_label, sf::Rect<sf::Uint32>(1, 6, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(target_fleet_file, sf::Rect<sf::Uint32>(2, 6, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(target_fleet_name_label, sf::Rect<sf::Uint32>(1, 7, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(target_fleet_name, sf::Rect<sf::Uint32>(2, 7, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(mutation_blocks_label, sf::Rect<sf::Uint32>(1, 8, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(mutation_blocks, sf::Rect<sf::Uint32>(2, 8, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(m_symm_1, sf::Rect<sf::Uint32>(1, 9, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(m_symm_2, sf::Rect<sf::Uint32>(2, 9, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(m_symm_3, sf::Rect<sf::Uint32>(3, 9, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(start_button, sf::Rect<sf::Uint32>(1, 10, 2, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		m_window->Add(table);

	}
	else if (mode == 1) { // Threaded
		//Widgets
		auto start_button = sfg::Button::Create("Start");
		m_fleet_p_value = sfg::Entry::Create(std::to_string(m_config.fleet_p_value));
		fleet_single_ship_p_value_max = sfg::Entry::Create(std::to_string(m_config.fleet_single_ship_p_value_max));
		fleet_single_ship_p_value_min = sfg::Entry::Create(std::to_string(m_config.fleet_single_ship_p_value_min));
		target_fleet_file = sfg::Entry::Create(m_config.target_fleet_file);
		target_fleet_name = sfg::Entry::Create(m_config.target_fleet_name);
		seed_file_name = sfg::Entry::Create(m_config.seed_file_name);
		fleet_population_size = sfg::Entry::Create(std::to_string(m_config.fleet_population_size));
		target_number_of_wins = sfg::Entry::Create(std::to_string(m_config.target_number_of_wins));
		faction = sfg::Entry::Create(std::to_string(m_config.faction));
		mutation_blocks = sfg::Entry::Create(std::to_string(m_config.mutation_blocks));
		block_count_limit = sfg::Entry::Create(std::to_string(m_config.block_count_limit));

		//Labels
		auto faction_label = sfg::Label::Create("Faction");
		auto m_fleet_p_value_label = sfg::Label::Create("Fleet P Value");
		auto fleet_single_ship_p_value_max_label = sfg::Label::Create("Max Ship Value");
		auto fleet_single_ship_p_value_min_label = sfg::Label::Create("Min Ship Value");
		auto fleet_population_size_label = sfg::Label::Create("Population Size");
		auto target_number_of_wins_label = sfg::Label::Create("Target Number Of Wins");
		auto mutation_blocks_label = sfg::Label::Create("Mutation Blocks");
		auto target_fleet_file_label = sfg::Label::Create("Target Fleet File");
		auto target_fleet_name_label = sfg::Label::Create("Target Fleet Name");
		auto block_count_limit_label = sfg::Label::Create("Block Limit");
		//auto seed_file_name_label = sfg::Label::Create("Seed File Name");

		//Radio
		m_symm_1 = sfg::RadioButton::Create("No Symmetry");
		m_symm_2 = sfg::RadioButton::Create("Symmetric", m_symm_1->GetGroup());
		m_symm_3 = sfg::RadioButton::Create("Random", m_symm_1->GetGroup());

		m_symm_2->SetActive(true);

		//Bind functions
		m_symm_1->GetSignal(sfg::ToggleButton::OnToggle).Connect(std::bind(&Display::symmSelect, this));
		m_symm_2->GetSignal(sfg::ToggleButton::OnToggle).Connect(std::bind(&Display::symmSelect, this));
		m_symm_3->GetSignal(sfg::ToggleButton::OnToggle).Connect(std::bind(&Display::symmSelect, this));
		start_button->GetSignal(sfg::ToggleButton::OnLeftClick).Connect(std::bind(&Display::startPoolThreaded, this));

		//Table Building
		auto table = sfg::Table::Create();

		float x_spacing = 5.f;
		float y_spacing = 5.f;

		table->Attach(faction_label, sf::Rect<sf::Uint32>(1, 0, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(faction, sf::Rect<sf::Uint32>(2, 0, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(m_fleet_p_value_label, sf::Rect<sf::Uint32>(1, 1, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(m_fleet_p_value, sf::Rect<sf::Uint32>(2, 1, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(fleet_single_ship_p_value_max_label, sf::Rect<sf::Uint32>(1, 2, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(fleet_single_ship_p_value_max, sf::Rect<sf::Uint32>(2, 2, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(fleet_single_ship_p_value_min_label, sf::Rect<sf::Uint32>(1, 3, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(fleet_single_ship_p_value_min, sf::Rect<sf::Uint32>(2, 3, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(fleet_population_size_label, sf::Rect<sf::Uint32>(1, 4, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(fleet_population_size, sf::Rect<sf::Uint32>(2, 4, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(target_number_of_wins_label, sf::Rect<sf::Uint32>(1, 5, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(target_number_of_wins, sf::Rect<sf::Uint32>(2, 5, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(target_fleet_file_label, sf::Rect<sf::Uint32>(1, 6, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(target_fleet_file, sf::Rect<sf::Uint32>(2, 6, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(target_fleet_name_label, sf::Rect<sf::Uint32>(1, 7, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(target_fleet_name, sf::Rect<sf::Uint32>(2, 7, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(mutation_blocks_label, sf::Rect<sf::Uint32>(1, 8, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(mutation_blocks, sf::Rect<sf::Uint32>(2, 8, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(m_symm_1, sf::Rect<sf::Uint32>(1, 9, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(m_symm_2, sf::Rect<sf::Uint32>(2, 9, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(m_symm_3, sf::Rect<sf::Uint32>(3, 9, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(start_button, sf::Rect<sf::Uint32>(1, 10, 2, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		m_window->Add(table);

	}
	else if (mode == 2) {
		auto create_ship = sfg::Button::Create("Create New Ship");
		auto add_block = sfg::Button::Create("Add Block");
		auto add_thrust = sfg::Button::Create("Add Thruster");
		auto save_ship = sfg::Button::Create("Save the Ship");
		auto print_block = sfg::Button::Create("Debug Block");

		//Mutator buttons
		auto start_pool = sfg::Button::Create("Start Pool");
		auto breed_two = sfg::Button::Create("Breed Last Two");

		auto p_label = sfg::Label::Create("Target P Value");
		m_point_entry = sfg::Entry::Create("900");

		auto b_label = sfg::Label::Create("Target Block Limit");
		m_block_entry = sfg::Entry::Create("1000");

		auto t_label = sfg::Label::Create("Target Thruster Value");
		m_thrust_entry = sfg::Entry::Create("200");

		m_symm_1 = sfg::RadioButton::Create("No Symmetry");
		m_symm_2 = sfg::RadioButton::Create("Symmetric", m_symm_1->GetGroup());
		m_symm_3 = sfg::RadioButton::Create("Random", m_symm_1->GetGroup());

		m_symm_2->SetActive(true);

		//Bind functions
		create_ship->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&Display::createNewShip, this));
		add_block->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&Display::addBlock, this));
		save_ship->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&Display::saveShip, this));
		print_block->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&Display::debugBlock, this));
		add_thrust->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&Display::addThrust, this));
		//Radio buttons
		m_symm_1->GetSignal(sfg::ToggleButton::OnToggle).Connect(std::bind(&Display::symmSelect, this));
		m_symm_2->GetSignal(sfg::ToggleButton::OnToggle).Connect(std::bind(&Display::symmSelect, this));
		m_symm_3->GetSignal(sfg::ToggleButton::OnToggle).Connect(std::bind(&Display::symmSelect, this));
		//Threaded and mutator
		start_pool->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&Display::saveShip, this));
		breed_two->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&Display::breedLastTwo, this));

		auto table = sfg::Table::Create();
		float x_spacing = 5.f;
		float y_spacing = 5.f;

		table->Attach(create_ship, sf::Rect<sf::Uint32>(1, 0, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(add_block, sf::Rect<sf::Uint32>(2, 0, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(save_ship, sf::Rect<sf::Uint32>(3, 0, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(add_thrust, sf::Rect<sf::Uint32>(4, 0, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(print_block, sf::Rect<sf::Uint32>(5, 0, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(m_symm_1, sf::Rect<sf::Uint32>(1, 1, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(m_symm_2, sf::Rect<sf::Uint32>(2, 1, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(m_symm_3, sf::Rect<sf::Uint32>(3, 1, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(p_label, sf::Rect<sf::Uint32>(1, 2, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(m_point_entry, sf::Rect<sf::Uint32>(2, 2, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(b_label, sf::Rect<sf::Uint32>(1, 3, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(m_block_entry, sf::Rect<sf::Uint32>(2, 3, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(t_label, sf::Rect<sf::Uint32>(1, 4, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(m_thrust_entry, sf::Rect<sf::Uint32>(2, 4, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));

		table->Attach(start_pool, sf::Rect<sf::Uint32>(1, 5, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		table->Attach(breed_two, sf::Rect<sf::Uint32>(2, 5, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(x_spacing, y_spacing));
		m_window->Add(table);
		
	}

	m_desktop.Add(m_window);
}

void Display::createNewShip()
{
    int is_symm = m_config.symmetry;
    if(m_config.symmetry == 2)
        is_symm = rand() % 2;

    ships.push_back(
                    SB.createShip(
                                  std::stoi(m_point_entry->GetText().toAnsiString()),
                                  m_config.faction,
                                  std::stoi(m_block_entry->GetText().toAnsiString()),
                                  std::stoi(m_thrust_entry->GetText().toAnsiString()),
                                  is_symm,
                                  "Debug"
                                  )
                    );

}

void Display::startPool() 
{
	m_view.move(sf::Vector2f(1200, 0));
	m_window->Show(0);

	//SP.set("p_value_target",p->GetText().toAnsiString());
	SP.set("faction", faction->GetText().toAnsiString());
	SP.set("block_count_limit", block_count_limit->GetText().toAnsiString());
	SP.set("ship_symmetry", m_config.symmetry);
	SP.set("mutation_blocks", mutation_blocks->GetText().toAnsiString());
	SP.set("fleet_p_value", m_fleet_p_value->GetText().toAnsiString());
	SP.set("fleet_single_ship_p_value_max", fleet_single_ship_p_value_max->GetText().toAnsiString());
	SP.set("fleet_single_ship_p_value_min", fleet_single_ship_p_value_min->GetText().toAnsiString());
	SP.set("fleet_population_size", fleet_population_size->GetText().toAnsiString());
	SP.get("target_number_of_wins", target_number_of_wins->GetText().toAnsiString());
	SP.set("block_count_limit", block_count_limit->GetText().toAnsiString());
	SP.set("target_fleet_file", target_fleet_file->GetText().toAnsiString());
	SP.set("target_fleet_name", target_fleet_name->GetText().toAnsiString());
	SP.set("seed_file_name", seed_file_name->GetText().toAnsiString());

	SP.saveToFile();

	FM.init(); 
	std::thread t1(&FleetMutator::geneticFleetBeater, &FM);
	t1.detach();

}

void Display::startPoolThreaded()
{
	m_view.move(sf::Vector2f(480, 0));
	m_window->Show(0);

	//SP.set("p_value_target",p->GetText().toAnsiString());
	SP.set("faction", faction->GetText().toAnsiString());
	SP.set("block_count_limit", block_count_limit->GetText().toAnsiString());
	SP.set("ship_symmetry", m_config.symmetry);
	SP.set("mutation_blocks", mutation_blocks->GetText().toAnsiString());
	SP.set("fleet_p_value", m_fleet_p_value->GetText().toAnsiString());
	SP.set("fleet_single_ship_p_value_max", fleet_single_ship_p_value_max->GetText().toAnsiString());
	SP.set("fleet_single_ship_p_value_min", fleet_single_ship_p_value_min->GetText().toAnsiString());
	SP.set("fleet_population_size", fleet_population_size->GetText().toAnsiString());
	SP.get("target_number_of_wins", target_number_of_wins->GetText().toAnsiString());
	SP.set("block_count_limit", block_count_limit->GetText().toAnsiString());
	SP.set("target_fleet_file", target_fleet_file->GetText().toAnsiString());
	SP.set("target_fleet_name", target_fleet_name->GetText().toAnsiString());
	SP.set("seed_file_name", seed_file_name->GetText().toAnsiString());

	SP.saveToFile();

	FM.init();
	std::thread t1(&FleetMutator::threadedGeneticFleetBeater, &FM);
	t1.detach();
}