#include <pebble.h>
#include "header_layer.h"
#include "bt_icon.h"
#include "battery_icon.h"
#include "date_display.h"
#include "time_display.h"
#include "character.h"
#include "bardock.h"
#include "steps_display.h"

// ── Visibility toggles — comment a line to hide that element ─────────────────
// #define SHOW_HEADER
// #define SHOW_CLOCK

#define TIME_H 42

static Window *s_window;

// ── Service callbacks ────────────────────────────────────────────────────────

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
#ifdef SHOW_CLOCK
  time_display_update(tick_time);
#endif
#ifdef SHOW_HEADER
  date_display_update(tick_time);
  header_layer_mark_dirty();
#endif
  steps_display_update();
}

static void bt_handler(bool connected)
{
  bt_icon_set_connected(connected);
#ifdef SHOW_HEADER
  header_layer_mark_dirty();
#endif
  if (!connected)
    vibes_short_pulse();
}

static void battery_handler(BatteryChargeState state)
{
  battery_icon_set_state(state);
#ifdef SHOW_HEADER
  header_layer_mark_dirty();
#endif
}

// ── Window ───────────────────────────────────────────────────────────────────

static void window_load(Window *window)
{
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);

  bardock_create(root); // first = behind everything
#ifdef SHOW_HEADER
  header_layer_create(root, bounds);
#endif
  character_create(root);
  steps_display_create(root);
#ifdef SHOW_CLOCK
  time_display_create(root, bounds);
#endif

  bt_icon_set_connected(connection_service_peek_pebble_app_connection());
  battery_icon_set_state(battery_state_service_peek());

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  tick_handler(t, MINUTE_UNIT);
}

static void window_unload(Window *window)
{
  bardock_destroy();
#ifdef SHOW_HEADER
  header_layer_destroy();
#endif
  character_destroy();
  steps_display_destroy();
#ifdef SHOW_CLOCK
  time_display_destroy();
#endif
}

// ── App lifecycle ────────────────────────────────────────────────────────────

#if defined(PBL_HEALTH)
static void health_handler(HealthEventType event, void *context)
{
  steps_display_update();
}
#endif

static void init(void)
{
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_window_handlers(s_window, (WindowHandlers){
                                           .load = window_load,
                                           .unload = window_unload,
                                       });
  window_stack_push(s_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  connection_service_subscribe((ConnectionHandlers){
      .pebble_app_connection_handler = bt_handler,
  });
  battery_state_service_subscribe(battery_handler);
#if defined(PBL_HEALTH)
  health_service_events_subscribe(health_handler, NULL);
#endif
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
