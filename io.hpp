#pragma once


#include "types.hpp"
#include "fileexchange.hpp"
#include <array>
#include <cerrno>
#include <cstring>

#ifdef HAS_GPIOD
#include <gpiod.h>
#endif



static constexpr const char *GPIO_CHIP = "/dev/gpiochip0";


class ioManager {
public:
    ioManager(ConfigManager *cfgM);
    ~ioManager();

    void setRelay(Relay relay, int state);
    int getRelay(Relay relay);

private:
    ConfigManager *configM;

#ifdef HAS_GPIOD
    bool initGPIOOutputs(const unsigned int* gpios, size_t count, gpiod_line_request*& request);

    bool setGPIO(gpiod_line_request* request, unsigned int gpio, bool state);
    bool getGPIO(gpiod_line_request* request, unsigned int gpio);

    struct gpiod_chip *chip;
    gpiod_line_request* relayRequest;
#endif
};
