#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#define DEFAULT_BUFFER_SIZE 1024

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

    void sendMessage() {
        // Send the sentence to the server
        if (send(client_socket, sentence.c_str(), static_cast<int>(sentence.size()), 0) == SOCKET_ERROR) {
            std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
            cleanUp();
            return;
        }
        std::cout << "Sent: " << sentence << std::endl;
    }

    void receiveMessage() {
        // Receive the reversed sentence from the server
        char buffer[DEFAULT_BUFFER_SIZE] = { 0 };
        int bytes_received = recv(client_socket, buffer, DEFAULT_BUFFER_SIZE - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; // Null-terminate the received data
            std::cout << "Received from server: " << buffer << std::endl;
        } else if (bytes_received == 0) {
            std::cout << "Connection closed by server." << std::endl;
        } else {
            std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
        }
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

