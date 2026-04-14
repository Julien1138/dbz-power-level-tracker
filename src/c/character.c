#include "character.h"

// ── Position ──────────────────────────────────────────────────────────────────
#if defined(PBL_PLATFORM_EMERY)
#define CHARACTER_X 118
#define CHARACTER_Y 48
#elif defined(PBL_PLATFORM_GABBRO)
#define CHARACTER_X 153
#define CHARACTER_Y 73
#elif defined(PBL_PLATFORM_CHALK)
#define CHARACTER_X 88
#define CHARACTER_Y 28
#else
#define CHARACTER_X 68
#define CHARACTER_Y 18
#endif

// ── Vibration patterns ────────────────────────────────────────────────────────
// Alternating on/off durations in ms (starts with on).

static const uint32_t VIBE_TENSION[] = {80, 600, 80, 500, 100, 400, 100, 300, 150, 200, 150, 150};
static const uint32_t VIBE_BUILDING[] = {100, 900, 120, 700, 150, 550, 180, 400, 200, 300, 220, 200, 250, 100, 300, 50, 350, 30, 400};
//  pulses grow: 100→400 ms │ gaps shrink: 900→30 ms │ total ≈ 5500 ms │ 19 segments
static const uint32_t VIBE_EXPLOSION[] = {150, 60, 250, 50, 350, 40, 450, 40, 500, 40, 550, 30, 600, 30, 650, 30, 700, 30, 900};
//  escalating heavy impacts │ gaps always short (explosive) │ total ≈ 5450 ms │ 19 segments
static const uint32_t VIBE_POWER[] = {800, 100, 400, 150, 300, 200, 600};

// ── Blink patterns ────────────────────────────────────────────────────────────
// Reused across visual phases — define separate arrays to give each phase its own rhythm.
// Last entry of blink_off MUST be 0 (signals the final flash).

static const uint16_t BLINK_ON[] = {60, 120, 200, 350, 500, 700, 900};
static const uint16_t BLINK_OFF[] = {640, 580, 500, 350, 200, 50, 0};

// ── Phase table ───────────────────────────────────────────────────────────────
// One row per phase. The state machine steps through rows top to bottom.

typedef struct
{
  uint32_t bitmap_res;          // bitmap shown at the start of this phase
  uint32_t next_bitmap_res;     // blink target (0 = non-blink phase)
  const uint16_t *blink_on_ms;  // NULL if non-blink phase
  const uint16_t *blink_off_ms; // last entry must be 0; NULL if non-blink phase
  uint8_t blink_count;          // 0 if non-blink phase
  const uint32_t *vibe;         // vibration pattern (NULL = silent)
  uint32_t vibe_count;
  uint32_t duration_ms; // non-blink: total phase duration (ms)
                        // blink:     initial delay before first flash (ms)
} Phase;

