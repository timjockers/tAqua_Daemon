#include "daemon.hpp"

#include <thread>
#include <chrono>
#include <csignal>
#include <iostream>
#include <memory>

using namespace std;

namespace
{
    volatile sig_atomic_t stopRequested = 0;

    void requestStop(int)
    {
        stopRequested = 1;
    }
}

tAquaDaemon::tAquaDaemon()
    : configM("taqua.cfg"),
      ioM(&configM),
      queueM(ioM),
      buttonC(&configM, &ioM, &queueM),
      scheduler(&queueM)
{
    cout << "LOG: tAquaDaemon constructor loads schedule with " << configM.getScheduledEvents().size() << " events" << endl; // LOG
    scheduler.setEventVector(configM.getScheduledEvents());
}

tAquaDaemon::~tAquaDaemon()
{
    cout << "LOG: tAquaDaemon destructor stops queue" << endl; // LOG
    queueM.stop();
}

void tAquaDaemon::run()
{
    cout << "LOG: tAquaDaemon::run() install signal handlers" << endl; // LOG
    signal(SIGINT, requestStop);
    signal(SIGTERM, requestStop);
    
    cout << "LOG: tAquaDaemon::run() start button callback" << endl; // LOG
    buttonC.startButtonCallback();

    while (!stopRequested)
    {   
        scheduler.work();
        queueM.work();
    }

    cout << "LOG: tAquaDaemon::run() shutting down and stopping queue" << endl; // LOG
    queueM.stop();
}
