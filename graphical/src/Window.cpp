/*
** EPITECH PROJECT, 2025
** Zappyloop
** File description:
** zappy
*/

#include "../lib/window.hpp"

GameWindow::GameWindow(unsigned int width, unsigned int height, const std::string& title)
    : m_width(width), m_height(height), m_title(title)
{
    sf::String sfmlTitle(title);
    m_window.create(sf::VideoMode({width, height}), sfmlTitle);
    m_window.setFramerateLimit(60);
}

void GameWindow::run()
{
    while(m_window.isOpen()) {
        sf::Event event;
        while(m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_window.close();
            }
        }
        m_window.clear(sf::Color::Black);
        m_window.display();
    }
}
