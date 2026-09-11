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
{}

tAquaDaemon::~tAquaDaemon()
{
    queueM.stop();
}

void tAquaDaemon::run()
{
    signal(SIGINT, requestStop);
    signal(SIGTERM, requestStop);

    buttonC.startButtonCallback();

    while (!stopRequested)
    {   
        scheduler.work();
        queueM.work();
    }

    queueM.stop();
}
