/*
** EPITECH PROJECT, 2025
** main
** File description:
** zappy GUI main
*/

#include "../lib/window.hpp"
#include "../lib/Client_network.hpp"
#include "../lib/GameState.hpp"
#include "../lib/Renderer.hpp"
#include <iostream>
#include <string>

class ZappyGUI {
public:
    ZappyGUI(const std::string& ip, int port)
        : network(ip, port), window(1980, 920, "Zappy GUI"), renderer(window.getWindow()) {
    }

    bool initialize() {
        if (!network.connectToServer()) {
            std::cerr << "Failed to connect to server" << std::endl;
            return false;
        }
        
        std::string welcome = network.receiveData();
        std::cout << "Server: " << welcome << std::endl;
        network.sendData("GUI\n");
        return true;
    }

    void run() {
        if (!initialize()) {
            return;
        }
        
        std::cout << "GUI connected, starting main loop..." << std::endl;
        
        while (window.isOpen()) {
            handleEvents();
            updateFromServer();
            render();
        }
    }

private:
    ClientNetwork network;
    GameWindow window;
    GameState gameState;
    Renderer renderer;

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }

    void updateFromServer() {
        std::string data = network.receiveData();
        if (!data.empty()) {
            std::cout << "GUI received: " << data << std::endl;
            
            if (data == "SERVER_SHUTDOWN\n") {
                std::cout << "Server closed the connection." << std::endl;
                window.close();
                return;
            }
            gameState.processServerMessage(data);
        }
    }

    void render() {
        window.clear();
        renderer.render(gameState);
        window.display();
    }
};

void print_usage(const char* program_name) {
    std::cout << "USAGE: " << program_name << " -p port [-h machine]" << std::endl;
    std::cout << "DESCRIPTION:" << std::endl;
    std::cout << "\tport\t\tis the port number" << std::endl;
    std::cout << "\tmachine\t\tis the name of the machine; localhost by default" << std::endl;
}

int main(int argc, char** argv) {
    std::string ip = "localhost";
    int port = 4242;
    bool port_specified = false;
    
    for (int i = 1; i < argc - 1; i++) {
        if (std::string(argv[i]) == "-p") {
            port = std::atoi(argv[i + 1]);
            port_specified = true;
            i++;
        } else if (std::string(argv[i]) == "-h") {
            ip = argv[i + 1];
            i++;
        }
    }
    if (argc > 1) {
        if (std::string(argv[1]) == "--help") {
            print_usage(argv[0]);
            return 0;
        }
        
        if (!port_specified && argc > 1) {
            std::cerr << "Error: Port is required when using arguments" << std::endl;
            print_usage(argv[0]);
            return 84;
        }
    }
    std::cout << "Starting Zappy GUI..." << std::endl;
    std::cout << "Connecting to " << ip << ":" << port << std::endl;
    try {
        ZappyGUI gui(ip, port);
        gui.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }

    return 0;
}