#pragma once

#include "fileexchange.hpp"
#include "io.hpp"

class QuickControl {
public:
    QuickControl(ConfigManager& m, ioManager& io);

    void work();

private:
    ConfigManager& configM;
    ioManager& ioM;

    std::uint8_t oldStates;
};