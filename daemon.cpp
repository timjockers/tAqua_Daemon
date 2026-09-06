#include "daemon.hpp"

#include <thread>
#include <chrono>
#include <iostream>
#include <memory>

using namespace std;

tAquaDaemon::tAquaDaemon()
    : configM("taqua.cfg"),
      ioM(&configM),
      queueM()
{}

void tAquaDaemon::run()
{
    ioM.setButtonCallback(
        [this](Button button, bool pressed)
        {
            handleButton(button, pressed);
        }
    );

    ioM.startButtonThread();

    while (true)
    {   
        queueM.work();

        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void tAquaDaemon::handleButton(Button button, bool pressed)
{
    cout << "Button " << buttonIndex(button) << (pressed ? " PRESSED" : " RELEASED") << endl;
            
    if (!pressed) // Add event when button is released
    {   
        unique_ptr<buttonEvent> event = make_unique<buttonEvent>(
            RELAYS[buttonIndex(button)],
            chrono::seconds(5 * 60)
        );

        queueM.addEvent(std::move(event));
    }
}
