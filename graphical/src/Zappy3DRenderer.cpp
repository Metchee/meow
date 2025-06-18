/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** Enhanced Zappy3DRenderer.cpp with advanced UI
*/

#include "../lib/Zappy3DRenderer.hpp"
#include "../lib/ZappyAbstractGUI.hpp"
#include <raylib.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

Zappy3DRenderer::Zappy3DRenderer(const std::string& ip, int port)
    : ZappyAbstractGUI(ip, port), selectedPlayer(nullptr), selectedTileX(-1), selectedTileY(-1),
      showPlayerPanel(false), showTilePanel(false), showTeamStats(true), 
      gameSpeed(1.0f), isPaused(false), showEventLog(true)
{
    camera = {0};
    eventLog.reserve(100); // Reserve space for events
}

void Zappy3DRenderer::setupWindow() {
    // Set initial Full HD window size
    windowWidth = 1920;
    windowHeight = 1080;
    InitWindow(windowWidth, windowHeight, "Zappy 3D - Enhanced Interface");
    
    // Make window resizable
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(800, 600); // Minimum size to keep UI usable
    
    // Setup camera with better positioning
    camera.position = { (float)gameState.getMapWidth() / 2, 20.0f, (float)gameState.getMapHeight() * 1.2f };
    camera.target = { (float)gameState.getMapWidth() / 2, 0.0f, (float)gameState.getMapHeight() / 2 };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    SetTargetFPS(60);
    
    // Initialize UI state
    addEventToLog("3D Interface initialized");
    addEventToLog("Use mouse to interact with map");
    addEventToLog("SPACE to pause, +/- to change speed");
}

bool Zappy3DRenderer::isWindowOpen() {
    return !WindowShouldClose();
}

void Zappy3DRenderer::pollEvents() {
    // Handle camera controls
    UpdateCamera(&camera, CAMERA_ORBITAL);
    
    // Check if window was resized
    if (IsWindowResized()) {
        windowWidth = GetScreenWidth();
        windowHeight = GetScreenHeight();
        addEventToLog("Window resized to " + std::to_string(windowWidth) + "x" + std::to_string(windowHeight));
    }
    
    // Handle UI input
    handleUIInput();
    
    // Handle mouse interactions
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        handleMouseClick();
    }
    
    // Handle keyboard shortcuts
    if (IsKeyPressed(KEY_SPACE)) {
        isPaused = !isPaused;
        addEventToLog(isPaused ? "Game paused" : "Game resumed");
    }
    
    if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
        gameSpeed = std::min(gameSpeed + 0.5f, 5.0f);
        addEventToLog("Speed: " + std::to_string(gameSpeed) + "x");
    }
    
    if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
        gameSpeed = std::max(gameSpeed - 0.5f, 0.1f);
        addEventToLog("Speed: " + std::to_string(gameSpeed) + "x");
    }
    
    // Toggle panels
    if (IsKeyPressed(KEY_P)) {
        showPlayerPanel = !showPlayerPanel;
    }
    if (IsKeyPressed(KEY_T)) {
        showTeamStats = !showTeamStats;
    }
    if (IsKeyPressed(KEY_L)) {
        showEventLog = !showEventLog;
    }
    
    // Toggle fullscreen
    if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
        addEventToLog(IsWindowFullscreen() ? "Entered fullscreen" : "Exited fullscreen");
    }
}

void Zappy3DRenderer::renderFrame() {
    BeginDrawing();
    ClearBackground({20, 25, 40, 255}); // Dark blue background
    
    // 3D Scene
    BeginMode3D(camera);
    drawMap();
    drawPlayers();
    drawSelection();
    EndMode3D();
    
    // 2D UI Overlay
    drawUI();
    
    EndDrawing();
}

void Zappy3DRenderer::closeWindow() {
    CloseWindow();
}

