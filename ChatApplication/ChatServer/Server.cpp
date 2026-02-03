#include "Server.h"
#include "MessageHandler.h"

void clientReceive(Client* client, MessageHandler& messages) {
    while (!client->is_invalid) {
        char buffer[1024] = { 0 };
        int bytes_received = recv(client->client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::cout << "Received message from " << client->username << ": " << buffer << std::endl;

            ChatMessage message;
            if (client->username == "") {
                if (bytes_received > 32) {
                    std::string response = "Username is too long! Disconnecting...";
                    send(client->client_socket, response.c_str(), static_cast<int>(response.size()), 0);
                    std::cout << "Username too long, disconnecting client." << std::endl;
                    client->is_invalid = true;
                    continue;
                }
                client->username = buffer;
                strncpy_s(message.buffer, "Connected", sizeof(message.buffer));
                message.timestamp = 0;
            } else {
                if (buffer[0] == ':') {
                    char* open = std::strstr(buffer, ": ");
                    if (open) { message.is_dm = true; }
                }
                strncpy_s(message.buffer, buffer, sizeof(message.buffer));
                message.timestamp = 1;  // TODO timestamp
            }
            strncpy_s(message.user, client->username.c_str(), sizeof(message.user));

            messages.add_message(message);
        }
    }
    closesocket(client->client_socket);
}

void clientSend(Client* client, MessageHandler& messages) {
    while (!client->is_invalid) {
        messages.got_new_messages();
        if (client->messages_sent < messages.processed_messages.size()) {
            ChatMessage message = messages.processed_messages.at(client->messages_sent);
            client->messages_sent++;
            std::string response = std::string(message.user) + " (" + std::to_string(message.timestamp) + "):" + 
                (message.is_dm ? "" : " ") + message.buffer;
            if (message.is_dm && message.user != client->username) {
                int index = std::string(message.buffer).find(": ");
                if (client->username != std::string(message.buffer).substr(1, index - 2)) {
                    continue;
                }
            }
            send(client->client_socket, response.c_str(), response.length(), 0);
            std::cout << "Sent from " << message.user << " to " << client->username << ": " << response << std::endl;
        }
        // Sleep(100);
    }
}