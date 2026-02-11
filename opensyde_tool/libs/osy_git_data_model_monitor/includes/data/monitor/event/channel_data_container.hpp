#pragma once

#include <data/container.hpp>

#include <unordered_map>
#include <exception>
namespace data::monitor
{

    // ========================================================================== //
    // ChannelDataContainer
    // -------------------------------------------------------------------------- //
    class ChannelDataContainer
    {

    public:
        template <typename T>
        void create_uninitialized_channel(const uint32_t id, const uint32_t dim = 1);
        void create_initialized_channel(const uint32_t id, const data::container::StdDataVariant &channel);
        const data::container::StdDataVariant &get(const uint32_t id) const;
        const data::container::StdDataVariant &get_uninitialized(const uint32_t id) const;
        void set(const uint32_t id, const data::container::StdDataVariant &channel);
        bool has_channel(const uint32_t id) const;
        std::size_t number_of_channels_in_dataContainer();

    private:
        class Channel
        {
        public:
            Channel(const data::container::StdDataVariant &channel, bool isInitialized = false);
            void set(const data::container::StdDataVariant &channel);
            const data::container::StdDataVariant &get() const;
            const data::container::StdDataVariant &get_uninitialized() const;

        private:
            data::container::StdDataVariant channel;
            bool isInitialized;
        };
        std::unordered_map<uint32_t, Channel> channels;
    };
}

#include "channel_data_container.tpp"