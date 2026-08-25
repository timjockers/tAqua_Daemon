#pragma once

#include <string>

class irrigationEvent {
public:
    virtual std::string getInfo();

    virtual void activate();
    virtual void deactivate();
    virtual bool isActive();
};