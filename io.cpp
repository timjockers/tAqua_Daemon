#include "io.hpp"

#include <iostream>
#include <bitset>
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

    // Init relays
    auto relayGPIOs = toUIntArray(RELAYS);

    if (!initGPIOOutputs(relayGPIOs.data(), relayGPIOs.size(), relayRequest))
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
bool ioManager::initGPIOOutputs(const unsigned int* gpios, size_t count, gpiod_line_request*& request)
{
    gpiod_line_settings* settings = gpiod_line_settings_new();

    if (!settings)
    {
        cerr << "Failed to create GPIO line settings" << endl;
        return false;
    }

    if (gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT) < 0)
    {
        cerr << "Failed to set GPIO direction: " << strerror(errno) << endl;

        gpiod_line_settings_free(settings);
        return false;
    }

    if (gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE) < 0)
    {
        cerr << "Failed to set GPIO initial value: " << strerror(errno) << endl;

        gpiod_line_settings_free(settings);
        return false;
    }

    gpiod_line_config* lineConfig = gpiod_line_config_new();

    if (!lineConfig)
    {
        cerr << "Failed to create GPIO line config" << endl;

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

    gpiod_request_config* requestConfig = gpiod_request_config_new();

    if (!requestConfig)
    {
        cerr << "Failed to create request config" << endl;

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
        cerr << "Failed to request GPIO lines: " << strerror(errno) << endl;
    }

    gpiod_request_config_free(requestConfig);
    gpiod_line_config_free(lineConfig);
    gpiod_line_settings_free(settings);

    return request != nullptr;
}
#endif


#ifdef HAS_GPIOD
bool ioManager::setGPIO(gpiod_line_request* request, unsigned int gpio, bool state)
{
    if (!request)
    {
        cerr << "GPIO request is not initialized" << endl;

        return false;
    }

    const gpiod_line_value value =
        state
            ? GPIOD_LINE_VALUE_ACTIVE
            : GPIOD_LINE_VALUE_INACTIVE;

    if (gpiod_line_request_set_value(
            request,
            gpio,
            value) < 0)
    {
        cerr << "Failed to set GPIO "
             << gpio
             << ": "
             << strerror(errno)
             << endl;

        return false;
    }

    return true;
}
#endif


#ifdef HAS_GPIOD
bool ioManager::getGPIO(gpiod_line_request* request, unsigned int gpio)
{
    if (!request)
    {
        cerr << "GPIO request is not initialized" << endl;

        return false;
    }

    const gpiod_line_value value =
        gpiod_line_request_get_value(
            request,
            gpio
        );

    if (value == GPIOD_LINE_VALUE_ERROR)
    {
        cerr << "Failed to read GPIO "
             << gpio
             << ": "
             << strerror(errno)
             << endl;

        return false;
    }

    return value == GPIOD_LINE_VALUE_ACTIVE;
}
#endif


#ifdef HAS_GPIOD
bool ioManager::setGPIOs(gpiod_line_request* request, array<unsigned int, 8> gpios, unsigned int states)
{   
    for (size_t i = 0; i < gpios.size(); ++i)
    {
        if (!setGPIO(request, gpios[i], (1 & (states >> i)) == 1))
        {
            return false;
        }
    }

    return true;
}
#endif


void ioManager::setRelay(Relay relay, int state)
{
    if (configM->getRelayConfig(relay) == RelayConfig::VALVE)
    {
        auto relayGPIOs = toUIntArray(RELAYS);

        if (state >= 1)
        {
            unsigned int states = 0;

            // Deactivate permanent power
            for (size_t i = 0; i < relayGPIOs.size(); ++i)
            {
                if (configM->getRelayConfig(RELAYS[i]) == RelayConfig::PERMANENTPOWER)
                {
                    states = states + (1 << i);
                }
            }

            // Activate valve
            states = states + (1 << static_cast<int>(relayIndex(relay)));

#ifdef HAS_GPIOD
            if (!setGPIOs(relayRequest, relayGPIOs, states))
            {
                cerr << "Failed to set relay!" << endl;
            }
#else
            cout << "Set relays: " << bitset<8>(states) << endl;
#endif
        }
        else
        {
#ifdef HAS_GPIOD
            // Check if relay is on
            if (getGPIO(relayRequest, static_cast<int>(relay)))
            {
                if (!setGPIOs(relayRequest, relayGPIOs, 0))
                {
                    cerr << "Failed to set relay!" << endl;
                }
            }
#else
            cout << "Set relays: 0" << endl;
#endif
        }
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
