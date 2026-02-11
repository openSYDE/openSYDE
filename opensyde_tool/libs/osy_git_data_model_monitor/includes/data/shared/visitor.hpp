#pragma once

#include <data/container.hpp>
#include <cstddef>
#include <variant>

namespace data::container
{

    // ========================================================================== //
    // Visitor
    // -------------------------------------------------------------------------- //
    class ChannelUpdateVisitor
    {
    public:
        ChannelUpdateVisitor(const uint8_t *buffer, const uint32_t offset);
        template <typename T>
        void operator()(T &ch) const;

    private:
        const uint8_t *buffer;
        uint32_t offset;
    };

    class BufferUpdateVisitor
    {
    public:
        BufferUpdateVisitor(uint8_t *buffer, const uint32_t offset);
        template <typename T>
        void operator()(const T &ch) const;

    private:
        uint8_t *buffer;
        uint32_t offset;
    };

}

#include <data/shared/visitor.tpp>