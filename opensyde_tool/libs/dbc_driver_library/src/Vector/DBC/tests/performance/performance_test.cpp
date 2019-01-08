/*
 * Copyright (C) 2013 Tobias Lorenz.
 * Contact: tobias.lorenz@gmx.net
 *
 * This file is part of Tobias Lorenz's Toolkit.
 *
 * Commercial License Usage
 * Licensees holding valid commercial licenses may use this file in
 * accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Tobias Lorenz.
 *
 * GNU General Public License 3.0 Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 */

#include <cassert>
#include <chrono>
#include <iostream>
#include <string>

#include "Vector/DBC.h"

/** number of test iterations */
const int measurements = 10000;

/**
 * This measures the lookup time per messages.
 *
 * The columns are:
 * - Number of messages in database (random in range 1..1000)
 * - Measured lookup time (nanoseconds)
 */
void performance_test_1()
{
    Vector::DBC::Network network;

    /* multiple measurement loops */
    for (int i = 0; i < measurements; ++i) {
        unsigned int messageCount = (rand() % 1000) + 1;

        /* setup the database with random number of messages */
        for (unsigned int id = 0; id < messageCount; ++id) {
            Vector::DBC::Message & message = network.messages[id];
            message.id = id;
            message.name = "message_" + std::to_string(id);
            message.size = 8;
        }

        /* now just take a random number */
        unsigned int id = rand() % messageCount;

        /* and look it up */
        auto t1 = std::chrono::high_resolution_clock::now();
        Vector::DBC::Message & message = network.messages[id];
        auto t2 = std::chrono::high_resolution_clock::now();
        assert(message.name == "message_" + std::to_string(id));

        /* print result */
        std::chrono::nanoseconds ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1);
        std::cout << messageCount << "\t" << ns.count() << std::endl;
    }
}

/**
 * This measures the time to loop over all signals within a message.
 *
 * The generated columns are:
 * - Number of signals in message (random in range 1..8)
 * - Measured lookup time (nanoseconds)
 */
void performance_test_2()
{
    Vector::DBC::Message message;

    /* multiple measurement loops */
    for (int i = 0; i < measurements; ++i) {
        unsigned int signalCount = (rand() % 8) + 1;

        /* setup the message with random number of signals */
        for (unsigned int nr = 0; nr < signalCount; ++nr) {
            std::string signalName = "signal_" + std::to_string(nr);
            Vector::DBC::Signal & signal = message.signals[signalName];
            signal.name = signalName;
            signal.startBit = 8 * nr;
            signal.bitSize = 8;
        }

        /* and look it up */
        auto t1 = std::chrono::high_resolution_clock::now();
        for (auto signal : message.signals) {
            std::string & signalName = signal.second.name;
        }
        auto t2 = std::chrono::high_resolution_clock::now();

        /* print result */
        std::chrono::nanoseconds ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1);
        std::cout << signalCount << "\t" << ns.count() << std::endl;
    }
}

/**
 * This measures the time to decode a signal.
 *
 * The generated columns are:
 * - Bit size of signal (random in range 1..64)
 * - Measured decode time (nanoseconds)
 */
void performance_test_3(Vector::DBC::ByteOrder byteOrder, Vector::DBC::ValueType valueType)
{
    /* multiple measurement loops */
    for (int i = 0; i < measurements; ++i) {
        unsigned int bitSize = (rand() % 64) + 1;

        /* setup 8 byte random data */
        std::vector<uint8_t> data;
        for (int b = 0; b < 8; ++b)
            data.push_back(rand() % 0x100);

        /* setup signal */
        Vector::DBC::Signal signal;
        signal.byteOrder = byteOrder;
        signal.startBit = 0;
        signal.bitSize = bitSize;
        signal.valueType = valueType;

        /* and decode it */
        auto t1 = std::chrono::high_resolution_clock::now();
        std::uint64_t value = signal.decode(data);
        auto t2 = std::chrono::high_resolution_clock::now();

        /* print result */
        std::chrono::nanoseconds ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1);
        std::cout << bitSize << "\t" << ns.count() << std::endl;
    }
}

int main(int argc, char **argv)
{
    /* safety check */
    if (argc != 2) {
        std::cout << "Syntax: performance_test <test id>" << std::endl;
        return -1;
    }

    std::string id;
    id.assign(argv[1]);

    if (id == "1")
        performance_test_1();
    else
    if (id == "2")
        performance_test_2();
    else
    if (id == "3ls")
        performance_test_3(Vector::DBC::ByteOrder::LittleEndian, Vector::DBC::ValueType::Signed);
    else
    if (id == "3lu")
        performance_test_3(Vector::DBC::ByteOrder::LittleEndian, Vector::DBC::ValueType::Unsigned);
    else
    if (id == "3bs")
        performance_test_3(Vector::DBC::ByteOrder::BigEndian, Vector::DBC::ValueType::Signed);
    else
    if (id == "3bu")
        performance_test_3(Vector::DBC::ByteOrder::BigEndian, Vector::DBC::ValueType::Unsigned);

    return 0;
}
