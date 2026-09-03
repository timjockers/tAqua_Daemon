#include "fileexchange.hpp"
#include "events.hpp"
#include "io.hpp"

#include <chrono>
#include <unistd.h>
#include <iostream>
#include <thread>

int main() {
    ConfigManager *confManager = new ConfigManager("taqua.cfg");

    ioManager *ioM = new ioManager(confManager);

    ioM->setButtonCallback(
        [](Button button, bool pressed)
        {
            std::cout
                << "Button "
                << buttonIndex(button)
                << (pressed ? " PRESSED" : " RELEASED")
                << std::endl;
        }
    );

    ioM->startButtonThread();
    
    while (true)
    {
        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
    }

    return 0;
}