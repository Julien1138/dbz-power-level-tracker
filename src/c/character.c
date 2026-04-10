#include "character.h"

static BitmapLayer *s_layer;
static GBitmap *s_bitmap;
static bool s_is_super = false;

#if defined(PBL_PLATFORM_EMERY)
#define CHARACTER_X 133
#define CHARACTER_Y 50
#elif defined(PBL_PLATFORM_GABBRO)
#define CHARACTER_X 158
#define CHARACTER_Y 75
#elif defined(PBL_PLATFORM_BASALT)
#define CHARACTER_X 83
#define CHARACTER_Y 20
#elif defined(PBL_PLATFORM_CHALK)
#define CHARACTER_X 103
#define CHARACTER_Y 25
#else
#define CHARACTER_X 120
#define CHARACTER_Y 42
#endif

void character_create(Layer *root)
{
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GOKU_STILL);
  GSize img_size = gbitmap_get_bounds(s_bitmap).size;

  s_layer = bitmap_layer_create(GRect(CHARACTER_X, CHARACTER_Y, img_size.w, img_size.h));
  bitmap_layer_set_bitmap(s_layer, s_bitmap);
  bitmap_layer_set_compositing_mode(s_layer, GCompOpSet);
  layer_add_child(root, bitmap_layer_get_layer(s_layer));
}

void character_set_super(bool super)
{
  if (super == s_is_super) return;
  s_is_super = super;

  gbitmap_destroy(s_bitmap);
  s_bitmap = gbitmap_create_with_resource(
    super ? RESOURCE_ID_IMAGE_GOKU_SS_STILL : RESOURCE_ID_IMAGE_GOKU_STILL);
  bitmap_layer_set_bitmap(s_layer, s_bitmap);
}

void character_destroy(void)
{
  bitmap_layer_destroy(s_layer);
  gbitmap_destroy(s_bitmap);
}
