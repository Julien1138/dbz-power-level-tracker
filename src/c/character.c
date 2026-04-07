#include "character.h"

static BitmapLayer *s_layer;
static GBitmap     *s_bitmap;

void character_create(Layer *root, GRect bounds, int header_h, int time_h)
{
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GOKU_STILL);
  GSize img_size   = gbitmap_get_bounds(s_bitmap).size;
  int available_h  = bounds.size.h - header_h - time_h;
  int img_x        = (bounds.size.w - img_size.w) / 2;
  int img_y        = header_h + (available_h - img_size.h) / 2;

  s_layer = bitmap_layer_create(GRect(img_x, img_y, img_size.w, img_size.h));
  bitmap_layer_set_bitmap(s_layer, s_bitmap);
  bitmap_layer_set_compositing_mode(s_layer, GCompOpSet);
  layer_add_child(root, bitmap_layer_get_layer(s_layer));
}

void character_destroy(void)
{
  bitmap_layer_destroy(s_layer);
  gbitmap_destroy(s_bitmap);
}
