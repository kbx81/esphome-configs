#pragma once
#define USE_TIME

#include <iomanip>
#include <sstream>

namespace esp32_thermostat {
enum DisplaySensor : size_t {
  DS_CURRENT_TEMP = 0,
  DS_CURRENT_HUM = 1,
  DS_WEATHER_TEMP = 2,
  DS_WEATHER_HUM = 3,
  DS_WEATHER_TEMPHIGH = 4,
  DS_WEATHER_TEMPLOW = 5,
  DS_NUM_OF_SENSORS = 6
};

enum DisplayConversion : uint8_t { DC_NONE = 0, DC_C_TO_F = 1, DC_F_TO_C = 2 };

struct TableRow {
  nextion::NextionTextSensor *name;
  nextion::NextionSensor *temp;
  nextion::NextionSensor *hum;
};

struct IaqCondPairing {
  const uint16_t threshold;
  const Color color;
  const std::string condition;
};

struct WeatherCondPairing {
  const char icon_id;
  const std::string condition;
};

// maximum number of missed template sensor updates allowed before using the on-board sensor
const uint8_t max_missed_online_updates = 60 * (60 / 15);
const uint8_t max_missed_offline_updates = 5 * (60 / 15);
// number of rooms supported for the room list
const uint8_t num_rooms = 8;
// other constants
const char default_weather_cond_icon_id = 'O';

const std::vector<climate::ClimateMode> supported_modes{climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_HEAT_COOL,
                                                        climate::CLIMATE_MODE_COOL, climate::CLIMATE_MODE_HEAT,
                                                        climate::CLIMATE_MODE_FAN_ONLY};

const std::vector<esp32_thermostat::WeatherCondPairing> weather_cond_icon{
    {'F', "clear-night"}, {'A', "cloudy"},          {'O', "exceptional"},  {'B', "fog"},     {'C', "hail"},
    {'E', "lightning"},   {'D', "lightning-rainy"}, {'G', "partlycloudy"}, {'H', "pouring"}, {'I', "rainy"},
    {'K', "snowy"},       {'J', "snowy-rainy"},     {'L', "sunny"},        {'M', "windy"},   {'N', "windy-variant"}};

const std::vector<esp32_thermostat::IaqCondPairing> iaq_cond_text{
    {50, Color(0, 255, 96), "Excellent"},        {100, Color(160, 255, 0), "Good"},
    {150, Color(255, 255, 0), "Moderate"},       {200, Color(255, 127, 0), "Unhealthy"},
    {300, Color(255, 32, 32), "Very Unhealthy"}, {9000, Color(160, 24, 64), "Hazardous"}};

std::vector<nextion::NextionSensor *> sensor_vector(6);
std::vector<esp32_thermostat::TableRow> room_table(num_rooms);
std::vector<float> room_humidity(num_rooms);
std::vector<float> room_temperature(num_rooms);

esp32_thermostat::TableRow room_row(int row) {
  if ((row >= 0) && (row < num_rooms)) {
    if ((room_table[row].temp != nullptr) && (room_table[row].hum != nullptr) && (room_table[row].name != nullptr)) {
      return room_table[row];
    }
    room_table[0].temp = nextionRoom1Temp;
    room_table[1].temp = nextionRoom2Temp;
    room_table[2].temp = nextionRoom3Temp;
    room_table[3].temp = nextionRoom4Temp;
    room_table[4].temp = nextionRoom5Temp;
    room_table[5].temp = nextionRoom6Temp;
    room_table[6].temp = nextionRoom7Temp;
    room_table[7].temp = nextionRoom8Temp;

    room_table[0].hum = nextionRoom1Hum;
    room_table[1].hum = nextionRoom2Hum;
    room_table[2].hum = nextionRoom3Hum;
    room_table[3].hum = nextionRoom4Hum;
    room_table[4].hum = nextionRoom5Hum;
    room_table[5].hum = nextionRoom6Hum;
    room_table[6].hum = nextionRoom7Hum;
    room_table[7].hum = nextionRoom8Hum;

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

nextion::NextionSensor *display_sensor(DisplaySensor sensor) {
  if ((sensor >= 0) && (sensor < static_cast<uint8_t>(DS_NUM_OF_SENSORS))) {
    if (sensor_vector[sensor] != nullptr) {
      return sensor_vector[sensor];
    }
    sensor_vector[0] = nextionCurrentTemp;
    sensor_vector[1] = nextionCurrentHum;
    sensor_vector[2] = nextionWeatherTemp;
    sensor_vector[3] = nextionWeatherHum;
    sensor_vector[4] = nextionWeatherTempHigh;
    sensor_vector[5] = nextionWeatherTempLow;

    return sensor_vector[sensor];
  }
  return nullptr;
}

std::string round_float_to_string(float value, uint8_t precision = 1) {
  std::stringstream strstr;
  if (precision == 0) {
    strstr << std::fixed << std::setprecision(precision) << std::noshowpoint << value;
  } else {
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

void display_refresh_lockout_status() {
  float lockout =
      (id(esp_thermostat).climate_action_change_delayed() || id(esp_thermostat).fan_mode_change_delayed()) ? 1 : 0;
  id(nextionLockoutState).set_state(lockout, false, true);
}

void display_refresh_action() {
  id(nextionClimateAction).set_state(static_cast<float>(id(esp_thermostat).action), false, true);
  display_refresh_lockout_status();
}

void display_refresh_mode() {
  id(nextionClimateMode).set_state(static_cast<float>(id(esp_thermostat).mode), false, true);
  display_refresh_action();
}

void display_refresh_fan_mode() {
  id(nextionClimateFanMode)
      .set_state(static_cast<float>(id(esp_thermostat).fan_mode.value_or(CLIMATE_FAN_AUTO)), false, true);
  display_refresh_lockout_status();
}

void display_refresh_iaq_txt(float iaq_value, bool is_weather = false) {
  auto iaq_string = esp32_thermostat::round_float_to_string(iaq_value);
  auto iaq_color = Color::WHITE;

  for (IaqCondPairing i : iaq_cond_text) {
    if (iaq_value < i.threshold) {
      iaq_string += " - " + i.condition;
      iaq_color = i.color;
      break;
    }
  }

  if (is_weather) {
    id(nextionTextWeatherAq).set_state(iaq_string, false, true);
    id(nextionTextWeatherAq).set_foreground_color(iaq_color);
  } else {
    id(nextionTextCurrentAq1).set_state(iaq_string, false, true);
    id(nextionTextCurrentAq1).set_foreground_color(iaq_color);
    id(nextionTextCurrentAq2).set_state(iaq_string, false, true);
    id(nextionTextCurrentAq2).set_foreground_color(iaq_color);
  }
}

void display_refresh_sensor_names() {
  id(nextionTextSensor1).set_state("BME680:", false, true);
  id(nextionTextSensor2).set_state("BME280:", false, true);
  id(nextionTextSensor3).set_state("SHTC3:", false, true);
  id(nextionTextSensor4).set_state("TMP117:", false, true);
  id(nextionTextSensor5).set_state("DHT22:", false, true);
  id(nextionTextSensor6).set_state("Thermistor 1:", false, true);
  id(nextionTextSensor7).set_state("Thermistor 2:", false, true);
  id(nextionTextSensor8).set_state("SGP40:", false, true);
}

void display_refresh_sensor(float value, nextion::NextionSensor *sensor,
                            const DisplayConversion conv = esp32_thermostat::DC_NONE) {
  switch (conv) {
    case DC_C_TO_F:
      value = value * 1.8 + 32;
      break;

    case DC_F_TO_C:
      value = (value - 32) * 5 / 9;
      break;

    default:
      break;
  }

  value = roundf(value * 10);  // we want just one decimal place

  sensor->set_state(value, false, true);
}

void display_refresh_sensor(const double value, const DisplaySensor sensor_num,
                            const DisplayConversion conv = esp32_thermostat::DC_NONE) {
  display_refresh_sensor(value, display_sensor(sensor_num), conv);
}

void display_refresh_set_points() {
  id(nextionCurrentSetLower)
      .set_state(roundf((id(esp_thermostat).target_temperature_low * 1.8 + 32) * 10), false, true);
  id(nextionCurrentSetUpper)
      .set_state(roundf((id(esp_thermostat).target_temperature_high * 1.8 + 32) * 10), false, true);
  id(nextionCurrentSetHum).set_state(id(esp_thermostat_target_humidity).state * 10, false, true);
  if (id(nextionCurrentSetMin).state !=
      (id(esp_thermostat).get_traits().get_visual_min_temperature() * 1.8 + 32) * 10) {
    id(nextionCurrentSetMin)
        .set_state((id(esp_thermostat).get_traits().get_visual_min_temperature() * 1.8 + 32) * 10, true, true);
  }
  if (id(nextionCurrentSetMax).state !=
      (id(esp_thermostat).get_traits().get_visual_max_temperature() * 1.8 + 32) * 10) {
    id(nextionCurrentSetMax)
        .set_state((id(esp_thermostat).get_traits().get_visual_max_temperature() * 1.8 + 32) * 10, true, true);
  }
}

void display_refresh_table_name(int row, std::string name) {
  if ((row >= 0) && (row < num_rooms)) {
    room_row(row).name->set_state(name, false, true);
    if ((name == "Local") || (name == "Local:")) {
      id(climate_table_local_row) = row;
    }
  }
}

void display_refresh_table_humidity(int row, float humidity) {
  if ((row >= 0) && (row < num_rooms)) {
    if (room_humidity[row] != humidity) {
      room_humidity[row] = humidity;
      display_refresh_sensor(room_humidity[row], room_row(row).hum, esp32_thermostat::DC_NONE);
    }
  }
}

void display_refresh_table_temperature(int row, float temperature) {
  if ((row >= 0) && (row < num_rooms)) {
    if (room_temperature[row] != temperature) {
      room_temperature[row] = temperature;
      display_refresh_sensor(room_temperature[row], room_row(row).temp, esp32_thermostat::DC_C_TO_F);
    }
  }
}

void display_refresh_weather_cond() {
  std::string icon_id(1, default_weather_cond_icon_id);

  for (WeatherCondPairing i : weather_cond_icon) {
    if (i.condition == id(weather_condition)) {
      icon_id[0] = i.icon_id;
      break;
    }
  }
  id(nextionTextWeatherIcon).set_state(icon_id, false, true);
}

void display_refresh_status() {
  std::string offline_message = "offline";
  std::string sensor_message = "on-board sensor in use";
  std::string status_message;

  if (id(esp_thermostat_api_status).state == false && id(esp_thermostat_on_board_sensor_active).state) {
    status_message = offline_message + " - " + sensor_message;
  } else if (id(esp_thermostat_api_status).state == false) {
    status_message = offline_message;
  } else if (id(esp_thermostat_on_board_sensor_active).state) {
    status_message = sensor_message;
  }

  if (status_message.empty()) {
    status_message = id(status_string);
  } else if (!id(status_string).empty()) {
    status_message = status_message + " - " + id(status_string);
  }

  if (!status_message.empty())
    status_message[0] = toupper(status_message[0]);

  if (id(nextionTextStatus1).state != status_message)
    id(nextionTextStatus1).set_state(status_message, false, true);
  if (id(nextionTextStatus2).state != status_message)
    id(nextionTextStatus2).set_state(status_message, false, true);
  if (id(nextionTextStatus3).state != status_message)
    id(nextionTextStatus3).set_state(status_message, false, true);
}

void draw_main_screen(bool fullRefresh = false) {
  auto dateTime = id(esptime).now();
  // only do a full refresh once per hour (and at start-up)
  if (id(display_last_full_refresh) != dateTime.hour) {
    id(display_last_full_refresh) = dateTime.hour;
    // main_lcd->set_touch_sleep_timeout(60);
    main_lcd->set_nextion_rtc_time(dateTime);
  }

  if (fullRefresh) {
    display_refresh_status();
    // display_refresh_action();
    display_refresh_mode();
    display_refresh_fan_mode();
    display_refresh_set_points();
    display_refresh_weather_cond();
    display_refresh_sensor(id(weather_humidity), esp32_thermostat::DS_WEATHER_HUM, esp32_thermostat::DC_NONE);
    display_refresh_sensor(id(weather_temperature), esp32_thermostat::DS_WEATHER_TEMP, esp32_thermostat::DC_NONE);
    display_refresh_sensor(id(weather_temperature_high), esp32_thermostat::DS_WEATHER_TEMPHIGH,
                           esp32_thermostat::DC_NONE);
    display_refresh_sensor(id(weather_temperature_low), esp32_thermostat::DS_WEATHER_TEMPLOW,
                           esp32_thermostat::DC_NONE);
    display_refresh_sensor_names();
  }
}

float thermostat_humidity_sensor_value() {
  bool template_sensor_valid = id(current_humidity) > 0;
  float sensor_value = id(esp_thermostat_bme680_humidity).state;
  // determine what value to return for the sensor
  // if the on-board sensor is flagged as active or if the template sensor's value (aka current_humidity) is not
  // valid,
  //  we must (try to) use the on-board hardware sensor
  if (id(esp_thermostat_on_board_sensor_active).state || !template_sensor_valid) {
    // if the hardware sensor is "ready" (not NaN), we use that value
    if (!isnan(sensor_value)) {
      id(sensor_ready) = true;
      id(current_humidity) = sensor_value;
    } else {
      // if the hardware sensor is NOT "ready" (not NaN), we return a (fake) neutral value. not ideal but this should be
      // only very rarely used
      sensor_value = id(esp_thermostat_target_humidity).state;
    }
  } else {
    // if the on-board sensor is NOT flagged as active and the template sensor's value (aka current_humidity)
    //  is valid, we return that value
    id(sensor_ready) = true;
    sensor_value = id(current_humidity);
  }
  // switch humidifier on/off as required
  if ((sensor_value >= id(esp_thermostat_target_humidity).state + 1) ||
      (id(esp_thermostat).action != CLIMATE_ACTION_HEATING)) {
    id(esp_thermostat_humidify).turn_off();
  } else if (sensor_value <= id(esp_thermostat_target_humidity).state - 1) {
    id(esp_thermostat_humidify).turn_on();
  }

  return sensor_value;
}

float thermostat_temperature_sensor_value() {
  bool template_sensor_valid =
      (id(current_temperature) >= id(esp_thermostat).get_traits().get_visual_min_temperature()) &&
      (id(current_temperature) <= id(esp_thermostat).get_traits().get_visual_max_temperature());
  float sensor_value = id(esp_thermostat_bme680_temperature).state;
  // always increment the counter -- it'll get reset later if appropriate
  id(missed_update_count) += 1;
  // update the display to refresh the status as appropriate
  esp32_thermostat::display_refresh_status();
  // measure the thermistor (kind of arbitrary but we do it here)
  id(esp_thermostat_thermistor_vcc).turn_on();
  id(adc_sensor_thermistor).update();
  id(esp_thermostat_thermistor_vcc).turn_off();
  // update the display to refresh the lockout status as appropriate
  display_refresh_lockout_status();
  // determine what value to return for the sensor
  // if the on-board sensor is flagged as active or if the template sensor's value (aka current_temperature) is not
  // valid,
  //  we must (try to) use the on-board hardware sensor
  if (id(esp_thermostat_on_board_sensor_active).state || !template_sensor_valid) {
    // if the hardware sensor is "ready" (not NaN), we use that value
    if (!isnan(sensor_value)) {
      id(sensor_ready) = true;
      id(current_temperature) = sensor_value;
      return sensor_value;
    } else {
      // if the hardware sensor is NOT "ready" (not NaN), we return a (fake) neutral value. not ideal but this should be
      // only very rarely used
      return (id(esp_thermostat).target_temperature_low + id(esp_thermostat).target_temperature_high) / 2;
    }
  } else {
    // if the on-board sensor is NOT flagged as active and the template sensor's value (aka current_temperature)
    //  is valid, we return that value
    id(sensor_ready) = true;
    return id(current_temperature);
  }
}
}  // namespace esp32_thermostat