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
    }


    /*
     * GPIO settings
     */
    gpiod_line_settings* settings = gpiod_line_settings_new();

    if (!settings)
    {
        cerr << "Failed to create GPIO line settings" << endl;
        return;
    }

    gpiod_line_settings_set_direction(
        settings,
        GPIOD_LINE_DIRECTION_OUTPUT
    );

    gpiod_line_settings_set_output_value(
        settings,
        GPIOD_LINE_VALUE_INACTIVE
    );

    if (gpiod_line_settings_set_direction(
            settings,
            GPIOD_LINE_DIRECTION_OUTPUT) < 0)
    {
        cerr << "Failed to set GPIO direction: "
             << strerror(errno)
             << endl;

        gpiod_line_settings_free(settings);
        return;
    }

    /*
     * GPIO config
     */
    gpiod_line_config* lineConfig = gpiod_line_config_new();

    if (!lineConfig)
    {
        cerr << "Failed to create GPIO line config" << endl;

        gpiod_line_settings_free(settings);
        return;
    }

    /*
     * Add all Relay GPIOs to the request
     */
    for (Relay relay : RELAYS)
    {
        const unsigned int gpio = static_cast<unsigned int>(relay);

        if (gpiod_line_config_add_line_settings(
                lineConfig,
                &gpio,
                1,
                settings) < 0)
        {
            cerr << "Failed to configure GPIO "
                 << gpio
                 << ": "
                 << strerror(errno)
                 << endl;

            gpiod_line_config_free(lineConfig);
            gpiod_line_settings_free(settings);
            return;
        }
    }

    /*
     * Configure request
     */
    gpiod_request_config* requestConfig = gpiod_request_config_new();

    if (!requestConfig)
    {
        cerr << "Failed to create request config" << endl;

        gpiod_line_config_free(lineConfig);
        gpiod_line_settings_free(settings);
        return;
    }

    gpiod_request_config_set_consumer(
        requestConfig,
        "tAqua_Daemon"
    );

    /*
     * Request GPIOs
     */
    relayRequest =
        gpiod_chip_request_lines(
            chip,
            requestConfig,
            lineConfig
        );

    if (!relayRequest)
    {
        cerr << "Failed to request GPIO lines: "
             << strerror(errno)
             << endl;
    }

    /*
     * Clean up the temporary configuration
     */
    gpiod_request_config_free(requestConfig);
    gpiod_line_config_free(lineConfig);
    gpiod_line_settings_free(settings);
#endif
}
ioManager::~ioManager()
{
#ifdef HAS_GPIOD
    gpiod_chip_close(chip);
#endif
}

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
