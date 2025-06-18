/*
** EPITECH PROJECT, 2025
** Renderer
** File description:
** Implementation of Renderer
*/

#include "../lib/Renderer.hpp"
#include <iostream>
#include <cmath>
#include <filesystem>

Renderer::Renderer(sf::RenderWindow& win) : window(win), tile_size(32.0f), offset(50, 50) {
    if (!font.loadFromFile("/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf")) {
        std::cout << "Warning: Could not load font, using default" << std::endl;
    }
    gameView = window.getDefaultView();
}

void Renderer::render(const GameState& gameState) {
    window.clear(sf::Color(30, 30, 30));

    if (gameState.getMapWidth() > 0 && gameState.getMapHeight() > 0) {
        calculateTileSize(gameState);
        renderMap(gameState);
        renderPlayers(gameState);
    }

    renderUI(gameState);
}

void Renderer::calculateTileSize(const GameState& gameState) {
    float window_width = static_cast<float>(window.getSize().x) - 2 * offset.x;
    float window_height = static_cast<float>(window.getSize().y) - 2 * offset.y - 100;

    float tile_width = window_width / gameState.getMapWidth();
    float tile_height = window_height / gameState.getMapHeight();

    tile_size = std::min(tile_width, tile_height);
    tile_size = std::max(tile_size, 8.0f);
}

void Renderer::renderMap(const GameState& gameState) {
    for (int y = 0; y < gameState.getMapHeight(); ++y) {
        for (int x = 0; x < gameState.getMapWidth(); ++x) {
            renderTile(gameState.getTile(x, y));
        }
    }
}

void Renderer::renderTile(const Tile& tile) {
    sf::Vector2f pos = worldToScreen(tile.x, tile.y);
    sf::RectangleShape tileRect(sf::Vector2f(tile_size - 1, tile_size - 1));
    tileRect.setPosition(pos);
    tileRect.setFillColor(getResourceColor(tile.resources));
    tileRect.setOutlineThickness(1);
    tileRect.setOutlineColor(sf::Color(100, 100, 100));

    window.draw(tileRect);

    int total_resources = 0;
    for (int res : tile.resources) {
        total_resources += res;
    }
    if (total_resources > 0 && tile_size > 15) {
        sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(total_resources));
        text.setCharacterSize(static_cast<unsigned int>(tile_size * 0.4f));
        text.setFillColor(sf::Color::White);

        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition(
            pos.x + (tile_size - textBounds.width) / 2,
            pos.y + (tile_size - textBounds.height) / 2
        );

        // renderRes(tile, 1, "../graphical/asset/stone.png");
        renderStone(tile);
        renderRes(tile, 0, "../graphical/asset/steak.png");
        renderFood(tile);
        window.draw(text);
    }
}

void Renderer::renderPlayers(const GameState& gameState) {
    for (const Player& player : gameState.getPlayers()) {
        renderPlayer(player);
    }
}

void Renderer::renderPlayer(const Player& player) {
    sf::Vector2f pos = worldToScreen(player.x, player.y);

    float radius = tile_size * 0.3f;
    sf::CircleShape playerCircle(radius);
    playerCircle.setPosition(pos.x + tile_size/2 - radius, pos.y + tile_size/2 - radius);
    playerCircle.setFillColor(getTeamColor(player.team_name));
    playerCircle.setOutlineThickness(2);
    playerCircle.setOutlineColor(sf::Color::White);

    window.draw(playerCircle);

    sf::Vector2f center(pos.x + tile_size/2, pos.y + tile_size/2);
    sf::Vector2f direction;

    switch (player.orientation) {
        case 1: direction = sf::Vector2f(0, -1); break;
        case 2: direction = sf::Vector2f(1, 0); break;
        case 3: direction = sf::Vector2f(0, 1); break;
        case 4: direction = sf::Vector2f(-1, 0); break;
        default: direction = sf::Vector2f(0, -1); break;
    }

    sf::Vertex line[] = {
        sf::Vertex(center, sf::Color::Yellow),
        sf::Vertex(center + direction * radius * 1.5f, sf::Color::Yellow)
    };
    window.draw(line, 2, sf::Lines);

    if (tile_size > 20) {
        sf::Text idText;
        idText.setFont(font);
        idText.setString(std::to_string(player.id) + "\nLv" + std::to_string(player.level));
        idText.setCharacterSize(static_cast<unsigned int>(tile_size * 0.2f));
        idText.setFillColor(sf::Color::Black);
        idText.setPosition(pos.x + 2, pos.y + 2);

        window.draw(idText);
    }
}

