#pragma once

#include "types.hpp"
#include "fileexchange.hpp"
#include "io.hpp"
#include "queue.hpp"


class ButtonController {
public:
    explicit ButtonController(ConfigManager *configManager, ioManager *io_manager, QueueManager *queueManager);

    void startButtonCallback();

private:
    void handleButton(Button button, bool pressed);
    
    ConfigManager *configM;
    ioManager *ioM;
    QueueManager *queueM;
};
