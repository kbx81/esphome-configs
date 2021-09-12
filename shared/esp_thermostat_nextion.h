#pragma once
#define USE_TIME

#include <iomanip>
#include <sstream>

namespace esp32_thermostat
{
  enum DisplaySensor : size_t
  {
    DS_CURRENT_TEMP = 0,
    DS_CURRENT_HUM = 1,
    DS_WEATHER_TEMP = 2,
    DS_WEATHER_HUM = 3,
    DS_WEATHER_TEMPHIGH = 4,
    DS_WEATHER_TEMPLOW = 5,
    DS_NUM_OF_SENSORS = 6
  };

  enum DisplayConversion : uint8_t
  {
    DC_NONE = 0,
    DC_C_TO_F = 1,
    DC_F_TO_C = 2
  };

  struct NextionSensorPairing
  {
    nextion::NextionSensor *whole;
    nextion::NextionSensor *frac;
  };

  struct TableRow
  {
    nextion::NextionTextSensor *name;
    NextionSensorPairing temp;
    NextionSensorPairing hum;
  };

  struct IaqCondPairing
  {
    const uint16_t threshold;
    const Color color;
    const std::string condition;
  };

  struct WeatherCondPairing
  {
    const char icon_id;
    const std::string condition;
  };

  const std::vector<climate::ClimateMode> supported_modes{
      climate::CLIMATE_MODE_OFF,
      climate::CLIMATE_MODE_HEAT_COOL,
      climate::CLIMATE_MODE_COOL,
      climate::CLIMATE_MODE_HEAT,
      climate::CLIMATE_MODE_FAN_ONLY};
  const std::vector<esp32_thermostat::WeatherCondPairing> weather_cond_icon{
      {'F', "clear-night"},
      {'A', "cloudy"},
      {'O', "exceptional"},
      {'B', "fog"},
      {'C', "hail"},
      {'E', "lightning"},
      {'D', "lightning-rainy"},
      {'G', "partlycloudy"},
      {'H', "pouring"},
      {'I', "rainy"},
      {'K', "snowy"},
      {'J', "snowy-rainy"},
      {'L', "sunny"},
      {'M', "windy"},
      {'N', "windy-variant"}};
  const std::vector<esp32_thermostat::IaqCondPairing> iaq_cond_text{
      {50, Color(0, 255, 96), "Excellent"},
      {100, Color(160, 255, 0), "Good"},
      {150, Color(255, 255, 0), "Moderate"},
      {200, Color(255, 127, 0), "Unhealthy"},
      {300, Color(255, 32, 32), "Very Unhealthy"},
      {9000, Color(160, 24, 64), "Hazardous"}};
  const char default_weather_cond_icon_id = 'O';
  const uint8_t num_rooms = 8;
  const uint8_t num_modes = supported_modes.size();
  const uint8_t max_missed_online_updates = 60 * (60 / 15);
  const uint8_t max_missed_offline_updates = 2 * (60 / 15);
  const float temp_step_size = 5.0 / 9.0;

  std::vector<esp32_thermostat::NextionSensorPairing> sensor_pairing(6);
  std::vector<esp32_thermostat::TableRow> room_table(num_rooms);
  std::vector<float> room_humidity(num_rooms);
  std::vector<float> room_temperature(num_rooms);

