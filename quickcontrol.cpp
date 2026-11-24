#include "quickcontrol.hpp"

using namespace std;

QuickControl::QuickControl(ConfigManager& m, ioManager& io)
    : configM(m),
      ioM(io)
{
    oldStates = 0;
}

void QuickControl::work()
{
    const uint8_t newStates = configM.getQCStates();

    if (newStates != oldStates) {
        oldStates = newStates;

    }
}
