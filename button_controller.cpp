#include "button_controller.hpp"

#include <iostream>
using namespace std;


ButtonController::ButtonController(ConfigManager *configManager, ioManager *io_manager, QueueManager *queueManager)
    : configM(configManager), ioM(io_manager), queueM(queueManager)
{}

void ButtonController::startButtonCallback()
{
    cout << "LOG: ButtonController::startButtonCallback() register GPIO callback" << endl; // LOG
    ioM->setButtonCallback(
        [this](Button button, bool pressed)
        {
            cout << "LOG: ButtonController callback invoked for button " << buttonIndex(button) << " pressed=" << pressed << endl; // LOG
            handleButton(button, pressed);
        }
    );

    cout << "LOG: ButtonController::startButtonCallback() start button thread" << endl; // LOG
    ioM->startButtonThread();
}

void ButtonController::handleButton(Button button, bool pressed)
{
    cout << "Button " << buttonIndex(button) << (pressed ? " PRESSED" : " RELEASED") << endl;
    cout << "LOG: ButtonController::handleButton() button " << buttonIndex(button) << " pressed=" << pressed << endl; // LOG

    if (!pressed) // Always handle button when released
    {
        cout << "LOG: ButtonController::handleButton() release event for button " << buttonIndex(button) << endl; // LOG
        const Relay relay = RELAYS[buttonIndex(button)];

        if (ioM->getRelay(relay))
        {
            cout << "LOG: ButtonController::handleButton() cancel active event on relay " << relayIndex(relay) << endl; // LOG
            queueM->cancelActiveEvent();
            return;
        }

        if (queueM->containsDurationEvent(relay))
        {
            cout << "LOG: ButtonController::handleButton() remove queued duration event for relay " << relayIndex(relay) << endl; // LOG
            queueM->removeQueuedDurationEvent(relay);
            return;
        }

        cout << "LOG: ButtonController::handleButton() create button duration event for relay " << relayIndex(relay) << endl; // LOG
        unique_ptr<durationEvent> event = make_unique<durationEvent>(
            relay,
            configM->getButtonIrrTime()
        );

        cout << "LOG: ButtonController::handleButton() queue button event " << event->getInfo() << endl; // LOG
        queueM->addEvent(std::move(event));
    }
}
