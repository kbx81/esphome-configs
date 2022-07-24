#pragma once

namespace esp_led_triangles
{
  enum HSTColorMode : uint8_t
  {
    MANUAL = 0,
    RANDOM = 1,
    RANDOM_ROTATE = 2
  };

  const uint8_t num_leds = 128;

  const uint8_t grid_height = 16;
  const uint8_t grid_width  = 8;
  const uint8_t grid_center_x = (grid_width / 2) - 1;
  const uint8_t grid_center_y = (grid_height / 2) - 1;

  const uint8_t hst_cell_offset[] = {0, 1, 9, 17, 25, 24, 16, 8, 0, 1, 9, 17, 25, 24, 16, 8};
  const uint8_t hst_cell_origin[] = {0, 2, 4, 6, 32, 34, 36, 38, 64, 66, 68, 70, 96, 98, 100, 102};

  static const uint8_t number_of_models = 73;

  const uint8_t m_leds[num_leds] = {
    14,  13,  48,  47,  80,  79,  112, 111,
    15,  12,  49,  46,  81,  78,  113, 110,
    16,  50,  11,  82,  45,  114, 77,  109,
    17,  51,  10,  83,  44,  115, 76,  108,
    52,  18,  84,  9,   116, 43,  107, 75,
    53,  19,  85,  8,   117, 42,  106, 74,
    54,  86,  20,  118, 7,   105, 41,  73,
    55,  87,  21,  119, 6,   104, 40,  72,
    88,  56,  120, 22,  103, 5,   71,  39,
    89,  57,  121, 23,  102, 4,   70,  38,
    90,  122, 58,  101, 24,  69,  3,   37,
    91,  123, 59,  100, 25,  68,  2,   36,
    124, 92,  99,  60,  67,  26,  35,  1,
    125, 93,  98,  61,  66,  27,  34,  0,
    126, 97,  94,  65,  62,  33,  28,  31,
    127, 96,  95,  64,  63,  32,  29,  30
  };

