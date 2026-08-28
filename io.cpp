#include "io.hpp"

using namespace std;


ioManager::ioManager(ConfigManager *cfgM)
{
    configM = cfgM;

#ifdef HAS_GPIOD
    gpiod_chip_open(CHIP_PATH);
    if (!chip) {
        cerr << "Failed to open gpio chip." << endl;
    }
#endif
}

void ioManager::setRelay(int relay, int state)
{

}

int ioManager::getRelay(int relay)
{

}
