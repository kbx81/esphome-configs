#pragma once
#define USE_TIME
#include "esphome.h"
#include <iomanip>
#include <sstream>

namespace esp32_thermostat {

static const char *TAG = "esp32_thermostat";

const std::vector<climate::ClimateMode> supported_modes = {climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_AUTO,
                                                           climate::CLIMATE_MODE_COOL, climate::CLIMATE_MODE_HEAT,
                                                           climate::CLIMATE_MODE_FAN_ONLY};
const uint8_t num_rooms = 8;
const uint8_t num_modes = supported_modes.size();
const uint8_t max_missed_online_updates = 60 * (60 / 15);
const uint8_t max_missed_offline_updates = 2 * (60 / 15);
const uint8_t encoder_step_size = 1;

const float lower_temp_boundary = (61 - 32) * 5 / 9;
const float upper_temp_boundary = (90 - 32) * 5 / 9;
const float second_stage_activation_delta = 2 * 5 / 9;
const float temp_step_size = encoder_step_size * (5.0 / 9.0);

#ifdef USING_ROOMS
float room_humidity[num_rooms];
float room_temperature[num_rooms];
nextion::NextionTextSensor *room_ts_arr[num_rooms];
nextion::NextionTextSensor *temperature_ts_arr[num_rooms];
nextion::NextionTextSensor *humidity_ts_arr[num_rooms];

nextion::NextionTextSensor *room_ts(int row) {
  if ((row >= 0) && (row < num_rooms)) {
    if (room_ts_arr[row] != nullptr) {
      return room_ts_arr[row];
    }
    room_ts_arr[0] = textRoom1;
    room_ts_arr[1] = textRoom2;
    room_ts_arr[2] = textRoom3;
    room_ts_arr[3] = textRoom4;
    room_ts_arr[4] = textRoom5;
    room_ts_arr[5] = textRoom6;
    room_ts_arr[6] = textRoom7;
    room_ts_arr[7] = textRoom8;
    return room_ts_arr[row];
  }
  return nullptr;
}

nextion::NextionTextSensor *temperature_ts(int row) {
  if ((row >= 0) && (row < num_rooms)) {
    if (temperature_ts_arr[row] != nullptr) {
      return temperature_ts_arr[row];
    }
    temperature_ts_arr[0] = textTemp1;
    temperature_ts_arr[1] = textTemp2;
    temperature_ts_arr[2] = textTemp3;
    temperature_ts_arr[3] = textTemp4;
    temperature_ts_arr[4] = textTemp5;
    temperature_ts_arr[5] = textTemp6;
    temperature_ts_arr[6] = textTemp7;
    temperature_ts_arr[7] = textTemp8;
    return temperature_ts_arr[row];
  }
  return nullptr;
}

nextion::NextionTextSensor *humidity_ts(int row) {
  if ((row >= 0) && (row < num_rooms)) {
    if (humidity_ts_arr[row] != nullptr) {
      return humidity_ts_arr[row];
    }
    humidity_ts_arr[0] = textHum1;
    humidity_ts_arr[1] = textHum2;
    humidity_ts_arr[2] = textHum3;
    humidity_ts_arr[3] = textHum4;
    humidity_ts_arr[4] = textHum5;
    humidity_ts_arr[5] = textHum6;
    humidity_ts_arr[6] = textHum7;
    humidity_ts_arr[7] = textHum8;
    return humidity_ts_arr[row];
  }
  return nullptr;
}
#endif

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

void mode_button_click() {
  // if (id(main_lcd).is_on()) {
  uint8_t mode_selected = 0;
  // set mode_selected to current thermostat climate mode
  for (uint8_t i = 0; i < num_modes; i++) {
    if (supported_modes[i] == id(esp_thermostat).mode)
      mode_selected = i;
  }
  // increment mode_selected, resetting it if it overflowed
  if (++mode_selected >= num_modes)
    mode_selected = 0;

  // set the new climate mode and refresh the thermostat to fire triggers
  id(esp_thermostat).mode = supported_modes[mode_selected];
  id(esp_thermostat).refresh();
}

uint8_t get_weather_pic(std::string condition) {
  ESP_LOGD("get_weather_pic", "checking %s", condition.c_str());
  if (condition == "Partlycloudy")
    return 31;
  if (condition == "Cloudy")
    return 27;
  if (condition == "Rainy")
    return 25;
  if (condition == "Pouring")
    return 23;

  return 28;
}

std::string fix_up_weather_cond(std::string condition) {
  if (condition == "Partlycloudy") {
    return "Partly\\rCloudy";
  }
  return condition;
}

void update_temp_text() {
  if (main_lcd != nullptr) {
    std::string low_set_point_string =
        round_float_to_string(id(esp_thermostat).target_temperature_low * 1.8 + 32, 0) + "\xB0";
    std::string high_set_point_string =
        round_float_to_string(id(esp_thermostat).target_temperature_high * 1.8 + 32, 0) + "\xB0";

    id(textTempCool).set_state(high_set_point_string, false, true);
    id(textTempHeat).set_state(low_set_point_string, false, true);
  }
}

void display_refresh_sensor_names() {
  id(textSensor1).set_state("BME680:", false, true);
  id(textSensor2).set_state("BME280:", false, true);
  id(textSensor3).set_state("SHTC3:", false, true);
  id(textSensor4).set_state("TMP117:", false, true);
  id(textSensor5).set_state("DHT22:", false, true);
  id(textSensor6).set_state("Thermistor 1:", false, true);
  id(textSensor7).set_state("Thermistor 2:", false, true);
  id(textSensor8).set_state("SGP40:", false, true);
}

void update_climate_current_temperature(float temperature) {
  std::string temperature_string =
      round_float_to_string(id(esp_thermostat_temperature_sensor).state * 1.8 + 32) + "\xB0";
  id(tempCurrent).set_state(temperature_string, false, true);
}

void update_climate_current_humidity(float humidity) {
  std::string humidity_string = round_float_to_string(id(esp_thermostat_humidity_sensor).state) + "% RH";
  id(humCurrent).set_state(humidity_string, false, true);
}

#ifdef USING_ROOMS
void update_climate_table_name(int row, std::string name) {
  if ((row >= 0) && (row < num_rooms)) {
    room_ts(row)->set_state(name, false, true);
    if ((name == "Local") || (name == "Local:")) {
      id(climate_table_local_row) = row;
    }
  }
}

void update_climate_table_humidity(int row, float humidity) {
  if ((row >= 0) && (row < num_rooms)) {
    if (room_humidity[row] != humidity) {
      room_humidity[row] = humidity;
      humidity_ts(row)->set_state(round_float_to_string(humidity) + "%", false, true);
    }
  }
}

void update_climate_table_temperature(int row, float temperature) {
  if ((row >= 0) && (row < num_rooms)) {
    if (room_temperature[row] != temperature) {
      room_temperature[row] = temperature;
      temperature_ts(row)->set_state(round_float_to_string(temperature * 1.8 + 32) + "\xB0", false, true);
    }
  }
}
#else
void update_climate_table_name(int row, std::string name) {}
void update_climate_table_humidity(int row, float humidity) {}
void update_climate_table_temperature(int row, float temperature) {}
#endif

void update_status() {
  std::string offline_message = "offline";
  std::string sensor_message = "on-board sensor in use";
  std::string status_message;
  // std::string status_message = id(status_string);

  if (id(esp_thermostat_api_status).state == false && id(on_board_sensor_active) == true) {
    status_message = offline_message + " - " + sensor_message;
  } else if (id(esp_thermostat_api_status).state == false) {
    status_message = offline_message;
  } else if (id(on_board_sensor_active) == true) {
    status_message = sensor_message;
  }

  if (status_message.empty() == true) {
    status_message = id(status_string);
  } else if (id(status_string).empty() == false) {
    status_message = status_message + " - " + id(status_string);
  }

  if (status_message.empty() == false)
    status_message[0] = toupper(status_message[0]);

  id(textStatus).set_state(status_message, false, true);
}

bool weather_set = false;
uint32_t weather_update = 0;
void draw_main_screen(bool fullRefresh = false) {
  if (weather_update == 0) {
    weather_update = millis();
  }
  if (!weather_set && (millis() - weather_update) > 5000) {
    weather_set = true;
    update_weather->execute();
  }

  auto dateTime = id(esptime).now();
  // only do a full refresh once per hour (and at start-up)
  if (id(display_last_full_refresh) != dateTime.hour) {
    id(display_last_full_refresh) = dateTime.hour;
    main_lcd->set_nextion_rtc_time(dateTime);
  }

  update_temp_text();
  update_status();

  if (fullRefresh) {
    update_status();
    display_refresh_action->execute();
    display_refresh_mode->execute();
    display_refresh_fan_mode->execute();
    display_refresh_sensor_names();
  }
}

float adjust_high_set_point(float adjustment) {
  float high_set_point = id(esp_thermostat).target_temperature_high += adjustment,
        low_set_point = id(esp_thermostat).target_temperature_low;

  if (high_set_point < esp32_thermostat::lower_temp_boundary + temp_step_size)
    high_set_point = esp32_thermostat::lower_temp_boundary + temp_step_size;
  else if (high_set_point > esp32_thermostat::upper_temp_boundary)
    high_set_point = esp32_thermostat::upper_temp_boundary;

  if (high_set_point <= low_set_point)
    id(esp_thermostat).target_temperature_low = high_set_point - temp_step_size;

  id(esp_thermostat).target_temperature_high = high_set_point;

  update_temp_text();

  return high_set_point;
}

float adjust_low_set_point(float adjustment) {
  float high_set_point = id(esp_thermostat).target_temperature_high,
        low_set_point = id(esp_thermostat).target_temperature_low += adjustment;

  if (low_set_point < esp32_thermostat::lower_temp_boundary)
    low_set_point = esp32_thermostat::lower_temp_boundary;
  else if (low_set_point > esp32_thermostat::upper_temp_boundary - temp_step_size)
    low_set_point = esp32_thermostat::upper_temp_boundary - temp_step_size;
  if (low_set_point >= high_set_point)
    id(esp_thermostat).target_temperature_high = low_set_point + temp_step_size;

  id(esp_thermostat).target_temperature_low = low_set_point;

  update_temp_text();

  return low_set_point;
}

float thermostat_sensor_update() {
  bool template_sensor_valid = (id(current_temperature) >= esp32_thermostat::lower_temp_boundary) &&
                               (id(current_temperature) <= esp32_thermostat::upper_temp_boundary);
  float sensor_value = id(esp_thermostat_bme680_temperature).state;
  int max_missed_updates = esp32_thermostat::max_missed_offline_updates;

  if (id(esp_thermostat_api_status).state)
    max_missed_updates = esp32_thermostat::max_missed_online_updates;

  id(missed_update_count) += 1;

  if (id(missed_update_count) > max_missed_updates) {
    if (id(on_board_sensor_active) != true) {
      id(on_board_sensor_active) = true;
      esp32_thermostat::update_status();
    }
  } else {
    if (id(on_board_sensor_active) != false) {
      id(on_board_sensor_active) = false;
      esp32_thermostat::update_status();
    }
  }

  id(esp_thermostat_thermistor_vcc).turn_on();
  id(adc_sensor_thermistor).update();
  id(esp_thermostat_thermistor_vcc).turn_off();

  if (id(esp_thermostat_cool_1).state &&
      (id(esp_thermostat_temperature_sensor).state - id(esp_thermostat).target_temperature_high >=
       esp32_thermostat::second_stage_activation_delta))
    id(esp_thermostat_cool_2).turn_on();

  if (id(esp_thermostat_heat_1).state &&
      (id(esp_thermostat).target_temperature_low - id(esp_thermostat_temperature_sensor).state >=
       esp32_thermostat::second_stage_activation_delta))
    id(esp_thermostat_heat_2).turn_on();

  if (id(on_board_sensor_active) || !template_sensor_valid) {
    if (!isnan(sensor_value)) {
      id(sensor_ready) = true;
      id(current_temperature) = sensor_value;
      return sensor_value;
    } else {
      return (id(esp_thermostat).target_temperature_low + id(esp_thermostat).target_temperature_high) / 2;
    }
  } else {
    id(sensor_ready) = true;
    return id(current_temperature);
  }
}
}  // namespace esp32_thermostat