  const uint8_t hst_models[number_of_models][grid_height] = {
    {1, 1, 2, 2, 1, 1, 2, 2, 0, 0, 3, 3, 0, 0, 3, 3},
    {2, 2, 3, 3, 2, 2, 3, 3, 1, 1, 0, 0, 1, 1, 0, 0},
    {2, 1, 2, 3, 1, 1, 2, 2, 0, 0, 3, 3, 1, 0, 3, 0},
    {2, 3, 3, 3, 2, 2, 3, 0, 2, 1, 0, 0, 1, 1, 1, 0},
    {3, 3, 0, 0, 3, 0, 1, 0, 2, 3, 2, 1, 2, 2, 1, 1},
    {0, 0, 2, 1, 1, 0, 1, 1, 3, 3, 2, 3, 3, 0, 2, 2},
    {3, 1, 2, 0, 1, 1, 2, 2, 0, 0, 3, 3, 2, 0, 3, 1},
    {2, 0, 3, 3, 2, 2, 3, 1, 3, 1, 0, 0, 1, 1, 2, 0},
    {3, 3, 0, 0, 3, 1, 2, 0, 2, 0, 3, 1, 2, 2, 1, 1},
    {1, 2, 2, 2, 1, 1, 2, 3, 1, 0, 3, 3, 0, 0, 0, 3},
    {2, 2, 3, 3, 2, 3, 0, 3, 1, 2, 1, 0, 1, 1, 0, 0},
    {3, 3, 1, 0, 0, 3, 0, 0, 2, 2, 1, 2, 2, 3, 1, 1},
    {1, 0, 1, 2, 0, 0, 1, 1, 3, 3, 2, 2, 0, 3, 2, 3},
    {2, 2, 2, 3, 1, 1, 2, 3, 1, 0, 3, 3, 1, 0, 0, 0},
    {3, 3, 1, 0, 0, 0, 1, 0, 2, 3, 2, 2, 2, 3, 1, 1},
    {3, 2, 2, 0, 1, 1, 2, 3, 1, 0, 3, 3, 2, 0, 0, 1},
    {2, 0, 3, 3, 2, 3, 0, 1, 3, 2, 1, 0, 1, 1, 2, 0},
    {3, 3, 1, 0, 0, 1, 2, 3, 2, 0, 3, 2, 2, 3, 1, 1},
    {1, 0, 3, 2, 2, 0, 1, 1, 3, 3, 2, 0, 0, 1, 2, 3},
    {0, 2, 2, 1, 1, 1, 2, 3, 1, 0, 3, 3, 3, 0, 0, 2},
    {2, 1, 3, 3, 2, 3, 0, 2, 0, 2, 1, 0, 1, 1, 3, 0},
    {3, 3, 1, 0, 0, 2, 3, 0, 2, 1, 0, 2, 2, 3, 1, 1},
    {1, 0, 0, 2, 3, 0, 1, 1, 3, 3, 2, 1, 0, 2, 2, 3},
    {1, 3, 2, 2, 1, 1, 2, 0, 2, 0, 3, 3, 0, 0, 1, 3},
    {2, 2, 3, 3, 2, 0, 1, 3, 1, 3, 2, 0, 1, 1, 0, 0},
    {2, 0, 1, 3, 0, 0, 1, 1, 3, 3, 2, 2, 1, 3, 2, 0},
    {2, 3, 2, 3, 1, 1, 2, 0, 2, 0, 3, 3, 1, 0, 1, 0},
    {2, 3, 3, 3, 2, 0, 1, 0, 2, 3, 2, 0, 1, 1, 1, 3},
    {3, 3, 2, 0, 1, 0, 1, 0, 2, 3, 2, 3, 2, 0, 1, 1},
    {2, 0, 2, 3, 1, 0, 1, 1, 3, 3, 2, 3, 1, 0, 2, 0},
    {3, 3, 2, 0, 1, 1, 2, 3, 2, 0, 3, 3, 2, 0, 1, 1},
    {2, 0, 3, 3, 2, 0, 1, 1, 3, 3, 2, 0, 1, 1, 2, 0},
    {0, 3, 2, 1, 1, 1, 2, 3, 2, 0, 3, 3, 3, 0, 1, 2},
    {2, 1, 3, 3, 2, 0, 1, 2, 0, 3, 2, 0, 1, 1, 3, 0},
    {3, 3, 2, 0, 1, 2, 3, 0, 2, 1, 0, 3, 2, 0, 1, 1},
    {2, 0, 0, 3, 3, 0, 1, 1, 3, 3, 2, 1, 1, 2, 2, 0},
    {2, 0, 2, 3, 1, 1, 2, 1, 3, 0, 3, 3, 1, 0, 2, 0},
    {2, 3, 3, 3, 2, 1, 2, 0, 2, 0, 3, 0, 1, 1, 1, 3},
    {3, 3, 3, 0, 2, 0, 1, 0, 2, 3, 2, 3, 2, 1, 1, 1},
    {3, 0, 2, 0, 1, 0, 1, 1, 3, 3, 2, 3, 2, 0, 2, 1},
    {3, 0, 2, 0, 1, 1, 2, 1, 3, 0, 3, 3, 2, 0, 2, 1},
    {2, 0, 3, 3, 2, 1, 2, 1, 3, 0, 3, 0, 1, 1, 2, 0},
    {3, 3, 3, 0, 2, 1, 2, 3, 2, 0, 3, 0, 2, 1, 1, 1},
    {3, 0, 3, 0, 2, 0, 1, 1, 3, 3, 2, 0, 2, 1, 2, 1},
    {0, 0, 2, 1, 1, 1, 2, 1, 3, 0, 3, 3, 3, 0, 2, 2},
    {3, 3, 3, 0, 2, 2, 3, 0, 2, 1, 0, 0, 2, 1, 1, 1},
    {2, 1, 2, 3, 1, 2, 3, 2, 0, 1, 0, 3, 1, 0, 3, 0},
    {3, 1, 2, 0, 1, 2, 3, 2, 0, 1, 0, 3, 2, 0, 3, 1},
    {2, 0, 0, 3, 3, 2, 3, 1, 3, 1, 0, 1, 1, 2, 2, 0},
    {0, 3, 0, 1, 3, 1, 2, 0, 2, 0, 3, 1, 3, 2, 1, 2},
    {0, 1, 3, 1, 2, 0, 1, 2, 0, 3, 2, 0, 3, 1, 3, 2},
    {0, 1, 2, 1, 1, 2, 3, 2, 0, 1, 0, 3, 3, 0, 3, 2},
    {2, 1, 0, 3, 3, 2, 3, 2, 0, 1, 0, 1, 1, 2, 3, 0},
    {0, 3, 0, 1, 3, 2, 3, 0, 2, 1, 0, 1, 3, 2, 1, 2},
    {0, 1, 0, 1, 3, 0, 1, 2, 0, 3, 2, 1, 3, 2, 3, 2},
    {0, 3, 2, 1, 1, 2, 3, 0, 2, 1, 0, 3, 3, 0, 1, 2},
    {3, 0, 2, 0, 1, 2, 3, 1, 3, 1, 0, 3, 2, 0, 2, 1},
    {2, 0, 0, 3, 3, 1, 2, 1, 3, 0, 3, 1, 1, 2, 2, 0},
    {3, 1, 2, 0, 1, 3, 0, 2, 0, 2, 1, 3, 2, 0, 3, 1},
    {2, 0, 1, 3, 0, 2, 3, 1, 3, 1, 0, 2, 1, 3, 2, 0},
    {3, 2, 2, 0, 1, 3, 0, 3, 1, 2, 1, 3, 2, 0, 0, 1},
    {2, 0, 1, 3, 0, 3, 0, 1, 3, 2, 1, 2, 1, 3, 2, 0},
    {1, 3, 1, 2, 0, 1, 2, 0, 2, 0, 3, 2, 0, 3, 1, 3},
    {1, 2, 3, 2, 2, 0, 1, 3, 1, 3, 2, 0, 0, 1, 0, 3},
    {2, 0, 2, 3, 1, 3, 0, 1, 3, 2, 1, 3, 1, 0, 2, 0},
    {1, 3, 3, 2, 2, 0, 1, 0, 2, 3, 2, 0, 0, 1, 1, 3},
    {2, 3, 2, 3, 1, 0, 1, 0, 2, 3, 2, 3, 1, 0, 1, 0},
    {1, 2, 3, 2, 2, 1, 2, 3, 1, 0, 3, 0, 0, 1, 0, 3},
    {3, 2, 3, 0, 2, 1, 2, 3, 1, 0, 3, 0, 2, 1, 0, 1},
    {3, 0, 3, 0, 2, 3, 0, 1, 3, 2, 1, 0, 2, 1, 2, 1},
    {1, 0, 3, 2, 2, 1, 2, 1, 3, 0, 3, 0, 0, 1, 2, 3},
    {3, 0, 3, 0, 2, 1, 2, 1, 3, 0, 3, 0, 2, 1, 2, 1},
    {1, 0, 3, 2, 2, 3, 0, 1, 3, 2, 1, 0, 0, 1, 2, 3}
  };

