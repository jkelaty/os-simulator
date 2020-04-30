/// @author Jonathan Kelaty
/// @file CS OS Home Project - main.cpp
/// @date 2020-04-14
/// @brief Main function which accepts RAM size and HDD count
/// for simulated OS, then creates OS object and calls run()
/// to begin simulated operating system.

#include <iostream>

#include "DataTypes.h"
#include "OS.h"

int main() {

    uint RAM_size{ 0 };
    uint HDD_count{ 0 };

    std::cout << "\n\tHow much RAM (in bytes) should the simulated computer use?\n\n>> ";
    std::cin >> RAM_size;

    std::cout << "\n\tHow many HDDs should the simulated computer use?\n\n>> ";
    std::cin >> HDD_count;

    OS os(RAM_size, HDD_count);
    os.run();

    return 0;
}
