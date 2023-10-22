#pragma once
#define USE_TIME

#include <iomanip>
#include <sstream>

namespace esp32_thermostat {

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

float c_to_f(float degrees_c) { return (degrees_c * 1.8) + 32; }
float f_to_c(float degrees_f) { return (degrees_f - 32.0) * 5.0 / 9.0; }

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
    id(nextionTextCurrentAq).set_state(iaq_string, false, true);
    id(nextionTextCurrentAq).set_foreground_color(iaq_color);
  }
}

void display_refresh_set_points() {
  id(nextionCurrentSetLower).set_state(roundf((c_to_f(id(esp_thermostat).target_temperature_low)) * 10), false, true);
  id(nextionCurrentSetUpper).set_state(roundf((c_to_f(id(esp_thermostat).target_temperature_high)) * 10), false, true);
  id(nextionCurrentSetHum).set_state(id(esp_thermostat_target_humidity).state * 10, false, true);
  if (id(nextionCurrentSetMin).state != (c_to_f(id(esp_thermostat).get_traits().get_visual_min_temperature())) * 10) {
    id(nextionCurrentSetMin)
        .set_state((c_to_f(id(esp_thermostat).get_traits().get_visual_min_temperature())) * 10, true, true);
  }
  if (id(nextionCurrentSetMax).state != (c_to_f(id(esp_thermostat).get_traits().get_visual_max_temperature())) * 10) {
    id(nextionCurrentSetMax)
        .set_state((c_to_f(id(esp_thermostat).get_traits().get_visual_max_temperature())) * 10, true, true);
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
  std::string status_message;

  if (id(esp_thermostat_api_status).state == false) {
    status_message = offline_message;
  }

  if (status_message.empty()) {
    status_message = id(status_string);
  } else if (!id(status_string).empty()) {
    status_message = status_message + " - " + id(status_string);
  }

  if (!status_message.empty())
    status_message[0] = toupper(status_message[0]);

  if (id(nextionTextStatus).state != status_message)
    id(nextionTextStatus).set_state(status_message, false, true);
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
  }
}
}  // namespace esp32_thermostat