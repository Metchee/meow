/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** Zappy3DRenderer.cpp
*/

#include "../lib/Zappy3DRenderer.hpp"
#include "../lib/ZappyAbstractGUI.hpp"
#include <raylib.h>
#include <algorithm>
#include <iostream>
#include <sstream>

Zappy3DRenderer::Zappy3DRenderer(const std::string& ip, int port)
    : ZappyAbstractGUI(ip, port)
{
    camera = {0};
}

void Zappy3DRenderer::setupWindow() {
    InitWindow(windowWidth, windowHeight, "Zappy 3D Map");
    camera.position = { (float)gameState.getMapWidth() / 2, 15.0f, (float)gameState.getMapHeight() * 1.5f };
    camera.target = { (float)gameState.getMapWidth() / 2, 0.0f, (float)gameState.getMapHeight() / 2 };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetTargetFPS(60);
}

bool Zappy3DRenderer::isWindowOpen() {
    return !WindowShouldClose();
}

void Zappy3DRenderer::pollEvents() {
    UpdateCamera(&camera, CAMERA_ORBITAL);
}

void Zappy3DRenderer::renderFrame() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
    drawMap();
    drawPlayers();
    EndMode3D();

    DrawText("Zappy 3D Map - ESC to quit", 10, 10, 20, DARKGRAY);
    EndDrawing();
}

void Zappy3DRenderer::closeWindow() {
    CloseWindow();
}

void Zappy3DRenderer::drawMap() {
    for (int y = 0; y < gameState.getMapHeight(); ++y) {
        for (int x = 0; x < gameState.getMapWidth(); ++x) {
            const Tile& tile = gameState.getTile(x, y);
            Color color = LIGHTGRAY;
            int total = 0;
            for (int res : tile.resources) total += res;
            if (total > 0) color = YELLOW;
            DrawCube({(float)x, 0.5f, (float)y}, 0.95f, 1.0f, 0.95f, color);
            DrawCubeWires({(float)x, 0.5f, (float)y}, 1.0f, 1.0f, 1.0f, DARKGRAY);
        }
    }
}

void Zappy3DRenderer::drawPlayers() {
    for (const Player& player : gameState.getPlayers()) {
        DrawSphere({(float)player.x, 1.0f, (float)player.y}, 0.3f, RED);
    }
}
