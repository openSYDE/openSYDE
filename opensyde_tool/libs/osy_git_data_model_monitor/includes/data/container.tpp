#pragma once

#include <data/container.hpp>

#include <cstring>
#include <stdexcept>

namespace data::container
{
    // ========================================================================== //
    // Channel
    // -------------------------------------------------------------------------- //
    template <typename T>
    constexpr DataType datatype_of()
    {
        if constexpr (std::is_same_v<T, uint8_t>)
            return DataType::UINT8;
        else if constexpr (std::is_same_v<T, uint16_t>)
            return DataType::UINT16;
        else if constexpr (std::is_same_v<T, uint32_t>)
            return DataType::UINT32;
        else if constexpr (std::is_same_v<T, int8_t>)
            return DataType::INT8;
        else if constexpr (std::is_same_v<T, int16_t>)
            return DataType::INT16;
        else if constexpr (std::is_same_v<T, int32_t>)
            return DataType::INT32;
        else if constexpr (std::is_same_v<T, float>)
            return DataType::FLOAT;
        return DataType::CUSTOM;
    }

    // ========================================================================== //
    // Scalar
    // -------------------------------------------------------------------------- //
    template <typename T>
    Scalar<T>::Scalar(const T &value)
        : value(value) {}

    template <typename T>
    Scalar<T>::Scalar(const Scalar &scalar)
        : value(scalar.value) {}

    template <typename T>
    void Scalar<T>::read_from(const uint8_t *buffer)
    {
        memcpy(&this->value, buffer, sizeof(T));
    }

    template <typename T>
    void Scalar<T>::write_to(uint8_t *buffer) const
    {
        memcpy(buffer, &this->value, sizeof(T));
    }

    template <typename T>
    const size_t Scalar<T>::size() const
    {
        return sizeof(T) + this->byteOffset;
    }

    template <typename T>
    const T &Scalar<T>::get() const
    {
        return this->value;
    }

    template <typename T>
    void Scalar<T>::set(const T &value)
    {
        this->value = value;
    }

    template <typename T>
    const DataContainerInfo &Scalar<T>::get_type() const
    {
        static const DataContainerInfo info{ContainerType::Scalar, datatype_of<T>()};
        return info;
    }

    // ========================================================================== //
    // Vector
    // -------------------------------------------------------------------------- //
    template <typename T>
    Vector<T>::Vector(const size_t length)
        : value(std::make_unique<T[]>(length)), len(length) {}

    template <typename T>
    Vector<T>::Vector(const Vector &vector)
        : value(std::make_unique<T[]>(vector.len)), len(vector.len)
    {
        for (size_t pos = 0; pos < vector.len; ++pos)
        {
            this->value[pos] = vector[pos];
        }
    }

    template <typename T>
    Vector<T> &Vector<T>::operator=(const Vector &v)
    {
        if (not this->len == v.len)
        {
            std::out_of_range("Cannot copy Vector! Resizing the vector is not allowed in container class implementation!");
        }
        for (size_t n = 0; n < this->len; ++n)
        {
            this->value[n] = v[n];
        }
        return *this;
    }

    template <typename T>
    void Vector<T>::read_from(const uint8_t *buffer)
    {
        memcpy(this->value.get(), buffer, this->len * sizeof(T));
    }

    template <typename T>
    void Vector<T>::write_to(uint8_t *buffer) const
    {
        memcpy(buffer, this->value.get(), this->len * sizeof(T));
    }

    template <typename T>
    const T *Vector<T>::get() const
    {
        return this->value;
    }

    template <typename T>
    const size_t Vector<T>::size() const
    {
        return this->len * sizeof(T);
    }

    template <typename T>
    const size_t Vector<T>::length() const
    {
        return this->len;
    }

    template <typename T>
    const T &Vector<T>::operator[](size_t index) const
    {
        return this->value[index];
    }

    template <typename T>
    T &Vector<T>::operator[](size_t index)
    {
        return this->value[index];
    }

    template <typename T>
    const DataContainerInfo &Vector<T>::get_type() const
    {
        static const DataContainerInfo info{ContainerType::Vector, datatype_of<T>()};
        return info;
    }

}