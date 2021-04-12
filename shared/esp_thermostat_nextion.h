#pragma once
#define USE_TIME
#include "esphome.h"
#include <iomanip>
#include <sstream>

namespace esp32_thermostat {

static const char *TAG = "esp32_thermostat";

// const Color color_control_enabled {255, 255, 255, 0};
// const Color color_control_disabled {76, 76, 76, 0};
// const Color color_control_enabled {1.0, 1.0, 1.0, 0};
// const Color color_control_disabled {0.3, 0.3, 0.3, 0};

const std::vector<climate::ClimateMode> supported_modes = {
    climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_AUTO,
    climate::CLIMATE_MODE_COOL, climate::CLIMATE_MODE_HEAT,
    climate::CLIMATE_MODE_FAN_ONLY};
const uint8_t num_rooms = 8;
const uint8_t num_modes = supported_modes.size();
const uint8_t max_missed_online_updates = 60 * (60 / 15);
const uint8_t max_missed_offline_updates = 2 * (60 / 15);
const uint8_t encoder_step_size = 1;
// const char    color_lowlight[] = "16904";
// const char    color_medlight[] = "50712";
// const char    color_highlight[] = "65535";
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
    strstr << std::fixed << std::setprecision(precision) << std::noshowpoint
           << value;
  } else {
    strstr << std::fixed << std::setprecision(precision) << std::showpoint
           << value;
  }
  std::string formatted_str = strstr.str();
  return formatted_str;
}

// void set_display_page(uint8_t page) {
//   if (page < esp32_thermostat::last_page_number) {
//     id(selected_display_page) = page;
//     // if any page in the array is set to nullptr, refresh them to get valid
//     pointers if (display_pages[id(selected_display_page)].page == nullptr)
//       refresh_display_pages();
//     // finally, set/show the new page
//     id(main_lcd).show_page(display_pages[id(selected_display_page)].page);
//   }
// }

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
  // }
}

uint8_t get_weather_pic(std::string condition) {
  if (condition == "cloudy")
    return 27;
  if (condition == "rainy")
    return 28;
  if (condition == "pouring")
    return 26;

  return 24;
}

void update_temp_text() {
  if (main_lcd != nullptr) {
    std::string low_set_point_string =
        round_float_to_string(
            id(esp_thermostat).target_temperature_low * 1.8 + 32, 0) +
        "\xB0";
    std::string high_set_point_string =
        round_float_to_string(
            id(esp_thermostat).target_temperature_high * 1.8 + 32, 0) +
        "\xB0";

    id(textTempCool).set_state(high_set_point_string, false, true);
    id(textTempHeat).set_state(low_set_point_string, false, true);
  }
}

