#pragma once
#include <mutex>
#include <queue>
#include <vector>

struct ChatMessage {
    char buffer[512] = "";
    char user[32] = "";
    int timestamp = -1;  // If timestamp == -1, invalid message; if timestamp == 0, server message
    bool is_dm = false;
};

class MessageHandler {
    std::mutex lock;
    std::queue<ChatMessage> pending_incoming_messages{};
public:
    std::vector<ChatMessage> processed_messages{};
    // std::condition_variable cv;

    void add_message(const ChatMessage& message) {
        std::lock_guard<std::mutex> l(lock);
        pending_incoming_messages.push(message);
    }

    bool got_new_messages() {
        if (pending_incoming_messages.empty()) { return false; }
        std::lock_guard<std::mutex> l(lock);
        while (!pending_incoming_messages.empty()) {
            processed_messages.push_back(pending_incoming_messages.front());
            pending_incoming_messages.pop();
        }
        return true;
    }
};