  esp32_thermostat::TableRow room_row(int row)
  {
    if ((row >= 0) && (row < num_rooms))
    {
      if ((room_table[row].temp.whole != nullptr) && (room_table[row].temp.frac != nullptr) &&
          (room_table[row].hum.whole != nullptr) && (room_table[row].hum.frac != nullptr) &&
          (room_table[row].name != nullptr))
      {
        return room_table[row];
      }
      room_table[0].temp.whole = nextionRoom1Temp;
      room_table[0].temp.frac = nextionRoom1TempFrac;
      room_table[1].temp.whole = nextionRoom2Temp;
      room_table[1].temp.frac = nextionRoom2TempFrac;
      room_table[2].temp.whole = nextionRoom3Temp;
      room_table[2].temp.frac = nextionRoom3TempFrac;
      room_table[3].temp.whole = nextionRoom4Temp;
      room_table[3].temp.frac = nextionRoom4TempFrac;
      room_table[4].temp.whole = nextionRoom5Temp;
      room_table[4].temp.frac = nextionRoom5TempFrac;
      room_table[5].temp.whole = nextionRoom6Temp;
      room_table[5].temp.frac = nextionRoom6TempFrac;
      room_table[6].temp.whole = nextionRoom7Temp;
      room_table[6].temp.frac = nextionRoom7TempFrac;
      room_table[7].temp.whole = nextionRoom8Temp;
      room_table[7].temp.frac = nextionRoom8TempFrac;

      room_table[0].hum.whole = nextionRoom1Hum;
      room_table[0].hum.frac = nextionRoom1HumFrac;
      room_table[1].hum.whole = nextionRoom2Hum;
      room_table[1].hum.frac = nextionRoom2HumFrac;
      room_table[2].hum.whole = nextionRoom3Hum;
      room_table[2].hum.frac = nextionRoom3HumFrac;
      room_table[3].hum.whole = nextionRoom4Hum;
      room_table[3].hum.frac = nextionRoom4HumFrac;
      room_table[4].hum.whole = nextionRoom5Hum;
      room_table[4].hum.frac = nextionRoom5HumFrac;
      room_table[5].hum.whole = nextionRoom6Hum;
      room_table[5].hum.frac = nextionRoom6HumFrac;
      room_table[6].hum.whole = nextionRoom7Hum;
      room_table[6].hum.frac = nextionRoom7HumFrac;
      room_table[7].hum.whole = nextionRoom8Hum;
      room_table[7].hum.frac = nextionRoom8HumFrac;

      room_table[0].name = nextionTextRoom1;
      room_table[1].name = nextionTextRoom2;
      room_table[2].name = nextionTextRoom3;
      room_table[3].name = nextionTextRoom4;
      room_table[4].name = nextionTextRoom5;
      room_table[5].name = nextionTextRoom6;
      room_table[6].name = nextionTextRoom7;
      room_table[7].name = nextionTextRoom8;

      return room_table[row];
    }
    return esp32_thermostat::TableRow();
  }

  esp32_thermostat::NextionSensorPairing display_sensor(DisplaySensor sensor)
  {
    if ((sensor >= 0) && (sensor < static_cast<uint8_t>(DS_NUM_OF_SENSORS)))
    {
      if ((sensor_pairing[sensor].whole != nullptr) && (sensor_pairing[sensor].frac != nullptr))
      {
        return sensor_pairing[sensor];
      }
      sensor_pairing[0].whole = nextionCurrentTemp;
      sensor_pairing[0].frac = nextionCurrentTempFrac;
      sensor_pairing[1].whole = nextionCurrentHum;
      sensor_pairing[1].frac = nextionCurrentHumFrac;
      sensor_pairing[2].whole = nextionWeatherTemp;
      sensor_pairing[2].frac = nextionWeatherTempFrac;
      sensor_pairing[3].whole = nextionWeatherHum;
      sensor_pairing[3].frac = nextionWeatherHumFrac;
      sensor_pairing[4].whole = nextionWeatherTempHigh;
      sensor_pairing[4].frac = nextionWeatherTempHighFrac;
      sensor_pairing[5].whole = nextionWeatherTempLow;
      sensor_pairing[5].frac = nextionWeatherTempLowFrac;

      return sensor_pairing[sensor];
    }
    return esp32_thermostat::NextionSensorPairing();
  }

  std::string round_float_to_string(float value, uint8_t precision = 1)
  {
    std::stringstream strstr;
    if (precision == 0)
    {
      strstr << std::fixed << std::setprecision(precision) << std::noshowpoint << value;
    }
    else
    {
      strstr << std::fixed << std::setprecision(precision) << std::showpoint << value;
    }
    std::string formatted_str = strstr.str();
    return formatted_str;
  }

