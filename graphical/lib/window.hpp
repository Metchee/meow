/*
** EPITECH PROJECT, 2025
** window
** File description:
** zappy
*/

#ifndef WINDOW_HPP
    #define WINDOW_HPP
    #include <SFML/Graphics.hpp>
    #include <string>

class GameWindow {
    public :
        GameWindow(unsigned int width, unsigned int height, const std::string& title);
        void run();
    private :
        unsigned int m_width;
        unsigned int m_height;
        std::string m_title;
        sf::RenderWindow m_window;
};

#endif /* !WINDOW_HPP */
