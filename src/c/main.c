#include <pebble.h>

static Window *s_window;
static Layer *s_header_layer;
static TextLayer *s_time_layer;
static BitmapLayer *s_goku_layer;
static GBitmap *s_goku_bitmap;
#if defined(PBL_ROUND)
static Layer *s_status_layer;
#endif

static bool s_bt_connected;
static BatteryChargeState s_battery_state;
static char s_date_buf[16] = "";

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
    graphics_context_set_stroke_width(ctx, 1);
    // Vertical staff
    graphics_draw_line(ctx, GPoint(cx, y0), GPoint(cx, y1));
    // Upper right lobe
    graphics_draw_line(ctx, GPoint(cx, y0), GPoint(cx + arm, q1));
    graphics_draw_line(ctx, GPoint(cx + arm, q1), GPoint(cx - 2, mid + 2));
    // Lower right lobe
    graphics_draw_line(ctx, GPoint(cx - 2, mid - 2), GPoint(cx + arm, q3));
    graphics_draw_line(ctx, GPoint(cx + arm, q3), GPoint(cx, y1));
  }
  else
  {
    // Red X
    graphics_context_set_stroke_color(ctx, GColorRed);
    graphics_context_set_stroke_width(ctx, 2);
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

// ── Header layer ─────────────────────────────────────────────────────────────

static void header_update_proc(Layer *layer, GContext *ctx)
{
  GRect bounds = layer_get_bounds(layer);
  int w = bounds.size.w;
  int h = bounds.size.h;

#if defined(PBL_ROUND)
  // Round: date only, centered (BT + battery are in the status layer above clock)
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, s_date_buf, font, GRect(0, -2, w, h + 4),
                     GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
#else
  // Non-round: BT | date | battery all in one top row
  draw_bt_icon(ctx, GRect(4, (h - 12) / 2, 12, 12));

  int batt_w = 24, batt_h = 11;
  draw_battery(ctx, GRect(w - batt_w, (h - batt_h) / 2, batt_w, batt_h));

  int mid_left = 4 + 12 + 4;
  int mid_right = w - batt_w - 4;
  int mid_w = mid_right - mid_left;

  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  GRect text_rect = GRect(mid_left, -2, mid_w, h + 4);
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, s_date_buf, font, text_rect,
                     GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
#endif
}

#if defined(PBL_ROUND)
// Round-only: BT + battery centered together above the clock
static void status_update_proc(Layer *layer, GContext *ctx)
{
  GRect bounds = layer_get_bounds(layer);
  int w = bounds.size.w;
  int h = bounds.size.h;

  int bt_w = 12, bt_h = 12;
  int batt_w = 24, batt_h = 11;
  int gap = 8;
  int group_w = bt_w + gap + batt_w;
  int x = (w - group_w) / 2;

  draw_bt_icon(ctx, GRect(x, (h - bt_h) / 2, bt_w, bt_h));
  draw_battery(ctx, GRect(x + bt_w + gap, (h - batt_h) / 2, batt_w, batt_h));
}
#endif

// ── Service callbacks ────────────────────────────────────────────────────────

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  static char time_buf[6];
  strftime(time_buf, sizeof(time_buf),
           clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  text_layer_set_text(s_time_layer, time_buf);

  strftime(s_date_buf, sizeof(s_date_buf), "%b %e %Y", tick_time);
  layer_mark_dirty(s_header_layer);
}

static void bt_handler(bool connected)
{
  s_bt_connected = connected;
  layer_mark_dirty(s_header_layer);
#if defined(PBL_ROUND)
  if (s_status_layer) layer_mark_dirty(s_status_layer);
#endif
  if (!connected)
    vibes_short_pulse();
}

static void battery_handler(BatteryChargeState state)
{
  s_battery_state = state;
  layer_mark_dirty(s_header_layer);
#if defined(PBL_ROUND)
  if (s_status_layer) layer_mark_dirty(s_status_layer);
#endif
}

// ── Window ───────────────────────────────────────────────────────────────────

static void window_load(Window *window)
{
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);

  // Top row: bluetooth | steps | battery
  s_header_layer = layer_create(GRect(0, 0, bounds.size.w, 20));
  layer_set_update_proc(s_header_layer, header_update_proc);
  layer_add_child(root, s_header_layer);

  // Goku sprite — centred between header and time bar
  s_goku_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GOKU_STILL);
  GSize img_size = gbitmap_get_bounds(s_goku_bitmap).size;
  int header_h = 20;
  int time_h = 42;
  int available_h = bounds.size.h - header_h - time_h;
  int img_x = (bounds.size.w - img_size.w) / 2;
  int img_y = header_h + (available_h - img_size.h) / 2;
  s_goku_layer = bitmap_layer_create(GRect(img_x, img_y, img_size.w, img_size.h));
  bitmap_layer_set_bitmap(s_goku_layer, s_goku_bitmap);
  bitmap_layer_set_compositing_mode(s_goku_layer, GCompOpSet);
  layer_add_child(root, bitmap_layer_get_layer(s_goku_layer));

#if defined(PBL_ROUND)
  // Status row (BT + battery) just above the clock on round watches
  s_status_layer = layer_create(GRect(0, bounds.size.h - 42 - 20, bounds.size.w, 20));
  layer_set_update_proc(s_status_layer, status_update_proc);
  layer_add_child(root, s_status_layer);
#endif

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

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  tick_handler(t, MINUTE_UNIT);
}

static void window_unload(Window *window)
{
  layer_destroy(s_header_layer);
#if defined(PBL_ROUND)
  layer_destroy(s_status_layer);
#endif
  bitmap_layer_destroy(s_goku_layer);
  gbitmap_destroy(s_goku_bitmap);
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
