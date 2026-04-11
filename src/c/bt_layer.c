#include "bt_layer.h"

// ── Position & size ───────────────────────────────────────────────────────────
// BT_X / BT_Y : top-left corner of the bounding box.
// BT_RADIUS   : radius of the dot in pixels.
// The layer is (2*BT_RADIUS) × (2*BT_RADIUS); the circle is centred inside it.

#if defined(PBL_PLATFORM_EMERY)
#define BT_X 47
#define BT_Y 143
#define BT_RADIUS 5

#elif defined(PBL_PLATFORM_GABBRO)
#define BT_X 68
#define BT_Y 177
#define BT_RADIUS 5

#elif defined(PBL_PLATFORM_CHALK)
#define BT_X 48
#define BT_Y 125
#define BT_RADIUS 3

#elif defined(PBL_PLATFORM_BASALT)
#define BT_X 29
#define BT_Y 112
#define BT_RADIUS 3

#else // aplite, diorite, flint
#define BT_X 130
#define BT_Y 5
#define BT_RADIUS 4
#endif

// ── Color ─────────────────────────────────────────────────────────────────────
// On color displays: red with partial transparency (alpha bits = 0b10).
// On B&W displays  : solid black.
#if defined(PBL_COLOR)
#define BT_DOT_COLOR ((GColor){.argb = 0b10110000})
#else
#define BT_DOT_COLOR GColorBlack
#endif

// ── State ─────────────────────────────────────────────────────────────────────
static Layer *s_layer;

static bool s_connected = true;

// ── Draw ──────────────────────────────────────────────────────────────────────
static void bt_layer_draw(Layer *layer, GContext *ctx)
{
  if (s_connected)
    return; // nothing to draw when connected

  graphics_context_set_fill_color(ctx, BT_DOT_COLOR);
  graphics_fill_circle(ctx, GPoint(BT_RADIUS, BT_RADIUS), BT_RADIUS);
}

// ── Public API ────────────────────────────────────────────────────────────────
void bt_layer_create(Layer *root)
{
  int d = BT_RADIUS * 2;
  s_layer = layer_create(GRect(BT_X, BT_Y, d, d));
  layer_set_update_proc(s_layer, bt_layer_draw);
  layer_add_child(root, s_layer);
}

void bt_layer_set_connected(bool connected)
{
  s_connected = connected;
}

void bt_layer_destroy(void)
{
  layer_destroy(s_layer);
}

void bt_layer_mark_dirty(void)
{
  layer_mark_dirty(s_layer);
}
