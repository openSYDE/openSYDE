#pragma once
#include <concepts/ddd.hpp>
#include <data/monitor/event/token.hpp>
#include <data/monitor/event/channel_data_container.hpp>

#include <data/infrastructure/event_dispatcher.hpp>
namespace di = data::infrastructure;

namespace data::monitor
{
    enum class EventState
    {
        Undefined,
        True,
        False
    };

    // ========================================================================== //
    // Factory
    // -------------------------------------------------------------------------- //
    class Event;
    using EventOwner = std::unique_ptr<Event>;
    class EventStateChangedObserver;

    FACTORY class EventFactory
    {
    public:
        EventFactory(const ChannelDataContainer &ctx, di::Dispatcher &dispatcher);
        EventOwner create_event(const std::vector<Token> &token);

    private:
        static uint32_t generate_id();
        const ChannelDataContainer &ctx;
        di::Dispatcher &dispatcher;
    };

    // ========================================================================== //
    // Event
    // -------------------------------------------------------------------------- //
    class BooleanExpression
    {
    public:
        virtual bool evaluate(const ChannelDataContainer &ctx) = 0;
    };
    using BooleanExpressionOwner = std::unique_ptr<BooleanExpression>;

    class EventStatusChange : public di::Event<EventStatusChange>
    {
    public:
        uint32_t id;
        EventState result;
        EventStatusChange(const uint32_t &id, const EventState &result) : id(id), result(result) {}
    };

    AGGREGATE class Event
    {
        friend EventOwner EventFactory::create_event(const std::vector<Token> &token);

    public:
        using State = EventState;
        uint32_t get_id() const;
        State get_result() const;
        void evaluate(const ChannelDataContainer &ctx);

    private:
        Event(const uint32_t id, BooleanExpressionOwner expression, di::Dispatcher &dispatcher);
        const uint32_t id;
        State result;
        BooleanExpressionOwner expression;
        di::Dispatcher &dispatcher;
    };

    // ========================================================================== //
    // Repository
    // -------------------------------------------------------------------------- //
    REPOSITORY class EventRepository
    {
    public:
        void take_event(EventOwner event);

        uint32_t get_number_of_events() const;
        Event &find_event_by_id(const uint32_t id) const;

        class Iterator
        {
            using InnerIt = typename std::unordered_map<uint32_t, EventOwner>::iterator;
            InnerIt it;

        public:
            Iterator(InnerIt inner);

            Event &operator*() const;
            Event *operator->() const;
            Iterator &operator++();
            bool operator!=(const Iterator &other) const;
        };
        Iterator begin();
        Iterator end();

        class ConstIterator
        {
            using InnerIt = typename std::unordered_map<uint32_t, EventOwner>::const_iterator;
            InnerIt it;

        public:
            ConstIterator(InnerIt inner);

            const Event &operator*() const;
            const Event *operator->() const;
            ConstIterator &operator++();
            bool operator!=(const ConstIterator &other) const;
        };
        ConstIterator begin() const;
        ConstIterator end() const;

    private:
        std::unordered_map<uint32_t, EventOwner> events;
    };

}
