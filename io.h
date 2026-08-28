#pragma once

#ifdef HAS_GPIOD
#include <gpiod.hpp>
#endif

#include "fileexchange.hpp"


class ioManager {
public:
    ioManager(ConfigManager *cfgM);

private:
    ConfigManager *configM;
};
