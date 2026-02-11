#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

namespace data::infrastructure
{
    // ========================================================================== //
    // EventQueue
    // -------------------------------------------------------------------------- //
    template <typename Event>
    class EventQueue
    {
    public:
        void push(const Event &key, const uint8_t *data, size_t length);
        // std::optional<Message<Event>> pop();
        bool is_empty();
        void set_timeout_ms(const uint32_t timeout);

    private:
        // std::queue<Message<Event>> queue;
        std::mutex mtx;
        std::condition_variable cv;
        std::chrono::milliseconds timeout = std::chrono::milliseconds(1000);
    };
}
#include <data/infrastructure/event_dispatcher/event_queue.tpp>