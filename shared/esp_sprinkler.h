#pragma once

namespace esp_sprinkler {
  // array of zone names. item 0 is "Off" because reasons
  const std::vector<const char*> zone_names = {
    "Off",
    "Parkway",
    "Front yard",
    "East side",
    "Back yard",
    "West side",
  };
  // displaymode type
  typedef struct {
    DisplayPage* page;  // pointer to associated page
    uint8_t numItems;   // number of menu items on page
  } displaymode_t;
  // zone type
  typedef struct {
    const char* name;                         // pointer to zone name string
    uint16_t run_time;                        // zone run time
    gpio::GPIOSwitch* hard_switch;            // pointer to hardware switch object
    template_::TemplateSwitch* soft_switch;   // pointer to software/template switch object
    template_::TemplateSwitch* enable_switch; // pointer to "zone enable" switch object
  } zone_t;

  std::vector<displaymode_t> display_pages = {
    {main_screen, 0},   // main screen, no menu items
    {setup_screen, 9}   // setup screen, 9 menu items
  };
  // 'zone' objects
  std::vector<zone_t> zone = {
    {zone_names[1], 0, esp_sprinkler_controller_triac_zone_1, esp_sprinkler_controller_zone_1, esp_sprinkler_controller_zone_1_enable},
    {zone_names[2], 0, esp_sprinkler_controller_triac_zone_2, esp_sprinkler_controller_zone_2, esp_sprinkler_controller_zone_2_enable},
    {zone_names[3], 0, esp_sprinkler_controller_triac_zone_3, esp_sprinkler_controller_zone_3, esp_sprinkler_controller_zone_3_enable},
    {zone_names[4], 0, esp_sprinkler_controller_triac_zone_4, esp_sprinkler_controller_zone_4, esp_sprinkler_controller_zone_4_enable},
    {zone_names[5], 0, esp_sprinkler_controller_triac_zone_5, esp_sprinkler_controller_zone_5, esp_sprinkler_controller_zone_5_enable}
  };
  // various constants
  const uint8_t last_page_number = display_pages.size();
  const uint8_t last_zone_number = zone.size();
  const uint8_t line_height_small = 10;
  const uint8_t line_height_tiny = 8;
  // enum for display modes/screens/pages
  enum DisplayPageEnum : uint8_t {
    MAIN_SCREEN = 0,
    SETUP_SCREEN = 1
  };
  // this series of 'refresh' functions update pointers to objects; likely only used at startup
  void refresh_display_pages() {
    display_pages[MAIN_SCREEN].page = main_screen;
    display_pages[SETUP_SCREEN].page = setup_screen;
  }

  void refresh_zone_pointers() {
    zone[0].enable_switch = esp_sprinkler_controller_zone_1_enable;
    zone[1].enable_switch = esp_sprinkler_controller_zone_2_enable;
    zone[2].enable_switch = esp_sprinkler_controller_zone_3_enable;
    zone[3].enable_switch = esp_sprinkler_controller_zone_4_enable;
    zone[4].enable_switch = esp_sprinkler_controller_zone_5_enable;

    zone[0].soft_switch = esp_sprinkler_controller_zone_1;
    zone[1].soft_switch = esp_sprinkler_controller_zone_2;
    zone[2].soft_switch = esp_sprinkler_controller_zone_3;
    zone[3].soft_switch = esp_sprinkler_controller_zone_4;
    zone[4].soft_switch = esp_sprinkler_controller_zone_5;

    zone[0].hard_switch = esp_sprinkler_controller_triac_zone_1;
    zone[1].hard_switch = esp_sprinkler_controller_triac_zone_2;
    zone[2].hard_switch = esp_sprinkler_controller_triac_zone_3;
    zone[3].hard_switch = esp_sprinkler_controller_triac_zone_4;
    zone[4].hard_switch = esp_sprinkler_controller_triac_zone_5;
  }

