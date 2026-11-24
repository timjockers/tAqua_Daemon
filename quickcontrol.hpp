#pragma once

#include "fileexchange.hpp"

class QuickControl {
public:
    QuickControl(ConfigManager& m);
    
private:
    ConfigManager& configM;
};