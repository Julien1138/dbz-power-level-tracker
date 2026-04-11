// #include "header_layer.h"
// #include "bt_icon.h"
// #include "battery_icon.h"
// #include "date_display.h"

// static Layer *s_header;
// #if defined(PBL_ROUND)
// static Layer *s_status;
// #endif

// static void header_update_proc(Layer *layer, GContext *ctx)
// {
//   GRect bounds = layer_get_bounds(layer);
//   int w = bounds.size.w;
//   int h = bounds.size.h;

// #if defined(PBL_ROUND)
//   // Round: date only (BT + battery are in the status layer above the clock)
//   GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
//   graphics_context_set_text_color(ctx, GColorWhite);
//   graphics_draw_text(ctx, date_display_get_text(), font,
//                      GRect(0, -2, w, h + 4),
//                      GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
// #else
//   // Non-round: BT | date | battery in one row
//   bt_icon_draw(ctx, GRect(4, (h - 12) / 2, 12, 12));

//   int batt_w = 24, batt_h = 11;
//   battery_icon_draw(ctx, GRect(w - batt_w, (h - batt_h) / 2, batt_w, batt_h));

//   int mid_left  = 4 + 12 + 4;
//   int mid_right = w - batt_w - 4;
//   int mid_w     = mid_right - mid_left;
//   GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
//   graphics_context_set_text_color(ctx, GColorWhite);
//   graphics_draw_text(ctx, date_display_get_text(), font,
//                      GRect(mid_left, -2, mid_w, h + 4),
//                      GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
// #endif
// }

// #if defined(PBL_ROUND)
// static void status_update_proc(Layer *layer, GContext *ctx)
// {
//   GRect bounds = layer_get_bounds(layer);
//   int w = bounds.size.w;
//   int h = bounds.size.h;

//   int bt_w   = 12, bt_h   = 12;
//   int batt_w = 24, batt_h = 11;
//   int gap     = 8;
//   int group_w = bt_w + gap + batt_w;
//   int x       = (w - group_w) / 2;

//   bt_icon_draw(ctx, GRect(x, (h - bt_h) / 2, bt_w, bt_h));
//   battery_icon_draw(ctx, GRect(x + bt_w + gap, (h - batt_h) / 2, batt_w, batt_h));
// }
// #endif

// void header_layer_create(Layer *root, GRect bounds)
// {
//   s_header = layer_create(GRect(0, 0, bounds.size.w, HEADER_H));
//   layer_set_update_proc(s_header, header_update_proc);
//   layer_add_child(root, s_header);

// #if defined(PBL_ROUND)
//   s_status = layer_create(GRect(0, bounds.size.h - 42 - HEADER_H,
//                                 bounds.size.w, HEADER_H));
//   layer_set_update_proc(s_status, status_update_proc);
//   layer_add_child(root, s_status);
// #endif
// }

// void header_layer_destroy(void)
// {
//   layer_destroy(s_header);
// #if defined(PBL_ROUND)
//   layer_destroy(s_status);
// #endif
// }

// void header_layer_mark_dirty(void)
// {
//   layer_mark_dirty(s_header);
// #if defined(PBL_ROUND)
//   layer_mark_dirty(s_status);
// #endif
// }
