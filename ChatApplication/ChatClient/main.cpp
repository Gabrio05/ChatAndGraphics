// CLIENT SIDE - Using WinSock to connect to a TCP server
// This program connects to the server, sends a sentence, and receives the reversed response.

#include <iostream>
#include <string>
#include "Socket.h"
#include <thread>


#pragma comment(lib, "ws2_32.lib")


void receiveMessage(SOCKET client_socket) {
    bool is_connected = true;
    // Receive the reversed sentence from the server
    while (is_connected) {
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
}

// void run_client(const char* host, unsigned int port, const std::string& sentence) {
void run_client() {
    Socket client_socket{};
    std::unique_ptr<std::thread> t = std::make_unique<std::thread>(receiveMessage, client_socket.client_socket);
    while (true) {
        client_socket.sendMessage();
    }
}


int main() {
    run_client();
    return 0;
}
