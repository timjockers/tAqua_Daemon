#include "fileexchange.hpp"
#include "events.hpp"
#include "io.hpp"

#include <chrono>
#include <unistd.h>
#include <iostream>
#include <thread>

int main() {
    /*buttonEvent *e = new buttonEvent(0, std::chrono::seconds(10));

    std::cout << e->getInfo() << std::endl;

    e->activate();
    while (e->isActive())
    {
        sleep(1);
        std::cout << "Active" << std::endl;
    }
    std::cout << "Deactive" << std::endl;*/

    ConfigManager *manager = new ConfigManager("taqua.cfg");

    ioManager *ioM = new ioManager(manager);

    /*int relay;
    int state;
    while (true)
    {
        std::cout << "\nRelay: ";
        std::cin >> relay;

        std::cout << "\nState: ";
        std::cin >> state;

        ioM->setRelay(RELAYS[relay], state > 0);
        ioM->setYLED(YLEDS[relay], state > 0);

        std::cout << "YLED States:";
        for (unsigned int i = 0; i < 8; i++)
        {
            std::cout << ioM->getYLED(YLEDS[i]) << " ";
        }
        std::cout << std::endl;
    }*/
    while (true)
    {
        for (unsigned int i = 0; i < 8; i++)
        {
            std::cout << ioM->isButtonPressed(BUTTONS[i]) << " ";
        }
        std::cout << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}