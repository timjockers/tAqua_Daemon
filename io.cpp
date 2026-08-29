#include "io.hpp"

using namespace std;


ioManager::ioManager(ConfigManager *cfgM)
{
    configM = cfgM;

#ifdef HAS_GPIOD
    chip = gpiod_chip_open(CHIP_PATH);

    if (!chip) {
        cerr << "Failed to open gpio chip: "
            << strerror(errno)
            << " (errno=" << errno << ")" << endl;
    }
#endif
}

void ioManager::setRelay(int relay, int state)
{
    if (configM->getRelayConfig(relay) == RC_VALVE)
    {
        cout << "Setting valve" << endl;
    }
}

int ioManager::getRelay(int relay)
{   
    if (configM->getRelayConfig(relay) == RC_UNUSED)
    {
        return 0;
    }

    return 1;
}
