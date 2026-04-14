#include "time_display.h"

// ── Position & size ───────────────────────────────────────────────────────────

#if defined(PBL_PLATFORM_EMERY)
#define CLOCK_X 65
#define CLOCK_W 135
#define CLOCK_H 50
#define CLOCK_Y 0
#define CLOCK_FONT RESOURCE_ID_FONT_ANIMEACE_36

#elif defined(PBL_PLATFORM_GABBRO)
#define CLOCK_X 0
#define CLOCK_W 260
#define CLOCK_H 50
#define CLOCK_Y 10
#define CLOCK_FONT RESOURCE_ID_FONT_ANIMEACE_36

#elif defined(PBL_PLATFORM_CHALK)
#define CLOCK_X 0
#define CLOCK_W 180
#define CLOCK_H 42
#define CLOCK_Y 6
#define CLOCK_FONT RESOURCE_ID_FONT_ANIMEACE_24

#else // basalt, aplite, diorite, flint
#define CLOCK_X 0
#define CLOCK_W 144
#define CLOCK_H 42
#define CLOCK_Y 0
#define CLOCK_FONT RESOURCE_ID_FONT_ANIMEACE_24
#endif

static TextLayer *s_layer;

void time_display_create(Layer *root)
{
  s_layer = text_layer_create(GRect(CLOCK_X, CLOCK_Y, CLOCK_W, CLOCK_H));
  text_layer_set_text(s_layer, "00:00");
  text_layer_set_text_alignment(s_layer, GTextAlignmentCenter);
  text_layer_set_font(s_layer, fonts_load_custom_font(resource_get_handle(CLOCK_FONT)));
  text_layer_set_background_color(s_layer, GColorClear);
  text_layer_set_text_color(s_layer, GColorBlack);
  layer_add_child(root, text_layer_get_layer(s_layer));
}

void time_display_destroy(void)
{
  text_layer_destroy(s_layer);
}

// Uncomment and edit to display a fixed time for testing:
// #define CLOCK_TEST_TIME "23:59"

void time_display_update(struct tm *t)
{
#ifdef CLOCK_TEST_TIME
  text_layer_set_text(s_layer, CLOCK_TEST_TIME);
#else
  static char buf[6];
  strftime(buf, sizeof(buf), clock_is_24h_style() ? "%H:%M" : "%I:%M", t);
  text_layer_set_text(s_layer, buf);
#endif
}
