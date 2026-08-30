#include "io.hpp"

#include <iostream>
using namespace std;


ioManager::ioManager(ConfigManager *cfgM)
{
    configM = cfgM;

#ifdef HAS_GPIOD
    chip = gpiod_chip_open(GPIO_CHIP);

    if (!chip) {
        cerr << "Failed to open gpio chip: "
            << strerror(errno)
            << " (errno=" << errno << ")" << endl;

        return;
    }


    if (!initGpioOutputs(
        RELAY_PINS.data(),
        RELAY_PINS.size(),
        relayRequest))
    {
        cerr << "Failed to initialize relay GPIOs" << endl;
    }
#endif
}

ioManager::~ioManager()
{
#ifdef HAS_GPIOD

    if (relayRequest)
        gpiod_line_request_release(relayRequest);

    if (chip)
        gpiod_chip_close(chip);

#endif
}

#ifdef HAS_GPIOD
bool ioManager::initGpioOutputs(const unsigned int* gpios, size_t count, gpiod_line_request*& request)
{
    gpiod_line_settings* settings =
        gpiod_line_settings_new();

    if (!settings)
    {
        cerr << "Failed to create GPIO line settings"
             << endl;
        return false;
    }

    if (gpiod_line_settings_set_direction(
            settings,
            GPIOD_LINE_DIRECTION_OUTPUT) < 0)
    {
        cerr << "Failed to set GPIO direction: "
             << strerror(errno)
             << endl;

        gpiod_line_settings_free(settings);
        return false;
    }

    if (gpiod_line_settings_set_output_value(
            settings,
            GPIOD_LINE_VALUE_INACTIVE) < 0)
    {
        cerr << "Failed to set GPIO initial value: "
             << strerror(errno)
             << endl;

        gpiod_line_settings_free(settings);
        return false;
    }

    gpiod_line_config* lineConfig =
        gpiod_line_config_new();

    if (!lineConfig)
    {
        cerr << "Failed to create GPIO line config"
             << endl;

        gpiod_line_settings_free(settings);
        return false;
    }

    for (size_t i = 0; i < count; ++i)
    {
        if (gpiod_line_config_add_line_settings(
                lineConfig,
                &gpios[i],
                1,
                settings) < 0)
        {
            cerr << "Failed to configure GPIO "
                 << gpios[i]
                 << ": "
                 << strerror(errno)
                 << endl;

            gpiod_line_config_free(lineConfig);
            gpiod_line_settings_free(settings);
            return false;
        }
    }

    gpiod_request_config* requestConfig =
        gpiod_request_config_new();

    if (!requestConfig)
    {
        cerr << "Failed to create request config"
             << endl;

        gpiod_line_config_free(lineConfig);
        gpiod_line_settings_free(settings);
        return false;
    }

    gpiod_request_config_set_consumer(
        requestConfig,
        "tAqua_Daemon"
    );

    request = gpiod_chip_request_lines(
        chip,
        requestConfig,
        lineConfig
    );

    if (!request)
    {
        cerr << "Failed to request GPIO lines: "
             << strerror(errno)
             << endl;
    }

    gpiod_request_config_free(requestConfig);
    gpiod_line_config_free(lineConfig);
    gpiod_line_settings_free(settings);

    return request != nullptr;
}
#endif


void ioManager::setRelay(Relay relay, int state)
{
    if (configM->getRelayConfig(relay) == RelayConfig::VALVE)
    {
        cout << "Setting valve" << endl;
    }
}

int ioManager::getRelay(Relay relay)
{   
    if (configM->getRelayConfig(relay) == RelayConfig::UNUSED)
    {
        return 0;
    }

    return 1;
}
