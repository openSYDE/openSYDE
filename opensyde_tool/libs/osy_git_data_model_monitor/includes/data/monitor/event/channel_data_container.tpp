#pragma once
#include <data/monitor/event/channel_data_container.hpp>

namespace data::monitor
{

    template <typename T>
    void ChannelDataContainer::create_uninitialized_channel(const uint32_t id, const uint32_t dim)
    {
        if (dim <= 1)
        {
            this->channels.emplace(id, Channel(data::container::Scalar<T>(), false));
        }
        else
        {
            this->channels.emplace(id, Channel(data::container::Vector<T>(dim), false));
        }
    }

}
