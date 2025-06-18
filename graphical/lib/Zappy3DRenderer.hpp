/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** Zappy3DRenderer.hpp
*/

#pragma once
#include "window.hpp"
#include "ZappyAbstractGUI.hpp"
#include <raylib.h>
#include <string>

class Zappy3DRenderer : public ZappyAbstractGUI {
public:
    Zappy3DRenderer(const std::string& ip, int port);
    ~Zappy3DRenderer() = default;

protected:
    void setupWindow() override;
    bool isWindowOpen() override;
    void pollEvents() override;
    void renderFrame() override;
    void closeWindow() override;

private:
    Camera camera;
    void drawMap();
    void drawPlayers();
};