void Renderer::renderUI(const GameState& gameState) {
    sf::RectangleShape infoPanel(sf::Vector2f(static_cast<float>(window.getSize().x), 80));
    infoPanel.setPosition(0, static_cast<float>(window.getSize().y) - 80);
    infoPanel.setFillColor(sf::Color(50, 50, 50, 200));
    window.draw(infoPanel);

    sf::Text mapInfo;
    mapInfo.setFont(font);
    mapInfo.setString("Map: " + std::to_string(gameState.getMapWidth()) + "x" +
                     std::to_string(gameState.getMapHeight()) +
                     "  Players: " + std::to_string(gameState.getPlayers().size()));
    mapInfo.setCharacterSize(16);
    mapInfo.setFillColor(sf::Color::White);
    mapInfo.setPosition(10, static_cast<float>(window.getSize().y) - 70);

    window.draw(mapInfo);
    sf::Text playerList;
    playerList.setFont(font);
    std::string playerText = "Teams: ";

    std::map<std::string, int> teamCounts;
    for (const Player& player : gameState.getPlayers()) {
        teamCounts[player.team_name]++;
    }

    for (const auto& team : teamCounts) {
        playerText += team.first + "(" + std::to_string(team.second) + ") ";
    }

    playerList.setString(playerText);
    playerList.setCharacterSize(14);
    playerList.setFillColor(sf::Color::White);
    playerList.setPosition(10, static_cast<float>(window.getSize().y) - 45);

    window.draw(playerList);
}

sf::Color Renderer::getResourceColor(const std::vector<int>& resources) {
    int maxRes = 0;
    int maxIndex = 0;

    for (size_t i = 0; i < resources.size(); ++i) {
        if (resources[i] > maxRes) {
            maxRes = resources[i];
            maxIndex = i;
        }
    }

    if (maxRes == 0) {
        return sf::Color(80, 80, 80);
    }

    switch (maxIndex) {
        case 0: return sf::Color(139, 69, 19);
        case 1: return sf::Color(192, 192, 192);
        case 2: return sf::Color(255, 215, 0);
        case 3: return sf::Color(0, 0, 255);
        case 4: return sf::Color(128, 0, 128);
        case 5: return sf::Color(255, 165, 0);
        case 6: return sf::Color(255, 0, 0);
        default: return sf::Color(100, 100, 100);
    }
}

sf::Color Renderer::getTeamColor(const std::string& team_name) {
    unsigned int hash = 0;
    for (char c : team_name) {
        hash = hash * 31 + c;
    }
    int r = 100 + (hash % 155);
    int g = 100 + ((hash >> 8) % 155);
    int b = 100 + ((hash >> 16) % 155);

    return sf::Color(r, g, b);
}

sf::Vector2f Renderer::worldToScreen(int world_x, int world_y) {
    return sf::Vector2f(
        offset.x + world_x * tile_size,
        offset.y + world_y * tile_size
    );
}

void Renderer::renderRes(const Tile& tile, int res_id, std::string sprit_path) {
    if (tile.resources[res_id] <= 0) return;

    static sf::Texture Texture;
    static bool isLoaded = false;

    if (!isLoaded) {
        if (!Texture.loadFromFile(sprit_path)) {
            std::cerr << "Erreur : impossible de charger steak.png" << std::endl;
            return;
        }
        isLoaded = true;
    }

    sf::Sprite Sprite;
    Sprite.setTexture(Texture);

    float scale = tile_size / Texture.getSize().x;
    Sprite.setScale(scale, scale);
    Sprite.setPosition(tile.x * tile_size + offset.x, tile.y * tile_size + offset.y);

    window.draw(Sprite);
}

void Renderer::renderFood(const Tile& tile) {
    if (tile.resources[0] <= 0) return;

    static sf::Texture Texture;
    static bool isLoaded = false;

    if (!isLoaded) {
        if (!Texture.loadFromFile("../graphical/asset/steak.png")) {
            std::cerr << "Erreur : impossible de charger steak.png" << std::endl;
            return;
        }
        isLoaded = true;
    }

    sf::Sprite Sprite;
    Sprite.setTexture(Texture);

    float scale = tile_size / Texture.getSize().x;
    Sprite.setScale(scale, scale);
    Sprite.setPosition(tile.x * tile_size + offset.x, tile.y * tile_size + offset.y);

    window.draw(Sprite);
}

void Renderer::renderStone(const Tile& tile) {
    if (tile.resources[1] <= 0) return;

    static sf::Texture Texture;
    static bool isLoaded = false;

    if (!isLoaded) {
        if (!Texture.loadFromFile("../graphical/asset/stone.png")) {
            std::cerr << "Erreur : impossible de charger steak.png" << std::endl;
            return;
        }
        isLoaded = true;
    }

    sf::Sprite Sprite;
    Sprite.setTexture(Texture);

    float scale = tile_size / Texture.getSize().x;
    Sprite.setScale(scale, scale);
    Sprite.setPosition(tile.x * tile_size + offset.x, tile.y * tile_size + offset.y);

    window.draw(Sprite);
}
