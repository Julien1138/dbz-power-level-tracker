#include "background.h"

static Layer *s_layer;
static GBitmap *s_bitmap;

static void background_draw(Layer *layer, GContext *ctx)
{
  if (!s_bitmap)
    return;
  graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  graphics_draw_bitmap_in_rect(ctx, s_bitmap, layer_get_bounds(layer));
}

void background_create(Layer *root)
{
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CELL_GAMES_RING);
  // s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CLIFFS);

  GRect bounds = layer_get_bounds(root);
  s_layer = layer_create(bounds);
  layer_set_update_proc(s_layer, background_draw);
  layer_add_child(root, s_layer);
}

void background_destroy(void)
{
  layer_destroy(s_layer);
  gbitmap_destroy(s_bitmap);
}
