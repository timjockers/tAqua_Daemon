#include "button_controller.hpp"

#include <iostream>
using namespace std;


ButtonController::ButtonController(ConfigManager *configManager, ioManager *io_manager, QueueManager *queueManager)
    : configM(configManager), ioM(io_manager), queueM(queueManager)
{}

void ButtonController::startButtonCallback()
{
    ioM->setButtonCallback(
        [this](Button button, bool pressed)
        {
            cout << "LOG: ButtonController callback invoked for button " << buttonIndex(button) << " pressed=" << pressed << endl; // LOG
            handleButton(button, pressed);
        }
    );

    ioM->startButtonThread();
}

void ButtonController::handleButton(Button button, bool pressed)
{
    if (!pressed) // Always handle button when released
    {
        const Relay relay = RELAYS[buttonIndex(button)];

        if (ioM->getRelay(relay))
        {
            queueM->cancelActiveEvent();
            return;
        }

        if (queueM->containsButtonEvent(relay))
        {
            queueM->removeQueuedButtonEvent(relay);
            return;
        }

        unique_ptr<buttonEvent> event = make_unique<buttonEvent>(
            relay,
            configM->getButtonIrrTime()
        );

        queueM->addEvent(std::move(event));
    }
}
