// CLIENT SIDE - Using WinSock to connect to a TCP server
// This program connects to the server, sends a sentence, and receives the reversed response.

#include <iostream>
#include <string>
#include <thread>
#include "imguiHandler.h"


#pragma comment(lib, "ws2_32.lib")


int main() {
    Socket client_socket{};
    /*if (client_socket.is_invalid) {
        return 1;
    }*/
    MessageHandler message_handler{};
    std::unique_ptr<std::thread> thread = std::make_unique<std::thread>(receiveMessage, client_socket.client_socket, std::ref(message_handler));
    runImgui(client_socket.client_socket, message_handler);
    thread->join();
    return 0;
}
