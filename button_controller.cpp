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
            handleButton(button, pressed);
        }
    );

    ioM->startButtonThread();
}

void ButtonController::handleButton(Button button, bool pressed)
{
    cout << "Button " << buttonIndex(button) << (pressed ? " PRESSED" : " RELEASED") << endl;

    if (!pressed) // Always handle button when released
    {
        const Relay relay = RELAYS[buttonIndex(button)];

        if (ioM->getRelay(relay))
        {
            queueM->cancelActiveEvent();
            return;
        }

        if (queueM->containsDurationEvent(relay))
        {
            queueM->removeQueuedDurationEvent(relay);
            return;
        }

        unique_ptr<durationEvent> event = make_unique<durationEvent>(
            relay,
            configM->getButtonIrrTime()
        );

        queueM->addEvent(std::move(event));
    }
}
