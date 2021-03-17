#pragma once

namespace esp32_thermostat {
  typedef struct displaymode_t {
    DisplayPage* page;
    uint8_t numItems;
  } displaymode_t;

  std::vector<displaymode_t> display_pages = {
    {main_screen, 0},
    {rooms_screen, 0},
    {setpoint_screen, 2},
    {mode_screen, 2},
    {sensors_screen, 0}
  };

  const std::vector<climate::ClimateMode> supported_modes = {
      climate::CLIMATE_MODE_OFF,
      climate::CLIMATE_MODE_AUTO,
      climate::CLIMATE_MODE_COOL,
      climate::CLIMATE_MODE_HEAT,
      climate::CLIMATE_MODE_FAN_ONLY
  };
  const std::vector<climate::ClimateFanMode> supported_fan_modes = {
      climate::CLIMATE_FAN_ON,
      climate::CLIMATE_FAN_AUTO
  };
  const uint8_t num_rooms = 4;
  const uint8_t num_modes = supported_modes.size();
  const uint8_t num_fan_modes = supported_fan_modes.size();
  const uint8_t last_page_number = display_pages.size();
  const uint8_t max_missed_online_updates = 60 * (60 / 15);
  const uint8_t max_missed_offline_updates = 2 * (60 / 15);
  const uint8_t encoder_step_size = 1;
  const uint8_t line_height_small = 10;
  const uint8_t line_height_tiny = 8;
  const float   lower_temp_boundary = (61 - 32) * 5 / 9;
  const float   upper_temp_boundary = (90 - 32) * 5 / 9;
  const float   second_stage_activation_delta = 2 * 5 / 9;
  const float   temp_step_size = encoder_step_size * (5.0 / 9.0);

  // enum for footer climate display
  enum FooterClimateDisplayItem : uint8_t {
    NO_CLIMATE = 0,
    LOCAL_CLIMATE = 1,
    ROOM_CLIMATE = 2
  };
  // enum for display modes/screens/pages
  enum DisplayPageEnum : uint8_t {
    MAIN_SCREEN = 0,
    ROOMS_SCREEN = 1,
    SETPOINT_SCREEN = 2,
    MODE_SCREEN = 3,
    SENSORS_SCREEN = 4
  };

  void refresh_display_pages() {
    display_pages[MAIN_SCREEN].page = main_screen;
    display_pages[ROOMS_SCREEN].page = rooms_screen;
    display_pages[SETPOINT_SCREEN].page = setpoint_screen;
    display_pages[MODE_SCREEN].page = mode_screen;
    display_pages[SENSORS_SCREEN].page = sensors_screen;
  }

  void set_display_page(uint8_t page) {
    if (page < esp32_thermostat::last_page_number) {
      id(selected_display_page) = page;
      // if any page in the array is set to nullptr, refresh them to get valid pointers
      if (display_pages[id(selected_display_page)].page == nullptr)
        refresh_display_pages();
      // finally, set/show the new page
      id(main_lcd).show_page(display_pages[id(selected_display_page)].page);
    }
  }

  bool itemOverflow() {
    if ((id(selected_display_page) == SETPOINT_SCREEN) && (id(esp_thermostat).mode != CLIMATE_MODE_AUTO))
      return id(selected_display_item) > 0;

    return id(selected_display_item) >= display_pages[id(selected_display_page)].numItems;
  }

