#pragma once

#include "types.hpp"
#include "fileexchange.hpp"
#include "events.hpp"
#include "io.hpp"
#include "queue.hpp"
#include "button_controller.hpp"
#include "scheduler.hpp"


#include <fstream>


class tAquaDaemon {
public:
    tAquaDaemon();
    ~tAquaDaemon();
    
    void run();

private:
    void writePID();
    void deletePID();
    const std::filesystem::path pidPath = "taqua.pid";

    void reloadConfig();

    ConfigManager configM;
    ioManager ioM;
    QueueManager queueM;
    ButtonController buttonC;
    Scheduler scheduler;
};

