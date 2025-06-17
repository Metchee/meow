/*
** EPITECH PROJECT, 2025
** main
** File description:
** zappy
*/

#include "../lib/window.hpp"
#include "../lib/Client_network.hpp"
#include "../lib/Map.hpp"
#include <iostream>
#include <string>
#include <sstream>

int main() {
    printf("in\n");
    std::string title = "Zappy GUI";
    ClientNetwork client("127.0.0.1", 4242);
    if (!client.connectToServer()) {
        std::cerr << "Erreur de connexion au serveur\n";
        return 1;
    }
    std::string welcome = client.receiveData();
    std::cout << "Serveur : " << welcome << std::endl;
    client.sendData("GUI\n");
    Map map;
    while (true) {
        std::string data = client.receiveData();
        if (data == "SERVER_SHUTDOWN\n") {
            std::cout << "Server closed the connection." << std::endl;
            break;
        }
        if (!data.empty()) {
            printf("%s\n", data.c_str());
            std::istringstream iss(data);
            std::string line;
            while (std::getline(iss, line)) {
                if (line.rfind("msz", 0) == 0) {
                    int w, h;
                    std::istringstream(line.substr(4)) >> w >> h;
                    map.setSize(w, h);
                    std::cout << "Map size: " << w << "x" << h << std::endl;
                } else if (line.rfind("bct", 0) == 0) {
                    map.parseTileLine(line);
                // map.printMap();
                }
            }
        }
        // GameWindow window(1920, 1080, title);
        // window.run();
    }
    return 0;
}