void display_refresh_mode() {
  Color btnControlCool_color = id(color_control_disabled),
        btnControlHeat_color = id(color_control_disabled),
        btnControlHum_color = id(color_control_disabled);

  int btnModeHeat_Pic = id(btnModeHeat_Pic_Off);
  int btnModeCool_Pic = id(btnModeCool_Pic_Off);
  int btnModeFan_Pic = id(btnModeFan_Pic_Off);

  // const char *btnControlCool_color = color_lowlight,
  //            *btnModeCool_color = color_lowlight,
  //            *btnModeFan_color = color_lowlight,
  //            *btnControlHeat_color = color_lowlight,
  //            *btnModeHeat_color = color_lowlight,
  //            *btnModeOff_color = color_lowlight,
  //            *btnControlHum_color = color_highlight;

  update_temp_text();

  switch (id(esp_thermostat).mode) {
  case CLIMATE_MODE_OFF:
    // btnModeOff_color = id(color_control_enabled);
    btnControlHum_color = id(color_control_disabled);
    // btnModeOff_color = color_highlight;
    // btnControlHum_color = color_lowlight;
    break;

  case CLIMATE_MODE_AUTO:
    btnControlCool_color = id(color_control_enabled);
    // btnModeCool_color = id(color_control_enabled);
    btnControlHeat_color = id(color_control_enabled);
    // btnModeHeat_color = id(color_control_enabled);
    btnModeCool_Pic = id(btnModeCool_Pic_On);
    btnModeHeat_Pic = id(btnModeHeat_Pic_On);
    // btnControlCool_color = color_highlight;
    // btnModeCool_color = color_highlight;
    // btnControlHeat_color = color_highlight;
    // btnModeHeat_color = color_highlight;
    break;

  case CLIMATE_MODE_COOL:
    btnControlCool_color = id(color_control_enabled);
    btnModeCool_Pic = id(btnModeCool_Pic_On);
    // btnControlCool_color = color_highlight;
    // btnModeCool_color = color_highlight;
    break;

  case CLIMATE_MODE_HEAT:
    btnControlHeat_color = id(color_control_enabled);
    btnModeHeat_Pic = id(btnModeHeat_Pic_On);
    // btnControlHeat_color = color_highlight;
    // btnModeHeat_color = color_highlight;
    break;

  case CLIMATE_MODE_FAN_ONLY:
    btnControlCool_color = id(color_control_enabled);
    btnModeFan_Pic = id(btnModeFan_Pic_On);
    // btnControlCool_color = color_highlight;
    // btnModeFan_color = color_highlight;
    break;

  case CLIMATE_MODE_DRY:
    // btnModeDry_color = color_highlight;
    break;
  }

  // cool button and setpoint controls

  id(nextion_touch_down_cool).set_foreground_color(btnControlCool_color);
  id(nextion_touch_up_cool).set_foreground_color(btnControlCool_color);
  id(textTempCool).set_foreground_color(btnControlCool_color);
  // main_lcd->set_component_font_color("btnModeCool", btnModeCool_color);
  // main_lcd->set_component_font_color("btnDownCool", btnControlCool_color);
  // main_lcd->set_component_font_color("btnUpCool", btnControlCool_color);
  // main_lcd->set_component_font_color("textTempCool", btnControlCool_color);
  // heat button and setpoint controls

  // id(nextion_touch_mode_heat).set_foreground_color(btnModeHeat_color);

  // id(nextion_touch_mode_cool).set_foreground_color(btnModeCool_color);

  id(main_lcd).set_component_pic(
      id(nextion_touch_mode_cool).get_variable_name().c_str(), btnModeCool_Pic);
  id(main_lcd).set_component_pic(
      id(nextion_touch_mode_heat).get_variable_name().c_str(), btnModeHeat_Pic);
  id(main_lcd).set_component_pic(
      id(nextion_touch_mode_fan).get_variable_name().c_str(), btnModeFan_Pic);

  // id(nextion_touch_mode_fan).set_foreground_color(btnModeFan_color);
  // id(nextion_touch_mode_off).set_foreground_color(btnModeOff_color);

  id(nextion_touch_down_heat).set_foreground_color(btnControlHeat_color);
  id(nextion_touch_up_heat).set_foreground_color(btnControlHeat_color);
  id(textTempHeat).set_foreground_color(btnControlHeat_color);

  // main_lcd->set_component_font_color("btnModeHeat", btnModeHeat_color);
  // main_lcd->set_component_font_color("btnDownHeat", btnControlHeat_color);
  // main_lcd->set_component_font_color("btnUpHeat", btnControlHeat_color);
  // main_lcd->set_component_font_color("textTempHeat", btnControlHeat_color);
  // the rest of it

  // main_lcd->set_component_font_color("btnModeFan", btnModeFan_color);
  // main_lcd->set_component_font_color("btnModeOff", btnModeOff_color);
  // humidity controls
  id(nextion_touch_down_hum).set_foreground_color(btnControlHum_color);
  id(nextion_touch_up_hum).set_foreground_color(btnControlHum_color);
  id(textHumSet).set_foreground_color(btnControlHum_color);
  // main_lcd->set_component_font_color("btnDownHum", btnControlHum_color);
  // main_lcd->set_component_font_color("btnUpHum", btnControlHum_color);
  // main_lcd->set_component_font_color("textHumSet", btnControlHum_color);
}

void display_refresh_fan_mode() {
  // Color btnFanModeAuto_color = id(color_control_disabled), btnFanModeOn_color
  // = id(color_control_disabled); const char *btnFanModeAuto_color =
  // color_lowlight,
  //            *btnFanModeOn_color = color_lowlight;

  int btnModeFan_Pic = id(btnModeFan_Pic_Off);

  switch (id(esp_thermostat).fan_mode) {
  case CLIMATE_FAN_ON:
    btnModeFan_Pic = id(btnModeFan_Pic_On);
    // btnFanModeOn_color = color_highlight;
    break;
  case CLIMATE_FAN_OFF:
    // btnFanModeOff_color = color_highlight;
    break;
  case CLIMATE_FAN_AUTO:
    btnModeFan_Pic = id(btnModeFan_Pic_Auto);
    // btnFanModeAuto_color = color_highlight;
    break;
  case CLIMATE_FAN_LOW:
    // btnFanModeLow_color = color_highlight;
    break;
  case CLIMATE_FAN_MEDIUM:
    // btnFanModeMedium_color = color_highlight;
    break;
  case CLIMATE_FAN_HIGH:
    // btnFanModeHigh_color = color_highlight;
    break;
  case CLIMATE_FAN_MIDDLE:
    // btnFanModeMiddle_color = color_highlight;
    break;
  case CLIMATE_FAN_FOCUS:
    // btnFanModeFocus_color = color_highlight;
    break;
  case CLIMATE_FAN_DIFFUSE:
    // btnFanModeDiffuse_color = color_highlight;
    break;
  }

  // id(nextion_touch_fan_mode_on).set_foreground_color(btnFanModeOn_color);
  // id(nextion_touch_fan_mode_auto).set_foreground_color(btnFanModeAuto_color);

  id(main_lcd).set_component_pic(
      id(nextion_touch_mode_fan).get_variable_name().c_str(), btnModeFan_Pic);

  // main_lcd->set_component_font_color("btnFanModeOn", btnFanModeOn_color);
  // main_lcd->set_component_font_color("btnFanModeAuto", btnFanModeAuto_color);
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
      round_float_to_string(id(esp_thermostat_temperature_sensor).state * 1.8 +
                            32) +
      "\xB0";
  id(tempCurrent).set_state(temperature_string, false, true);
}

