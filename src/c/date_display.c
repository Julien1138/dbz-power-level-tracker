#include "date_display.h"

// ── Position & size ───────────────────────────────────────────────────────────
// Positioned just below the clock — mirror its X/W and use the next font size down.

#if defined(PBL_PLATFORM_EMERY)
#define DATE_X 0
#define DATE_Y 205
#define DATE_W 200
#define DATE_H 26
#define DATE_FONT RESOURCE_ID_FONT_ANIMEACE_18

#elif defined(PBL_PLATFORM_GABBRO)
#define DATE_X 0
#define DATE_Y 220
#define DATE_W 260
#define DATE_H 26
#define DATE_FONT RESOURCE_ID_FONT_ANIMEACE_18

#elif defined(PBL_PLATFORM_CHALK)
#define DATE_X 0
#define DATE_Y 150
#define DATE_W 180
#define DATE_H 22
#define DATE_FONT RESOURCE_ID_FONT_ANIMEACE_14

#elif defined(PBL_PLATFORM_BASALT)
#define DATE_X 0
#define DATE_Y 150
#define DATE_W 144
#define DATE_H 22
#define DATE_FONT RESOURCE_ID_FONT_ANIMEACE_14

#else // aplite, diorite, flint
#define DATE_X 64
#define DATE_Y 145
#define DATE_W 80
#define DATE_H 20
#define DATE_FONT RESOURCE_ID_FONT_ANIMEACE_12
#endif

static TextLayer *s_layer;
static char s_buf[12];

void date_display_create(Layer *root)
{
  s_layer = text_layer_create(GRect(DATE_X, DATE_Y, DATE_W, DATE_H));
  text_layer_set_text_alignment(s_layer, GTextAlignmentCenter);
  text_layer_set_font(s_layer, fonts_load_custom_font(resource_get_handle(DATE_FONT)));
  text_layer_set_background_color(s_layer, GColorClear);
  text_layer_set_text_color(s_layer, GColorBlack);
  layer_add_child(root, text_layer_get_layer(s_layer));
}

void date_display_destroy(void)
{
  text_layer_destroy(s_layer);
}

void date_display_update(struct tm *t)
{
  strftime(s_buf, sizeof(s_buf), "%a %d %b", t);
  text_layer_set_text(s_layer, s_buf);
}
