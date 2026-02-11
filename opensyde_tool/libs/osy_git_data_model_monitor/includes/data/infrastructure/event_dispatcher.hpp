#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

namespace data::infrastructure
{
    // ========================================================================== //
    // EventId
    // -------------------------------------------------------------------------- //
    inline constexpr size_t InvalidEventId = static_cast<size_t>(-1);

    class EventId
    {
    public:
        template <typename T>
        static size_t value();

    private:
        static inline size_t counter = 0;
    };

    // ========================================================================== //
    // Event
    // -------------------------------------------------------------------------- //
    template <typename Derived>
    struct Event
    {
        static constexpr size_t get_type_id();
    };

    // ========================================================================== //
    // Dispatcher
    // -------------------------------------------------------------------------- //
    class Dispatcher
    {
    public:
        using HandlerId = size_t;

        template <typename EventT>
        HandlerId subscribe(std::function<void(const EventT &)> handler);

        template <typename EventT>
        void unsubscribe(const HandlerId id);
        void unsubscribe(const HandlerId id);

        template <typename EventT>
        void publish(const EventT &e) const;

    private:
        HandlerId static generate_handler_id();

        std::unordered_map<size_t, std::vector<std::pair<HandlerId, std::function<void(const void *)>>>> handlers_;
    };

}

#include <data/infrastructure/event_dispatcher/event_dispatcher.tpp>