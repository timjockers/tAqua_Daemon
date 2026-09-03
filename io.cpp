#include "io.hpp"

#include <iostream>
#include <bitset>
#include <algorithm>
#include <utility>
#include <optional>
using namespace std;


static array<Button, 2> PHYSICAL_PULLUP = {
    Button::B3,
    Button::B4
};

static bool hasPhysicalPullup(Button button)
{
    return find(
        PHYSICAL_PULLUP.begin(),
        PHYSICAL_PULLUP.end(),
        button
    ) != PHYSICAL_PULLUP.end();
}

static std::optional<Button> buttonFromGPIO(unsigned int gpio)
{
    for (Button button : BUTTONS)
    {
        if (static_cast<unsigned int>(button) == gpio)
            return button;
    }

    return std::nullopt;
}


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

    // Init yellow LEDs
    auto yledGPIOs = toUIntArray(YLEDS);

    if (!initGPIOOutputs(yledGPIOs.data(), yledGPIOs.size(), yledRequest))
    {
        cerr << "Failed to initialize yellow LED GPIOs" << endl;
    }

    // Init Buttons
    if (!initGPIOInputs(BUTTONS.data(), BUTTONS.size(), buttonRequest))
    {
        cerr << "Failed to initialize button GPIOs" << endl;
    }
#endif
}

ioManager::~ioManager()
{
#ifdef HAS_GPIOD
    buttonThreadRunning = false;
    if (buttonThread.joinable())
    {
        buttonThread.join();
    }


    if (relayRequest)
    {
        gpiod_line_request_release(relayRequest);
    }

    if (yledRequest)
    {
        gpiod_line_request_release(yledRequest);
    }

    if (buttonRequest)
    {
        gpiod_line_request_release(buttonRequest);
    }

    if (chip)
    {
        gpiod_chip_close(chip);
    }

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
bool ioManager::initGPIOInputs(const Button* buttons, size_t count, gpiod_line_request*& request)
{
    gpiod_line_config* lineConfig = gpiod_line_config_new();

    if (!lineConfig)
    {
        cerr << "Failed to create GPIO line config" << endl;
        return false;
    }

    for (size_t i = 0; i < count; ++i)
    {
        Button button = buttons[i];

        gpiod_line_settings* settings =
            gpiod_line_settings_new();

        if (!settings)
        {
            cerr << "Failed to create GPIO line settings" << endl;

            gpiod_line_config_free(lineConfig);
            return false;
        }

        if (gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT) < 0)
        {
            cerr << "Failed to set GPIO direction: " << strerror(errno) << endl;

            gpiod_line_settings_free(settings);
            gpiod_line_config_free(lineConfig);
            return false;
        }

        if (!hasPhysicalPullup(button))
        {
            if (gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP) < 0)
            {
                cerr << "Failed to set GPIO pull-up: " << strerror(errno) << endl;

                gpiod_line_settings_free(settings);
                gpiod_line_config_free(lineConfig);
                return false;
            }
        }

        if (gpiod_line_settings_set_edge_detection(settings, GPIOD_LINE_EDGE_BOTH) < 0)
        {
            cerr << "Failed to set edge detection: " << strerror(errno) << endl;

            gpiod_line_settings_free(settings);
            gpiod_line_config_free(lineConfig);
            return false;
        }

        gpiod_line_settings_set_debounce_period_us(settings, 50000); // 50ms
        
        
        const unsigned int gpio = static_cast<unsigned int>(button);

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

            gpiod_line_settings_free(settings);
            gpiod_line_config_free(lineConfig);
            return false;
        }

        gpiod_line_settings_free(settings);
    }

    gpiod_request_config* requestConfig = gpiod_request_config_new();

    if (!requestConfig)
    {
        cerr << "Failed to create request config" << endl;

        gpiod_line_config_free(lineConfig);
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


void ioManager::setRelay(Relay relay, bool state)
{
    if (configM->getRelayConfig(relay) == RelayConfig::VALVE)
    {
        auto relayGPIOs = toUIntArray(RELAYS);

        if (state)
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

bool ioManager::getRelay(Relay relay)
{   
    if (configM->getRelayConfig(relay) == RelayConfig::UNUSED)
    {
        return false;
    }
    else
    {
#ifdef HAS_GPIOD
        return (getGPIO(relayRequest, static_cast<int>(relay)) > 0);
#endif
    }
    return false;
}


void ioManager::setYLED(YLED yled, bool state)
{
#ifdef HAS_GPIOD
    setGPIO(yledRequest, static_cast<unsigned int>(yled), state);
#else
    cout << "Setting yellow LED" << endl;
#endif
}

bool ioManager::getYLED(YLED yled)
{
#ifdef HAS_GPIOD
    return getGPIO(yledRequest, static_cast<unsigned int>(yled));
#else
    return false;
#endif
}


bool ioManager::isButtonPressed(Button button)
{
#ifdef HAS_GPIOD
    return !getGPIO(buttonRequest, static_cast<unsigned int>(button));
#else
    return false;
#endif
}


void ioManager::setButtonCallback(ButtonCallback callback)
{
    buttonCallback = std::move(callback);
}

void ioManager::startButtonThread()
{
#ifdef HAS_GPIOD
    if (buttonRequest)
    {
        buttonThreadRunning = true;
        buttonThread = std::thread(&ioManager::processButtonEvents, this);
    }
#endif
}

#ifdef HAS_GPIOD
void ioManager::processButtonEvents()
{
    gpiod_edge_event_buffer* buffer = gpiod_edge_event_buffer_new(16);

    if (!buffer)
    {
        cerr << "Failed to create GPIO event buffer" << endl;
        return;
    }

    while (buttonThreadRunning)
    {
        int ret = gpiod_line_request_wait_edge_events(buttonRequest, 100000000);

        if (ret < 0)
        {
            if (buttonThreadRunning)
            {
                cerr << "Failed to wait for GPIO events" << endl;
            }

            break;
        }

        if (ret == 0)
        {
            continue;
        }

        int numEvents = gpiod_line_request_read_edge_events(buttonRequest, buffer, 16);

        if (numEvents < 0)
        {
            cerr << "Failed to read GPIO events" << endl;
            break;
        }

        for (int i = 0; i < numEvents; ++i)
        {
            gpiod_edge_event* event = gpiod_edge_event_buffer_get_event(buffer, i);

            if (!event)
            {
                continue;
            }

            unsigned int gpio = gpiod_edge_event_get_line_offset(event);

            auto button = buttonFromGPIO(gpio);

            if (!button)
            {
                cerr << "Received event for unknown GPIO: " << gpio << endl;
                continue;
            }

            auto eventType = gpiod_edge_event_get_event_type(event);

            bool pressed = (eventType == GPIOD_EDGE_EVENT_FALLING_EDGE);

            if (buttonCallback)
            {
                buttonCallback(*button, pressed);
            }
        }
    }

    gpiod_edge_event_buffer_free(buffer);
}
#endif