static const Phase PHASES[] = {
    {
        // ── Tension ───────────────────────────────────────────────────────────────
        // No visual change. Slow heartbeat vibration, growing dread.
        .bitmap_res = RESOURCE_ID_IMAGE_GOKU_STILL,
        .next_bitmap_res = 0,
        .blink_on_ms = NULL,
        .blink_off_ms = NULL,
        .blink_count = 0,
        .vibe = VIBE_TENSION,
        .vibe_count = ARRAY_LENGTH(VIBE_TENSION),
        .duration_ms = 2800,
    },
    {
        // ── Growing tension ───────────────────────────────────────────────────────
        // Blink between frames, accelerating vibration.
        .bitmap_res = RESOURCE_ID_IMAGE_GOKU_TO_SS_1,
        .next_bitmap_res = RESOURCE_ID_IMAGE_GOKU_TO_SS_2,
        .blink_on_ms = BLINK_ON,
        .blink_off_ms = BLINK_OFF,
        .blink_count = ARRAY_LENGTH(BLINK_ON),
        .vibe = VIBE_BUILDING,
        .vibe_count = ARRAY_LENGTH(VIBE_BUILDING),
        .duration_ms = 300, // initial delay before first flash
    },
    {
        // ── Explosion ─────────────────────────────────────────────────────────────
        // Power bursts through. Heavy impact vibration.
        .bitmap_res = RESOURCE_ID_IMAGE_GOKU_TO_SS_2,
        .next_bitmap_res = RESOURCE_ID_IMAGE_GOKU_SS_STILL,
        .blink_on_ms = BLINK_ON,
        .blink_off_ms = BLINK_OFF,
        .blink_count = ARRAY_LENGTH(BLINK_ON),
        .vibe = VIBE_EXPLOSION,
        .vibe_count = ARRAY_LENGTH(VIBE_EXPLOSION),
        .duration_ms = 300, // initial delay before first flash
    },
    {
        // ── Stabilisation ─────────────────────────────────────────────────────────
        // Final state locked in. Powerful, settled vibration.
        .bitmap_res = RESOURCE_ID_IMAGE_GOKU_SS_STILL,
        .next_bitmap_res = 0,
        .blink_on_ms = NULL,
        .blink_off_ms = NULL,
        .blink_count = 0,
        .vibe = VIBE_POWER,
        .vibe_count = ARRAY_LENGTH(VIBE_POWER),
        .duration_ms = 2500,
    },
};

#define PHASE_COUNT ((int)ARRAY_LENGTH(PHASES))

// ── Persistence ───────────────────────────────────────────────────────────────
// The watchface is killed and restarted on every menu navigation, so C statics
// are not enough. Use Pebble's persistent storage to remember the super state.
#define PERSIST_KEY_SUPER 1

// ── Stretching animation ──────────────────────────────────────────────────────
#define STRETCH_FRAME_MS 200      // default frame duration
#define STRETCH_FRAME_MS_LONG 400 // duration for the neutral/resting frame

static const uint32_t STRETCH_FRAMES[] = {
    RESOURCE_ID_IMAGE_GOKU_STRETCHING_1,
    RESOURCE_ID_IMAGE_GOKU_STRETCHING_2,
    RESOURCE_ID_IMAGE_GOKU_STRETCHING_3,
    RESOURCE_ID_IMAGE_GOKU_STRETCHING_2,
    RESOURCE_ID_IMAGE_GOKU_STRETCHING_3,
    RESOURCE_ID_IMAGE_GOKU_STRETCHING_1,
    RESOURCE_ID_IMAGE_GOKU_STRETCHING_2_FLIPPED,
    RESOURCE_ID_IMAGE_GOKU_STRETCHING_3_FLIPPED,
    RESOURCE_ID_IMAGE_GOKU_STRETCHING_2_FLIPPED,
    RESOURCE_ID_IMAGE_GOKU_STRETCHING_3_FLIPPED,
    RESOURCE_ID_IMAGE_GOKU_STRETCHING_1,
};

static const uint32_t STRETCH_SS_FRAMES[] = {
    RESOURCE_ID_IMAGE_GOKU_SS_STRETCHING_1,
    RESOURCE_ID_IMAGE_GOKU_SS_STRETCHING_2,
    RESOURCE_ID_IMAGE_GOKU_SS_STRETCHING_3,
    RESOURCE_ID_IMAGE_GOKU_SS_STRETCHING_2,
    RESOURCE_ID_IMAGE_GOKU_SS_STRETCHING_3,
    RESOURCE_ID_IMAGE_GOKU_SS_STRETCHING_1,
    RESOURCE_ID_IMAGE_GOKU_SS_STRETCHING_2_FLIPPED,
    RESOURCE_ID_IMAGE_GOKU_SS_STRETCHING_3_FLIPPED,
    RESOURCE_ID_IMAGE_GOKU_SS_STRETCHING_2_FLIPPED,
    RESOURCE_ID_IMAGE_GOKU_SS_STRETCHING_3_FLIPPED,
    RESOURCE_ID_IMAGE_GOKU_SS_STRETCHING_1,
};