  // void set_display_page(uint8_t page) {
  //   if (page < esp32_thermostat::last_page_number) {
  //     id(selected_display_page) = page;
  //     // if any page in the array is set to nullptr, refresh them to get valid pointers
  //     if (display_pages[id(selected_display_page)].page == nullptr)
  //       refresh_display_pages();
  //     // finally, set/show the new page
  //     id(main_lcd).show_page(display_pages[id(selected_display_page)].page);
  //   }
  // }

  void display_refresh_lockout_status()
  {
    float lockout = (id(esp_thermostat).climate_action_change_delayed() || id(esp_thermostat).fan_mode_change_delayed()) ? 1 : 0;
    id(nextionLockoutState).set_state(lockout, false, true);
  }

  void display_refresh_action()
  {
    id(nextionClimateAction).set_state(static_cast<float>(id(esp_thermostat).action), false, true);
    display_refresh_lockout_status();
  }

  void display_refresh_mode()
  {
    id(nextionClimateMode).set_state(static_cast<float>(id(esp_thermostat).mode), false, true);
    display_refresh_action();
  }

  void display_refresh_fan_mode()
  {
    id(nextionClimateFanMode).set_state(static_cast<float>(id(esp_thermostat).fan_mode.value_or(CLIMATE_FAN_AUTO)), false, true);
    display_refresh_lockout_status();
  }

  void display_refresh_iaq_txt(float iaq_value, bool is_weather = false)
  {
    auto iaq_string = esp32_thermostat::round_float_to_string(iaq_value);
    auto iaq_color = Color::WHITE;

    for (IaqCondPairing i : iaq_cond_text)
    {
      if (iaq_value < i.threshold)
      {
        iaq_string += " - " + i.condition;
        iaq_color = i.color;
        break;
      }
    }

    if (is_weather)
    {
      id(nextionTextWeatherAq).set_state(iaq_string, false, true);
      id(nextionTextWeatherAq).set_foreground_color(iaq_color);
    }
    else
    {
      id(nextionTextCurrentAq1).set_state(iaq_string, false, true);
      id(nextionTextCurrentAq1).set_foreground_color(iaq_color);
      id(nextionTextCurrentAq2).set_state(iaq_string, false, true);
      id(nextionTextCurrentAq2).set_foreground_color(iaq_color);
    }
  }

  void display_refresh_sensor_names()
  {
    id(nextionTextSensor1).set_state("BME680:", false, true);
    id(nextionTextSensor2).set_state("BME280:", false, true);
    id(nextionTextSensor3).set_state("SHTC3:", false, true);
    id(nextionTextSensor4).set_state("TMP117:", false, true);
    id(nextionTextSensor5).set_state("DHT22:", false, true);
    id(nextionTextSensor6).set_state("Thermistor 1:", false, true);
    id(nextionTextSensor7).set_state("Thermistor 2:", false, true);
    id(nextionTextSensor8).set_state("SGP40:", false, true);
  }

  void display_refresh_sensor_pair(double value, const NextionSensorPairing sensor, const DisplayConversion conv = esp32_thermostat::DC_NONE)
  {
    switch (conv)
    {
    case DC_C_TO_F:
      value = value * 1.8 + 32;
      break;

    case DC_F_TO_C:
      value = (value - 32) * 5 / 9;
      break;

    default:
      break;
    }

    value = round(value * 10); // we want just one decimal place
    double value_int = 0, value_frac = modf(value / 10, &value_int) * 10;

    sensor.whole->set_state(static_cast<float>(value_int), false, true);
    sensor.frac->set_state(roundf(static_cast<float>(value_frac)), false, true);
  }

  void display_refresh_sensor_pair(const double value, const DisplaySensor sensor_num, const DisplayConversion conv = esp32_thermostat::DC_NONE)
  {
    display_refresh_sensor_pair(value, display_sensor(sensor_num), conv);
  }

  void display_refresh_set_points()
  {
    id(nextionCurrentSetLower).set_state(id(esp_thermostat).target_temperature_low * 1.8 + 32, false, true);
    id(nextionCurrentSetUpper).set_state(id(esp_thermostat).target_temperature_high * 1.8 + 32, false, true);
    // id(nextionCurrentSetHum).set_state(id(esp_thermostat).target_humidity, false, true);
  }

