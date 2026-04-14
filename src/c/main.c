#include <pebble.h>
#include "background.h"
#include "bt_layer.h"
#include "battery_layer.h"
#include "date_display.h"
#include "time_display.h"
#include "character.h"
#include "bardock.h"
#include "steps_display.h"

// ── Visibility toggles — comment a line to hide that element ─────────────────
#define SHOW_CLOCK
#define SHOW_DATE
#define SHOW_BATTERY
#define SHOW_BT_DOT
#define SHOW_BARDOCK

static Window *s_window;

// ── Service callbacks ────────────────────────────────────────────────────────

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
#ifdef SHOW_CLOCK
  time_display_update(tick_time);
#endif
#ifdef SHOW_DATE
  date_display_update(tick_time);
#endif
  steps_display_update();
}

static void bt_handler(bool connected)
{
  bt_layer_set_connected(connected);
#ifdef SHOW_BT_DOT
  bt_layer_mark_dirty();
#endif
  if (!connected)
    vibes_short_pulse();
}

static void tap_handler(AccelAxisType axis, int32_t direction)
{
  character_tap();
}

static void battery_handler(BatteryChargeState state)
{
#ifdef SHOW_BATTERY
  battery_layer_mark_dirty();
#endif
}

// ── Window ───────────────────────────────────────────────────────────────────

static void window_load(Window *window)
{
  Layer *root = window_get_root_layer(window);

#ifdef PBL_COLOR
  background_create(root); // very first = behind everything
#endif
#ifdef SHOW_BARDOCK
  bardock_create(root);
#endif
  character_create(root);
  steps_display_create(root);
#ifdef SHOW_BT_DOT
  bt_layer_create(root);
#endif
#ifdef SHOW_BATTERY
  battery_layer_create(root);
#endif
#ifdef SHOW_CLOCK
  time_display_create(root);
#endif
#ifdef SHOW_DATE
  date_display_create(root);
#endif

  bt_layer_set_connected(connection_service_peek_pebble_app_connection());

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  tick_handler(t, MINUTE_UNIT);

  // app_timer_register(2000, (AppTimerCallback)character_tap, NULL); // For Screenshot: simulate a tap after 2s to show the character stretching.
}

static void window_unload(Window *window)
{
#ifdef PBL_COLOR
  background_destroy();
#endif
#ifdef SHOW_BARDOCK
  bardock_destroy();
#endif
  character_destroy();
  steps_display_destroy();
#ifdef SHOW_BT_DOT
  bt_layer_destroy();
#endif
#ifdef SHOW_BATTERY
  battery_layer_destroy();
#endif
#ifdef SHOW_CLOCK
  time_display_destroy();
#endif
#ifdef SHOW_DATE
  date_display_destroy();
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
  window_set_background_color(s_window, GColorWhite);
  window_set_window_handlers(s_window, (WindowHandlers){
                                           .load = window_load,
                                           .unload = window_unload,
                                       });
  window_stack_push(s_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  accel_tap_service_subscribe(tap_handler);
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
  accel_tap_service_unsubscribe();
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
