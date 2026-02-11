#pragma once

#include "operand.hpp"
namespace data::monitor
{

    // ========================================================================== //
    // Constant
    // -------------------------------------------------------------------------- //
    template <typename T>
    Constant<T>::Constant(const T value) : value(value) {}

    template <typename T>
    T Constant<T>::get(const ChannelDataContainer &ctx) const
    {
        return this->value;
    }

    // ========================================================================== //
    // Channel
    // -------------------------------------------------------------------------- //
    template <typename T>
    Channel<T>::Channel(const uint32_t channelId) : channelId(channelId) {}

    template <typename T>
    class ChannelDataContainerVisitor
    {
    public:
        template <typename U>
        T operator()(const data::container::Scalar<U> &s) const
        {
            return static_cast<T>(s.get());
        }

        template <typename U>
        T operator()(const data::container::Vector<U> &v) const
        {
            throw std::runtime_error("no vector support in data container, now");
        }

        T operator()(const data::container::Message &m) const
        {
            throw std::runtime_error("cannot work with messages, here");
        }
    };

    template <typename T>
    T Channel<T>::get(const ChannelDataContainer &ctx) const
    {
        return std::visit(ChannelDataContainerVisitor<T>{}, ctx.get(this->channelId));
    }

}