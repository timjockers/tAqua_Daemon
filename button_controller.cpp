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

    if (!pressed) // Add event when button is released
    {
        const Relay relay = RELAYS[buttonIndex(button)];

        if (queueM->containsButtonEvent(relay))
        {
            return;
        }

        unique_ptr<buttonEvent> event = make_unique<buttonEvent>(
            relay,
            chrono::seconds(30)
        );

        queueM->addEvent(std::move(event));
    }
}
