#include "fileexchange.hpp"
#include "events.hpp"
#include "io.hpp"
#include "queue.hpp"
#include "types.hpp"

#include <chrono>
#include <unistd.h>
#include <iostream>
#include <thread>

int main() {
    ConfigManager *confManager = new ConfigManager("taqua.cfg");

    ioManager *ioM = new ioManager(confManager);

    QueueManager *queueManager = new QueueManager();

    
    ioM->setButtonCallback(
        [queueManager](Button button, bool pressed)
        {
            std::cout
                << "Button "
                << buttonIndex(button)
                << (pressed ? " PRESSED" : " RELEASED")
                << std::endl;
            
            if (!pressed)
            {   
                buttonEvent *e = new buttonEvent(
                    RELAYS[buttonIndex(button)],
                    std::chrono::seconds(5 * 60)
                );

                queueManager->addEvent(e);
            }
        }
    );

    ioM->startButtonThread();
    
    while (true)
    {
        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
    }

    return 0;
}