/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** Enhanced Zappy3DRenderer.hpp with advanced UI
*/

#pragma once
#include "window.hpp"
#include "ZappyAbstractGUI.hpp"
#include <raylib.h>
#include <string>
#include <vector>
#include <map>

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
    // Core rendering
    Camera camera;
    void drawMap();
    void drawPlayers();
    void drawTileResources(const Tile& tile, int x, int y);
    void drawSelection();

    // UI System
    void drawUI();
    void drawTopBar();
    void drawTeamStatsPanel();
    void drawPlayerPanel();
    void drawTilePanel();
    void drawEventLogPanel();
    void drawControlsPanel();
    void drawHelpText();

    // Interaction system
    void handleMouseClick();
    void handleUIInput();
    
    // Selection state
    const Player* selectedPlayer;
    int selectedTileX, selectedTileY;
    
    // UI state
    bool showPlayerPanel;
    bool showTilePanel;
    bool showTeamStats;
    bool showEventLog;
    
    // Game control
    float gameSpeed;
    bool isPaused;
    
    // Event logging
    std::vector<std::string> eventLog;
    void addEventToLog(const std::string& event);
    
    // Helper methods
    Color getTileColor(const Tile& tile);
    Color getTeamColor(const std::string& teamName);
    Vector3 getDirectionVector(int orientation);
    std::string getOrientationString(int orientation);
    
    // Responsive UI helpers
    int getScaledSize(int baseSize);
    int getScaledFontSize(int baseFontSize);
    int getPanelWidth(float percentage);
    int getPanelHeight(float percentage);
};