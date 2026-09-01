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

    void setRelay(Relay relay, bool state);
    bool getRelay(Relay relay);

    void setYLED(YLED yled, bool state);
    bool getYLED(YLED yled);

private:
    ConfigManager *configM;

#ifdef HAS_GPIOD
    bool initGPIOOutputs(const unsigned int* gpios, size_t count, gpiod_line_request*& request);

    bool setGPIO(gpiod_line_request* request, unsigned int gpio, bool state);
    bool getGPIO(gpiod_line_request* request, unsigned int gpio);

    bool setGPIOs(gpiod_line_request* request, std::array<unsigned int, 8> gpios, unsigned int states);

    struct gpiod_chip* chip;
    gpiod_line_request* relayRequest;
    gpiod_line_request* yledRequest;
#endif
};
