#include "daemon.hpp"

#include <thread>
#include <chrono>
#include <csignal>
#include <iostream>
#include <unistd.h>
#include <fstream>

using namespace std;

namespace
{
    volatile sig_atomic_t stopRequested = 0;
    volatile sig_atomic_t configReloadRequested = 0;

    void requestStop(int)
    {
        stopRequested = 1;
    }

    void requestConfigReload(int)
    {
        configReloadRequested = 1;
    }
}

tAquaDaemon::tAquaDaemon()
    : configM("taqua.cfg"),
      ioM(&configM),
      queueM(ioM),
      buttonC(&configM, &ioM, &queueM),
      scheduler(&queueM)
{
    scheduler.setEventVector(configM.getScheduledEvents());
}

tAquaDaemon::~tAquaDaemon()
{
    queueM.stop();
}

void tAquaDaemon::run()
{
    signal(SIGINT, requestStop);
    signal(SIGTERM, requestStop);
    
    signal(SIGUSR1, requestConfigReload);

    pid_t pid = getpid();

    filesystem::path pidPath = "taqua.pid";

    ofstream pidFile(pidPath);
    if (!pidFile)
    {
        cerr << "Could not open PID file" << endl;
        return;
    }
    pidFile << pid << '\n';
    if (!pidFile)
    {
        cerr << "Could not write PID file" << endl;
        return;
    }

    pidFile.close();

    
    buttonC.startButtonCallback();

    while (!stopRequested)
    {   
        scheduler.work();
        queueM.work();

        if (configReloadRequested)
        {
            configReloadRequested = 0;
            reloadConfig();
        }
    }

    queueM.stop();
    filesystem::remove(pidPath);
}

void tAquaDaemon::reloadConfig()
{
    cout << "reload" << endl;
}
