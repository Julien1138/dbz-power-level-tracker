#include "bardock.h"

#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)
#define BARDOCK_SCALE 3
#define BARDOCK_X -80
#else
#define BARDOCK_SCALE 2
#define BARDOCK_X -60
#endif

#define BARDOCK_Y_OFFSET 0 // px from bottom: 0 = flush, + = below screen, - = above

static Layer *s_layer;
static GBitmap *s_bitmap;

static GColor read_pixel(GBitmapDataRowInfo row, GBitmapFormat fmt,
                         GColor *palette, int x)
{
  switch (fmt)
  {
    // case GBitmapFormat8Bit:
    //   return (GColor){.argb = row.data[x]};

  case GBitmapFormat4BitPalette:
  {
    uint8_t byte = row.data[x / 2];
    uint8_t idx = (x % 2 == 0) ? (byte >> 4) : (byte & 0x0F);
    return palette[idx];
  }

    // case GBitmapFormat2BitPalette:
    // {
    //   uint8_t byte = row.data[x / 4];
    //   uint8_t idx = (byte >> (6 - (x % 4) * 2)) & 0x03;
    //   return palette[idx];
    // }

    // case GBitmapFormat1Bit:
    // {
    //   uint8_t byte = row.data[x / 8];
    //   uint8_t bit = (byte >> (x % 8)) & 0x01;
    //   return bit ? GColorWhite : GColorBlack;
    // }

  default:
    return GColorClear;
  }
}

static void bardock_update_proc(Layer *layer, GContext *ctx)
{
  if (!s_bitmap)
    return;

  GBitmapFormat fmt = gbitmap_get_format(s_bitmap);
  GColor *palette = gbitmap_get_palette(s_bitmap);
  GRect src = gbitmap_get_bounds(s_bitmap);

  for (int sy = 0; sy < src.size.h; sy++)
  {
    GBitmapDataRowInfo row = gbitmap_get_data_row_info(s_bitmap, sy);

    for (int sx = row.min_x; sx <= row.max_x; sx++)
    {
      GColor c = read_pixel(row, fmt, palette, sx);
      if (c.a == 0)
        continue; // transparent

      graphics_context_set_fill_color(ctx, c);
      graphics_fill_rect(ctx,
                         GRect(sx * BARDOCK_SCALE, sy * BARDOCK_SCALE,
                               BARDOCK_SCALE, BARDOCK_SCALE),
                         0, GCornerNone);
    }
  }
}

void bardock_create(Layer *root)
{
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BARDOCK_0);

  GSize src = gbitmap_get_bounds(s_bitmap).size;
  int w = src.w * BARDOCK_SCALE;
  int h = src.h * BARDOCK_SCALE;
  int screen_h = layer_get_bounds(root).size.h;
  int y = screen_h - h + BARDOCK_Y_OFFSET;

  s_layer = layer_create(GRect(BARDOCK_X, y, w, h));
  layer_set_update_proc(s_layer, bardock_update_proc);
  layer_add_child(root, s_layer); // first = behind everything
}

void bardock_destroy(void)
{
  layer_destroy(s_layer);
  gbitmap_destroy(s_bitmap);
}
