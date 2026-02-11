#pragma once
#include <data/infrastructure/event_dispatcher.hpp>
#include <algorithm>

namespace data::infrastructure
{
    // ========================================================================== //
    // EventId
    // -------------------------------------------------------------------------- //
    template <typename T>
    size_t EventId::value()
    {
        static const size_t id = counter++;
        return id;
    }

    // ========================================================================== //
    // Event
    // -------------------------------------------------------------------------- //
    template <typename Derived>
    constexpr size_t Event<Derived>::get_type_id() { return EventId::value<Derived>(); }

    // ========================================================================== //
    // Dispatcher
    // -------------------------------------------------------------------------- //
    template <typename EventT>
    Dispatcher::HandlerId Dispatcher::subscribe(std::function<void(const EventT &)> handler)
    {
        auto [it, isNew] = this->handlers_.emplace(EventT::get_type_id(), std::vector<std::pair<HandlerId, std::function<void(const void *)>>>{});
        const HandlerId id = this->generate_handler_id();

        it->second.emplace_back(id, [handler = std::move(handler)](const void *e)
                                { handler(*static_cast<const EventT *>(e)); });

        return id;
    }

    template <typename EventT>
    void Dispatcher::unsubscribe(const HandlerId id)
    {
        auto it = this->handlers_.find(EventT::get_type_id());
        if (it == this->handlers_.end())
        {
            return;
        }

        auto &vec = it->second;
        vec.erase(std::remove_if(vec.begin(), vec.end(),
                                 [id](auto &pair)
                                 { return pair.first == id; }),
                  vec.end());
    }

    template <typename EventT>
    void Dispatcher::publish(const EventT &e) const
    {
        auto it = handlers_.find(EventT::get_type_id());
        if (it != handlers_.end())
        {
            for (const auto &handler : it->second)
            {
                handler.second(&e);
            }
        }
    }
}