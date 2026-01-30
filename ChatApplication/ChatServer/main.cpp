#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include "Server.h"


#pragma comment(lib, "Ws2_32.lib")


void clientCommunication(SOCKET client_socket) {
    bool is_connected = true;
    while (is_connected) {
        char buffer[1024] = { 0 };
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::cout << "Received: " << buffer << std::endl;

            // Reverse the string
            std::string response(buffer);
            std::reverse(response.begin(), response.end());

            // Send the reversed string back
            send(client_socket, response.c_str(), static_cast<int>(response.size()), 0);
            std::cout << "Reversed string sent back to client." << std::endl;
        }
    }
}

int server() {
    Server server{};
    std::vector<std::unique_ptr<std::thread>> client_connections{};
    while (true) {
        SOCKET client_socket = server.incomingConnection();
        client_connections.push_back(std::make_unique<std::thread>(clientCommunication, client_socket));
    }
    // server.clientCommunication(i);
    return 0;
}

int main() {
    server();
    return 0;
}
