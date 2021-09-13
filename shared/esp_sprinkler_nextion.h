#pragma once
#define USE_TIME

#include <iomanip>
#include <sstream>

namespace esp_sprinkler
{
  enum DisplaySensor : size_t
  {
    DS_WEATHER_TEMP = 0,
    DS_WEATHER_HUM = 1,
    DS_WEATHER_TEMPHIGH = 2,
    DS_WEATHER_TEMPLOW = 3,
    DS_NUM_OF_SENSORS = 4
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

  const std::vector<esp_sprinkler::WeatherCondPairing> weather_cond_icon{
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
  const std::vector<esp_sprinkler::IaqCondPairing> iaq_cond_text{
      {50, Color(0, 255, 96), "Excellent"},
      {100, Color(160, 255, 0), "Good"},
      {150, Color(255, 255, 0), "Moderate"},
      {200, Color(255, 127, 0), "Unhealthy"},
      {300, Color(255, 32, 32), "Very Unhealthy"},
      {9000, Color(160, 24, 64), "Hazardous"}};
  const char default_weather_cond_icon_id = 'O';
  std::vector<esp_sprinkler::NextionSensorPairing> sensor_pairing(4);

  // verifies that times (hours/minutes) sent through the API are valid; makes them valid if not
  // void validate_start_time() {
  //   if ((id(start_hour) < 0) || (id(start_hour) > 23))
  //     id(start_hour) = 0;
  //   if ((id(start_minute) < 0) || (id(start_minute) > 59))
  //     id(start_minute) = 0;
  // }
  // sets the display page; called by the API
  // void set_display_page(uint8_t page) {
  //   if (page < esp_sprinkler::last_page_number) {
  //     id(selected_display_page) = page;
  //     // if any page in the array is set to nullptr, refresh them to get valid pointers
  //     if (display_pages[id(selected_display_page)].page == nullptr)
  //       refresh_display_pages();
  //     // finally, set/show the new page
  //     id(main_lcd).show_page(display_pages[id(selected_display_page)].page);
  //   }
  // }

  // verifies that times (hours/minutes) sent through the API are valid; makes them valid if not
  void validate_start_time()
  {
    if ((id(start_hour) < 0) || (id(start_hour) > 23))
      id(start_hour) = 0;
    if ((id(start_minute) < 0) || (id(start_minute) > 59))
      id(start_minute) = 0;
  }

  esp_sprinkler::NextionSensorPairing display_sensor(DisplaySensor sensor)
  {
    if ((sensor >= 0) && (sensor < static_cast<uint8_t>(DS_NUM_OF_SENSORS)))
    {
      if ((sensor_pairing[sensor].whole != nullptr) && (sensor_pairing[sensor].frac != nullptr))
      {
        return sensor_pairing[sensor];
      }
      sensor_pairing[0].whole = nextionWeatherTemp;
      sensor_pairing[0].frac = nextionWeatherTempFrac;
      sensor_pairing[1].whole = nextionWeatherHum;
      sensor_pairing[1].frac = nextionWeatherHumFrac;
      sensor_pairing[2].whole = nextionWeatherTempHigh;
      sensor_pairing[2].frac = nextionWeatherTempHighFrac;
      sensor_pairing[3].whole = nextionWeatherTempLow;
      sensor_pairing[3].frac = nextionWeatherTempLowFrac;

      return sensor_pairing[sensor];
    }
    return esp_sprinkler::NextionSensorPairing();
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

  void display_refresh_zone_names()
  {
    id(nextionTextZone1Name).set_state(id(sprinkler_ctrlr).valve_name(0), false, true);
    id(nextionTextZone2Name).set_state(id(sprinkler_ctrlr).valve_name(1), false, true);
    id(nextionTextZone3Name).set_state(id(sprinkler_ctrlr).valve_name(2), false, true);
    id(nextionTextZone4Name).set_state(id(sprinkler_ctrlr).valve_name(3), false, true);
    id(nextionTextZone5Name).set_state(id(sprinkler_ctrlr).valve_name(4), false, true);
  }

  void display_refresh_zone_run_durations()
  {
    id(nextionZone1RunTimeMin).set_state(id(sprinkler_ctrlr).valve_run_duration_adjusted(0) / 60);
    id(nextionZone2RunTimeMin).set_state(id(sprinkler_ctrlr).valve_run_duration_adjusted(1) / 60);
    id(nextionZone3RunTimeMin).set_state(id(sprinkler_ctrlr).valve_run_duration_adjusted(2) / 60);
    id(nextionZone4RunTimeMin).set_state(id(sprinkler_ctrlr).valve_run_duration_adjusted(3) / 60);
    id(nextionZone5RunTimeMin).set_state(id(sprinkler_ctrlr).valve_run_duration_adjusted(4) / 60);
  }

  void display_refresh_zone_run_enable_states()
  {
    id(nextionZone1Enabled).set_state(id(esp_sprinkler_controller_zone_1_enable).state);
    id(nextionZone2Enabled).set_state(id(esp_sprinkler_controller_zone_2_enable).state);
    id(nextionZone3Enabled).set_state(id(esp_sprinkler_controller_zone_3_enable).state);
    id(nextionZone4Enabled).set_state(id(esp_sprinkler_controller_zone_4_enable).state);
    id(nextionZone5Enabled).set_state(id(esp_sprinkler_controller_zone_5_enable).state);
  }

  void display_refresh_sensor_pair(double value, const NextionSensorPairing sensor, const DisplayConversion conv = esp_sprinkler::DC_NONE)
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

  void display_refresh_sensor_pair(const double value, const DisplaySensor sensor_num, const DisplayConversion conv = esp_sprinkler::DC_NONE)
  {
    display_refresh_sensor_pair(value, display_sensor(sensor_num), conv);
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
    std::string status_message;

    if (id(esp_sprinkler_controller_api_status).state == false)
    {
      status_message = offline_message;
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

  void display_refresh_sprinkler_state()
  {
    id(nextionSprinklerZone).set_state(id(sprinkler_ctrlr).active_valve());
    id(nextionSprinklerAutoAdv).set_state(id(sprinkler_ctrlr).auto_advance());
    id(nextionSprinklerReverse).set_state(id(sprinkler_ctrlr).reverse());
  }

  void display_refresh_door_state()
  {
    id(nextionDoorState).set_state(id(esp_sprinkler_controller_door_sensor).state);
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

    if (id(sprinkler_ctrlr).is_a_valid_valve(id(sprinkler_ctrlr).active_valve()))
    {
      id(nextionSprinklerZoneTotalSecRemain).set_state(id(sprinkler_ctrlr).time_remaining());
    }

    if (fullRefresh)
    {
      display_refresh_status();
      display_refresh_weather_cond();
      display_refresh_sensor_pair(id(weather_humidity), esp_sprinkler::DS_WEATHER_HUM, esp_sprinkler::DC_NONE);
      display_refresh_sensor_pair(id(weather_temperature), esp_sprinkler::DS_WEATHER_TEMP, esp_sprinkler::DC_NONE);
      display_refresh_sensor_pair(id(weather_temperature_high), esp_sprinkler::DS_WEATHER_TEMPHIGH, esp_sprinkler::DC_NONE);
      display_refresh_sensor_pair(id(weather_temperature_low), esp_sprinkler::DS_WEATHER_TEMPLOW, esp_sprinkler::DC_NONE);
      display_refresh_door_state();
      display_refresh_sensor_names();
      display_refresh_zone_run_enable_states();
      display_refresh_zone_names();
      display_refresh_zone_run_durations();
    }
  }
}