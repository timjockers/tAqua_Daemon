#include "button_controller.hpp"


ButtonController::ButtonController(ConfigManager *configManager, ioManager *io_manager, QueueManager *queueManager)
    : configM(configManager), ioM(io_manager), queueM(queueManager)
{}