void Zappy3DRenderer::drawMap() {
    // Draw grid base
    for (int y = 0; y < gameState.getMapHeight(); ++y) {
        for (int x = 0; x < gameState.getMapWidth(); ++x) {
            const Tile& tile = gameState.getTile(x, y);
            
            // Calculate tile color based on resources
            Color tileColor = getTileColor(tile);
            
            // Base tile
            DrawCube({(float)x, 0.0f, (float)y}, 0.9f, 0.1f, 0.9f, tileColor);
            DrawCubeWires({(float)x, 0.0f, (float)y}, 1.0f, 0.1f, 1.0f, {100, 100, 100, 100});
            
            // Draw resources as small cubes on top
            drawTileResources(tile, x, y);
            
            // Highlight selected tile
            if (selectedTileX == x && selectedTileY == y) {
                DrawCubeWires({(float)x, 0.5f, (float)y}, 1.1f, 1.0f, 1.1f, YELLOW);
            }
        }
    }
}

void Zappy3DRenderer::drawPlayers() {
    for (const Player& player : gameState.getPlayers()) {
        Vector3 position = {(float)player.x, 1.0f, (float)player.y};
        
        // Player body - different colors per team
        Color playerColor = getTeamColor(player.team_name);
        DrawSphere(position, 0.3f, playerColor);
        
        // Player level indicator (crown height)
        float crownHeight = 0.1f * player.level;
        DrawCube({position.x, position.y + 0.3f + crownHeight/2, position.z}, 
                 0.2f, crownHeight, 0.2f, GOLD);
        
        // Direction indicator
        Vector3 direction = getDirectionVector(player.orientation);
        Vector3 arrowEnd = {
            position.x + direction.x * 0.5f,
            position.y,
            position.z + direction.z * 0.5f
        };
        
        // Draw arrow
        DrawLine3D(position, arrowEnd, WHITE);
        DrawSphere(arrowEnd, 0.05f, WHITE);
        
        // Highlight selected player
        if (selectedPlayer && selectedPlayer->id == player.id) {
            DrawSphereWires(position, 0.4f, 8, 8, YELLOW);
        }
        
        // Player ID floating text
        Vector2 screenPos = GetWorldToScreen(position, camera);
        if (screenPos.x >= 0 && screenPos.x < GetScreenWidth() && 
            screenPos.y >= 0 && screenPos.y < GetScreenHeight()) {
            DrawText(TextFormat("P%d", player.id), (int)screenPos.x - 15, (int)screenPos.y - 50, 16, WHITE);
            DrawText(TextFormat("Lv%d", player.level), (int)screenPos.x - 20, (int)screenPos.y - 30, 14, LIGHTGRAY);
        }
    }
}

void Zappy3DRenderer::drawTileResources(const Tile& tile, int x, int y) {
    const std::vector<Color> resourceColors = {
        BROWN,      // Food
        LIGHTGRAY,  // Linemate
        GOLD,       // Deraumere
        BLUE,       // Sibur
        PURPLE,     // Mendiane
        ORANGE,     // Phiras
        RED         // Thystame
    };
    
    int resourceCount = 0;
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < tile.resources[i]; ++j) {
            if (resourceCount >= 9) break; // Max 9 resources shown per tile
            
            float offsetX = (resourceCount % 3 - 1) * 0.2f;
            float offsetZ = (resourceCount / 3 - 1) * 0.2f;
            
            DrawCube({(float)x + offsetX, 0.15f, (float)y + offsetZ}, 
                     0.1f, 0.1f, 0.1f, resourceColors[i]);
            resourceCount++;
        }
        if (resourceCount >= 9) break;
    }
}

void Zappy3DRenderer::drawSelection() {
    // Draw selection ray for debugging
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Ray mouseRay = GetMouseRay(GetMousePosition(), camera);
        
        // Visual ray for debugging
        Vector3 rayEnd = {
            mouseRay.position.x + mouseRay.direction.x * 100,
            mouseRay.position.y + mouseRay.direction.y * 100,
            mouseRay.position.z + mouseRay.direction.z * 100
        };
        DrawLine3D(mouseRay.position, rayEnd, RED);
    }
}

