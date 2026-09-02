#pragma once


#include "types.hpp"
#include "fileexchange.hpp"
#include <array>
#include <cerrno>
#include <cstring>
#include <functional>
#include <thread>
#include <atomic>

#ifdef HAS_GPIOD
#include <gpiod.h>
#endif



static constexpr const char *GPIO_CHIP = "/dev/gpiochip0";


class ioManager {
public:
    using ButtonCallback = std::function<void(Button, bool)>;

    ioManager(ConfigManager *cfgM);
    ~ioManager();

    void setRelay(Relay relay, bool state);
    bool getRelay(Relay relay);

    void setYLED(YLED yled, bool state);
    bool getYLED(YLED yled);

    bool isButtonPressed(Button button);

    void setButtonCallback(ButtonCallback callback);
private:
    ConfigManager *configM;

    ButtonCallback buttonCallback;

#ifdef HAS_GPIOD
    bool initGPIOOutputs(const unsigned int* gpios, size_t count, gpiod_line_request*& request);
    bool initGPIOInputs(const Button* buttons, size_t count, gpiod_line_request*& request);

    bool setGPIO(gpiod_line_request* request, unsigned int gpio, bool state);
    bool getGPIO(gpiod_line_request* request, unsigned int gpio);

    bool setGPIOs(gpiod_line_request* request, std::array<unsigned int, 8> gpios, unsigned int states);

    gpiod_chip* chip = nullptr;
    gpiod_line_request* relayRequest = nullptr;
    gpiod_line_request* yledRequest = nullptr;
    gpiod_line_request* buttonRequest = nullptr;

    std::thread buttonThread;
    std::atomic<bool> buttonThreadRunning{false};
    void processButtonEvents();
#endif
};
