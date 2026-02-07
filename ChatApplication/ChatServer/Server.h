#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>
#include <iostream>
#include "MessageHandler.h"

#pragma comment(lib, "Ws2_32.lib")

struct Client {
    SOCKET client_socket;
    std::string username = "";
    bool is_invalid = true;
    int messages_sent = 0;
};

class Server {
    WSADATA wsaData;
    SOCKET server_socket;
    std::vector<std::unique_ptr<Client>> clients;
public:
    bool is_invalid = false;

    Server() {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
            is_invalid = true;
            return;
        }
        server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_socket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            is_invalid = true;
            return;
        }
        
        sockaddr_in server_address = {};
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(8080);  // Server port
        server_address.sin_addr.s_addr = INADDR_ANY;  // Accept connections on any IP address

        if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
            std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
            cleanUp();
            return;
        }
    }

    Client* incomingConnection(MessageHandler& messages) {
        if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
            cleanUp();
            return nullptr;
        }
        std::cout << "Server is listening..." << std::endl;
        
        sockaddr_in client_address = {};
        int client_address_len = sizeof(client_address);
        int i = clients.size();
        clients.emplace_back(std::make_unique<Client>());
        clients.at(i)->client_socket = accept(server_socket, (sockaddr*)&client_address, &client_address_len);
        if (clients.at(i)->client_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            cleanUp();
            return nullptr;
        }
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "Accepted connection from " << client_ip << ":" << ntohs(client_address.sin_port) << std::endl;


        std::string response = " (-1): Hi! What's your username?";
        clients.at(i)->messages_sent = messages.processed_messages.size();
        send(clients.at(i)->client_socket, response.c_str(), static_cast<int>(response.size()), 0);
        clients.at(i)->is_invalid = false;
        return clients.at(i).get();
    }

    void cleanUp() {
        closesocket(server_socket);
        WSACleanup();
        is_invalid = true;
    }

    ~Server() {
        cleanUp();
    }

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
};

void clientReceive(Client* client, MessageHandler& messages);
void clientSend(Client* client, MessageHandler& messages);