void update_climate_current_humidity(float humidity) {
  std::string humidity_string =
      round_float_to_string(id(esp_thermostat_humidity_sensor).state) + "% RH";
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
      humidity_ts(row)->set_state(round_float_to_string(humidity) + "%", false,
                                  true);
    }
  }
}

void update_climate_table_temperature(int row, float temperature) {
  if ((row >= 0) && (row < num_rooms)) {
    if (room_temperature[row] != temperature) {
      room_temperature[row] = temperature;
      temperature_ts(row)->set_state(
          round_float_to_string(temperature * 1.8 + 32) + "\xB0", false, true);
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

  if (id(esp_thermostat_api_status).state == false &&
      id(on_board_sensor_active) == true) {
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

  ESP_LOGD("TEST", "Sending textStatus \"%s\"", status_message.c_str());
  id(textStatus).set_state(status_message, false, true);
}

void draw_main_screen(bool fullRefresh = false) {
  ESP_LOGD(TAG, "draw_main_screen start fullRefresh : %s",
           TRUEFALSE(fullRefresh));
  auto dateTime = id(esptime).now();
  // only do a full refresh once per hour (and at start-up)
  if (id(display_last_full_refresh) != dateTime.hour) {
    id(display_last_full_refresh) = dateTime.hour;
    // main_lcd->set_touch_sleep_timeout(60);
    main_lcd->set_nextion_rtc_time(dateTime);
  }
  // ESP_LOGD("********** HEAP **********", " Free: %d", ESP.getFreeHeap());
  // ESP_LOGD("********** HEAP **********", "Total: %d", ESP.getHeapSize());

  update_temp_text();

  if (fullRefresh) {
    update_status();
    display_refresh_action->execute();
    display_refresh_mode();
    display_refresh_fan_mode();
    display_refresh_sensor_names();
  }
}

float adjust_high_set_point(float adjustment) {
  float high_set_point = id(esp_thermostat).target_temperature_high +=
      adjustment,
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
  else if (low_set_point >
           esp32_thermostat::upper_temp_boundary - temp_step_size)
    low_set_point = esp32_thermostat::upper_temp_boundary - temp_step_size;
  if (low_set_point >= high_set_point)
    id(esp_thermostat).target_temperature_high = low_set_point + temp_step_size;

  id(esp_thermostat).target_temperature_low = low_set_point;

  update_temp_text();

  return low_set_point;
}

float thermostat_sensor_update() {
  bool template_sensor_valid =
      (id(current_temperature) >= esp32_thermostat::lower_temp_boundary) &&
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
      (id(esp_thermostat_temperature_sensor).state -
           id(esp_thermostat).target_temperature_high >=
       esp32_thermostat::second_stage_activation_delta))
    id(esp_thermostat_cool_2).turn_on();

  if (id(esp_thermostat_heat_1).state &&
      (id(esp_thermostat).target_temperature_low -
           id(esp_thermostat_temperature_sensor).state >=
       esp32_thermostat::second_stage_activation_delta))
    id(esp_thermostat_heat_2).turn_on();

  if (id(on_board_sensor_active) || !template_sensor_valid) {
    if (!isnan(sensor_value)) {
      id(sensor_ready) = true;
      id(current_temperature) = sensor_value;
      return sensor_value;
    } else {
      return (id(esp_thermostat).target_temperature_low +
              id(esp_thermostat).target_temperature_high) /
             2;
    }
  } else {
    id(sensor_ready) = true;
    return id(current_temperature);
  }
}
} // namespace esp32_thermostat