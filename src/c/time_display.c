#include "time_display.h"

static TextLayer *s_layer;

void time_display_create(Layer *root, GRect bounds)
{
  s_layer = text_layer_create(GRect(0, bounds.size.h - 42, bounds.size.w, 42));
  text_layer_set_text(s_layer, "00:00");
  text_layer_set_text_alignment(s_layer, GTextAlignmentCenter);
  text_layer_set_font(s_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_background_color(s_layer, GColorClear);
  text_layer_set_text_color(s_layer, GColorWhite);
  layer_add_child(root, text_layer_get_layer(s_layer));
}

void time_display_destroy(void)
{
  text_layer_destroy(s_layer);
}

void time_display_update(struct tm *t)
{
  static char buf[6];
  strftime(buf, sizeof(buf), clock_is_24h_style() ? "%H:%M" : "%I:%M", t);
  text_layer_set_text(s_layer, buf);
}
