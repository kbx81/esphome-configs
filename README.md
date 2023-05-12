# kbx's ESPHome Configuration Repository

This repository contains the various configuration files I use for the various [ESPHome](https://esphome.io) devices I have and use around my home.

## Directory Structure

The [fairfield](fairfield/) and [waltham](waltham/) directories contain the various device configurations.

The [resources](resources/) directory contains resources/assets (fonts, images, etc.) used by the various configurations.

The [shared](shared/) directory contains source code and configuration files shared/used  by multiple device configurations.

The [dev](dev/) directory contains various device configurations that I use(d) to test stuff. These are not actively maintained and may or may not build and/or work.

The only necessary file that is not here is `shared/esp__common_secrets.yaml`; an [example](shared/esp__common_secrets.example.yaml) is provided, instead. If renamed and updated with appropriate values, all the code here will compile and run.

## Device Documentation

See the [CSC.md](CSC.md) document for information regarding the [Climate and Sprinkler Controller](https://github.com/kbx81/ClimateSprinklerController) configurations.

The other configurations are fairly self-explanatory and probably don't need an entire document to themselves. Still, here's quick rundown:
 - [`esp-rc-ir-front-room.yaml`](fairfield/esp-rc-ir-front-room.yaml) and [`esp-rc-ir-bedroom.yaml`](fairfield/esp-rc-ir-bedroom.yaml) are configurations I use with a couple of [IR Blasters](https://github.com/kbx81/kbxIRBlaster) I have set up for remote control of various other devices I have around.
 - `esp-lights-*.yaml` are configurations I use with my [LED Strip Controllers](https://github.com/kbx81/kbxLEDController-16ch) for control of various LED lighting I've installed.
 - `esp-sensor-*.yaml` are configurations I use with my [sensor boards](https://github.com/kbx81/TempHumSensWithESP01) for monitoring the temperature and humidity in various locations around my home.
 - [`esp-snowflake-controller.yaml`](fairfield/esp-snowflake-controller.yaml) is a device I use to remotely control my [snowflakes](https://luckyresistor.me/projects/snow-flake-decoration/).

### Half Square Triangles LED Art

This is a project I did based on [this](https://www.instructables.com/Half-Square-Triangles-LED-Art/) guide from Instructables. I made a number of modifications:
 - I used APA102 ("DotStar") LEDs because I had them on-hand.
 - I wired the strips of LEDs into a single string (as opposed to four).
 - I used a D1 Mini (ESP8266) module running [ESPHome](https://esphome.io) connected to [Home Assistant](https://www.home-assistant.io) to allow full remote control of the pieces.
 - I redrew the body in Fusion 360 to be a single, multi-part (and consequently multi-material) print. It no longer needs a picture frame to be complete and can be printed a few different ways because of the multiple parts/bodies. My (updated) design is available [here](https://a360.co/3HVdIhd). The caveat is that it's less friendly to smaller 3-D printers as it requires a printable area of at least 205x205mm.

That said, the [ESPHome](https://esphome.io) configuration bits of interest are:
 - [`esp-lights-led-triangles-X.yaml`](fairfield/esp-lights-led-triangles-1.yaml) files are the base configurations for the displays.
 - [`esp-lights-led-triangles.yaml`](shared/esp-lights-led-triangles.yaml) is the "real" part of the [ESPHome](https://esphome.io) configuration.
 - [`esp_led_triangles.h`](shared/esp_led_triangles.h) is the code that renders the display effects I ported from Andrei's [repo](https://github.com/ancalex/Half-Square-Triangles-LED-Art).
 - [`esp__common_core.yaml`](shared/esp__common_core.yaml) is also imported and required by the HST base configurations (above).

---

If you found any of this helpful and feel so inclined, please [Buy Me A Coffee](https://bmc.link/kbx81)! ☕️

_Thanks for visiting!_