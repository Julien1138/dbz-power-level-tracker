#include "steps_display.h"

#if defined(PBL_PLATFORM_EMERY)
#define STEPS_X 85
#define STEPS_Y 140
#elif defined(PBL_PLATFORM_GABBRO)
#define STEPS_X 120
#define STEPS_Y 170
#elif defined(PBL_PLATFORM_CHALK)
#define STEPS_X 55
#define STEPS_Y 120
#else
#define STEPS_X 35
#define STEPS_Y 110
#endif

#define STEPS_W 144
#define STEPS_H 28

static TextLayer *s_layer;
static char s_buf[16];
static StepsCallback s_callback;

void steps_display_set_callback(StepsCallback cb)
{
  s_callback = cb;
}

void steps_display_update(void)
{
#if defined(PBL_HEALTH)
  HealthValue steps = health_service_sum_today(HealthMetricStepCount);
  // steps = 7500; // For testing: simulate 7500 steps to trigger the Super Saiyan transformation.
  snprintf(s_buf, sizeof(s_buf), "%05d", (int)steps);
  if (s_callback)
    s_callback((int)steps);
#else
  snprintf(s_buf, sizeof(s_buf), "-----");
#endif
  text_layer_set_text(s_layer, s_buf);
}

void steps_display_create(Layer *root)
{
  s_layer = text_layer_create(GRect(STEPS_X, STEPS_Y, STEPS_W, STEPS_H));
  text_layer_set_background_color(s_layer, GColorClear);
#if defined(PBL_COLOR)
  text_layer_set_text_color(s_layer, GColorScreaminGreen);
#else
  text_layer_set_text_color(s_layer, GColorBlack);
#endif
  text_layer_set_font(s_layer,
                      fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIGITALIX_14)));
  text_layer_set_text_alignment(s_layer, GTextAlignmentCenter);
  layer_add_child(root, text_layer_get_layer(s_layer));

  steps_display_update();
}

void steps_display_destroy(void)
{
  text_layer_destroy(s_layer);
}
