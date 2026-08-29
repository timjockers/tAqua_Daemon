#pragma once

#ifdef HAS_GPIOD
#include <gpiod.h>
#endif

#include "fileexchange.hpp"



#define CHIP_PATH "/dev/gpiochip0"



class ioManager {
public:
    ioManager(ConfigManager *cfgM);

    void setRelay(int relay, int state);
    int getRelay(int relay);

private:
    ConfigManager *configM;

#ifdef HAS_GPIOD
    struct gpiod_chip *chip;
#endif
};
