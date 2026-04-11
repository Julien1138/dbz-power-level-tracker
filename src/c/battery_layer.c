#include "battery_layer.h"

// ── Layout ────────────────────────────────────────────────────────────────────
// BATT_X / BATT_Y : top-left corner of the layer.
// BATT_W          : total layer width (label + gap + bar).
// BATT_H          : layer height — must fit the font (~8 px for a 6 px font).
// TEXT_W          : pixels reserved for the "BATTERY" label.
// BAR_GAP         : gap between label and bar.
// BAR_H           : thickness of the bar (px).

#define TEXT_W 25
#define BAR_GAP 4
#define BAR_H 4

#if defined(PBL_PLATFORM_EMERY)
#define BATT_X 120
#define BATT_Y 160
#define BATT_W 70
#define BATT_H 8

#elif defined(PBL_PLATFORM_GABBRO)
#define BATT_X 155
#define BATT_Y 190
#define BATT_W 70
#define BATT_H 8

#elif defined(PBL_PLATFORM_CHALK)
#define BATT_X 90
#define BATT_Y 140
#define BATT_W 65
#define BATT_H 8

#else // basalt, aplite, diorite, flint
#define BATT_X 70
#define BATT_Y 130
#define BATT_W 70
#define BATT_H 8
#endif

// ── Color ─────────────────────────────────────────────────────────────────────
static GColor get_color(int pct)
{
#if defined(PBL_COLOR)
  if (pct <= 10)
    return GColorRed;
  if (pct <= 20)
    return GColorYellow;
  return GColorScreaminGreen;
#else
  (void)pct;
  return GColorBlack;
#endif
}

// ── State ─────────────────────────────────────────────────────────────────────
static Layer *s_layer;
static GFont s_font;

// ── Draw ──────────────────────────────────────────────────────────────────────
static void battery_layer_draw(Layer *layer, GContext *ctx)
{
  GRect bounds = layer_get_bounds(layer);
  int pct = battery_state_service_peek().charge_percent;
  GColor color = get_color(pct);

  // "BATTERY" label
  graphics_context_set_text_color(ctx, color);
  graphics_draw_text(ctx, "BAT.", s_font,
                     GRect(0, 0, TEXT_W, bounds.size.h),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);

  // Bar outline
  int bar_x = TEXT_W + BAR_GAP;
  int bar_w = bounds.size.w - bar_x;
  int bar_y = (bounds.size.h - BAR_H) / 2;

  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_rect(ctx, GRect(bar_x, bar_y, bar_w, BAR_H));

  // Bar fill
  int fill_w = (bar_w - 2) * pct / 100;
  if (fill_w > 0)
  {
    graphics_context_set_fill_color(ctx, color);
    graphics_fill_rect(ctx, GRect(bar_x + 1, bar_y + 1, fill_w, BAR_H - 2), 0, GCornerNone);
  }
}

// ── Public API ────────────────────────────────────────────────────────────────
void battery_layer_create(Layer *root)
{
  s_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIGITALIX_6));
  s_layer = layer_create(GRect(BATT_X, BATT_Y, BATT_W, BATT_H));
  layer_set_update_proc(s_layer, battery_layer_draw);
  layer_add_child(root, s_layer);
}

void battery_layer_destroy(void)
{
  layer_destroy(s_layer);
  fonts_unload_custom_font(s_font);
}

void battery_layer_mark_dirty(void)
{
  layer_mark_dirty(s_layer);
}
