#pragma once

#include "types.hpp"
#include "fileexchange.hpp"
#include "events.hpp"
#include "io.hpp"
#include "queue.hpp"
#include "button_controller.hpp"
#include "scheduler.hpp"

#include <atomic>


class tAquaDaemon {
public:
    tAquaDaemon();
    ~tAquaDaemon();
    
    void run();

private:
    void reloadConfig();

    ConfigManager configM;
    ioManager ioM;
    QueueManager queueM;
    ButtonController buttonC;
    Scheduler scheduler;
};

