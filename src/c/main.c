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

// ── Configurable settings ─────────────────────────────────────────────────────
#define STEPS_THRESHOLD_DEFAULT 7000
#define VIBE_ON_BT_DISCONNECT_DEFAULT true


#define PERSIST_KEY_STEPS_THRESHOLD 2
#define PERSIST_KEY_VIBE_ON_BT_DISCONNECT 3
#define PERSIST_KEY_CHARACTER 5
#define PERSIST_KEY_VIBE_ON_TRANSFORM 6

static int s_steps_threshold = STEPS_THRESHOLD_DEFAULT;
static bool s_vibe_on_bt_disconnect = VIBE_ON_BT_DISCONNECT_DEFAULT;

static Window *s_window;

// ── Cross-module callbacks ────────────────────────────────────────────────────

static void on_steps_updated(int steps)
{
  character_set_super(steps >= s_steps_threshold);
}

static void inbox_received(DictionaryIterator *iter, void *ctx)
{
  Tuple *t;

  t = dict_find(iter, MESSAGE_KEY_STEPS_THRESHOLD);
  if (t)
  {
    s_steps_threshold = (t->type == TUPLE_CSTRING) ? atoi(t->value->cstring) : (int)t->value->int32;
    persist_write_int(PERSIST_KEY_STEPS_THRESHOLD, s_steps_threshold);
    steps_display_update();
  }

  t = dict_find(iter, MESSAGE_KEY_VIBE_ON_BT_DISCONNECT);
  if (t)
  {
    s_vibe_on_bt_disconnect = (t->type == TUPLE_CSTRING) ? (atoi(t->value->cstring) != 0) : (bool)t->value->int32;
    persist_write_bool(PERSIST_KEY_VIBE_ON_BT_DISCONNECT, s_vibe_on_bt_disconnect);
  }

  t = dict_find(iter, MESSAGE_KEY_CHARACTER);
  if (t)
  {
    bool is_vegeta = (t->type == TUPLE_CSTRING) ? (atoi(t->value->cstring) != 0) : (bool)t->value->int32;
    persist_write_bool(PERSIST_KEY_CHARACTER, is_vegeta);
    character_set_character(is_vegeta);
  }

  t = dict_find(iter, MESSAGE_KEY_VIBE_ON_TRANSFORM);
  if (t)
  {
    bool enabled = (t->type == TUPLE_CSTRING) ? (atoi(t->value->cstring) != 0) : (bool)t->value->int32;
    persist_write_bool(PERSIST_KEY_VIBE_ON_TRANSFORM, enabled);
    character_set_vibe_on_transform(enabled);
  }
}

#ifdef SHOW_BARDOCK
static void apply_bardock_expr(CharacterState state)
{
  switch (state)
  {
  case CharacterStateTransforming:
    break; // handled per-phase by on_character_phase_changed
  case CharacterStateSuper:
    bardock_set_expr(1);
    break;
  case CharacterStateNormal:
    bardock_set_expr(0);
    break;
  }
}

static void on_stretch_changed(bool stretching)
{
  if (stretching)
    bardock_set_expr(3);
  else
    apply_bardock_expr(character_get_state());
}

static void on_character_state_changed(CharacterState state)
{
  apply_bardock_expr(state);
}

static void on_character_phase_changed(int phase)
{
  bardock_set_expr(phase <= 1 ? 3 : 2);
}
#endif

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
  if (!connected && s_vibe_on_bt_disconnect)
    vibes_short_pulse();
}

static void tap_handler(AccelAxisType axis, int32_t direction)
{
  if (character_get_state() == CharacterStateTransforming)
    character_mute_vibe();
  else
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

  steps_display_set_callback(on_steps_updated);
#ifdef SHOW_BARDOCK
  character_set_stretch_listener(on_stretch_changed);
  character_set_state_listener(on_character_state_changed);
  character_set_phase_listener(on_character_phase_changed);
#endif

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
  if (persist_exists(PERSIST_KEY_STEPS_THRESHOLD))
    s_steps_threshold = persist_read_int(PERSIST_KEY_STEPS_THRESHOLD);
  if (persist_exists(PERSIST_KEY_VIBE_ON_BT_DISCONNECT))
    s_vibe_on_bt_disconnect = persist_read_bool(PERSIST_KEY_VIBE_ON_BT_DISCONNECT);
  if (persist_exists(PERSIST_KEY_CHARACTER))
    character_set_character(persist_read_bool(PERSIST_KEY_CHARACTER));
  if (persist_exists(PERSIST_KEY_VIBE_ON_TRANSFORM))
    character_set_vibe_on_transform(persist_read_bool(PERSIST_KEY_VIBE_ON_TRANSFORM));

  app_message_register_inbox_received(inbox_received);
  app_message_open(64, 0);

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
