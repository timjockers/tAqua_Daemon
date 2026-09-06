#include "daemon.hpp"

#include <thread>
#include <chrono>
#include <csignal>
#include <iostream>
#include <memory>

using namespace std;

namespace
{
    volatile sig_atomic_t stopRequested = 0;

    void requestStop(int)
    {
        stopRequested = 1;
    }
}

tAquaDaemon::tAquaDaemon()
    : configM("taqua.cfg"),
      ioM(&configM),
      queueM(ioM)
{}

tAquaDaemon::~tAquaDaemon()
{
    queueM.stop();
}

void tAquaDaemon::run()
{
    signal(SIGINT, requestStop);
    signal(SIGTERM, requestStop);

    ioM.setButtonCallback(
        [this](Button button, bool pressed)
        {
            handleButton(button, pressed);
        }
    );

    ioM.startButtonThread();

    while (!stopRequested)
    {
        queueM.work();
    }

    queueM.stop();
}

void tAquaDaemon::handleButton(Button button, bool pressed)
{
    cout << "Button " << buttonIndex(button) << (pressed ? " PRESSED" : " RELEASED") << endl;
            
    if (!pressed) // Add event when button is released
    {   
        unique_ptr<buttonEvent> event = make_unique<buttonEvent>(
            RELAYS[buttonIndex(button)],
            chrono::seconds(30)
        );

        queueM.addEvent(std::move(event));
    }
}
