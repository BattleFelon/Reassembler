
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Display.h"
#include "Vector2D.h"

Display::Display():
    debug_ship(8),
    attachment_point(5)
{
    //Set Window and View varibles
	m_renderWindow.create(sf::VideoMode(800, 600), "Reassembler");
	m_view = m_renderWindow.getDefaultView();
	m_renderWindow.setFramerateLimit(60);

    ships.push_back(SB.createShip(1300,8,1000,0));
	createGui();

	attachment_point.setOrigin(5,5);

}

Display::~Display()
{

}

void Display::run()
{
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
                draw_block[i].position = sf::Vector2f(bound.x * scale + position.x,bound.y * scale + position.y);
                i++;
            }
            //Complete the shape
            draw_block[block.getBounds().size()].position = sf::Vector2f(block.getBounds()[0].x * scale + position.x,block.getBounds()[0].y * scale + position.y);
            //Draw it
            m_renderWindow.draw(draw_block);

            //Draw the attachments
            for(auto attachment : block.getAttachments()){
                attachment_point.setPosition(sf::Vector2f(attachment.position.x * scale + position.x, attachment.position.y * scale + position.y));
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
            m_zoom = 1.0f;
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

void Display::createGui()
{

	// We have to do this because we don't use SFML to draw.
	m_renderWindow.resetGLStates();

	m_desktop.SetProperty( "Button#Button", "FontSize", 18.f );
	auto create_ship = sfg::Button::Create("Create New Ship");
	auto add_block = sfg::Button::Create("Add Block");
	auto save_ship = sfg::Button::Create("Save the Ship");
	auto print_block = sfg::Button::Create("Debug Block");

		//Create Settings Window
	m_window = sfg::Window::Create();
	m_window->SetTitle( "Reassembler" );

    auto box = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 5.f );
    box->Pack( create_ship, false );
    box->Pack( add_block, false );
	box->Pack( save_ship, false );
	box->Pack( print_block, false );

	create_ship->GetSignal( sfg::Widget::OnLeftClick ).Connect( std::bind( &Display::createNewShip, this ) );
    add_block->GetSignal( sfg::Widget::OnLeftClick ).Connect( std::bind( &Display::addBlock, this ) );
	save_ship->GetSignal( sfg::Widget::OnLeftClick ).Connect( std::bind( &Display::saveShip, this ) );
	print_block->GetSignal( sfg::Widget::OnLeftClick ).Connect( std::bind( &Display::debugBlock, this ) );


	m_window->Add(box);
	m_desktop.Add(m_window);
}