  // returns the right LED index within the serpentine string/matrix
  uint8_t led_index_from_xy( uint8_t x, uint8_t y) {
    return m_leds[y * 8 + x];
  }

  Color blend(const Color& existing, const Color& overlay, const uint8_t amountOfOverlay) {
    if (amountOfOverlay == 0) {
      return existing;
    }
    if (amountOfOverlay == 255) {
      return overlay;
    }

    const uint8_t amountOfKeep = 255 - amountOfOverlay;
    Color blended;

    blended.red   = scale8(existing.red,   amountOfKeep) + scale8(overlay.red,   amountOfOverlay);
    blended.green = scale8(existing.green, amountOfKeep) + scale8(overlay.green, amountOfOverlay);
    blended.blue  = scale8(existing.blue,  amountOfKeep) + scale8(overlay.blue,  amountOfOverlay);

    return blended;
  }

  void hst_art(AddressableLight &it, const bool initial_run, const uint8_t animation_speed = 1, const HSTColorMode color_mode = MANUAL, Color color_a = Color(0, 0, 0), Color color_b = Color(0, 0, 0)) {
    static const uint8_t max_animation_steps = 8;
    static uint8_t animation_steps[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // how many animation steps between start and end position
    static uint8_t hst_cell_position[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // array for intermediary hst positions
    static uint8_t cell = 0;
    static uint8_t model_number = 0;
    static uint8_t blend_progress = 0;
    static Color color_a_start[grid_height];
    static Color color_b_start[grid_height];
    static Color color_a_target;
    static Color color_b_target;

    color_a_target = color_mode != MANUAL ? (initial_run ? Color::random_color() : color_a_target) : color_a;
    color_b_target = color_mode != MANUAL ? (initial_run ? Color::random_color() : color_b_target) : color_b;

    if (initial_run) {
      for (uint8_t i = 0; i < grid_height; i++) {
        auto origin = esp_led_triangles::hst_cell_origin[i];
        for (uint8_t j = 0; j < 8; j++) {
          auto offset = esp_led_triangles::hst_cell_offset[j];
          if (j < 4) {
            it[esp_led_triangles::m_leds[origin + offset]] = color_a_target;
          }
          else {
            it[esp_led_triangles::m_leds[origin + offset]] = color_b_target;
          }
        }
      }
      cell = max_animation_steps;
      model_number = -1;   // this will be incremented right away below
    }

    if ((cell > max_animation_steps - 1) || initial_run) {  // 8 steps of animation for one 360 degrees rotation
      cell = 0;
      blend_progress = 0;
      if (!initial_run) {
        switch (color_mode)
        {
        case RANDOM:
          color_a_target = Color::random_color();
          color_b_target = Color::random_color();
          break;
        
        case RANDOM_ROTATE:
          color_a_target = color_b_target;
          color_b_target = Color::random_color();
          break;
        
        default:
          break;
        }
      }
      if (++model_number > number_of_models - 1) {
        model_number = 0;
      }
      for (uint8_t m = 0; m < grid_height; m++) {
        if (initial_run)
          hst_cell_position[m] = 0;
        else if (model_number > 0)
          hst_cell_position[m] = esp_led_triangles::hst_models[model_number - 1][m];  // save model position
        else
          hst_cell_position[m] = esp_led_triangles::hst_models[number_of_models - 1][m];  // save model position

        int step = esp_led_triangles::hst_models[model_number][m] - hst_cell_position[m]; // how many steps from start to end position
        if (step >= 0) {
          animation_steps[m] = 2 * step; // 0-1, 0-2, 0-3, 1-2, 1-3, 2-3 positions
        } else {
          animation_steps[m] = 2 * (4 + step); // 3-0, 3-1, 3-2, 2-1, 2-0, 1-0
        }
      }
    }

    for (uint8_t j = 0; j < grid_height; j++) {    // animate all hst...
      if (animation_steps[j] > 0) { // ...but only if there are any remaining steps for a hst
        auto led1 = esp_led_triangles::m_leds[esp_led_triangles::hst_cell_origin[j] + esp_led_triangles::hst_cell_offset[cell + (hst_cell_position[j] * 2)]];
        auto led2 = esp_led_triangles::m_leds[esp_led_triangles::hst_cell_origin[j] + esp_led_triangles::hst_cell_offset[cell + (hst_cell_position[j] * 2) + 4]];
        if (blend_progress == 0) {
          color_a_start[j] = it[led1].get();
          color_b_start[j] = it[led2].get();
        }
        it[led1] = esp_led_triangles::blend(color_a_start[j], color_b_target, blend_progress); // color of one end of the triangle
        it[led2] = esp_led_triangles::blend(color_b_start[j], color_a_target, blend_progress); // other end of the triangle
      }
    }
    // wrap/reset if it's time to do so
    if (blend_progress == UINT8_MAX)
      blend_progress = 0;
    // verify that blend_progress won't overflow if we add animation_speed
    else if (255 - blend_progress < animation_speed)
      blend_progress = UINT8_MAX;
    // ...otherwise just increment it
    else
      blend_progress += animation_speed;

    if (blend_progress == 0) {
      for (uint8_t n = 0; n < grid_height; n++) { // one step less after an iteration
        if (animation_steps[n] > 0) {
          animation_steps[n]--;
        }
      }
      cell++;
    }
  }

  void hst_fire(AddressableLight &it, const bool initial_run, const uint8_t speed = 32) {
    // heatmap data with the size matrix width * height
    static uint8_t fire_heat[num_leds];
    // storage for the noise data -- adjust the size to suit your setup
    uint8_t fire_noise[grid_width][grid_height];
    // other control parameters for the fire noise array
    uint32_t fire_x;
    uint32_t fire_y;
    uint32_t fire_z;
    uint32_t fire_scale_x;
    uint32_t fire_scale_y;
    // get one noise value out of a moving noise space
    uint16_t ctrl1 = inoise16(11 * millis(), 0, 0);
    // get another one
    uint16_t ctrl2 = inoise16(13 * millis(), 100000, 100000);
    // average of both to get a more unpredictable curve
    uint16_t  ctrl = ((ctrl1 + ctrl2) / 2);
    // the color palette
    CRGBPalette16 fire_pal = HeatColors_p;

    // here we define the impact of the wind
    // high factor = a lot of movement to the sides
    fire_x = 3 * ctrl * speed;
    // this is the speed of the upstream itself
    // high factor = fast movement
    fire_y = 15 * millis() * speed;
    // just for ever changing patterns we move through z as well
    fire_z = 3 * millis() * speed;
    // ...and dynamically scale the complete heatmap for some changes in the size of the heatspots.
    // The speed of change is influenced by the factors in the calculation of ctrl1 & 2 above.
    // The divisor sets the impact of the size-scaling.
    fire_scale_x = ctrl1 / 2;
    fire_scale_y = ctrl2 / 2;

    // Calculate the noise array based on the control parameters
    for (uint8_t i = 0; i < grid_width; i++) {
      uint32_t ioffset = fire_scale_x * (i - grid_center_x);
      for (uint8_t j = 0; j < grid_height; j++) {
        uint32_t joffset = fire_scale_y * (j - grid_center_y);
        uint16_t data = ((inoise16(fire_x + ioffset, fire_y + joffset, fire_z)) + 1);
        fire_noise[i][j] = data >> 8;
      }
    }

    // Draw the first (lowest) line -- seed the fire.
    // It could be random pixels or anything else as well.
    for (uint8_t x = 0; x < grid_width; x++) {
      // draw
      CRGB pixel_color = ColorFromPalette(fire_pal, fire_noise[x][0]);
      it[esp_led_triangles::led_index_from_xy(x, grid_height - 1)] = Color(pixel_color.red, pixel_color.green, pixel_color.blue);
      // and fill the lowest line of the heatmap, too
      fire_heat[esp_led_triangles::led_index_from_xy(x, grid_height - 1)] = fire_noise[x][0];
    }

    // Copy the heatmap one line up for the scrolling.
    for (uint8_t y = 0; y < grid_height - 1; y++) {
      for (uint8_t x = 0; x < grid_width; x++) {
        fire_heat[esp_led_triangles::led_index_from_xy(x, y)] = fire_heat[esp_led_triangles::led_index_from_xy(x, y + 1)];
      }
    }

    // Scale the heatmap values down based on the independent scrolling noise array
    for (uint8_t y = 0; y < grid_height - 1; y++) {
      for (uint8_t x = 0; x < grid_width; x++) {
        // get data from the calculated noise field
        uint8_t dim = fire_noise[x][y];

        // This number is critical
        // If it's to low (like 1.1) the fire dosn´t go up far enough.
        // If it's to high (like 3) the fire goes up too high.
        // It depends on the framerate which number is best.
        // If the number is not right you loose the uplifting fire clouds which seperate themself while rising up.
        dim = dim / 1.5;
        dim = 255 - dim;
        // do scaling of the heatmap
        fire_heat[esp_led_triangles::led_index_from_xy(x, y)] = scale8(fire_heat[esp_led_triangles::led_index_from_xy(x, y)] , dim);
      }
    }

    // Now just map the colors based on the heatmap
    for (uint8_t y = 0; y < grid_height - 1; y++) {
      for (uint8_t x = 0; x < grid_width; x++) {
        CRGB pixel_color = ColorFromPalette(fire_pal, fire_heat[esp_led_triangles::led_index_from_xy(x, y)]);
        it[esp_led_triangles::led_index_from_xy(x, y)] = Color(pixel_color.red, pixel_color.green, pixel_color.blue);
      }
    }
  }
}