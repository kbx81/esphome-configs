#pragma once
#define USE_TIME

#include <iomanip>
#include <sstream>

namespace esp_sprinkler {

struct IaqCondPairing {
  const uint16_t threshold;
  const Color color;
  const std::string condition;
};

struct WeatherCondPairing {
  const char icon_id;
  const std::string condition;
};

// constants
const char default_weather_cond_icon_id = 'O';

const std::vector<esp_sprinkler::WeatherCondPairing> weather_cond_icon{
    {'F', "clear-night"}, {'A', "cloudy"},          {'O', "exceptional"},  {'B', "fog"},     {'C', "hail"},
    {'E', "lightning"},   {'D', "lightning-rainy"}, {'G', "partlycloudy"}, {'H', "pouring"}, {'I', "rainy"},
    {'K', "snowy"},       {'J', "snowy-rainy"},     {'L', "sunny"},        {'M', "windy"},   {'N', "windy-variant"}};
const std::vector<esp_sprinkler::IaqCondPairing> iaq_cond_text{
    {50, Color(0, 255, 96), "Excellent"},        {100, Color(160, 255, 0), "Good"},
    {150, Color(255, 255, 0), "Moderate"},       {200, Color(255, 127, 0), "Unhealthy"},
    {300, Color(255, 32, 32), "Very Unhealthy"}, {9000, Color(160, 24, 64), "Hazardous"}};

float c_to_f(float degrees_c) { return (degrees_c * 1.8) + 32; }
float f_to_c(float degrees_f) { return (degrees_f - 32.0) * 5.0 / 9.0; }

// verifies that times (hours/minutes) sent through the API are valid; makes them valid if not
void validate_start_time() {
  if ((id(start_hour) < 0) || (id(start_hour) > 23))
    id(start_hour) = 0;
  if ((id(start_minute) < 0) || (id(start_minute) > 59))
    id(start_minute) = 0;
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

void display_refresh_iaq_txt(float iaq_value, bool is_weather = false) {
  auto iaq_string = esp_sprinkler::round_float_to_string(iaq_value);
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
  }
}

void display_refresh_zone_names() {
  // ...the '+ 16's below are there to trim off our switches' name prefix "Lawn Sprinklers "
  id(nextionTextZone1Name).set_state(id(sprinkler_ctrlr).valve_name(0) + 16, false, true);
  id(nextionTextZone2Name).set_state(id(sprinkler_ctrlr).valve_name(1) + 16, false, true);
  id(nextionTextZone3Name).set_state(id(sprinkler_ctrlr).valve_name(2) + 16, false, true);
  id(nextionTextZone4Name).set_state(id(sprinkler_ctrlr).valve_name(3) + 16, false, true);
  id(nextionTextZone5Name).set_state(id(sprinkler_ctrlr).valve_name(4) + 16, false, true);
}

void display_refresh_zone_run_durations() {
  id(nextionZone1RunTimeMin).set_state(id(sprinkler_ctrlr).valve_run_duration_adjusted(0) / 60);
  id(nextionZone2RunTimeMin).set_state(id(sprinkler_ctrlr).valve_run_duration_adjusted(1) / 60);
  id(nextionZone3RunTimeMin).set_state(id(sprinkler_ctrlr).valve_run_duration_adjusted(2) / 60);
  id(nextionZone4RunTimeMin).set_state(id(sprinkler_ctrlr).valve_run_duration_adjusted(3) / 60);
  id(nextionZone5RunTimeMin).set_state(id(sprinkler_ctrlr).valve_run_duration_adjusted(4) / 60);
}

void display_refresh_zone_run_enable_states() {
  id(nextionZone1Enabled).set_state(id(sprinkler_ctrlr).enable_switch(0)->state);
  id(nextionZone2Enabled).set_state(id(sprinkler_ctrlr).enable_switch(1)->state);
  id(nextionZone3Enabled).set_state(id(sprinkler_ctrlr).enable_switch(2)->state);
  id(nextionZone4Enabled).set_state(id(sprinkler_ctrlr).enable_switch(3)->state);
  id(nextionZone5Enabled).set_state(id(sprinkler_ctrlr).enable_switch(4)->state);
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

  if (id(esp_sprinkler_controller_api_status).state == false) {
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

void display_refresh_sprinkler_state() {
  id(nextionSprinklerActiveZone)
      .set_state(id(sprinkler_ctrlr).manual_valve().value_or(id(sprinkler_ctrlr).active_valve().value_or(-1)));
  id(nextionSprinklerPausedZone).set_state(id(sprinkler_ctrlr).paused_valve().value_or(-1));
  id(nextionSprinklerAutoAdv).set_state(id(sprinkler_ctrlr).auto_advance());
  id(nextionSprinklerReverse).set_state(id(sprinkler_ctrlr).reverse());
  id(nextionSprinklerStandby).set_state(id(sprinkler_ctrlr).standby());
  id(nextionSprinklerMultiplier).set_state(id(sprinkler_ctrlr).multiplier() * 10.0);
  id(nextionSprinklerRepeat).set_state(id(sprinkler_ctrlr).repeat().value_or(0));
  display_refresh_zone_run_enable_states();
}

void display_refresh_door_state() { id(nextionDoorState).set_state(id(esp_sprinkler_controller_door_sensor).state); }

void draw_main_screen(bool fullRefresh = false) {
  auto dateTime = id(esptime).now();
  // only do a full refresh once per hour (and at start-up)
  if (id(display_last_full_refresh) != dateTime.hour) {
    id(display_last_full_refresh) = dateTime.hour;
    // main_lcd->set_touch_sleep_timeout(60);
    main_lcd->set_nextion_rtc_time(dateTime);
  }

  if (id(sprinkler_ctrlr).active_valve().has_value()) {
    id(nextionSprinklerZoneTotalSecRemain).set_state(id(sprinkler_ctrlr).time_remaining_active_valve().value_or(0));

    // auto total_time_remaining = id(sprinkler_ctrlr).time_remaining_current_operation();
    // if (total_time_remaining.has_value()) {
    //   auto hours = total_time_remaining.value() / 3600;
    //   auto minutes = (total_time_remaining.value() - (hours * 3600)) / 60;
    //   auto seconds = total_time_remaining.value() - (hours * 3600) - (minutes * 60);
    //   ESP_LOGD("sprinkler", "Total time remaining: %u:%u:%u", hours, minutes, seconds);
    // }
  }

  if (fullRefresh) {
    display_refresh_status();
    display_refresh_door_state();
    display_refresh_zone_run_enable_states();
    display_refresh_zone_names();
    display_refresh_zone_run_durations();
  }
}
}  // namespace esp_sprinkler