  void display_refresh_table_name(int row, std::string name)
  {
    if ((row >= 0) && (row < num_rooms))
    {
      room_row(row).name->set_state(name, false, true);
      if ((name == "Local") || (name == "Local:"))
      {
        id(climate_table_local_row) = row;
      }
    }
  }

  void display_refresh_table_humidity(int row, float humidity)
  {
    if ((row >= 0) && (row < num_rooms))
    {
      if (room_humidity[row] != humidity)
      {
        room_humidity[row] = humidity;
        display_refresh_sensor_pair(room_humidity[row], room_row(row).hum, esp32_thermostat::DC_NONE);
      }
    }
  }

  void display_refresh_table_temperature(int row, float temperature)
  {
    if ((row >= 0) && (row < num_rooms))
    {
      if (room_temperature[row] != temperature)
      {
        room_temperature[row] = temperature;
        display_refresh_sensor_pair(room_temperature[row], room_row(row).temp, esp32_thermostat::DC_C_TO_F);
      }
    }
  }

  void display_refresh_weather_cond()
  {
    std::string icon_id(1, default_weather_cond_icon_id);

    for (WeatherCondPairing i : weather_cond_icon)
    {
      if (i.condition == id(weather_condition))
      {
        icon_id[0] = i.icon_id;
        break;
      }
    }
    id(nextionTextWeatherIcon).set_state(icon_id, false, true);
  }

  void display_refresh_status()
  {
    std::string offline_message = "offline";
    std::string sensor_message = "on-board sensor in use";
    std::string status_message;
    // std::string status_message = id(status_string);

    if (id(esp_thermostat_api_status).state == false && id(on_board_sensor_active) == true)
    {
      status_message = offline_message + " - " + sensor_message;
    }
    else if (id(esp_thermostat_api_status).state == false)
    {
      status_message = offline_message;
    }
    else if (id(on_board_sensor_active) == true)
    {
      status_message = sensor_message;
    }

    if (status_message.empty() == true)
    {
      status_message = id(status_string);
    }
    else if (id(status_string).empty() == false)
    {
      status_message = status_message + " - " + id(status_string);
    }

    if (status_message.empty() == false)
      status_message[0] = toupper(status_message[0]);

    id(nextionTextStatus1).set_state(status_message, false, true);
    id(nextionTextStatus2).set_state(status_message, false, true);
    id(nextionTextStatus3).set_state(status_message, false, true);
  }

  void draw_main_screen(bool fullRefresh = false)
  {
    auto dateTime = id(esptime).now();
    // only do a full refresh once per hour (and at start-up)
    if (id(display_last_full_refresh) != dateTime.hour)
    {
      id(display_last_full_refresh) = dateTime.hour;
      // main_lcd->set_touch_sleep_timeout(60);
      main_lcd->set_nextion_rtc_time(dateTime);
    }
    // ESP_LOGD("********** HEAP **********", " Free: %d", ESP.getFreeHeap());
    // ESP_LOGD("********** HEAP **********", "Total: %d", ESP.getHeapSize());

    if (fullRefresh)
    {
      display_refresh_status();
      // display_refresh_action();
      display_refresh_mode();
      display_refresh_fan_mode();
      display_refresh_set_points();
      display_refresh_weather_cond();
      display_refresh_sensor_pair(id(weather_humidity), esp32_thermostat::DS_WEATHER_HUM, esp32_thermostat::DC_NONE);
      display_refresh_sensor_pair(id(weather_temperature), esp32_thermostat::DS_WEATHER_TEMP, esp32_thermostat::DC_NONE);
      display_refresh_sensor_pair(id(weather_temperature_high), esp32_thermostat::DS_WEATHER_TEMPHIGH, esp32_thermostat::DC_NONE);
      display_refresh_sensor_pair(id(weather_temperature_low), esp32_thermostat::DS_WEATHER_TEMPLOW, esp32_thermostat::DC_NONE);
      display_refresh_sensor_names();
    }
  }

