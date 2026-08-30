#include "io.hpp"

#include <iostream>
using namespace std;


ioManager::ioManager(ConfigManager *cfgM)
{
    configM = cfgM;

#ifdef HAS_GPIOD
    chip = gpiod_chip_open(GPIO_CHIP);

    if (!chip) {
        cerr << "Failed to open gpio chip: "
            << strerror(errno)
            << " (errno=" << errno << ")" << endl;
    }
#endif
}
ioManager::~ioManager()
{
#ifdef HAS_GPIOD
    gpiod_chip_close(chip);
#endif
}

void ioManager::setRelay(Relay relay, int state)
{
    if (configM->getRelayConfig(relay) == RelayConfig::VALVE)
    {
        cout << "Setting valve" << endl;
    }
}

int ioManager::getRelay(Relay relay)
{   
    if (configM->getRelayConfig(relay) == RelayConfig::UNUSED)
    {
        return 0;
    }

    return 1;
}