  void refresh_zone_run_times() {
    zone[0].run_time = id(zone_1_run_time);
    zone[1].run_time = id(zone_2_run_time);
    zone[2].run_time = id(zone_3_run_time);
    zone[3].run_time = id(zone_4_run_time);
    zone[4].run_time = id(zone_5_run_time);
  }
  // verifies that times (hours/minutes) sent through the API are valid; makes them valid if not
  void validate_start_time() {
    if ((id(start_hour) < 0) || (id(start_hour) > 23))
      id(start_hour) = 0;
    if ((id(start_minute) < 0) || (id(start_minute) > 59))
      id(start_minute) = 0;
  }
  // verifies that the multiplier sent through the API is valid; makes it valid if not
  void validate_multiplier() {
    if (id(zone_run_time_multiplier) <= 0)
      id(zone_run_time_multiplier) = 1;
    if (id(zone_run_time_multiplier) > 100)
      id(zone_run_time_multiplier) = 100;
  }
  // returns true if any zone is active/turned on
  bool there_is_an_active_zone() {
    return ((id(active_zone) >= 0) && (id(active_zone) < last_zone_number));
  }
  // validation for zone numbers
  bool is_a_valid_zone(const int8_t zone_number) {
    return ((zone_number >= 0) && (zone_number < last_zone_number));
  }
  // returns a pointer to a zone's enable switch object
  template_::TemplateSwitch* zone_enable_switch(const uint8_t zone_number) {
    if (is_a_valid_zone(zone_number)) {
      if (zone[zone_number].enable_switch == nullptr) {
        refresh_zone_pointers();
      }
      return zone[zone_number].enable_switch;
    }
    return nullptr;
  }
  // returns a pointer to a zone's soft/template switch object
  template_::TemplateSwitch* zone_switch(const uint8_t zone_number) {
    if (is_a_valid_zone(zone_number)) {
      if (zone[zone_number].soft_switch == nullptr) {
        refresh_zone_pointers();
      }
      return zone[zone_number].soft_switch;
    }
    return nullptr;
  }
  // returns a pointer to a zone's hardware switch object
  gpio::GPIOSwitch* zone_triac(const uint8_t zone_number) {
    if (is_a_valid_zone(zone_number)) {
      if (zone[zone_number].hard_switch == nullptr) {
        refresh_zone_pointers();
      }
      return zone[zone_number].hard_switch;
    }
    return nullptr;
  }
  // returns a pointer to a zone's name string object
  const char* zone_name(const uint8_t zone_number) {
    if (is_a_valid_zone(zone_number)) {
      return zone_names[zone_number + 1];
    }
    return zone_names[0];
  }
  // if a zone is active, returns a pointer to the active zone's enable switch object; returns 'nullptr' otherwise
  template_::TemplateSwitch* active_zone_enable_switch() {
    if (there_is_an_active_zone()) {
      return zone_enable_switch(id(active_zone));
    }
    return nullptr;
  }
  // if a zone is active, returns a pointer to the active zone's soft/template switch object; returns 'nullptr' otherwise
  template_::TemplateSwitch* active_zone_switch() {
    if (there_is_an_active_zone()) {
      return zone_switch(id(active_zone));
    }
    return nullptr;
  }
  // if a zone is active, returns a pointer to the active zone's hardware switch object; returns 'nullptr' otherwise
  gpio::GPIOSwitch* active_zone_triac() {
    if (there_is_an_active_zone()) {
      return zone_triac(id(active_zone));
    }
    return nullptr;
  }
  // if a zone is active, returns a pointer to the active zone's name string object; returns a pointer to "Off" otherwise
  const char* active_zone_name() {
    if (there_is_an_active_zone()) {
      return zone_names[id(active_zone) + 1];
    }
    return zone_names[0];
  }
  // returns the number of the next zone that should be activated. if no zone is next (no more are enabled), returns -1
  int8_t next_zone(const int8_t first_zone) {
    if (is_a_valid_zone(first_zone) || (first_zone == -1)) {
      for (int8_t zone_number = first_zone + 1; zone_number < last_zone_number; zone_number++) {
        if (zone_enable_switch(zone_number)->state)
          return zone_number;
      }
      for (int8_t zone_number = 0; zone_number < first_zone; zone_number++) {
        if (zone_enable_switch(zone_number)->state)
          return zone_number;
      }
    }
    return -1;
  }
  // if a zone is next, returns a pointer to the next zone's enable switch object; returns 'nullptr' otherwise
  template_::TemplateSwitch* next_zone_enable_switch() {
    if (there_is_an_active_zone() || (id(active_zone) == -1)) {
      if (next_zone(id(active_zone)) >= 0)
        return zone_enable_switch(next_zone(id(active_zone)));
    }
    return nullptr;
  }
  // if a zone is next, returns a pointer to the next zone's soft/template switch object; returns 'nullptr' otherwise
  template_::TemplateSwitch* next_zone_switch() {
    if (there_is_an_active_zone() || (id(active_zone) == -1)) {
      if (next_zone(id(active_zone)) >= 0)
        return zone_switch(next_zone(id(active_zone)));
    }
    return nullptr;
  }
  // returns true if any zone is enabled
  bool any_zone_is_enabled() {
    for (uint8_t zone_number = 0; zone_number < last_zone_number; zone_number++) {
      if (zone_enable_switch(zone_number)->state == true)
        return true;
    }
    return false;
  }
  // returns zone_number's run time
  uint16_t zone_run_time(const uint8_t zone_number) {
    if (is_a_valid_zone(zone_number)) {
      refresh_zone_run_times();
      return zone[zone_number].run_time;
    }
    return 0;
  }
  // called when the master valve is turned on
  void start_master() {
    if (!there_is_an_active_zone()) {
      esp_sprinkler_controller_auto_advance->turn_on();
      // if no zones are enabled, enable them all so that auto-advance can work
      if (!any_zone_is_enabled()) {
        for (uint8_t zone_number = 0; zone_number < last_zone_number; zone_number++) {
          zone_enable_switch(zone_number)->turn_on();
        }
      }
      next_zone_switch()->turn_on();
    }
  }
  // called when the master valve is turned off
  void stop_master() {
    if (there_is_an_active_zone()) {
      id(active_zone) = -1;
      id(seconds_remaining) = -1;
    }
  }
  // called when a zone valve is turned on
  void start_zone(const uint8_t zone_number) {
    if (is_a_valid_zone(zone_number)) {
      id(active_zone) = zone_number;
      // zone run times are set in minutes -- we multiply by 60 for the time in seconds
      id(seconds_remaining) = zone_run_time(zone_number) * id(zone_run_time_multiplier) * 60;
    }
  }
  // called after zone_switch_delay completes
  void activate_zone() {
    if (active_zone_triac() != nullptr) {
      id(esp_sprinkler_controller_triac_master).turn_on();
      id(zone_switch_delay).execute();
      active_zone_triac()->turn_on();
    }
  }
  // called when a zone valve is turned off
  void stop_zone(const uint8_t zone_number) {
    if (is_a_valid_zone(zone_number)) {
      if (zone_number == id(active_zone)) {
        id(active_zone) = -1;
        id(seconds_remaining) = -1;
        esp_sprinkler_controller_master->turn_off();
      }
    }
  }
  // called to advance to next zone
  void advance_to_next_zone() {
    if (esp_sprinkler_controller_auto_advance->state == true)
      active_zone_enable_switch()->turn_off();

    if (next_zone(id(active_zone)) >= 0)
      next_zone_switch()->turn_on();
    else
      esp_sprinkler_controller_master->turn_off();
  }
  // called by 'interval:' at one-second intervals; updates count-down timer when a zone is active and starts a cycle at scheduled start time
  void periodic_refresh() {
    // run time count down
    if (there_is_an_active_zone()) {
      id(seconds_remaining) -= 1;
      if (id(seconds_remaining) < 0)
        advance_to_next_zone();
    }
    // scheduled start
    if (esp_sprinkler_controller_scheduled_start->state == true) {
      auto time = id(esptime).now();
      if ((id(start_hour) == time.hour) && (id(start_minute) == time.minute) && (time.second == 0))
        start_master();
    }
  }
  // sets the display page; called by the API
  void set_display_page(uint8_t page) {
    if (page < esp_sprinkler::last_page_number) {
      id(selected_display_page) = page;
      // if any page in the array is set to nullptr, refresh them to get valid pointers
      if (display_pages[id(selected_display_page)].page == nullptr)
        refresh_display_pages();
      // finally, set/show the new page
      id(main_lcd).show_page(display_pages[id(selected_display_page)].page);
    }
  }

