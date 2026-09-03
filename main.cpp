#include "fileexchange.hpp"
#include "events.hpp"
#include "io.hpp"

#include <chrono>
#include <unistd.h>
#include <iostream>
#include <thread>

int main() {
    buttonEvent *e = new buttonEvent(Relay::R2, std::chrono::seconds(10));

    std::cout << e->getInfo() << std::endl << std::endl;
    
    e->activate();

    while (e->isActive())
    {
        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
        std::cout << "Running" << std::endl;
    }
    std::cout << "Done" << std::endl;
    return 0;


    ConfigManager *manager = new ConfigManager("taqua.cfg");

    ioManager *ioM = new ioManager(manager);

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