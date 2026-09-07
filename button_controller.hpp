#pragma once

#include "types.hpp"
#include "fileexchange.hpp"
#include "queue.hpp"


class ButtonController {
public:
    explicit ButtonController(ConfigManager *configManager, QueueManager *queueManager);

private:
    ConfigManager *configM;
    QueueManager *queueM;
};
