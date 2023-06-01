#include <cstdlib>
#include <fstream>
#include <iostream>

#include <Vector/DBC.h>

int main(int argc, char * argv[]) {
    Vector::DBC::Network network;

    if (argc != 2) {
        std::cout << "List_Messages_Signals <database.dbc>" << std::endl;
        return -1;
    }

    /* load database file */
    std::ifstream ifs(argv[1]);
    if (!ifs.is_open()) {
        std::cout << "Unable to open file" << std::endl;
        return EXIT_FAILURE;
    }
    ifs >> network;
    if (!network.successfullyParsed) {
        std::cout << "Unable to parse file" << std::endl;
        return EXIT_FAILURE;
    }

    /* loop over messages */
    for (const auto & message : network.messages) {
        std::cout << "Message " << message.second.name << std::endl;

        /* loop over signals of this messages */
        for (const auto & signal : message.second.signals)
            std::cout << "  Signal " << signal.second.name << std::endl;
    }

    return EXIT_SUCCESS;
}
