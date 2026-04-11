#include "battery_icon.h"

static BatteryChargeState s_state;

void battery_icon_set_state(BatteryChargeState state)
{
  s_state = state;
}

void battery_icon_draw(GContext *ctx, GRect r, GColor color)
{
  int x = r.origin.x, y = r.origin.y;
  int w = r.size.w, h = r.size.h;
  int pct = s_state.charge_percent;

  GColor fill_color = (pct > 30)   ? GColorScreaminGreen
                      : (pct > 15) ? GColorYellow
                                   : GColorRed;

  int nub_w = 3;
  int body_w = w - nub_w;
  int nub_h = (h / 2 > 4) ? h / 2 : 4;
  int nub_y = y + (h - nub_h) / 2;

  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_rect(ctx, GRect(x, y, body_w, h));

  graphics_context_set_fill_color(ctx, color);
  graphics_fill_rect(ctx, GRect(x + body_w, nub_y, nub_w, nub_h), 0, GCornerNone);

  int fill_w = (body_w - 2) * pct / 100;
  if (fill_w > 0)
  {
    graphics_context_set_fill_color(ctx, fill_color);
    graphics_fill_rect(ctx, GRect(x + 1, y + 1, fill_w, h - 2), 0, GCornerNone);
  }

  if (s_state.is_charging)
  {
    int bx = x + body_w / 2;
    graphics_context_set_stroke_color(ctx, GColorYellow);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_line(ctx, GPoint(bx + 2, y + 1), GPoint(bx - 1, y + h / 2));
    graphics_draw_line(ctx, GPoint(bx - 1, y + h / 2), GPoint(bx + 2, y + h - 1));
  }
}
