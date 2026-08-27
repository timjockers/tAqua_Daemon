# tAqua_Daemon

`tAqua_Daemon` is a component of the **tAqua** project, an automated irrigation controller.

The overall project consists of several components. While **tAqua_Web** is responsible for configuration and operation through a web interface, **tAqua_Daemon** handles the actual hardware control and execution of the irrigation logic.

The daemon runs on a **Raspberry Pi 1 Model B+** using **Raspberry Pi OS** and controls eight relays as well as buttons and status LEDs.

## Architecture

The project is divided into two main components:

```text
┌─────────────────────┐
│     tAqua_Web       │
│                     │
│ Web interface       │
│ Configuration       │
└──────────┬──────────┘
           │
           │ writes
           ▼
      ┌───────────┐
      │ taqua.cfg │
      └─────┬─────┘
            │
            │ reads
            ▼
┌─────────────────────┐
│   tAqua_Daemon      │
│                     │
│ Configuration       │
│ Scheduling          │
│ Event Queue         │
│ Buttons             │
│ LEDs                │
│ Relays              │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────────────┐
│ Raspberry Pi 1 Model B+     │
│                             │
│  Relay 1 ───┐               │
│  Relay 2 ───┤               │
│  Relay 3 ───┤               │
│  ...        ├─ Hardware     │
│  Relay 8 ───┘               │
└─────────────────────────────┘
```

`tAqua_Web` writes the settings to the `taqua.cfg` file. `tAqua_Daemon` reads this file and applies the configured settings and schedules to the hardware.

Therefore, no direct communication between the web interface and the hardware is required.

## Hardware

The daemon is designed for the following hardware:

* **Raspberry Pi 1 Model B+**
* **8 relays**
* **Buttons for manual operation**
* **Green LEDs**
* **Yellow LEDs**

The Raspberry Pi controls the inputs and outputs and executes the irrigation logic.

## Relays

Each of the eight relays can be assigned a different function through the configuration.

### Unused

The relay is not used by the irrigation controller.

### Valve

The relay controls an irrigation valve.

A relay configured as a valve can have scheduled irrigation times. The valve can also be activated manually using the corresponding button.

### Permanent Power

A relay can also be configured as a **permanent power relay**.

Such a relay is normally active. As soon as another relay controlling an irrigation valve is switched on, the permanent power relay is switched off.

This can be used, for example, to control a pond filling system with its own sensor:

```text
Normal operation:

Pond filler
     │
     ▼
Permanent power
     │
     ▼
  Sensor
```

During irrigation:

```text
Irrigation valve is activated
             │
             ▼
     Permanent power relay
             │
             ▼
            OFF
```

This prevents, for example, a pond filling system from drawing water during an irrigation cycle.

## Manual Operation

The irrigation controller can also be operated directly using the buttons without requiring the web interface.

When the button corresponding to a relay configured as a **valve** is pressed, a manual irrigation event is created for that valve.

The valve is then activated for **5 minutes**.

The manual activation is processed through the same event queue as an automatically scheduled irrigation event.

## LEDs

Status information for each relay is displayed using LEDs.

### Green LED

The **green LED** indicates that the corresponding relay is currently switched on.

```text
Green ON
  │
  └──> Relay is currently active
```

### Yellow LED

The **yellow LED** indicates that an irrigation cycle for the corresponding valve is scheduled to start soon.

```text
Yellow ON
  │
  └──> Valve will be irrigated soon
```

This allows the current and upcoming state of the irrigation system to be determined directly from the controller without using the web interface.

## Event Queue

Relay control is based on an **event queue**.

Different types of events can therefore be processed independently of their source.

Examples of events include:

* scheduled irrigation times
* manual button presses
* future weather-related events
* other automatically generated control commands

For example, a manually triggered 5-minute irrigation event is added to the queue in the same way as a previously scheduled time-based event.

Simplified:

```text
                 ┌─────────────────────┐
                 │ Scheduled times     │
                 └──────────┬──────────┘
                            │
                            ▼
                       ┌─────────┐
                       │         │
Button ───────────────►│ Event   │
                       │ Queue   │
Future events ────────►│         │
                       └────┬────┘
                            │
                            ▼
                    ┌───────────────┐
                    │ Event Handler │
                    └───────┬───────┘
                            │
                            ▼
                       Relay control
```

This architecture allows new types of events to be added in the future without having to completely redesign the underlying relay control.

## Configuration

The irrigation controller configuration is created by **tAqua_Web** and stored in the file

```text
taqua.cfg
```

`tAqua_Daemon` reads this file and processes the settings contained within it.

The exact syntax and structure of the configuration file can be found in the **example configuration file** included in the repository.

```text
taqua.cfg
```

Among other things, the configuration file defines the function of each relay and the scheduled irrigation times.

## Installation

The daemon is written in **C++** and is intended to run on a Raspberry Pi using Raspberry Pi OS.

The project uses **CMake** as its build system.

### 1. Install required packages
```bash
sudo apt update
sudo apt upgrade
sudo apt install git
sudo apt install cmake
sudo apt install libconfig++-dev
```

### 2. Copy the repository to the Raspberry Pi

Create the folder `~/tadmin/` by running the following commands
```bash
cd
mkdir taqua
cd taqua
```

Clone the `tAqua_Daemon` git repository:
```bash
git clone https://github.com/timjockers/tAqua_Daemon
```

The folder `tAqua_Daemon/` should have been created automatically.


### 3. Build the project using CMake

Configure the project using the included `CMakeLists.txt`.
Run the following commands in the `~/taqua/tAqua_Daemon/` folder:

```bash
cmake .
```

Then compile the project:

```bash
make
```

### 4. Start the daemon

After successful compilation, the daemon can be started manually with:

```bash
./_daemon
```

It is recommended to start the `tAqua_Daemon` (as well as tAqua_Web) automatically when the Raspberry Pi boots.
Run the following commands to set this up:

```bash
Add instructions here
```

## Requirements

* Raspberry Pi 1 Model B+
* Raspberry Pi OS
* Connected tAqua hardware


## Development Status

`tAqua_Daemon` is currently **under development**.

The basic architecture and planned features are still being developed. In particular, the configuration format, event queue, and hardware control may change during development.

## Planned Features

A long-term goal of the project is to connect the irrigation controller to the Internet.

This would allow the daemon to retrieve current weather data and take it into account when planning irrigation.

Possible features include:

* Retrieving current weather data
* Taking rainfall into account
* Weather-dependent irrigation scheduling
* Skipping irrigation when rain is expected
* Adjusting irrigation duration based on weather conditions

The exact implementation of the weather integration has not yet been decided.

## Related Projects

### tAqua_Web

`tAqua_Web` provides the web interface for configuring and operating the irrigation controller.

The web interface writes the settings to the `taqua.cfg` file used by the daemon.

Development of `tAqua_Web` has not yet begun, but the repository is already available at:

```text
https://github.com/timjockers/tAqua_Web
```

# To-Dos
- [ ] README.md: Add autostart instructions
