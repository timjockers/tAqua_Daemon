#pragma once

#include "types.hpp"
#include "fileexchange.hpp"
#include "io.hpp"
#include "queue.hpp"


class ButtonController {
public:
    explicit ButtonController(ConfigManager *configManager, ioManager *io_manager, QueueManager *queueManager);

private:
    ConfigManager *configM;
    ioManager *ioM;
    QueueManager *queueM;
};
