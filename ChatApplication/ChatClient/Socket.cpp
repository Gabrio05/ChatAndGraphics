#include "Socket.h"
#include <cstdlib>
#include <string.h>

ChatMessage basicReturn(char buffer[_socket_client_buffer_size]) {
    ChatMessage out{};
    strncpy_s(out.buffer, buffer, sizeof(out.buffer));
    return out;
}

ChatMessage formatIncomingMessage(char buffer[_socket_client_buffer_size]) {
    // Message format is "User (Timestamp): Message"
    if (!buffer) { return basicReturn(buffer); }

    char* open = std::strstr(buffer, " (");
    if (!open) { return basicReturn(buffer); }
    *open = '\0';
    char* user = buffer;

    char* close = std::strstr(open + 2, ")");
    if (!close) { return basicReturn(buffer); }
    *close = '\0';
    char* ts = open + 2;
    int timestamp = atoi(ts);

    if (close[1] != ':') { return basicReturn(buffer); }
    char* msg = close + 3;;

    ChatMessage out{};
    if (close[2] == ':') { 
        out.is_dm;
        msg = close + 1;
    }
    
    strncpy_s(out.buffer, msg, sizeof(out.buffer));
    strncpy_s(out.user, user, sizeof(out.user));
    out.timestamp = timestamp;
    return out;
}

void receiveMessage(SOCKET client_socket, MessageHandler& message_handler) {
    bool is_connected = true;
    while (is_connected) {
        char buffer[_socket_client_buffer_size] = { 0 };
        int bytes_received = recv(client_socket, buffer, _socket_client_buffer_size - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; // Null-terminate the received data
            std::cout << "Received from server: " << buffer << std::endl;
            ChatMessage message = formatIncomingMessage(buffer);//formatIncomingMessage(buffer);
            message_handler.add_message(message);
        } else if (bytes_received == 0) {
            std::cout << "Connection closed by server." << std::endl;
            is_connected = false;
        } else {
            std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
            is_connected = false;
        }
    }
}

void sendMessage(SOCKET client_socket, std::string message) {
    // Send the sentence to the server
    if (send(client_socket, message.c_str(), static_cast<int>(message.size()), 0) == SOCKET_ERROR) {
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
        return;
    }
    std::cout << "Sent: " << message << std::endl;
}