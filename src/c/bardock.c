#include "bardock.h"

#if defined(PBL_PLATFORM_EMERY)
#define BARDOCK_RES RESOURCE_ID_IMAGE_BARDOCK_0_X3
#define BARDOCK_X -90
#elif defined(PBL_PLATFORM_GABBRO)
#define BARDOCK_RES RESOURCE_ID_IMAGE_BARDOCK_0_X3
#define BARDOCK_X -80
#elif defined(PBL_PLATFORM_CHALK)
#define BARDOCK_RES RESOURCE_ID_IMAGE_BARDOCK_0_X2
#define BARDOCK_X -50
#else
#define BARDOCK_RES RESOURCE_ID_IMAGE_BARDOCK_0_X2
#define BARDOCK_X -60
#endif

#define BARDOCK_Y_OFFSET 0 // px from bottom: 0 = flush, + = below screen, - = above

static BitmapLayer *s_layer;
static GBitmap *s_bitmap;

void bardock_create(Layer *root)
{
  s_bitmap = gbitmap_create_with_resource(BARDOCK_RES);

  GSize size = gbitmap_get_bounds(s_bitmap).size;
  int screen_h = layer_get_bounds(root).size.h;
  int y = screen_h - size.h + BARDOCK_Y_OFFSET;

  s_layer = bitmap_layer_create(GRect(BARDOCK_X, y, size.w, size.h));
  bitmap_layer_set_bitmap(s_layer, s_bitmap);
  bitmap_layer_set_compositing_mode(s_layer, GCompOpSet);
  layer_add_child(root, bitmap_layer_get_layer(s_layer));
}

void bardock_destroy(void)
{
  bitmap_layer_destroy(s_layer);
  gbitmap_destroy(s_bitmap);
}
