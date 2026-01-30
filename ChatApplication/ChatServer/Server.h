#pragma once
#include <vector>

class Server {
    WSADATA wsaData;
    SOCKET server_socket;
    std::vector<SOCKET> client_sockets{};
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

    SOCKET incomingConnection() {
        if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
            cleanUp();
            return -1;
        }
        std::cout << "Server is listening on port 65432..." << std::endl;
        
        sockaddr_in client_address = {};
        int client_address_len = sizeof(client_address);
        int i = client_sockets.size();
        client_sockets.emplace_back();
        client_sockets.at(i) = accept(server_socket, (sockaddr*)&client_address, &client_address_len);
        if (client_sockets.at(i) == INVALID_SOCKET) {
            std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            cleanUp();
            return -1;
        }
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "Accepted connection from " << client_ip << ":" << ntohs(client_address.sin_port) << std::endl;
        return client_sockets.at(i);
    }

    void clientCommunication(int i) {
        char buffer[1024] = { 0 };
        int bytes_received = recv(client_sockets.at(i), buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::cout << "Received: " << buffer << std::endl;

            // Reverse the string
            std::string response(buffer);
            std::reverse(response.begin(), response.end());

            // Send the reversed string back
            send(client_sockets.at(i), response.c_str(), static_cast<int>(response.size()), 0);
            std::cout << "Reversed string sent back to client." << std::endl;
        }
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