void Zappy3DRenderer::drawUI() {
    // Top bar with game info
    drawTopBar();
    
    // Side panels
    if (showTeamStats) drawTeamStatsPanel();
    if (showPlayerPanel && selectedPlayer) drawPlayerPanel();
    if (showTilePanel && selectedTileX >= 0) drawTilePanel();
    if (showEventLog) drawEventLogPanel();
    
    // Bottom controls
    drawControlsPanel();
    
    // Help text
    drawHelpText();
}

void Zappy3DRenderer::drawTopBar() {
    // Semi-transparent background - responsive height
    int barHeight = getScaledSize(80);
    DrawRectangle(0, 0, GetScreenWidth(), barHeight, {0, 0, 0, 180});
    
    // Game title and status - responsive text
    int titleSize = getScaledFontSize(32);
    int margin = getScaledSize(30);
    DrawText("ZAPPY 3D", margin, barHeight/2 - titleSize/2, titleSize, WHITE);
    
    // Map info
    int infoSize = getScaledFontSize(20);
    std::string mapInfo = "Map: " + std::to_string(gameState.getMapWidth()) + "x" + 
                         std::to_string(gameState.getMapHeight());
    DrawText(mapInfo.c_str(), getScaledSize(220), barHeight/2 - infoSize/2, infoSize, LIGHTGRAY);
    
    // Player count
    std::string playerInfo = "Players: " + std::to_string(gameState.getPlayers().size());
    DrawText(playerInfo.c_str(), getScaledSize(450), barHeight/2 - infoSize/2, infoSize, LIGHTGRAY);
    
    // Game speed and pause status
    std::string speedInfo = isPaused ? "PAUSED" : ("Speed: " + std::to_string(gameSpeed) + "x");
    Color speedColor = isPaused ? RED : GREEN;
    DrawText(speedInfo.c_str(), GetScreenWidth() - getScaledSize(200), barHeight/2 - infoSize/2, infoSize, speedColor);
    
    // FPS
    int fpsSize = getScaledFontSize(16);
    DrawText(TextFormat("FPS: %d", GetFPS()), GetScreenWidth() - getScaledSize(100), 10, fpsSize, YELLOW);
}

void Zappy3DRenderer::drawTeamStatsPanel() {
    int panelWidth = getPanelWidth(0.18f); // 18% of screen width
    int panelHeight = getPanelHeight(0.37f); // 37% of screen height
    int margin = getScaledSize(20);
    int topBarHeight = getScaledSize(80);
    int x = GetScreenWidth() - panelWidth - margin;
    int y = topBarHeight + margin;
    
    // Panel background
    DrawRectangle(x, y, panelWidth, panelHeight, {0, 0, 0, 200});
    DrawRectangleLines(x, y, panelWidth, panelHeight, WHITE);
    
    // Title
    int titleSize = getScaledFontSize(20);
    int padding = getScaledSize(15);
    DrawText("TEAM STATISTICS", x + padding, y + padding, titleSize, WHITE);
    
    // Calculate team stats
    std::map<std::string, std::vector<const Player*>> teamPlayers;
    for (const Player& player : gameState.getPlayers()) {
        teamPlayers[player.team_name].push_back(&player);
    }
    
    int yOffset = getScaledSize(50);
    int teamNameSize = getScaledFontSize(18);
    int statsSize = getScaledFontSize(14);
    int lineSpacing = getScaledSize(45);
    
    for (const auto& team : teamPlayers) {
        Color teamColor = getTeamColor(team.first);
        
        // Team name
        DrawText(team.first.c_str(), x + padding, y + yOffset, teamNameSize, teamColor);
        
        // Player count and average level
        int totalLevel = 0;
        for (const Player* player : team.second) {
            totalLevel += player->level;
        }
        float avgLevel = team.second.empty() ? 0 : (float)totalLevel / team.second.size();
        
        std::string stats = TextFormat("Players: %d  Avg Lvl: %.1f", 
                                     (int)team.second.size(), avgLevel);
        DrawText(stats.c_str(), x + padding + getScaledSize(5), y + yOffset + getScaledSize(20), statsSize, LIGHTGRAY);
        
        yOffset += lineSpacing;
    }
}

