/*
** EPITECH PROJECT, 2025
** ZappyGUI
** File description:
** Main GUI application class
*/

#pragma once
#include "ZappyAbstractGUI.hpp"
#include "window.hpp"
#include "Renderer.hpp"

class ZappyGUI : public ZappyAbstractGUI {
public:
    ZappyGUI(const std::string& ip, int port);

protected:
    void setupWindow() override;
    bool isWindowOpen() override;
    void pollEvents() override;
    void renderFrame() override;
    void closeWindow() override;

private:
    GameWindow window;
    Renderer renderer;
};
