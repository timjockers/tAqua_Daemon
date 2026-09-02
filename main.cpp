#include "fileexchange.hpp"
#include "events.hpp"
#include "io.hpp"

#include <chrono>
#include <unistd.h>
#include <iostream>
#include <thread>

int main() {
    ConfigManager *manager = new ConfigManager("taqua.cfg");

    ioManager *ioM = new ioManager(manager);

    return 0;
}