void Zappy3DRenderer::drawPlayerPanel() {
    if (!selectedPlayer) return;
    
    int panelWidth = 280;
    int panelHeight = 320;
    int x = 20;
    int y = 100;
    
    // Panel background
    DrawRectangle(x, y, panelWidth, panelHeight, {0, 0, 0, 200});
    DrawRectangleLines(x, y, panelWidth, panelHeight, WHITE);
    
    // Title
    DrawText("PLAYER INFO", x + 15, y + 15, 20, WHITE);
    
    // Player details
    int yOffset = 50;
    DrawText(TextFormat("ID: %d", selectedPlayer->id), x + 15, y + yOffset, 16, WHITE);
    yOffset += 25;
    
    Color teamColor = getTeamColor(selectedPlayer->team_name);
    DrawText(TextFormat("Team: %s", selectedPlayer->team_name.c_str()), x + 15, y + yOffset, 16, teamColor);
    yOffset += 25;
    
    DrawText(TextFormat("Level: %d", selectedPlayer->level), x + 15, y + yOffset, 16, WHITE);
    yOffset += 25;
    
    DrawText(TextFormat("Position: (%d, %d)", selectedPlayer->x, selectedPlayer->y), x + 15, y + yOffset, 16, WHITE);
    yOffset += 25;
    
    std::string orientationText = "Facing: " + getOrientationString(selectedPlayer->orientation);
    DrawText(orientationText.c_str(), x + 15, y + yOffset, 16, WHITE);
    yOffset += 35;
    
    // Inventory (placeholder - would need server support)
    DrawText("INVENTORY:", x + 15, y + yOffset, 16, YELLOW);
    yOffset += 25;
    DrawText("(Request from server)", x + 15, y + yOffset, 14, GRAY);
}

void Zappy3DRenderer::drawTilePanel() {
    if (selectedTileX < 0 || selectedTileY < 0) return;
    
    const Tile& tile = gameState.getTile(selectedTileX, selectedTileY);
    
    int panelWidth = 250;
    int panelHeight = 280;
    int x = 20;
    int y = GetScreenHeight() - panelHeight - 20;
    
    // Panel background
    DrawRectangle(x, y, panelWidth, panelHeight, {0, 0, 0, 200});
    DrawRectangleLines(x, y, panelWidth, panelHeight, WHITE);
    
    // Title
    DrawText("TILE INFO", x + 15, y + 15, 20, WHITE);
    DrawText(TextFormat("(%d, %d)", selectedTileX, selectedTileY), x + 15, y + 40, 16, LIGHTGRAY);
    
    // Resources
    const std::vector<std::string> resourceNames = {
        "Food", "Linemate", "Deraumere", "Sibur", 
        "Mendiane", "Phiras", "Thystame"
    };
    
    int yOffset = 75;
    for (int i = 0; i < 7; ++i) {
        if (tile.resources[i] > 0) {
            DrawText(TextFormat("%s: %d", resourceNames[i].c_str(), tile.resources[i]), 
                     x + 15, y + yOffset, 14, WHITE);
            yOffset += 20;
        }
    }
    
    if (yOffset == 75) {
        DrawText("No resources", x + 15, y + yOffset, 14, GRAY);
    }
}

void Zappy3DRenderer::drawEventLogPanel() {
    int panelWidth = 400;
    int panelHeight = 200;
    int x = GetScreenWidth() - panelWidth - 20;
    int y = GetScreenHeight() - panelHeight - 20;
    
    // Panel background
    DrawRectangle(x, y, panelWidth, panelHeight, {0, 0, 0, 200});
    DrawRectangleLines(x, y, panelWidth, panelHeight, WHITE);
    
    // Title
    DrawText("EVENT LOG", x + 15, y + 15, 18, WHITE);
    
    // Show last few events
    int maxEvents = 12;
    int startIdx = std::max(0, (int)eventLog.size() - maxEvents);
    
    for (int i = startIdx; i < eventLog.size(); ++i) {
        int yPos = y + 45 + (i - startIdx) * 14;
        DrawText(eventLog[i].c_str(), x + 15, yPos, 12, LIGHTGRAY);
    }
}

