#include "bardock.h"

static BitmapLayer *s_layer;
static GBitmap *s_bitmap;

void bardock_create(Layer *root)
{
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BARDOCK);

  GSize size = gbitmap_get_bounds(s_bitmap).size;
  int screen_h = layer_get_bounds(root).size.h;
  int y = screen_h - size.h;

  s_layer = bitmap_layer_create(GRect(0, y, size.w, size.h));
  bitmap_layer_set_bitmap(s_layer, s_bitmap);
  bitmap_layer_set_compositing_mode(s_layer, GCompOpSet);
  layer_add_child(root, bitmap_layer_get_layer(s_layer));
}

void bardock_destroy(void)
{
  bitmap_layer_destroy(s_layer);
  gbitmap_destroy(s_bitmap);
}