  void mode_button_click() {
    if (id(main_lcd).is_on()) {
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
  }

  void encoder_button_click() {
    if (id(main_lcd).is_on()) {
      // "zero" the encoder, if it has a value
      if (!isnan(id(esp_thermostat_encoder).state))
        id(encoder_value) = id(esp_thermostat_encoder).state;
      // increment selected_display_item and, if it overflows, zero it and...
      ++id(selected_display_item);
      if (itemOverflow() == true) {
        id(selected_display_item) = 0;
        // ...increment selected_display_page and zero it if it overflows
        if (++id(selected_display_page) >= esp32_thermostat::last_page_number)
          id(selected_display_page) = 0;
      }
      set_display_page(id(selected_display_page));
    }
  }
  
  float adjust_high_set_point(float adjustment) {
    float high_set_point = id(esp_thermostat).target_temperature_high += adjustment,
          low_set_point  = id(esp_thermostat).target_temperature_low;

    if (high_set_point < esp32_thermostat::lower_temp_boundary + temp_step_size)
      high_set_point = esp32_thermostat::lower_temp_boundary + temp_step_size;
    else if (high_set_point > esp32_thermostat::upper_temp_boundary)
      high_set_point = esp32_thermostat::upper_temp_boundary;
    if (high_set_point <= low_set_point)
      low_set_point = high_set_point - temp_step_size;

    id(esp_thermostat).target_temperature_high = high_set_point;

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
      high_set_point = low_set_point + temp_step_size;
    
    id(esp_thermostat).target_temperature_low = low_set_point;

    return low_set_point;
  }

  void encoder_value_change() {
    int encoder_offset = 0, mode_selected = 0, fan_mode_selected = 0;
    // we only need to do stuff if we are on a page that would allow something to be changed
    if ((id(selected_display_page) == SETPOINT_SCREEN) || (id(selected_display_page) == MODE_SCREEN)) {
      // set mode_selected to current thermostat climate mode
      for (uint8_t i = 0; i < num_modes; i++) {
          if (supported_modes[i] == id(esp_thermostat).mode)
            mode_selected = i;
      }
      // set fan_mode_selected to current thermostat fan mode
      for (uint8_t i = 0; i < num_fan_modes; i++) {
          if (supported_fan_modes[i] == id(esp_thermostat).fan_mode)
            fan_mode_selected = i;
      }
      // determine the encoder offset (how much whatever it is should move)
      if (!isnan(id(esp_thermostat_encoder).state)) {
          encoder_offset = id(esp_thermostat_encoder).state - id(encoder_value);
          id(encoder_value) = id(esp_thermostat_encoder).state;
      } else {
          encoder_offset = 0;
      }

      switch (id(selected_display_page)) {
        case SETPOINT_SCREEN:
          switch (id(esp_thermostat).mode) {
            case CLIMATE_MODE_OFF:
            case CLIMATE_MODE_AUTO:
            case CLIMATE_MODE_DRY:
              switch (id(selected_display_item)) {
                case 0:   // heat
                  adjust_low_set_point(encoder_offset * temp_step_size);
                  break;
                case 1:   // cool
                  adjust_high_set_point(encoder_offset * temp_step_size);
                  break;
                default:
                  break;
              }
              break;

            case CLIMATE_MODE_COOL:
            case CLIMATE_MODE_FAN_ONLY:
              adjust_high_set_point(encoder_offset * temp_step_size);
              break;

            case CLIMATE_MODE_HEAT:
              adjust_low_set_point(encoder_offset * temp_step_size);
              break;
          }
          break;
        case MODE_SCREEN:
          switch (id(selected_display_item)) {
            case 0:   // mode
              mode_selected += encoder_offset;
              if (mode_selected >= num_modes)
                  mode_selected = num_modes - 1;
              else if (mode_selected < 0)
                  mode_selected = 0;
              break;
            case 1:   // fan mode
              fan_mode_selected += encoder_offset;
              if (fan_mode_selected >= num_fan_modes)
                  fan_mode_selected = num_fan_modes - 1;
              else if (fan_mode_selected < 0)
                  fan_mode_selected = 0;
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }

      id(esp_thermostat).mode = supported_modes[mode_selected];
      id(esp_thermostat).fan_mode = supported_fan_modes[fan_mode_selected];
      id(esp_thermostat).refresh();
    }
  }

float thermostat_sensor_update() {
    bool  template_sensor_valid = (id(current_temperature) >= esp32_thermostat::lower_temp_boundary) && (id(current_temperature) <= esp32_thermostat::upper_temp_boundary);
    float sensor_value = id(esp_thermostat_bme280_temperature).state;
    int   max_missed_updates = esp32_thermostat::max_missed_offline_updates;

    if (id(esp_thermostat_api_status).state)
      max_missed_updates = esp32_thermostat::max_missed_online_updates;

    id(missed_update_count) += 1;

    if (id(missed_update_count) > max_missed_updates)
      id(on_board_sensor_active) = true;
    else
      id(on_board_sensor_active) = false;

    id(esp_thermostat_thermistor_vcc).turn_on();
    id(adc_sensor_thermistor).update();
    id(esp_thermostat_thermistor_vcc).turn_off();

    if (id(esp_thermostat_cool_1).state
        && (id(esp_thermostat_temperature_sensor).state - id(esp_thermostat).target_temperature_high >= esp32_thermostat::second_stage_activation_delta))
      id(esp_thermostat_cool_2).turn_on();

    if (id(esp_thermostat_heat_1).state
        && (id(esp_thermostat).target_temperature_low - id(esp_thermostat_temperature_sensor).state >= esp32_thermostat::second_stage_activation_delta))
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

  void update_climate_table_name(const int row, const std::string name) {
    if ((row >= 0) && (row < num_rooms))
      id(climate_table_name)[row] = name;
  }

  void update_climate_table_humidity(const int row, const float humidity) {
    if ((row >= 0) && (row < num_rooms))
      id(climate_table_humidity)[row] = humidity;
  }

  void update_climate_table_temperature(const int row, const float temperature) {
    if ((row >= 0) && (row < num_rooms))
      id(climate_table_temperature)[row] = temperature;
  }

  void draw_footer(DisplayBuffer* it, FooterClimateDisplayItem climate_display = NO_CLIMATE, uint8_t room_number = 0) {
    it->strftime(it->get_width(), it->get_height(), thermostat_tiny, color_time, TextAlign::BASELINE_RIGHT, "%I:%M:%S %p", id(esptime).now());
    if (id(esp_thermostat_api_status).state) {
      it->strftime(0, it->get_height(), thermostat_tiny, color_time, TextAlign::BASELINE_LEFT, "%m-%d-%Y", id(esptime).now());
    } else {
      it->print(0, it->get_height(), thermostat_tiny, color_highlight, TextAlign::BASELINE_LEFT, "Offline");
    }
    // room_number validation & adjustment
    if ((room_number < 0) || (room_number > num_rooms - 1))
      room_number = 0;
    // display local data if a room was requested but it is not available
    if ((climate_display == ROOM_CLIMATE) && (id(climate_table_name)[room_number].empty()))
      climate_display = LOCAL_CLIMATE;

    switch (climate_display) {
      case LOCAL_CLIMATE:
        if (id(on_board_sensor_active)) {
          it->printf(0, it->get_height() - line_height_tiny, thermostat_tiny, color_mode, TextAlign::BASELINE_LEFT, "On-board sensor in use");
        } else {
          if (!isnan(id(esp_thermostat_bme280_temperature).state)) {
            it->printf(0, it->get_height() - line_height_tiny, thermostat_tiny, color_footer, TextAlign::BASELINE_LEFT, "Local: %.1f°", id(esp_thermostat_bme280_temperature).state * 1.8 + 32);
          }
          if (!isnan(id(esp_thermostat_bme280_humidity).state)) {
            it->printf(it->get_width() - 1, it->get_height() - line_height_tiny, thermostat_tiny, color_footer, TextAlign::BASELINE_RIGHT, "%.1f%% RH", id(esp_thermostat_bme280_humidity).state);
          }
        }
        break;

      case ROOM_CLIMATE:
        it->printf(0, it->get_height() - line_height_tiny, thermostat_tiny, color_footer, TextAlign::BASELINE_LEFT, "%s %.1f°", id(climate_table_name)[room_number].c_str(), id(climate_table_temperature)[room_number] * 1.8 + 32);
        it->printf(it->get_width() - 1, it->get_height() - line_height_tiny, thermostat_tiny, color_footer, TextAlign::BASELINE_RIGHT, "%.1f%% RH", id(climate_table_humidity)[room_number]);
        break;
      
      default:
        break;
    }
  }

  void draw_main_screen(DisplayBuffer* it) {
    const size_t status_string_size = 30;
    char  status_string[status_string_size];
    auto  status_string_color = color_mode;
    float high_set_point = id(esp_thermostat).target_temperature_high * 1.8 + 32,
          low_set_point  = id(esp_thermostat).target_temperature_low  * 1.8 + 32,
          current_temperature = id(esp_thermostat_temperature_sensor).state * 1.8 + 32,
          current_humidity = id(esp_thermostat_humidity_sensor).state;
    
    if ((id(esptime).now().second % 10) < 5) {
      draw_footer(it, LOCAL_CLIMATE);
    } else {
      draw_footer(it, ROOM_CLIMATE, num_rooms - 1);
    }

    if(id(sensor_ready) == false) {
      status_string_color = color_lowlight;
      snprintf(status_string, status_string_size, "Waiting for sensor data...");
    } else {
      switch (id(esp_thermostat).action) {
        case CLIMATE_ACTION_OFF:
        case CLIMATE_ACTION_IDLE:
          switch (id(esp_thermostat).mode) {
            case CLIMATE_MODE_OFF:
              status_string_color = color_lowlight;
              snprintf(status_string, status_string_size, "System is off.");
              break;

            case CLIMATE_MODE_AUTO:
              snprintf(status_string, status_string_size, "Maintain %.0f° to %.0f°.", low_set_point, high_set_point);
              break;

            case CLIMATE_MODE_COOL:
              snprintf(status_string, status_string_size, "Cool to %.0f°.", high_set_point);
              break;

            case CLIMATE_MODE_HEAT:
              snprintf(status_string, status_string_size, "Heat to %.0f°.", low_set_point);
              break;

            case CLIMATE_MODE_FAN_ONLY:
              snprintf(status_string, status_string_size, "Fan only above %.0f°.", high_set_point);
              break;

            case CLIMATE_MODE_DRY:
              snprintf(status_string, status_string_size, "Dry only");
              break;
          }
          break;

        case CLIMATE_ACTION_COOLING:
          status_string_color = color_action;
          if (id(esp_thermostat_cool_2).state == true) {
            snprintf(status_string, status_string_size, "Cooling to %.0f°, 2-stage.", high_set_point);
          } else {
            snprintf(status_string, status_string_size, "Cooling to %.0f°.", high_set_point);
          }
          break;

        case CLIMATE_ACTION_HEATING:
          status_string_color = color_action;
          if (id(esp_thermostat_heat_2).state == true) {
            snprintf(status_string, status_string_size, "Heating to %.0f°, 2-stage.", low_set_point);
          } else {
            snprintf(status_string, status_string_size, "Heating to %.0f°.", low_set_point);
          }
          break;

        case CLIMATE_ACTION_DRYING:
          status_string_color = color_action;
          snprintf(status_string, status_string_size, "Drying.");
          break;

        case CLIMATE_ACTION_FAN:
          status_string_color = color_action;
          snprintf(status_string, status_string_size, "Fanning to %.0f°.", high_set_point);
          break;
      }
      it->printf(0, 0, thermostat_small, status_string_color, TextAlign::TOP_LEFT, status_string);

      switch (id(esp_thermostat).fan_mode) {
        case CLIMATE_FAN_ON:
          snprintf(status_string, status_string_size, "Fan on");
          break;
        case CLIMATE_FAN_OFF:
          break;
        case CLIMATE_FAN_AUTO:
          break;
        case CLIMATE_FAN_LOW:
          snprintf(status_string, status_string_size, "Fan low");
          break;
        case CLIMATE_FAN_MEDIUM:
          snprintf(status_string, status_string_size, "Fan medium");
          break;
        case CLIMATE_FAN_HIGH:
          snprintf(status_string, status_string_size, "Fan high");
          break;
        case CLIMATE_FAN_MIDDLE:
          snprintf(status_string, status_string_size, "Fan middle");
          break;
        case CLIMATE_FAN_FOCUS:
          snprintf(status_string, status_string_size, "Fan focus");
          break;
        case CLIMATE_FAN_DIFFUSE:
          snprintf(status_string, status_string_size, "Fan diffuse");
          break;
      }
      if ((id(esp_thermostat).fan_mode != CLIMATE_FAN_OFF) && (id(esp_thermostat).fan_mode != CLIMATE_FAN_AUTO))
        it->printf(0, 9, thermostat_small, color_action, TextAlign::TOP_LEFT, status_string);

      it->printf((it->get_width() / 2), (it->get_height() / 2), thermostat_large, color_temp, TextAlign::CENTER, "%.1f°", current_temperature);
    }
  }

  void draw_mode_screen(DisplayBuffer* it) {
    const size_t string_size = 10;
    char fan_mode_string[string_size];
    char mode_string[string_size];
    auto highlight_fan_mode = color_lowlight, highlight_mode = color_lowlight;

    draw_footer(it);

    it->printf(0, 0, thermostat_small, color_mode, TextAlign::TOP_LEFT, "Adjust modes:");

    switch (id(selected_display_item)) {
      case 0:   // mode
        highlight_mode = color_highlight;
        break;
      case 1:   // fan mode
        highlight_fan_mode = color_highlight;
        break;
      default:
        break;
    }

    switch (id(esp_thermostat).mode) {
      case climate::CLIMATE_MODE_OFF:
        snprintf(mode_string, string_size, "Off");
        break;
      case climate::CLIMATE_MODE_AUTO:
        snprintf(mode_string, string_size, "Auto");
        break;
      case climate::CLIMATE_MODE_COOL:
        snprintf(mode_string, string_size, "Cool");
        break;
      case climate::CLIMATE_MODE_HEAT:
        snprintf(mode_string, string_size, "Heat");
        break;
      case climate::CLIMATE_MODE_FAN_ONLY:
        snprintf(mode_string, string_size, "Fan");
        break;
      case climate::CLIMATE_MODE_DRY:
        snprintf(mode_string, string_size, "Dry");
        break;
    }

    switch (id(esp_thermostat).fan_mode) {
      case climate::CLIMATE_FAN_ON:
        snprintf(fan_mode_string, string_size, "On");
        break;
      case climate::CLIMATE_FAN_OFF:
        snprintf(fan_mode_string, string_size, "Off");
        break;
      case climate::CLIMATE_FAN_AUTO:
        snprintf(fan_mode_string, string_size, "Auto");
        break;
      case climate::CLIMATE_FAN_LOW:
        snprintf(fan_mode_string, string_size, "Low");
        break;
      case climate::CLIMATE_FAN_MEDIUM:
        snprintf(fan_mode_string, string_size, "Medium");
        break;
      case climate::CLIMATE_FAN_HIGH:
        snprintf(fan_mode_string, string_size, "High");
        break;
      case climate::CLIMATE_FAN_MIDDLE:
        snprintf(fan_mode_string, string_size, "Middle");
        break;
      case climate::CLIMATE_FAN_FOCUS:
        snprintf(fan_mode_string, string_size, "Focus");
        break;
      case climate::CLIMATE_FAN_DIFFUSE:
        snprintf(fan_mode_string, string_size, "Diffuse");
        break;
    }
    it->printf(((it->get_width() / 3) * 1) - 10, 20, thermostat_small, highlight_mode, TextAlign::CENTER, "Mode");
    it->printf(((it->get_width() / 3) * 1) - 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_mode, TextAlign::CENTER, mode_string);

    it->printf(((it->get_width() / 3) * 2) + 10, 20, thermostat_small, highlight_fan_mode, TextAlign::CENTER, "Fan Mode");
    it->printf(((it->get_width() / 3) * 2) + 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_fan_mode, TextAlign::CENTER, fan_mode_string);
  }

  void draw_rooms_screen(DisplayBuffer* it) {
    draw_footer(it, LOCAL_CLIMATE);

    for (uint8_t i = 0; i < num_rooms; i++) {
      if (!(id(climate_table_name)[i].empty())) {
        it->printf(0, i * line_height_small, thermostat_small, color_highlight, TextAlign::TOP_LEFT, "%s", id(climate_table_name)[i].c_str());
        it->printf(it->get_width() - 36, i * line_height_small, thermostat_small, color_highlight, TextAlign::TOP_RIGHT,  "%.1f°", id(climate_table_temperature)[i] * 1.8 + 32);
        it->printf(it->get_width() -  0, i * line_height_small, thermostat_small, color_highlight, TextAlign::TOP_RIGHT, "%.1f%%", id(climate_table_humidity)[i]);
      }
    }
  }

  void draw_setpoint_screen(DisplayBuffer* it) {
    auto  highlight_cool = color_lowlight, highlight_heat = color_lowlight;
    float high_set_point = id(esp_thermostat).target_temperature_high * 1.8 + 32,
          low_set_point  = id(esp_thermostat).target_temperature_low  * 1.8 + 32;

    draw_footer(it);

    it->printf(0, 0, thermostat_small, color_mode, TextAlign::TOP_LEFT, "Adjust setpoint(s):");

    switch (id(esp_thermostat).mode) {
      case CLIMATE_MODE_OFF:
      case CLIMATE_MODE_AUTO:
      case CLIMATE_MODE_DRY:
        switch (id(selected_display_item)) {
          case 0:   // heat
            highlight_heat = color_highlight;
            break;
          case 1:   // cool
            highlight_cool = color_highlight;
            break;
          default:
            break;
        }

        it->printf(((it->get_width() / 3) * 1) - 10, 20, thermostat_small, highlight_heat, TextAlign::CENTER, "Heat");
        it->printf(((it->get_width() / 3) * 1) - 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_heat, TextAlign::CENTER, "%.0f°", low_set_point);

        it->printf(((it->get_width() / 3) * 2) + 10, 20, thermostat_small, highlight_cool, TextAlign::CENTER, "Cool");
        it->printf(((it->get_width() / 3) * 2) + 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_cool, TextAlign::CENTER, "%.0f°", high_set_point);
        break;

      case CLIMATE_MODE_COOL:
      case CLIMATE_MODE_FAN_ONLY:
        it->printf(it->get_width() / 2, 20, thermostat_small, color_highlight, TextAlign::CENTER, "Cool");
        it->printf(it->get_width() / 2, (it->get_height() / 2) + 5, thermostat_medium, color_highlight, TextAlign::CENTER, "%.0f°", high_set_point);
        break;

      case CLIMATE_MODE_HEAT:
        it->printf(it->get_width() / 2, 20, thermostat_small, color_highlight, TextAlign::CENTER, "Heat");
        it->printf(it->get_width() / 2, (it->get_height() / 2) + 5, thermostat_medium, color_highlight, TextAlign::CENTER, "%.0f°", low_set_point);
        break;
    }
  }
}
