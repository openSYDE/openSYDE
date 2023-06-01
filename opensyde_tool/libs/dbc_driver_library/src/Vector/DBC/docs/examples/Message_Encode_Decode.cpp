#include <cstdlib>
#include <iostream>

#include <Vector/DBC.h>

Vector::DBC::Network network;

void initDatabase() {
    /* define message 0x100 */
    Vector::DBC::Message & message_0x100 = network.messages[0x100];
    message_0x100.id = 0x100;
    message_0x100.name = "TestMessage";
    message_0x100.size = 4;

    /* define multiplexor switch signal */
    Vector::DBC::Signal & mp = message_0x100.signals["multiplexor"];
    mp.name = "multiplexor";
    mp.multiplexor = Vector::DBC::Signal::Multiplexor::MultiplexorSwitch;
    mp.startBit = 0;
    mp.bitSize = 8;
    mp.byteOrder = Vector::DBC::ByteOrder::LittleEndian;
    mp.valueType = Vector::DBC::ValueType::Unsigned;

    /* define signal 1 */
    Vector::DBC::Signal & signal_1 = message_0x100.signals["signal_1"];
    signal_1.name = "signal_1";
    signal_1.multiplexor = Vector::DBC::Signal::Multiplexor::MultiplexedSignal;
    signal_1.startBit = 8;
    signal_1.bitSize = 8;
    signal_1.byteOrder = Vector::DBC::ByteOrder::LittleEndian;
    signal_1.valueType = Vector::DBC::ValueType::Unsigned;
    signal_1.factor = 1;
    signal_1.offset = 0;

    /* define multiplexed signal 2a */
    Vector::DBC::Signal & signal_2a = message_0x100.signals["signal_2a"];
    signal_2a.name = "signal_2a";
    signal_2a.multiplexor = Vector::DBC::Signal::Multiplexor::MultiplexedSignal;
    signal_2a.multiplexerSwitchValue = 1;
    signal_2a.startBit = 16;
    signal_2a.bitSize = 16;
    signal_2a.byteOrder = Vector::DBC::ByteOrder::LittleEndian;
    signal_2a.valueType = Vector::DBC::ValueType::Unsigned;
    signal_2a.factor = 1;
    signal_2a.offset = 0;

    /* define multiplexed signal 2b */
    Vector::DBC::Signal & signal_2b = message_0x100.signals["signal_2b"];
    signal_2b.name = "signal_2b";
    signal_2b.multiplexor = Vector::DBC::Signal::Multiplexor::MultiplexedSignal;
    signal_2b.multiplexerSwitchValue = 2;
    signal_2b.startBit = 16;
    signal_2b.bitSize = 16;
    signal_2b.byteOrder = Vector::DBC::ByteOrder::LittleEndian;
    signal_2b.valueType = Vector::DBC::ValueType::Unsigned;
    signal_2b.factor = 1;
    signal_2b.offset = 0;
}

void decodeMessage(unsigned int & canIdentifier, std::vector<std::uint8_t> & canData) {
    /* get the relevant message from the database */
    Vector::DBC::Message & message = network.messages[canIdentifier];
    std::cout << "Message " << message.name << std::endl;

    /* loop over signals of this message to find and get multiplexor */
    unsigned int multiplexerSwitchValue = 0;
    for (const auto & signal : message.signals) {
        if (signal.second.multiplexor == Vector::DBC::Signal::Multiplexor::MultiplexorSwitch) {
            unsigned int rawValue = signal.second.decode(canData);
            multiplexerSwitchValue = rawValue;
            std::cout << "  this is a multiplexed message with switch value = "
                      << std::dec << multiplexerSwitchValue << std::endl;
        }
    }

    /* loop over signals of this messages */
    for (const auto & signal : message.signals) {
        switch (signal.second.multiplexor) {
        case Vector::DBC::Signal::Multiplexor::MultiplexorSwitch: {
            /* if it's the multiplexorSwitch, only show raw value */
            std::cout << "  Signal (MultiplexorSwitch) " << signal.second.name << std::endl;
            unsigned int rawValue = signal.second.decode(canData);
            std::cout << "    Raw Value: 0x" << std::hex << rawValue << std::endl;
        }
        break;
        case Vector::DBC::Signal::Multiplexor::MultiplexedSignal: {
            /* if it's an multiplexed signal check that the value matches */
            std::cout << "  Signal (MultiplexedSignal) " << signal.second.name << std::endl;
            unsigned int rawValue = signal.second.decode(canData);
            std::cout << "    Raw Value: 0x" << std::hex << rawValue << std::endl;
            double physicalValue = signal.second.rawToPhysicalValue(rawValue);
            std::cout << "    Physical Value: " << physicalValue << std::endl;
        }
        break;
        case Vector::DBC::Signal::Multiplexor::NoMultiplexor: {
            /* if it's a not a multiplexed signal just proceed it */
            std::cout << "  Signal " << signal.second.name << std::endl;
            unsigned int rawValue = signal.second.decode(canData);
            std::cout << "    Raw Value: 0x" << std::hex << rawValue << std::endl;
            double physicalValue = signal.second.rawToPhysicalValue(rawValue);
            std::cout << "    Physical Value: " << physicalValue << std::endl;
        }
        break;
        }
    }
}

int main() {
    /* init database */
    initDatabase();

    /* now let's assume we have received a CAN message with the following identifier and content... */
    unsigned int canIdentifier = 0x100;
    std::vector<std::uint8_t> canData;
    canData.reserve(4);
    network.messages[canIdentifier].signals["multiplexor"].encode(canData, 0);
    network.messages[canIdentifier].signals["signal_1"].encode(canData, 0x12);
    decodeMessage(canIdentifier, canData);

    /* set multiplexor to 1 to show signal_2a */
    std::cout << std::endl;
    std::cout << "set multiplexor switch value = 1" << std::endl;
    network.messages[canIdentifier].signals["multiplexor"].encode(canData, 1);
    network.messages[canIdentifier].signals["signal_2a"].encode(canData, 0x3456);
    decodeMessage(canIdentifier, canData);

    /* set multiplexor to 1 to show signal_2b */
    std::cout << std::endl;
    std::cout << "set multiplexor switch value = 2" << std::endl;
    network.messages[canIdentifier].signals["multiplexor"].encode(canData, 2);
    network.messages[canIdentifier].signals["signal_2b"].encode(canData, 0x789a);
    decodeMessage(canIdentifier, canData);

    return EXIT_SUCCESS;
}
