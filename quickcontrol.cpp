#include "quickcontrol.hpp"

#include <chrono>
#include <thread>

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

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
