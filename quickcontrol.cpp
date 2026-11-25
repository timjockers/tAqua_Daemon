#include "quickcontrol.hpp"

using namespace std;

QuickControl::QuickControl(ConfigManager& m, ioManager& io)
    : configM(m),
      ioM(io)
{
    refresh();
}

void QuickControl::refresh()
{
    states = configM.getQCStates();
    updated = true;
}

void QuickControl::work()
{
    if (updated) {
        updated = false;

        ioM.setRelaysManual(states);
    }
}
