# kbx's Climate and Sprinkler Controller

![Climate and Sprinkler Controller](https://raw.githubusercontent.com/kbx81/ClimateSprinklerController/master/images/Nextion/front-small.jpg "Climate and Sprinkler Controller - Nextion 7\"")
![Climate and Sprinkler Controller](https://raw.githubusercontent.com/kbx81/ClimateSprinklerController/master/images/OLED/front-small.jpg "Climate and Sprinkler Controller - OLED")


Note that all of the configurations I've shared here are designed for controllers with either [SSD1325](https://www.adafruit.com/product/2674) or [Nextion](http://nextion.tech) displays. It is absolutely possible to use other display types with some modifications to the configurations; however, doing so is outside the scope of this document.

## Example YAML Configuration Files for a Climate Control System

This directory contains the YAML configuration files I use at home where I have my controllers installed.

- [esp_thermostat.h](shared/esp_thermostat.h) contains the majority of the controller UI functionality as well as some C++ constants used within the code/lambdas for units with [SSD1325](https://www.adafruit.com/product/2674) displays.
- [esp_thermostat_nextion.h](shared/esp_thermostat_nextion.h) contains code that functions primarily as glue between ESPHome and the "interface" I've constructed within the Nextion HMI environment/editor.
- [esp-thermostat-nook.yaml](fairfield/esp-thermostat-nook.yaml) is the configuration file I use for the controller that is physically wired/connected to my HVAC system.
- [esp-thermostat-front-room.yaml](fairfield/esp-thermostat-front-room.yaml) is a satellite thermostat I keep in my living room area.
- [esp-thermostat-bedroom.yaml](fairfield/esp-thermostat-bedroom.yaml) is a satellite thermostat I keep in my bedroom.
- [esp-thermostat-bathroom.yaml](fairfield/esp-thermostat-bathroom.yaml) is a satellite thermostat I've installed in the bathroom.
- [esp_thermostat_common_config.yaml](shared/esp_thermostat_common_config.yaml) contains configuration used by all of the thermostat devices with [SSD1325](https://www.adafruit.com/product/2674) displays.
- [esp_thermostat_nextion_common_config.yaml](shared/esp_thermostat_nextion_common_config.yaml) contains configuration used by all of the thermostat devices with [Nextion](http://nextion.tech) displays.
- [esp_thermostat_common_controller_config.yaml](shared/esp_thermostat_common_controller_config.yaml) contains configuration used by "controller" thermostat devices.
- [esp_thermostat_common_satellite_config.yaml](shared/esp_thermostat_common_satellite_config.yaml) contains configuration used by "satellite" thermostat devices.

The beauty here comes in with [Home Assistant's](https://www.home-assistant.io) [automations](https://www.home-assistant.io/docs/automation/). (In my mind, this is where it stops being a thermostat and becomes a "climate control system".) For example, your automation could use different sensors based on time of day -- at night, the bedroom thermostat's sensor is used while during the day, the living room thermostat's sensor is used. If you install into your build(s) either a PIR sensor or the [RCWL-0516 module](https://www.amazon.com/gp/product/B07MTWZDQZ/) I suggested, motion sensing could play a part in this. I have an automation that takes the readings from all of my sensors, performs a simple averaging of them and then feeds this back to the [template sensor](https://esphome.io/components/sensor/template.html) in the configurations. This drives the heating/cooling in my living space. That said, I have a unique situation in that the wall-mounted "nook" controller is on a wall that has behind it a large boiler unit that heats the upstairs areas -- this boiler warms the wall when it runs, skewing the sensor readings of the "nook" controller. Because of this, I've placed one of [these sensor boards](https://github.com/kbx81/TempHumSensWithESP01) in the boiler room and, as when the temperature it senses rises (specifically above 75 degrees F), my automation reduces the weight of the reading from the sensors in the wall-mounted "nook" controller and (consequently) begins to more heavily rely on the others (front room, bedroom and bathroom). If I don't do this, my living space gets colder than it should be based on the thermostat's setting. (You can probably surmise now why I am _very dependent_ on the remote sensors...particularly in the winter season.)

In addition to what I described above, I've also created automations that keep the [ESPHome](https://esphome.io) [thermostat](https://esphome.io/components/climate/thermostat.html) components synchronized across all of the controllers; a change on one is reflected on all of them.

(Sidebar: I use [Node-RED](https://nodered.org) to build my automations. It's super cool. If you haven't tried it, I strongly encourage you to do so!)

### Thermostat User Interface (UI)

#### Nextion Versions

As I'm not a (UI) designer, the interface I've created for the Nextion displays is relatively simple. On the larger displays (5-inch and 7-inch), the main screen displays the weather conditions/data on the left and the climate/thermostat status on the right. Icons for each climate mode (heat, cool, auto, fan, off) may be tapped to change the mode; the highlighted icon indicates the current mode. When a given climate action is in progress (heating, cooling, etc.) its respective icon appears in color. In addition, the current set points are indicated with up/down arrows to adjust them (no, I did not use a slider because, at least in the Nextion world, they are _ugly_. Also, why do you really need a slider control for this? You're probably never changing it more than a couple degrees up or down. To me, sliders are just plain annoying to use in these cases!) In the lower left corner, a "page" icon allows changing the display page to reveal temperatures in other rooms (the "room list" page). Last but not least, tapping on the current indoor temperature will navigate to a page which displays the values of all of the sensors.

#### SSD1325 Versions

The controller UI I've created for these is quite simple. Pressing/"clicking" the encoder advances to the next screen on the display. When available, a highlighted value may be modified by rotating the encoder.

The main screen displays the temperature indicated by the [template sensor](https://esphome.io/components/sensor/template.html) -- note that this is a computed value that's normally fed to the controller by the [Home Assistant](https://www.home-assistant.io) [automation](https://www.home-assistant.io/docs/automation/) that I've built. (This is the temperature that controls what the system ultimately does.) This screen also displays the temperature and humidity from a local sensor (specifically, the BME280) as well as the system's current mode or action ("heat"/"heating") and the time and date.

The second screen displays the values of other sensors my automations feed to the controller. This includes the temperatures and humidities from the other units as well as the outside temperature and humidity.

The third screen displays the set points and allows them to be adjusted.

The fourth screen displays the system's mode as well as the system's fan mode.

The final screen displays data from all installed sensors (there are several, in my case...because I can).

When the PIR or [RCWL-0516](https://www.amazon.com/gp/product/B07MTWZDQZ/) module senses motion, the display is activated. It turns off after one minute.

#### Other UI Thoughts

I intentionally designed the interface on these to be _very simple._ My aim was to make it so that my parents or grandparents could use them. There is no hierarchical menu structure to navigate and there is no on-device scheduling/programming available. Home Assistant already does automations exceptionally well and, with this in mind, building "advanced" automation into this configuration would be totally redundant. There simply is no situation in which I'd rather stand in front of one of these tiny displays and tap buttons/spin the encoder to navigate any number of menus and change values (time of day, day of week, etc.) to do on-device programming. I realize this won't meet everybody's needs, but I had to draw the line somewhere and, in every case, I'd much rather use the Home Assistant UI on my smartphone or laptop to set up schedules and such for automation. Sure, it would be cool to have, but (at least for me) I'd never use it as long as Home Assistant is a part of my system. Usually, at this point, people ask, "But what about if Home Assistant is down?! Your schedules won't work and blah blah blah!" My response to this is simple: if this is the _first_ thing that comes to mind after I explain this, I would argue that you have a different problem you need to be focusing on! :p

## Example YAML Configuration Files for a Sprinkler System

- [esp_sprinkler.h](shared/esp_sprinkler.h) contains the majority of the controller UI functionality as well as some C++ constants used within the code/lambdas.
- [esp_sprinkler_nextion.h](shared/esp_sprinkler_nextion.h) contains code that functions primarily as glue between ESPHome and the "interface" I've constructed within the Nextion HMI environment/editor.
- [esp-sprinkler-controller.yaml](fairfield/esp-sprinkler-controller.yaml) is a configuration file that can be used to control electric valves that are a part of a sprinkler/irrigation system.

The [esp-sprinkler-controller.yaml](fairfield/esp-sprinkler-controller.yaml) configuration implements a simple sprinkler valve controller. It is configured so that "zone 8" (aka `A7`) is a "master valve" (one that controls the water supply to the other individual zone valves). _The master valve is on when any other valve/zone is on_. In this configuration, five other valves/zones are defined. Zone 1 (`A0`) is the parkway zone, zone 2 (`A1`) is the front yard area, zone 3 (`A2`) is one side of the house, zone 4 (`A3`) is the back yard area and zone 5 (`A4`) is the other side of the house.

This configuration also doubles as a garage door monitor and controller. Relay 1 is configured as a virtual "button"; there is a template switch which turns this relay on for one second and then shuts it off again. This is to simulate a button press which is commonly used to trigger an electric garage door opener to open and close. In addition, pin `B5` on the MCP23017 is configured as a `binary_sensor` which may be used to monitor the state of the garage door (open/closed). A reed switch and magnet mounted to the door are commonly used for this purpose; in this case, the reed switch should short the `B5` pin to ground when the door is (fully) closed.

### Wiring

For this configuration, you'll want to install all of the jumpers on `J2`. Power must be supplied by the sprinkler valve power supply/transformer which _should_ be a 24-volt AC supply. This power may be fed into the controller through the barrel jack.

#### If using the barrel jack:

  - The common wire from the valve manifold would connect to the `C` terminal on the controller.
  - The remaining numbered terminals (`A0` - `A7`) would connect to the individual sprinkler zone valves (given this configuration `A7` would connect to the master valve).

#### If **not** using the barrel jack:

 - The common wire from the valve manifold would connect to the `C` terminal on the controller as well as one of the wires from the sprinkler valve power transformer.
 - The other wire from the the transformer would connect to one of the three `R` terminals on the controller (recall that all of the `R` terminals are now jumpered together because `J2`).
 - The remaining numbered terminals (`A0` - `A7`) would connect to the individual sprinkler zone valves (given this configuration `A7` would connect to the master valve).

### Usage

Default run durations for each sprinkler/valve zone are determined by global variables defined near the top of the [configuration file](fairfield/esp-sprinkler-controller.yaml). A multiplier is also available to extend or reduce the run time for all zones. All of these values may be adjusted arbitrarily through the API (also defined in the configuration) from within [Home Assistant](https://www.home-assistant.io).

The [configuration](fairfield/esp-sprinkler-controller.yaml) defines a switch for each zone, a switch to "enable" each zone, a switch for the master valve, an "auto-advance" switch and an "auto-start" switch. (At this time the various other sensors are also present; remove them if you like as they are not necessary.)

Enabling "auto-start" will cause the controller to begin a cycle at the configured time. This time may be arbitrarily changed via the API; it defaults to 5 am. The controller's behavior is as described below.

Zones that are "enabled" will run in the next cycle _if "auto-advance" is on/enabled_.

Turning on the master valve will turn on "auto-advance", causing the controller to iterate through all "enabled" zones; it will start with the lowest-numbered enabled zone. Each zone will run for its configured duration, then it will advance to the next zone. This continues until all enabled zones have run.

Turning on the master valve _with no zones enabled_ will turn on auto-advance _and enable all zones_. This effectively starts a full cycle of the system.

Turning on a single zone switch will cause that zone to run for its configured duration. If "auto-advance" is on/enabled, the controller will continue to iterate through any other enabled zones (in a circular fashion).

Locally (on the controller itself), press the encoder button to toggle between the main screen and the "setup" screen. The latter allows you to enable and disable the various individual zones by rotating the encoder wheel to select and then pressing/"clicking" the encoder to toggle them. Selecting and clicking on "Start" from this screen will start a cycle. The behavior is the same as described above with respect to zones being enabled/disabled, etc.

As mentioned above, this configuration can also control an electric garage door opener. Pressing and holding the encoder for one to two seconds will trigger relay 1 to perform a one-second "pulse", simulating a button press. Its normally-open (NO) contacts could be wired in parallel with an already-installed button used to trigger the door opener to open/close the door.

When the PIR or [RCWL-0516](https://www.amazon.com/gp/product/B07MTWZDQZ/) module senses motion, the display is activated. It turns off after one minute.

## Legal stuff and License

The [Climate and Sprinkler Controller](https://github.com/kbx81/ClimateSprinklerController) circuit schematics, PCBs and enclosures are licensed under the
 [Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0/).

![Creative Commons License badge](https://i.creativecommons.org/l/by-sa/4.0/88x31.png)

_Thanks for visiting!_