# kbx's ESPHome Configuration Repository

This repository contains the various configuration files I use for the various [ESPHome](https://esphome.io) devices I have and use around my home.

## Directory Structure

The top level directory contains the device configurations.

The [resources](resources/) directory contains resources/assets (fonts, images, etc.) used by the various configurations.

The [shared](shared/) directory contains source code files shared/used  by multiple device configurations.

The only necessary file that is not here is `shared/esp__common_secrets.yaml`; an [example](shared/esp__common_secrets.example.yaml) is provided, instead. If renamed and updated with appropriate values, all the code here will compile and run.

## Device Documentation

See the [CSC.md](CSC.md) document for information regarding the [Climate and Sprinkler Controller](https://github.com/kbx81/ClimateSprinklerController) configurations.

The other configurations are fairly self-explanatory and probably don't need an entire document to themselves. Still, here's quick rundown:
 - [`esp-rc-ir-front-room.yaml`](esp-rc-ir-front-room.yaml) and [`esp-rc-rf-front-room.yaml`](esp-rc-rf-front-room.yaml) are configurations I use with a couple of [IR Blasters](https://github.com/kbx81/kbxIRBlaster) I have set up for remote control of various other devices I have around.
 - [`esp-lights-3d-printer.yaml`](esp-lights-3d-printer.yaml), [`esp-lights-bathroom.yaml`](esp-lights-bathroom.yaml) and [`esp-lights-kitchen.yaml`](esp-lights-kitchen.yaml) are configurations I use with my [LED Strip Controllers](https://github.com/kbx81/kbxLEDController-16ch) for control of various LED lighting I've installed.
 - [`esp-sensor-xxxxx.yaml`] are configurations I use with my [sensor boards](https://github.com/kbx81/TempHumSensWithESP01) for monitoring the temperature and humidity in various locations around my home.
 - [`esp-snowflake-controller.yaml`](esp-snowflake-controller.yaml) is a device I use to remotely control my [snowflakes](https://luckyresistor.me/projects/snow-flake-decoration/).

_Thanks for visiting!_