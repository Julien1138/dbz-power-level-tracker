#include "bardock.h"

// ── Expression overlay position ───────────────────────────────────────────────
// On-screen (x, y) of the face expression overlay layer.
// x is always 0 (face is partially off-screen to the left; the bitmap is
// pre-cropped to the visible portion).
// y = screen_h - bardock_h + face_y_in_complete_sprite
#if defined(PBL_PLATFORM_EMERY)
#define EXPR_X 0
#define EXPR_Y 63
#elif defined(PBL_PLATFORM_GABBRO)
#define EXPR_X 0
#define EXPR_Y 95
#elif defined(PBL_PLATFORM_CHALK)
#define EXPR_X 0
#define EXPR_Y 70
#else
#define EXPR_X 0
#define EXPR_Y 58
#endif

static BitmapLayer *s_layer;
static GBitmap *s_bitmap;

static BitmapLayer *s_expr_layer;
static GBitmap *s_expr_bitmap;

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

  // Expression overlay — created hidden (zero-size) until first set_expr call
  s_expr_layer = bitmap_layer_create(GRect(EXPR_X, EXPR_Y, 0, 0));
  bitmap_layer_set_compositing_mode(s_expr_layer, GCompOpSet);
  layer_add_child(root, bitmap_layer_get_layer(s_expr_layer));
  s_expr_bitmap = NULL;
}

void bardock_set_expr(int expr)
{
  if (!s_expr_layer)
    return;

  if (s_expr_bitmap)
  {
    gbitmap_destroy(s_expr_bitmap);
    s_expr_bitmap = NULL;
  }

  if (expr == 0)
  {
    layer_set_frame(bitmap_layer_get_layer(s_expr_layer), GRect(EXPR_X, EXPR_Y, 0, 0));
    bitmap_layer_set_bitmap(s_expr_layer, NULL);
    return;
  }

  uint32_t res;
  switch (expr)
  {
  case 2:  res = RESOURCE_ID_IMAGE_BARDOCK_EXPR2; break;
  case 3:  res = RESOURCE_ID_IMAGE_BARDOCK_EXPR3; break;
  default: res = RESOURCE_ID_IMAGE_BARDOCK_EXPR1; break;
  }

  s_expr_bitmap = gbitmap_create_with_resource(res);
  GSize sz = gbitmap_get_bounds(s_expr_bitmap).size;
  layer_set_frame(bitmap_layer_get_layer(s_expr_layer), GRect(EXPR_X, EXPR_Y, sz.w, sz.h));
  bitmap_layer_set_bitmap(s_expr_layer, s_expr_bitmap);
}

void bardock_destroy(void)
{
  bitmap_layer_destroy(s_expr_layer);
  if (s_expr_bitmap)
    gbitmap_destroy(s_expr_bitmap);
  bitmap_layer_destroy(s_layer);
  gbitmap_destroy(s_bitmap);
}
