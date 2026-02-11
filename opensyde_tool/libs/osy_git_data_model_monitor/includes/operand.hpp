#pragma once
#include <memory>
#include <data/monitor/event.hpp>

namespace data::monitor
{
    template <typename T>
    class Operand
    {
    public:
        virtual T get(const ChannelDataContainer &ctx) const = 0;
    };
    template <typename T>
    using OperandOwner = std::unique_ptr<Operand<T>>;

    template <typename T>
    class Constant : public Operand<T>
    {
    public:
        Constant(const T value);
        T get(const ChannelDataContainer &ctx) const override;

    private:
        const T value;
    };

    template <typename T>
    class Channel : public Operand<T>
    {
    public:
        Channel(const uint32_t channelId);
        T get(const ChannelDataContainer &ctx) const override;

    private:
        const uint32_t channelId;
    };

}

#include "operand.tpp"