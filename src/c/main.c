#include <pebble.h>

static Window *s_window;
static TextLayer *s_time_layer;
static TextLayer *s_power_layer;
static TextLayer *s_label_layer;

// Returns a "power level" based on the current time (HHMM as a number)
static int calculate_power_level(struct tm *tick_time) {
  return (tick_time->tm_hour * 100 + tick_time->tm_min) * 9001 / 2359;
}

static void update_time(struct tm *tick_time) {
  // Update clock
  static char time_buf[6];
  strftime(time_buf, sizeof(time_buf), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  text_layer_set_text(s_time_layer, time_buf);

  // Update power level
  static char power_buf[16];
  int power = calculate_power_level(tick_time);
  snprintf(power_buf, sizeof(power_buf), "%d", power);
  text_layer_set_text(s_power_layer, power_buf);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // "POWER LEVEL" label at top
  s_label_layer = text_layer_create(GRect(0, 8, bounds.size.w, 30));
  text_layer_set_text(s_label_layer, "POWER LEVEL");
  text_layer_set_text_alignment(s_label_layer, GTextAlignmentCenter);
  text_layer_set_font(s_label_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_background_color(s_label_layer, GColorClear);
  text_layer_set_text_color(s_label_layer, GColorWhite);
  layer_add_child(window_layer, text_layer_get_layer(s_label_layer));

  // Power level number (large, center)
  s_power_layer = text_layer_create(GRect(0, 38, bounds.size.w, 80));
  text_layer_set_text(s_power_layer, "0");
  text_layer_set_text_alignment(s_power_layer, GTextAlignmentCenter);
  text_layer_set_font(s_power_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_background_color(s_power_layer, GColorClear);
  text_layer_set_text_color(s_power_layer, GColorYellow);
  layer_add_child(window_layer, text_layer_get_layer(s_power_layer));

  // Time at the bottom
  s_time_layer = text_layer_create(GRect(0, bounds.size.h - 40, bounds.size.w, 36));
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_power_layer);
  text_layer_destroy(s_label_layer);
}

static void init(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Display initial time
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  update_time(t);
}

static void deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
