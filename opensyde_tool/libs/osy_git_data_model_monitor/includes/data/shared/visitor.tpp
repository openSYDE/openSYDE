#pragma once

namespace data::container
{

    // ========================================================================== //
    // Visitor
    // -------------------------------------------------------------------------- //
    template <typename T>
    void ChannelUpdateVisitor::operator()(T &ch) const
    {
        ch.read_from(this->buffer + this->offset);
    }

    template <typename T>
    void BufferUpdateVisitor::operator()(const T &ch) const
    {
        ch.write_to(this->buffer + this->offset);
    }

}