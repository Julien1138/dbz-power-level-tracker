#include "character.h"

// ── Position ──────────────────────────────────────────────────────────────────
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

// ── Animation tuning ──────────────────────────────────────────────────────────
// 3 phases: still→to_ss_1, to_ss_1→to_ss_2, to_ss_2→ss_still
// Each phase runs the same blink sequence below.

// Number of blinks per phase
#define BLINK_COUNT 7

// How long the "next" frame flashes on each blink (ms) — increase to lengthen flashes
static const uint16_t BLINK_ON_MS[BLINK_COUNT] =
    {60, 120, 200, 350, 500, 700, 900};

// Gap (showing current frame) between flashes (ms).
// The last entry MUST be 0 — it signals the final blink (stay on next, advance phase).
static const uint16_t BLINK_OFF_MS[BLINK_COUNT] =
    {640, 580, 500, 350, 200, 50, 0};

// Delay before the very first flash of each phase (ms)
#define PHASE_START_DELAY_MS 300

// ── State ─────────────────────────────────────────────────────────────────────
static BitmapLayer *s_layer;
static GBitmap *s_bitmap;
static AppTimer *s_timer;
static int s_phase; // 0=idle, 1..3=active, 4=complete
static int s_blink_idx;
static bool s_showing_next;

// Resources for each phase indexed as [phase-1]: {current_frame, next_frame}
// To add/remove phases, only edit this array — nothing else needs changing.
static const uint32_t PHASE_RES[][2] = {
    {RESOURCE_ID_IMAGE_GOKU_TO_SS_1, RESOURCE_ID_IMAGE_GOKU_TO_SS_2},
    {RESOURCE_ID_IMAGE_GOKU_TO_SS_2, RESOURCE_ID_IMAGE_GOKU_SS_STILL},
};

// ── Helpers ───────────────────────────────────────────────────────────────────
static void set_bitmap(uint32_t res_id)
{
  gbitmap_destroy(s_bitmap);
  s_bitmap = gbitmap_create_with_resource(res_id);
  bitmap_layer_set_bitmap(s_layer, s_bitmap);
}

static void anim_tick(void *context);

static void start_phase(int phase)
{
  s_phase = phase;
  s_blink_idx = 0;
  s_showing_next = false;
  set_bitmap(PHASE_RES[phase - 1][0]);
  s_timer = app_timer_register(PHASE_START_DELAY_MS, anim_tick, NULL);
}

static void anim_tick(void *context)
{
  s_timer = NULL;

  if (!s_showing_next)
  {
    // Currently on frame A — flash frame B
    set_bitmap(PHASE_RES[s_phase - 1][1]);
    s_showing_next = true;
    s_timer = app_timer_register(BLINK_ON_MS[s_blink_idx], anim_tick, NULL);
  }
  else
  {
    // Currently on frame B
    uint16_t gap = BLINK_OFF_MS[s_blink_idx];
    s_blink_idx++;
    if (gap == 0)
    {
      // Last blink of this phase — stay on B, move to next phase
      if (s_phase < (int)ARRAY_LENGTH(PHASE_RES))
      {
        start_phase(s_phase + 1);
      }
      else
      {
        s_phase = (int)ARRAY_LENGTH(PHASE_RES) + 1; // animation complete
      }
    }
    else
    {
      // Go back to A, wait before next flash
      set_bitmap(PHASE_RES[s_phase - 1][0]);
      s_showing_next = false;
      s_timer = app_timer_register(gap, anim_tick, NULL);
    }
  }
}

// ── Public API ────────────────────────────────────────────────────────────────
void character_create(Layer *root)
{
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GOKU_STILL);
  GSize img_size = gbitmap_get_bounds(s_bitmap).size;

  s_layer = bitmap_layer_create(GRect(CHARACTER_X, CHARACTER_Y, img_size.w, img_size.h));
  bitmap_layer_set_bitmap(s_layer, s_bitmap);
  bitmap_layer_set_compositing_mode(s_layer, GCompOpSet);
  layer_add_child(root, bitmap_layer_get_layer(s_layer));

  s_phase = 0;
  s_timer = NULL;
}

void character_set_super(bool super)
{
  if (super)
  {
    if (s_phase == 0)
    {
      start_phase(1); // begin transformation
    }
    // already animating or complete — nothing to do
  }
  else
  {
    // Cancel any running animation and reset to normal
    if (s_timer)
    {
      app_timer_cancel(s_timer);
      s_timer = NULL;
    }
    s_phase = 0;
    set_bitmap(RESOURCE_ID_IMAGE_GOKU_STILL);
  }
}

void character_destroy(void)
{
  if (s_timer)
    app_timer_cancel(s_timer);
  bitmap_layer_destroy(s_layer);
  gbitmap_destroy(s_bitmap);
}
