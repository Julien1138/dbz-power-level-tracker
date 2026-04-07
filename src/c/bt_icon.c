#include "bt_icon.h"

static bool s_connected = true;

void bt_icon_set_connected(bool connected)
{
  s_connected = connected;
}

bool bt_icon_get_connected(void)
{
  return s_connected;
}

void bt_icon_draw(GContext *ctx, GRect r)
{
  int cx  = r.origin.x + r.size.w / 2;
  int y0  = r.origin.y + 1;
  int y1  = r.origin.y + r.size.h - 1;
  int arm = r.size.w / 3;
  int mid = r.origin.y + r.size.h / 2;
  int q1  = r.origin.y + r.size.h / 4;
  int q3  = r.origin.y + r.size.h * 3 / 4;

  if (s_connected)
  {
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_line(ctx, GPoint(cx, y0), GPoint(cx, y1));
    graphics_draw_line(ctx, GPoint(cx, y0), GPoint(cx + arm, q1));
    graphics_draw_line(ctx, GPoint(cx + arm, q1), GPoint(cx - 2, mid + 2));
    graphics_draw_line(ctx, GPoint(cx - 2, mid - 2), GPoint(cx + arm, q3));
    graphics_draw_line(ctx, GPoint(cx + arm, q3), GPoint(cx, y1));
  }
  else
  {
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
