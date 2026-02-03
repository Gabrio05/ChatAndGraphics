#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include "MessageHandler.h"

constexpr int _socket_client_buffer_size = 1024;

class Socket {
    const char* host = "127.0.0.1";
    const int port = 8080;
    std::string sentence = "Hello, server!";

    WSADATA wsaData;

public:
    SOCKET client_socket;
    bool is_invalid = false;

    Socket() {
        // Initialize
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
            is_invalid = true;
            return;
        }
        client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            is_invalid = true;
            return;
        }

        // Resolve the server address and port
        sockaddr_in server_address = {};
        server_address.sin_family = AF_INET;
        //server_address.sin_port = htons(std::stoi(port));
        server_address.sin_port = htons(port);
        if (inet_pton(AF_INET, host, &server_address.sin_addr) <= 0) {
            std::cerr << "Invalid address/Address not supported" << std::endl;
            cleanUp();
            return;
        }

        // Connect to the server
        if (connect(client_socket, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) == SOCKET_ERROR) {
            std::cerr << "Connection failed with error: " << WSAGetLastError() << std::endl;
            cleanUp();
            return;
        }
        std::cout << "Connected to the server." << std::endl;
    }

    void cleanUp() {
        closesocket(client_socket);
        WSACleanup();
        is_invalid = true;
    }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    ~Socket() {
        cleanUp();
    }
};

void receiveMessage(SOCKET client_socket, MessageHandler& message_handler);
void sendMessage(SOCKET client_socket, std::string message);

