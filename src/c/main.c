#include <pebble.h>

static Window *s_window;
static Layer *s_header_layer;
static TextLayer *s_time_layer;

static bool s_bt_connected;
static BatteryChargeState s_battery_state;
static int32_t s_steps = 0;
static char s_steps_buf[10] = "0";

// ── Drawing helpers ──────────────────────────────────────────────────────────

static void draw_bt_icon(GContext *ctx, GRect r)
{
  int cx = r.origin.x + r.size.w / 2;
  int y0 = r.origin.y + 1;
  int y1 = r.origin.y + r.size.h - 1;
  int arm = r.size.w / 3;
  int mid = r.origin.y + r.size.h / 2;
  int q1 = r.origin.y + r.size.h / 4;
  int q3 = r.origin.y + r.size.h * 3 / 4;

  if (s_bt_connected)
  {
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_width(ctx, 2);
    // Vertical staff
    graphics_draw_line(ctx, GPoint(cx, y0), GPoint(cx, y1));
    // Upper right lobe
    graphics_draw_line(ctx, GPoint(cx, y0), GPoint(cx + arm, q1));
    graphics_draw_line(ctx, GPoint(cx + arm, q1), GPoint(cx, mid));
    // Lower right lobe
    graphics_draw_line(ctx, GPoint(cx, mid), GPoint(cx + arm, q3));
    graphics_draw_line(ctx, GPoint(cx + arm, q3), GPoint(cx, y1));
  }
  else
  {
    // Red X
    graphics_context_set_stroke_color(ctx, GColorRed);
    graphics_context_set_stroke_width(ctx, 3);
    graphics_draw_line(ctx,
                       GPoint(r.origin.x + 2, r.origin.y + 2),
                       GPoint(r.origin.x + r.size.w - 2, r.origin.y + r.size.h - 2));
    graphics_draw_line(ctx,
                       GPoint(r.origin.x + r.size.w - 2, r.origin.y + 2),
                       GPoint(r.origin.x + 2, r.origin.y + r.size.h - 2));
  }
}

static void draw_battery(GContext *ctx, GRect r)
{
  int x = r.origin.x, y = r.origin.y;
  int w = r.size.w, h = r.size.h;
  int pct = s_battery_state.charge_percent;

  GColor fill_color = (pct > 30) ? GColorGreen : (pct > 15) ? GColorYellow
                                                            : GColorRed;

  int nub_w = 3;
  int body_w = w - nub_w;
  int nub_h = (h / 2 > 4) ? h / 2 : 4;
  int nub_y = y + (h - nub_h) / 2;

  // Body outline
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_rect(ctx, GRect(x, y, body_w, h));

  // Terminal nub
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(x + body_w, nub_y, nub_w, nub_h), 0, GCornerNone);

  // Charge fill
  int fill_w = (body_w - 2) * pct / 100;
  if (fill_w > 0)
  {
    graphics_context_set_fill_color(ctx, fill_color);
    graphics_fill_rect(ctx, GRect(x + 1, y + 1, fill_w, h - 2), 0, GCornerNone);
  }

  // Charging: lightning bolt overlay
  if (s_battery_state.is_charging)
  {
    int bx = x + body_w / 2;
    graphics_context_set_stroke_color(ctx, GColorYellow);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_line(ctx, GPoint(bx + 2, y + 1), GPoint(bx - 1, y + h / 2));
    graphics_draw_line(ctx, GPoint(bx - 1, y + h / 2), GPoint(bx + 2, y + h - 1));
  }
}

// Walking person silhouette used as the step icon
static void draw_step_icon(GContext *ctx, GRect r)
{
  int x = r.origin.x, y = r.origin.y;
  int w = r.size.w, h = r.size.h;
  int cx = x + w / 2;

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 2);

  // Head
  int head_r = 3;
  int head_cy = y + head_r + 1;
  graphics_fill_circle(ctx, GPoint(cx + 1, head_cy), head_r);

  // Torso
  int shoulder_y = head_cy + head_r + 1;
  int hip_y = y + h * 55 / 100;
  graphics_draw_line(ctx, GPoint(cx + 1, shoulder_y), GPoint(cx - 1, hip_y));

  // Legs
  graphics_draw_line(ctx, GPoint(cx - 1, hip_y), GPoint(cx + 3, y + h - 2)); // front
  graphics_draw_line(ctx, GPoint(cx - 1, hip_y), GPoint(cx - 4, y + h - 5)); // back

  // Arms (opposite phase to legs)
  int arm_y = shoulder_y + 2;
  graphics_draw_line(ctx, GPoint(cx, arm_y), GPoint(cx + 4, hip_y - 2)); // back arm
  graphics_draw_line(ctx, GPoint(cx, arm_y), GPoint(cx - 4, hip_y - 1)); // front arm
}

