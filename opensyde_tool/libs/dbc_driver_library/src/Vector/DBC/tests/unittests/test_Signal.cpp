#define BOOST_TEST_MODULE Signal
#if !defined(WIN32)
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <iterator>
#include <limits>
#include <boost/filesystem.hpp>

#include <Vector/DBC.h>

/**
 * Check raw to physical and vice-versa functions.
 * Check min/max functions.
 */
BOOST_AUTO_TEST_CASE(SignalRawPhysicalMinMax) {
    Vector::DBC::Signal signal;

    /* check for factor and offset */
    signal.factor = 1;
    signal.offset = 0;
    BOOST_CHECK_EQUAL(signal.rawToPhysicalValue(1.0), 1.0);
    BOOST_CHECK_EQUAL(signal.physicalToRawValue(1.0), 1.0);

    /* check for minimum and maximum */
    // undefined
    signal.bitSize = 16;
    signal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Undefined;
    signal.valueType = Vector::DBC::ValueType::Signed;
    BOOST_CHECK_EQUAL(signal.minimumRawValue(), -32768.0);
    BOOST_CHECK_EQUAL(signal.maximumRawValue(), 32767.0);
    signal.valueType = Vector::DBC::ValueType::Unsigned;
    BOOST_CHECK_EQUAL(signal.minimumRawValue(), 0.0);
    BOOST_CHECK_EQUAL(signal.maximumRawValue(), 65535.0);
    // integer
    signal.bitSize = 16;
    signal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Integer;
    signal.valueType = Vector::DBC::ValueType::Signed;
    BOOST_CHECK_EQUAL(signal.minimumRawValue(), -32768.0);
    BOOST_CHECK_EQUAL(signal.maximumRawValue(), 32767.0);
    signal.valueType = Vector::DBC::ValueType::Unsigned;
    BOOST_CHECK_EQUAL(signal.minimumRawValue(), 0.0);
    BOOST_CHECK_EQUAL(signal.maximumRawValue(), 65535.0);
    // float
    signal.bitSize = 32;
    signal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Float;
    BOOST_CHECK_EQUAL(signal.minimumRawValue(), std::numeric_limits<float>::min());
    BOOST_CHECK_EQUAL(signal.maximumRawValue(), std::numeric_limits<float>::max());
    // double
    signal.bitSize = 64;
    signal.extendedValueType = Vector::DBC::Signal::ExtendedValueType::Double;
    BOOST_CHECK_EQUAL(signal.minimumRawValue(), std::numeric_limits<double>::min());
    BOOST_CHECK_EQUAL(signal.maximumRawValue(), std::numeric_limits<double>::max());
}

/**
 * Check signal decode/encode functions.
 */
BOOST_AUTO_TEST_CASE(SignalDecodeEncode) {
    /* construct a signal */
    Vector::DBC::Signal signal;
    signal.startBit = 1;
    signal.bitSize = 4;
    signal.byteOrder = Vector::DBC::ByteOrder::LittleEndian;
    signal.valueType = Vector::DBC::ValueType::Signed;

    /* extract negative signal */
    std::vector<uint8_t> data;
    data.push_back(0x10); // xxx1000x
    BOOST_CHECK_EQUAL(signal.decode(data),  0xFFFFFFFFFFFFFFF8);

    /* extract positive signal */
    data[0] = 0x0E; // xxx0111x
    BOOST_CHECK_EQUAL(signal.decode(data),  0x07);

    /* check bitSize = 0 */
    signal.bitSize = 0;
    BOOST_CHECK_EQUAL(signal.decode(data), 0);
    signal.encode(data, 3);
    BOOST_CHECK_EQUAL(data[0], 0x0E);
}

/**
 * Checks that signal decode/encode and raw to physical and vice-versa functions
 * work in combination.
 */
BOOST_AUTO_TEST_CASE(BothFunctionsCombined) {
    /* construct a signal */
    Vector::DBC::Signal signal;
    signal.startBit = 0;
    signal.bitSize = 10;
    signal.byteOrder = Vector::DBC::ByteOrder::LittleEndian;
    signal.valueType = Vector::DBC::ValueType::Signed;
    signal.factor = 0.25;
    signal.offset = 0.0;

    /* construct raw data */
    std::vector<uint8_t> data;
    union {
        uint64_t unsignedDecodedData {};
        int64_t signedDecodedData;
    };
    double rawValue = 0.0;
    double physicalValue = 0.0;

    /* signed: data = 1000011010b ==> rawValue=0x21A ==> physicalValue = -121.5 */
    data.clear();
    data.push_back(0x1A); // 0001 1010
    data.push_back(0x02); // 10
    unsignedDecodedData = signal.decode(data);
    BOOST_CHECK_EQUAL(unsignedDecodedData & 0x3FF, 0x21A);
    switch (signal.valueType) {
    case Vector::DBC::ValueType::Unsigned:
        rawValue = unsignedDecodedData;
        break;
    case Vector::DBC::ValueType::Signed:
        rawValue = signedDecodedData;
        break;
    }
    physicalValue = signal.rawToPhysicalValue(rawValue);
    BOOST_CHECK_EQUAL(physicalValue, -121.5);

    /* unsigned: data = 0111010011b ==> rawValue=0x1D3 ==> physicalValue = 116.75 */
    data.clear();
    data.push_back(0xD3); // 1101 0011
    data.push_back(0x01); // 01
    unsignedDecodedData = signal.decode(data);
    BOOST_CHECK_EQUAL(unsignedDecodedData & 0x3FF, 0x1D3);
    switch (signal.valueType) {
    case Vector::DBC::ValueType::Unsigned:
        rawValue = unsignedDecodedData;
        break;
    case Vector::DBC::ValueType::Signed:
        rawValue = signedDecodedData;
        break;
    }
    physicalValue = signal.rawToPhysicalValue(rawValue);
    BOOST_CHECK_EQUAL(physicalValue, 116.75);
}
