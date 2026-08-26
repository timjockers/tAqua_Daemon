#include "fileexchange.hpp"

#include "events.hpp"
#include <chrono>
#include <unistd.h>
#include <iostream>

int main() {
    buttonEvent *e = new buttonEvent(0, std::chrono::seconds(10));
    e->activate();
    while (e->isActive())
    {
        sleep(1);
        std::cout << "Active" << std::endl;
    }
    std::cout << "Deactive" << std::endl;

    ConfigManager *manager = new ConfigManager("taqua.cfg");

    return 0;
}