  float adjust_high_set_point(float adjustment)
  {
    auto call = id(esp_thermostat).make_call();
    auto new_set_point = id(esp_thermostat).target_temperature_high += adjustment;
    call.set_target_temperature_high(new_set_point);
    call.perform();
    display_refresh_set_points();
    return id(esp_thermostat).target_temperature_high;
  }

  float adjust_low_set_point(float adjustment)
  {
    auto call = id(esp_thermostat).make_call();
    auto new_set_point = id(esp_thermostat).target_temperature_low += adjustment;
    call.set_target_temperature_low(new_set_point);
    call.perform();
    display_refresh_set_points();
    return id(esp_thermostat).target_temperature_low;
  }

  void set_thermostat_fan_mode(const climate::ClimateFanMode new_mode)
  {
    auto call = id(esp_thermostat).make_call();
    call.set_fan_mode(new_mode);
    call.perform();
    display_refresh_fan_mode();
  }

  void set_thermostat_mode(const climate::ClimateMode new_mode)
  {
    auto call = id(esp_thermostat).make_call();
    call.set_mode(new_mode);
    call.perform();
    display_refresh_mode();
    if (new_mode == climate::CLIMATE_MODE_OFF)
      set_thermostat_fan_mode(climate::CLIMATE_FAN_AUTO);
  }

  void fan_mode_button_click()
  {
    if (id(esp_thermostat).fan_mode.value_or(CLIMATE_FAN_AUTO) == CLIMATE_FAN_AUTO)
      set_thermostat_fan_mode(climate::CLIMATE_FAN_ON);
    else
      set_thermostat_fan_mode(climate::CLIMATE_FAN_AUTO);
  }

  void mode_button_click()
  {
    // if (id(main_lcd).is_on()) {
    uint8_t mode_selected = 0;
    // set mode_selected to current thermostat climate mode
    for (uint8_t i = 0; i < num_modes; i++)
    {
      if (supported_modes[i] == id(esp_thermostat).mode)
        mode_selected = i;
    }
    // increment mode_selected, resetting it if it overflowed
    if (++mode_selected >= num_modes)
      mode_selected = 0;
    // set the new climate mode and refresh the thermostat to fire triggers
    set_thermostat_mode(supported_modes[mode_selected]);
    // }
  }

  float thermostat_sensor_update()
  {
    bool template_sensor_valid = (id(current_temperature) >= id(esp_thermostat).get_traits().get_visual_min_temperature()) && (id(current_temperature) <= id(esp_thermostat).get_traits().get_visual_max_temperature());
    float sensor_value = id(esp_thermostat_bme280_temperature).state;
    int max_missed_updates = esp32_thermostat::max_missed_offline_updates;

    if (id(esp_thermostat_api_status).state)
      max_missed_updates = esp32_thermostat::max_missed_online_updates;

    id(missed_update_count) += 1;

    if (id(missed_update_count) > max_missed_updates)
    {
      if (id(on_board_sensor_active) != true)
      {
        id(on_board_sensor_active) = true;
        esp32_thermostat::display_refresh_status();
      }
    }
    else
    {
      if (id(on_board_sensor_active) != false)
      {
        id(on_board_sensor_active) = false;
        esp32_thermostat::display_refresh_status();
      }
    }

    id(esp_thermostat_thermistor_vcc).turn_on();
    id(adc_sensor_thermistor).update();
    id(esp_thermostat_thermistor_vcc).turn_off();

    display_refresh_lockout_status();

    if (id(on_board_sensor_active) || !template_sensor_valid)
    {
      if (!isnan(sensor_value))
      {
        id(sensor_ready) = true;
        id(current_temperature) = sensor_value;
        return sensor_value;
      }
      else
      {
        return (id(esp_thermostat).target_temperature_low + id(esp_thermostat).target_temperature_high) / 2;
      }
    }
    else
    {
      id(sensor_ready) = true;
      return id(current_temperature);
    }
  }
}