#include <pebble.h>
#include "header_layer.h"
#include "bt_icon.h"
#include "battery_icon.h"
#include "date_display.h"
#include "time_display.h"
#include "character.h"

#define TIME_H 42

static Window *s_window;

// ── Service callbacks ────────────────────────────────────────────────────────

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  time_display_update(tick_time);
  date_display_update(tick_time);
  header_layer_mark_dirty();
}

static void bt_handler(bool connected)
{
  bt_icon_set_connected(connected);
  header_layer_mark_dirty();
  if (!connected)
    vibes_short_pulse();
}

static void battery_handler(BatteryChargeState state)
{
  battery_icon_set_state(state);
  header_layer_mark_dirty();
}

// ── Window ───────────────────────────────────────────────────────────────────

static void window_load(Window *window)
{
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);

  header_layer_create(root, bounds);
  character_create(root, bounds, HEADER_H, TIME_H);
  time_display_create(root, bounds);

  bt_icon_set_connected(connection_service_peek_pebble_app_connection());
  battery_icon_set_state(battery_state_service_peek());

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  tick_handler(t, MINUTE_UNIT);
}

static void window_unload(Window *window)
{
  header_layer_destroy();
  character_destroy();
  time_display_destroy();
}

// ── App lifecycle ────────────────────────────────────────────────────────────

static void init(void)
{
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_window_handlers(s_window, (WindowHandlers){
                                           .load   = window_load,
                                           .unload = window_unload,
                                       });
  window_stack_push(s_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  connection_service_subscribe((ConnectionHandlers){
      .pebble_app_connection_handler = bt_handler,
  });
  battery_state_service_subscribe(battery_handler);
}

static void deinit(void)
{
  tick_timer_service_unsubscribe();
  connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  window_destroy(s_window);
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}
