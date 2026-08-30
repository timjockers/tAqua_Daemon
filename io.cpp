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


    for (Relay relay : RELAYS)
    {   
        const size_t index = relayIndex(relay);
        const unsigned int gpio = static_cast<unsigned int>(relay);

        relayLines[index] = gpiod_chip_get_line(chip, gpio);

        if (!relayLines[i]) {
            cerr
                << "Failed to get GPIO line "
                << gpio
                << ": "
                << strerror(errno)
                << endl;
            continue;
        }

        if (gpiod_line_request_output(
                relayLines[index],
                "tAqua_Daemon",
                0) < 0)
        {
            cerr
                << "Failed to request GPIO line "
                << gpio
                << ": "
                << strerror(errno)
                << endl;

            relayLines[index] = nullptr;
        }
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