  void mode_button_click() {
    set_display_page(MAIN_SCREEN);
  }

  void encoder_button_click() {
    if (id(main_lcd).is_on()) {
      if (!isnan(id(esp_sprinkler_controller_encoder).state))
        id(encoder_value) = id(esp_sprinkler_controller_encoder).state;

      switch (id(selected_display_page)) {
        case MAIN_SCREEN:
          esp_sprinkler::set_display_page(esp_sprinkler::DisplayPageEnum::SETUP_SCREEN);
          break;
        case SETUP_SCREEN:
          switch(id(selected_display_item)) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
              zone_enable_switch(id(selected_display_item))->toggle();
              break;
            case 5:
              id(esp_sprinkler_controller_auto_advance).toggle();
              break;
            case 6:
              id(esp_sprinkler_controller_scheduled_start).toggle();
              break;
            case 7:
              if (there_is_an_active_zone())
                advance_to_next_zone();
              else
                id(esp_sprinkler_controller_master).turn_on();
              break;
            default:
              esp_sprinkler::set_display_page(esp_sprinkler::DisplayPageEnum::MAIN_SCREEN);
              break;
          }
          break;        
        default:
          break;
      }
    }
  }

  void encoder_value_change() {
    int8_t encoder_offset = 0;

    // we only need to do stuff if we are on a page that would allow something to be changed
    if (id(selected_display_page) == SETUP_SCREEN) {
      if (!isnan(id(esp_sprinkler_controller_encoder).state)) {
          encoder_offset = id(esp_sprinkler_controller_encoder).state - id(encoder_value);
          id(encoder_value) = id(esp_sprinkler_controller_encoder).state;
      }

      id(selected_display_item) += encoder_offset;
      if (id(selected_display_item) >= display_pages[id(selected_display_page)].numItems)
        id(selected_display_item) = display_pages[id(selected_display_page)].numItems - 1;
      if (id(selected_display_item) < 0)
        id(selected_display_item) = 0;
    }
  }

  void draw_footer(DisplayBuffer* it, const bool include_climate = false) {
    it->strftime(it->get_width(), it->get_height(), controller_tiny, color_time, TextAlign::BASELINE_RIGHT, "%I:%M:%S %p", id(esptime).now());
    if (id(esp_sprinkler_controller_api_status).state)
      it->strftime(0, it->get_height(), controller_tiny, color_time, TextAlign::BASELINE_LEFT, "%m-%d-%Y", id(esptime).now());
    else
      it->print(0, it->get_height(), controller_tiny, color_highlight, TextAlign::BASELINE_LEFT, "Offline");

    if (include_climate) {
      if (!isnan(id(esp_sprinkler_controller_bme280_temperature).state))
        it->printf(0, it->get_height() - line_height_tiny, controller_tiny, color_footer, TextAlign::BASELINE_LEFT, "Local: %.1f°", id(esp_sprinkler_controller_bme280_temperature).state * 1.8 + 32);
      if (!isnan(id(esp_sprinkler_controller_bme280_humidity).state))
        it->printf(it->get_width() - 1, it->get_height() - line_height_tiny, controller_tiny, color_footer, TextAlign::BASELINE_RIGHT, "%.1f%% RH", id(esp_sprinkler_controller_bme280_humidity).state);
    }
  }

  void draw_menu_checkbox(DisplayBuffer* it, int x, int y, Color checkbox_color, const bool checkbox_state) {
    if (checkbox_state)
      it->printf(x, y - 1, controller_small, checkbox_color, TextAlign::TOP_LEFT, "+");
    else
      it->printf(x + 1, y, controller_small, checkbox_color, TextAlign::TOP_LEFT, "-");
  }

  void draw_main_screen(DisplayBuffer* it) {
    const uint8_t h_remaining = id(seconds_remaining) / (60 * 60);
    const uint8_t m_remaining = (id(seconds_remaining) - (h_remaining * 60 * 60)) / 60;
    const uint8_t s_remaining = id(seconds_remaining) - ((h_remaining * 60 * 60) + (m_remaining * 60));
    auto color_zone = color_lowlight;
    auto start_time = id(esptime).now();
    start_time.hour = id(start_hour);
    start_time.minute = id(start_minute);

    esp_sprinkler::draw_footer(it, true);

    if (id(seconds_remaining) >= 0) {
      it->printf(0, 0, controller_small, color_lowlight, TextAlign::TOP_LEFT, "Time remaining: %d:%02d:%02d", h_remaining, m_remaining, s_remaining);
    } else {
      if (id(esp_sprinkler_controller_scheduled_start).state == true)
        it->strftime(0, 0, controller_small, color_lowlight, TextAlign::TOP_LEFT, "Auto-start at %I:%M %p.", start_time);
      else
        it->printf(0, 0, controller_small, color_lowlight, TextAlign::TOP_LEFT, "No scheduled runs.");
    }

    if (there_is_an_active_zone()) {
      color_zone = color_action;
      if (id(esp_sprinkler_controller_door_sensor).state == true)
        it->printf(0, it->get_height() - (line_height_tiny * 2), controller_tiny, color_highlight, TextAlign::BASELINE_LEFT, "Door open");
    }

    if (there_is_an_active_zone() || (id(esp_sprinkler_controller_door_sensor).state == false))
      it->printf((it->get_width() / 2), (it->get_height() / 2), controller_large, color_zone, TextAlign::CENTER, active_zone_name());
    else
      it->printf((it->get_width() / 2), (it->get_height() / 2), controller_large, color_highlight, TextAlign::CENTER, "Door open");
  }

  void draw_setup_screen(DisplayBuffer* it) {
    auto  auto_adv_highlight = color_lowlight,
          auto_start_highlight = color_lowlight,
          schedule_highlight = color_lowlight,
          back_highlight = color_lowlight;

    switch(id(selected_display_item)) {
      case 5:
        auto_adv_highlight = color_highlight;
        break;
      case 6:
        auto_start_highlight = color_highlight;
        break;
      case 7:
        schedule_highlight = color_highlight;
        break;
      case 8:
        back_highlight = color_highlight;
        break;
      default:
        break;
    }

    it->printf(0, line_height_small * 0, controller_small, color_mode, TextAlign::TOP_LEFT, "Select Zone(s)/Operation:");
    it->printf(it->get_width(), line_height_small * 1, controller_small, auto_adv_highlight, TextAlign::TOP_RIGHT, "Auto-Adv");
    it->printf(it->get_width(), line_height_small * 2, controller_small, auto_start_highlight, TextAlign::TOP_RIGHT, "Auto-Start");
    if (there_is_an_active_zone())
      it->printf(it->get_width(), line_height_small * 3, controller_small, schedule_highlight, TextAlign::TOP_RIGHT, "Next Zone");
    else
      it->printf(it->get_width(), line_height_small * 3, controller_small, schedule_highlight, TextAlign::TOP_RIGHT, "Start Now");
    it->printf(it->get_width(), line_height_small * 4, controller_small, back_highlight, TextAlign::TOP_RIGHT, "Back");

    // print list of zones and enable states
    for (uint8_t zone_number = 0; zone_number < last_zone_number; zone_number++) {
      auto zone_highlight = color_lowlight;
      if (zone_number == id(selected_display_item))
        zone_highlight = color_highlight;
      
      it->printf(7, line_height_small * (zone_number + 1), controller_small, zone_highlight, TextAlign::TOP_LEFT, zone_name(zone_number));
      draw_menu_checkbox(it, 1, line_height_small * (zone_number + 1), zone_highlight, zone_enable_switch(zone_number)->state);
    }    

    draw_menu_checkbox(it, it->get_width() - 54, line_height_small * 1, auto_adv_highlight, id(esp_sprinkler_controller_auto_advance).state);
    draw_menu_checkbox(it, it->get_width() - 56, line_height_small * 2, auto_start_highlight, id(esp_sprinkler_controller_scheduled_start).state);
  }
}