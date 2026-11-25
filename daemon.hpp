#pragma once

#include "types.hpp"
#include "fileexchange.hpp"
#include "events.hpp"
#include "io.hpp"
#include "queue.hpp"
#include "button_controller.hpp"
#include "scheduler.hpp"
#include "quickcontrol.hpp"


#include <fstream>
#include <filesystem>


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

    bool web_pid_loaded;

    ConfigManager configM;
    LogManager logM;
    StatusManager statusM;
    ioManager ioM;
    QuickControl quickC;
    QueueManager queueM;
    ButtonController buttonC;
    Scheduler scheduler;
};