void Zappy3DRenderer::drawControlsPanel() {
    int panelHeight = getScaledSize(50);
    int y = GetScreenHeight() - panelHeight;
    
    DrawRectangle(0, y, GetScreenWidth(), panelHeight, {0, 0, 0, 150});
    
    std::string controls = "CONTROLS: Mouse=Select | SPACE=Pause | +/-=Speed | P=Player Panel | T=Team Stats | L=Event Log | H=Help | F11=Fullscreen";
    int fontSize = getScaledFontSize(14);
    int margin = getScaledSize(20);
    DrawText(controls.c_str(), margin, y + panelHeight/2 - fontSize/2, fontSize, LIGHTGRAY);
}

void Zappy3DRenderer::drawHelpText() {
    if (IsKeyDown(KEY_H)) {
        int panelWidth = getPanelWidth(0.26f); // 26% of screen width
        int panelHeight = getPanelHeight(0.28f); // 28% of screen height
        int x = (GetScreenWidth() - panelWidth) / 2;
        int y = (GetScreenHeight() - panelHeight) / 2;
        
        DrawRectangle(x, y, panelWidth, panelHeight, {0, 0, 0, 220});
        DrawRectangleLines(x, y, panelWidth, panelHeight, WHITE);
        
        int padding = getScaledSize(30);
        int titleSize = getScaledFontSize(28);
        int textSize = getScaledFontSize(16);
        int smallSize = getScaledFontSize(14);
        int lineSpacing = getScaledSize(30);
        
        DrawText("HELP", x + padding, y + padding, titleSize, WHITE);
        DrawText("Mouse: Rotate camera and select objects", x + padding, y + padding + lineSpacing * 1.3f, textSize, LIGHTGRAY);
        DrawText("SPACE: Pause/Resume game", x + padding, y + padding + lineSpacing * 2.3f, textSize, LIGHTGRAY);
        DrawText("+/-: Adjust game speed", x + padding, y + padding + lineSpacing * 3.3f, textSize, LIGHTGRAY);
        DrawText("P: Toggle player panel", x + padding, y + padding + lineSpacing * 4.3f, textSize, LIGHTGRAY);
        DrawText("T: Toggle team statistics", x + padding, y + padding + lineSpacing * 5.3f, textSize, LIGHTGRAY);
        DrawText("L: Toggle event log", x + padding, y + padding + lineSpacing * 6.3f, textSize, LIGHTGRAY);
        DrawText("F11: Toggle fullscreen", x + padding, y + padding + lineSpacing * 7.3f, textSize, LIGHTGRAY);
        
        DrawText("Hold H to show this help", x + padding, y + panelHeight - getScaledSize(45), smallSize, YELLOW);
    } else {
        int fontSize = getScaledFontSize(14);
        DrawText("Hold H for help", getScaledSize(20), 10, fontSize, GRAY);
    }
}

// Helper methods
void Zappy3DRenderer::handleMouseClick() {
    Ray mouseRay = GetMouseRay(GetMousePosition(), camera);
    
    // Check for player selection first
    selectedPlayer = nullptr;
    float closestPlayerDist = 1000000.0f;
    
    for (const Player& player : gameState.getPlayers()) {
        Vector3 playerPos = {(float)player.x, 1.0f, (float)player.y};
        RayCollision collision = GetRayCollisionSphere(mouseRay, playerPos, 0.4f);
        
        if (collision.hit && collision.distance < closestPlayerDist) {
            selectedPlayer = &player;
            closestPlayerDist = collision.distance;
            showPlayerPanel = true;
            showTilePanel = false;
            
            addEventToLog("Selected player " + std::to_string(player.id) + 
                         " (" + player.team_name + ")");
        }
    }
    
    // If no player selected, check for tile selection
    if (!selectedPlayer) {
        // Check collision with ground plane (y = 0)
        float t = -mouseRay.position.y / mouseRay.direction.y;
        if (t > 0) {
            Vector3 hitPoint = {
                mouseRay.position.x + mouseRay.direction.x * t,
                0,
                mouseRay.position.z + mouseRay.direction.z * t
            };
            
            int tileX = (int)round(hitPoint.x);
            int tileZ = (int)round(hitPoint.z);
            
            if (tileX >= 0 && tileX < gameState.getMapWidth() && 
                tileZ >= 0 && tileZ < gameState.getMapHeight()) {
                selectedTileX = tileX;
                selectedTileY = tileZ;
                showTilePanel = true;
                showPlayerPanel = false;
                
                addEventToLog("Selected tile (" + std::to_string(tileX) + 
                             ", " + std::to_string(tileZ) + ")");
            }
        }
    }
}

