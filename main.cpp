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

    ioM->setButtonCallback(
        [](Button button, bool pressed)
        {
            std::cout
                << "Button "
                << static_cast<int>(button)
                << (pressed ? " PRESSED" : " RELEASED")
                << std::endl;
        }
    );
    
    while (true)
    {
        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
    }

    return 0;
}