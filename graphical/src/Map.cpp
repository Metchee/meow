/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** Map.cpp
*/

#include "../lib/Map.hpp"

Map::Map() : width(0), height(0) {}

void Map::setSize(int w, int h) {
    width = w;
    height = h;
    tiles.clear();
    tiles.resize(width * height);
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            tiles[y * width + x] = {x, y, std::vector<int>(7, 0)};
}

void Map::updateTile(int x, int y, const std::vector<int>& resources) {
    if (x < 0 || y < 0 || x >= width || y >= height) return;
    tiles[y * width + x].resources = resources;
}

void Map::parseTileLine(const std::string& line) {
    std::istringstream iss(line);
    std::string cmd;
    int x, y, val;
    std::vector<int> res;
    iss >> cmd >> x >> y;
    for (int i = 0; i < 7; ++i) {
        iss >> val;
        res.push_back(val);
    }
    updateTile(x, y, res);
}

void Map::printMap() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const Tile& tile = tiles[y * width + x];
            std::cout << "Tile (" << tile.x << ", " << tile.y << "): ";
            for (const auto& res : tile.resources) {
                std::cout << res << " ";
            }
            std::cout << "\n";
        }
    }
}
