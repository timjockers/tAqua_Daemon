#pragma once

#include "types.hpp"
#include "fileexchange.hpp"
#include "events.hpp"
#include "io.hpp"
#include "queue.hpp"


class tAquaDaemon {
public:
    tAquaDaemon();
    
    void run();

private:
    void handleButton(Button button, bool pressed);

    ConfigManager configM;
    ioManager ioM;
    QueueManager queueM;
};