static const uint32_t STRETCH_DURATIONS_MS[] = {
    STRETCH_FRAME_MS_LONG, // STRETCHING_1 — resting pose
    STRETCH_FRAME_MS,      // STRETCHING_2
    STRETCH_FRAME_MS,      // STRETCHING_3
    STRETCH_FRAME_MS,      // STRETCHING_2
    STRETCH_FRAME_MS,      // STRETCHING_3
    STRETCH_FRAME_MS_LONG, // STRETCHING_1 — resting pose
    STRETCH_FRAME_MS,      // STRETCHING_2_FLIPPED
    STRETCH_FRAME_MS,      // STRETCHING_3_FLIPPED
    STRETCH_FRAME_MS,      // STRETCHING_2_FLIPPED
    STRETCH_FRAME_MS,      // STRETCHING_3_FLIPPED
    STRETCH_FRAME_MS_LONG, // STRETCHING_1 — resting pose
};
#define STRETCH_FRAME_COUNT ((int)ARRAY_LENGTH(STRETCH_FRAMES))

// ── State ─────────────────────────────────────────────────────────────────────
static BitmapLayer *s_layer;
static GBitmap *s_bitmap;
static AppTimer *s_timer;
static int s_phase_idx; // -1=idle, 0..N-1=active, N=complete
static int s_blink_idx;
static bool s_showing_next;
static bool s_was_super; // loaded from persistent storage on create
static AppTimer *s_stretch_timer;
static int s_stretch_idx;
static const uint32_t *s_stretch_frames; // points to STRETCH_FRAMES or STRETCH_SS_FRAMES
static CharacterStretchListener s_stretch_listener;
static CharacterStateListener s_state_listener;

// ── Public listener registration ─────────────────────────────────────────────
CharacterState character_get_state(void)
{
  if (s_phase_idx < 0)           return CharacterStateNormal;
  if (s_phase_idx < PHASE_COUNT) return CharacterStateTransforming;
  return CharacterStateSuper;
}

void character_set_stretch_listener(CharacterStretchListener listener)
{
  s_stretch_listener = listener;
}

void character_set_state_listener(CharacterStateListener listener)
{
  s_state_listener = listener;
}

// ── Helpers ───────────────────────────────────────────────────────────────────
static void enter_phase(int idx); // forward declaration

static void play_vibe(const uint32_t *durations, uint32_t count)
{
  VibePattern p = {.durations = (uint32_t *)durations, .num_segments = count};
  vibes_enqueue_custom_pattern(p);
}

static void set_bitmap(uint32_t res_id)
{
  gbitmap_destroy(s_bitmap);
  s_bitmap = gbitmap_create_with_resource(res_id);
  bitmap_layer_set_bitmap(s_layer, s_bitmap);
}

static void advance_phase(void *context)
{
  s_timer = NULL;
  s_phase_idx++;
  if (s_phase_idx < PHASE_COUNT)
    enter_phase(s_phase_idx);
  else
  {
    s_was_super = true;
    persist_write_bool(PERSIST_KEY_SUPER, true);
    if (s_state_listener) s_state_listener(CharacterStateSuper);
  }
}

static void blink_tick(void *context)
{
  s_timer = NULL;
  const Phase *p = &PHASES[s_phase_idx];

  if (!s_showing_next)
  {
    set_bitmap(p->next_bitmap_res);
    s_showing_next = true;
    s_timer = app_timer_register(p->blink_on_ms[s_blink_idx], blink_tick, NULL);
  }
  else
  {
    uint16_t gap = p->blink_off_ms[s_blink_idx];
    s_blink_idx++;
    if (gap == 0)
    {
      advance_phase(NULL); // last flash done — move to next phase
    }
    else
    {
      set_bitmap(p->bitmap_res);
      s_showing_next = false;
      s_timer = app_timer_register(gap, blink_tick, NULL);
    }
  }
}

