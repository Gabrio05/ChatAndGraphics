#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include "Server.h"
#include "MessageHandler.h"


#pragma comment(lib, "Ws2_32.lib")

int server() {
    Server server{};
    MessageHandler messages{};
    std::vector<std::unique_ptr<std::thread>> client_connections{};
    while (true) {
        Client* client = server.incomingConnection(messages);
        client_connections.push_back(std::make_unique<std::thread>(clientReceive, client, std::ref(messages)));
        client_connections.push_back(std::make_unique<std::thread>(clientSend, client, std::ref(messages)));
    }
    // server.clientCommunication(i);
    return 0;
}

int main() {
    server();
    return 0;
}
