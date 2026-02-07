// CLIENT SIDE - Using WinSock to connect to a TCP server
// This program connects to the server, sends a sentence, and receives the reversed response.

#include <iostream>
#include <string>
#include <thread>
#include "imguiHandler.h"
#include "Sound.h"

#pragma comment(lib, "ws2_32.lib")


int main() {
    Socket client_socket{};
    SoundManager sounds{};
    sounds.load("Audio/Inception_Horn_Sound_Effect.wav");
    sounds.load("Audio/Rifle_Shot_Echo_-_Sound_Effect.wav");
    sounds.play("Audio/Inception_Horn_Sound_Effect.wav");
    sounds.play("Audio/Rifle_Shot_Echo_-_Sound_Effect.wav");

    if (client_socket.is_invalid) {
        return 1;
    }
    MessageHandler message_handler{};
    std::unique_ptr<std::thread> thread = std::make_unique<std::thread>(receiveMessage, client_socket.client_socket, std::ref(message_handler), &sounds);
    runImgui(client_socket.client_socket, message_handler);
    thread->join();
    return 0;
}
