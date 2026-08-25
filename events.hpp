#pragma once

#include <string>

class irrigationEvent {
public:
    std::string getInfo();

    void activate();
    void deactivate();
    bool isActive();
};