// ── Header layer (BT | steps | battery) ─────────────────────────────────────

static void header_update_proc(Layer *layer, GContext *ctx)
{
  GRect bounds = layer_get_bounds(layer);
  int w = bounds.size.w;
  int h = bounds.size.h;

  // Bluetooth — top left
  draw_bt_icon(ctx, GRect(4, 1, 16, h - 2));

  // Battery — top right (14 px tall, centred vertically)
  int batt_w = 30, batt_h = 14;
  draw_battery(ctx, GRect(w - batt_w - 4, (h - batt_h) / 2, batt_w, batt_h));

  // Steps: icon + text, centred in the remaining middle space
  int mid_left = 4 + 16 + 4;          // 24
  int mid_right = w - batt_w - 4 - 4; // w - 38
  int mid_w = mid_right - mid_left;

  int icon_w = 14;
  int text_len = (int)strlen(s_steps_buf);
  int est_text_w = text_len * 11 + 6; // ~11 px/digit for GOTHIC_18_BOLD
  int group_w = icon_w + 3 + est_text_w;
  int group_x = mid_left + (mid_w - group_w) / 2;
  if (group_x < mid_left)
    group_x = mid_left;

  draw_step_icon(ctx, GRect(group_x, 1, icon_w, h - 2));

  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  GRect text_rect = GRect(group_x + icon_w + 3, -2, est_text_w + 10, h + 4);
  graphics_context_set_text_color(ctx, GColorYellow);
  graphics_draw_text(ctx, s_steps_buf, font, text_rect,
                     GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

// ── Service callbacks ────────────────────────────────────────────────────────

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  static char time_buf[6];
  strftime(time_buf, sizeof(time_buf),
           clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  text_layer_set_text(s_time_layer, time_buf);
}

static void bt_handler(bool connected)
{
  s_bt_connected = connected;
  layer_mark_dirty(s_header_layer);
  if (!connected)
    vibes_short_pulse();
}

static void battery_handler(BatteryChargeState state)
{
  s_battery_state = state;
  layer_mark_dirty(s_header_layer);
}

static void update_steps(void)
{
#if defined(PBL_HEALTH)
  HealthMetric metric = HealthMetricStepCount;
  time_t start = time_start_of_today();
  time_t end = time(NULL);
  if (health_service_metric_accessible(metric, start, end) &
      HealthServiceAccessibilityMaskAvailable)
  {
    s_steps = (int32_t)health_service_sum(metric, start, end);
  }
#endif
  snprintf(s_steps_buf, sizeof(s_steps_buf), "%d", (int)s_steps);
  if (s_header_layer)
    layer_mark_dirty(s_header_layer);
}

static void health_handler(HealthEventType event, void *context)
{
  if (event == HealthEventMovementUpdate ||
      event == HealthEventSignificantUpdate)
  {
    update_steps();
  }
}

// ── Window ───────────────────────────────────────────────────────────────────

static void window_load(Window *window)
{
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);

  // Top row: bluetooth | steps | battery
  s_header_layer = layer_create(GRect(0, 0, bounds.size.w, 30));
  layer_set_update_proc(s_header_layer, header_update_proc);
  layer_add_child(root, s_header_layer);

  // Time — bottom
  s_time_layer = text_layer_create(
      GRect(0, bounds.size.h - 42, bounds.size.w, 42));
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  layer_add_child(root, text_layer_get_layer(s_time_layer));

  // Seed initial state
  s_bt_connected = connection_service_peek_pebble_app_connection();
  s_battery_state = battery_state_service_peek();
  update_steps();

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  tick_handler(t, MINUTE_UNIT);
}

static void window_unload(Window *window)
{
  layer_destroy(s_header_layer);
  text_layer_destroy(s_time_layer);
}

// ── App lifecycle ────────────────────────────────────────────────────────────

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
#if defined(PBL_HEALTH)
  health_service_events_unsubscribe();
#endif
  window_destroy(s_window);
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}
