/*
** EPITECH PROJECT, 2025
** zappy
** File description:
** Map.hpp
*/

#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

struct Tile {
    int x;
    int y;
    std::vector<int> resources;
};

class Map {
    public:
        Map();
        void setSize(int w, int h);
        void updateTile(int x, int y, const std::vector<int>& resources);
        void parseTileLine(const std::string& line);
        void printMap();
    private:
        int width, height;
        std::vector<Tile> tiles;
};