Color Zappy3DRenderer::getTileColor(const Tile& tile) {
    // Color based on dominant resource
    int maxResource = 0;
    int dominantResource = -1;
    
    for (int i = 0; i < 7; ++i) {
        if (tile.resources[i] > maxResource) {
            maxResource = tile.resources[i];
            dominantResource = i;
        }
    }
    
    if (maxResource == 0) return {40, 40, 40, 255}; // Dark gray for empty
    
    const std::vector<Color> resourceColors = {
        {101, 67, 33, 255},   // Food - Brown
        {192, 192, 192, 255}, // Linemate - Silver
        {255, 215, 0, 255},   // Deraumere - Gold
        {0, 0, 255, 255},     // Sibur - Blue
        {128, 0, 128, 255},   // Mendiane - Purple
        {255, 165, 0, 255},   // Phiras - Orange
        {255, 0, 0, 255}      // Thystame - Red
    };
    
    Color baseColor = resourceColors[dominantResource];
    // Fade based on quantity
    float intensity = std::min(1.0f, maxResource / 5.0f);
    return {
        (unsigned char)(baseColor.r * intensity),
        (unsigned char)(baseColor.g * intensity),
        (unsigned char)(baseColor.b * intensity),
        200
    };
}

Color Zappy3DRenderer::getTeamColor(const std::string& teamName) {
    // Generate consistent color for team name
    unsigned int hash = 0;
    for (char c : teamName) {
        hash = hash * 31 + c;
    }
    
    return {
        (unsigned char)(100 + (hash % 155)),
        (unsigned char)(100 + ((hash >> 8) % 155)),
        (unsigned char)(100 + ((hash >> 16) % 155)),
        255
    };
}

Vector3 Zappy3DRenderer::getDirectionVector(int orientation) {
    switch (orientation) {
        case 1: return {0, 0, -1}; // North
        case 2: return {1, 0, 0};  // East
        case 3: return {0, 0, 1};  // South
        case 4: return {-1, 0, 0}; // West
        default: return {0, 0, -1};
    }
}

std::string Zappy3DRenderer::getOrientationString(int orientation) {
    switch (orientation) {
        case 1: return "North";
        case 2: return "East";
        case 3: return "South";
        case 4: return "West";
        default: return "Unknown";
    }
}

void Zappy3DRenderer::addEventToLog(const std::string& event) {
    eventLog.push_back(event);
    if (eventLog.size() > 50) { // Keep only last 50 events
        eventLog.erase(eventLog.begin());
    }
}

void Zappy3DRenderer::handleUIInput() {
    // Handle any additional UI input here
    // This could include text input, button clicks, etc.
}

// Responsive UI helper functions
int Zappy3DRenderer::getScaledSize(int baseSize) {
    // Scale based on window width relative to 1920 (our base resolution)
    float scale = (float)GetScreenWidth() / 1920.0f;
    return (int)(baseSize * scale);
}

int Zappy3DRenderer::getScaledFontSize(int baseFontSize) {
    // Scale font size but keep it readable
    float scale = (float)GetScreenWidth() / 1920.0f;
    int scaledSize = (int)(baseFontSize * scale);
    return std::max(scaledSize, 8); // Minimum font size of 8
}

int Zappy3DRenderer::getPanelWidth(float percentage) {
    return (int)(GetScreenWidth() * percentage);
}

int Zappy3DRenderer::getPanelHeight(float percentage) {
    return (int)(GetScreenHeight() * percentage);
}