#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <mutex>
namespace data::container
{
    // ========================================================================== //
    // UndefinedDataStatus
    // -------------------------------------------------------------------------- //
    class UndefinedDataStatus : public std::exception
    {
    public:
        const char *what() const noexcept override;
    };

    // ========================================================================== //
    // DataContainer
    // -------------------------------------------------------------------------- //
    enum class ContainerType
    {
        Scalar,
        Vector,
        Message,
        Custom
    };

    enum class DataType
    {
        UINT8,
        UINT16,
        UINT32,
        INT8,
        INT16,
        INT32,
        FLOAT,
        STRING,
        CUSTOM
    };

    struct DataContainerInfo
    {
        ContainerType channelType;
        DataType dataType;
        bool operator==(const DataContainerInfo &other) const
        {
            return channelType == other.channelType && dataType == other.dataType;
        }
        DataContainerInfo(const ContainerType ct, const DataType dt)
            : channelType(ct), dataType(dt) {}
    };

    class DataContainer
    {
    public:
        virtual const DataContainerInfo &get_type() const = 0;
        void raise_undefined_status_if_not_initialized() const;
        bool is_initialized() const;

    protected:
        void initialized();

    private:
        bool isInitialized = false;
    };

    // ========================================================================== //
    // Scalar
    // -------------------------------------------------------------------------- //
    template <typename T>
    class Scalar : public DataContainer
    {
    public:
        Scalar() = default;
        Scalar(const T &value);
        Scalar(const Scalar &s);
        Scalar(Scalar &&) = default;
        Scalar &operator=(const Scalar &) = default;

        void read_from(const uint8_t *);
        void write_to(uint8_t *) const;
        const size_t size() const;
        const T &get() const;
        void set(const T &value);

        const DataContainerInfo &get_type() const override;

    protected:
        T value;

    private:
        // std::mutex vector_mutex;
    };

    // ========================================================================== //
    // Vector
    // -------------------------------------------------------------------------- //
    template <typename T>
    class Vector : public DataContainer
    {
    public:
        Vector(const size_t length);
        Vector(const Vector &v);
        Vector(Vector &&) = default;
        Vector &operator=(const Vector &);
        void read_from(const uint8_t *);
        void write_to(uint8_t *) const;
        const size_t size() const;
        const size_t length() const;
        const T *get() const;
        const T &operator[](size_t index) const;
        T &operator[](size_t index);

        const DataContainerInfo &get_type() const override;

    private:
        std::unique_ptr<T[]> value;
        const size_t len;
    };

    // ========================================================================== //
    // Message
    // -------------------------------------------------------------------------- //
    class Message : public DataContainer
    {
    public:
        Message();
        Message(const Message &);
        Message(Message &&);
        Message &operator=(const Message &) = default;
        void read_from(const uint8_t *);
        void write_to(uint8_t *) const;
        const size_t size() const;
        const std::string &get() const;
        void set(const std::string &);

        const DataContainerInfo &get_type() const override;

    private:
        std::string message;
    };

    // ========================================================================== //
    // StdVariants
    // -------------------------------------------------------------------------- //
    template <typename... Ts>
    using DataContainerVariant = std::variant<Ts...>;

    using StdDataVariant = DataContainerVariant<
        Scalar<uint8_t>,
        Scalar<uint16_t>,
        Scalar<uint32_t>,
        Scalar<int8_t>,
        Scalar<int16_t>,
        Scalar<int32_t>,
        Scalar<float>,
        Vector<uint8_t>,
        Vector<uint16_t>,
        Vector<uint32_t>,
        Vector<int8_t>,
        Vector<int16_t>,
        Vector<int32_t>,
        Vector<float>,
        Message>;

};

#include <data/container.tpp>