static void enter_phase(int idx)
{
  s_phase_idx = idx;
  const Phase *p = &PHASES[idx];

  set_bitmap(p->bitmap_res);
  if (p->vibe)
    play_vibe(p->vibe, p->vibe_count);

  if (p->blink_count == 0)
  {
    // Non-blink phase: wait duration_ms then advance
    s_timer = app_timer_register(p->duration_ms, advance_phase, NULL);
  }
  else
  {
    // Blink phase: wait initial delay then start flashing
    s_blink_idx = 0;
    s_showing_next = false;
    s_timer = app_timer_register(p->duration_ms, blink_tick, NULL);
  }
}

// ── Stretching callbacks ──────────────────────────────────────────────────────
static void stretch_tick(void *context)
{
  s_stretch_timer = NULL;
  s_stretch_idx++;
  if (s_stretch_idx == 0)
  {
    // Initial delay elapsed — show first frame and schedule its duration
    set_bitmap(s_stretch_frames[0]);
    if (s_stretch_listener) s_stretch_listener(true);
    s_stretch_timer = app_timer_register(STRETCH_DURATIONS_MS[0], stretch_tick, NULL);
    return;
  }
  if (s_stretch_idx >= STRETCH_FRAME_COUNT)
  {
    // Return to the correct idle bitmap
    uint32_t idle = (s_phase_idx == PHASE_COUNT) ? RESOURCE_ID_IMAGE_GOKU_SS_STILL
                                                 : RESOURCE_ID_IMAGE_GOKU_STILL;
    set_bitmap(idle);
    if (s_stretch_listener) s_stretch_listener(false);
    return;
  }
  set_bitmap(s_stretch_frames[s_stretch_idx]);
  s_stretch_timer = app_timer_register(STRETCH_DURATIONS_MS[s_stretch_idx], stretch_tick, NULL);
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

  s_phase_idx = -1; // idle
  s_timer = NULL;
  s_was_super = persist_exists(PERSIST_KEY_SUPER) && persist_read_bool(PERSIST_KEY_SUPER);
}

void character_set_super(bool super)
{
  if (super)
  {
    if (s_was_super)
    {
      // Already transformed before this window load — skip animation, show final state directly
      if (s_phase_idx == -1)
      {
        set_bitmap(RESOURCE_ID_IMAGE_GOKU_SS_STILL);
        s_phase_idx = PHASE_COUNT;
        if (s_state_listener) s_state_listener(CharacterStateSuper);
      }
    }
    else if (s_phase_idx == -1)
    {
      if (s_state_listener) s_state_listener(CharacterStateTransforming);
      enter_phase(0); // first time crossing the threshold — play the full animation
    }
    // else: animation already running or complete this session — nothing to do
  }
  else
  {
    if (s_timer)
    {
      app_timer_cancel(s_timer);
      s_timer = NULL;
    }
    vibes_cancel();
    s_was_super = false;
    persist_write_bool(PERSIST_KEY_SUPER, false);
    s_phase_idx = -1;
    set_bitmap(RESOURCE_ID_IMAGE_GOKU_STILL);
    if (s_state_listener) s_state_listener(CharacterStateNormal);
  }
}

void character_tap(void)
{
  // Don't interrupt an ongoing transformation
  if (s_phase_idx >= 0 && s_phase_idx < PHASE_COUNT)
    return;

  // Restart if already stretching
  if (s_stretch_timer)
  {
    app_timer_cancel(s_stretch_timer);
    s_stretch_timer = NULL;
    if (s_stretch_listener) s_stretch_listener(false);
  }

  s_stretch_frames = (s_phase_idx == PHASE_COUNT) ? STRETCH_SS_FRAMES : STRETCH_FRAMES;
  s_stretch_idx = -1;
  s_stretch_timer = app_timer_register(500, stretch_tick, NULL);
}

void character_destroy(void)
{
  if (s_timer)
    app_timer_cancel(s_timer);
  if (s_stretch_timer)
    app_timer_cancel(s_stretch_timer);
  vibes_cancel();
  bitmap_layer_destroy(s_layer);
  gbitmap_destroy(s_bitmap);
}
