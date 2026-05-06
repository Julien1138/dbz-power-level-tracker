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
        .next_bitmap_res = RESOURCE_ID_IMAGE_GOKU_TO_SS_3,
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
        .bitmap_res = RESOURCE_ID_IMAGE_GOKU_TO_SS_3,
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
// PERSIST_KEY_SUPER stores today_key() when super, 0 otherwise — one value
// encodes both state and day, so stale data from a previous day is ignored.
#define PERSIST_KEY_SUPER 1

static int today_key(void)
{
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  return t->tm_yday + t->tm_year * 366 + 1; // +1 so result is always non-zero
}

// ── Goku: stretching animation ────────────────────────────────────────────────
#define STRETCH_FRAME_MS 200
#define STRETCH_FRAME_MS_LONG 400

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
    STRETCH_FRAME_MS_LONG,
    STRETCH_FRAME_MS,
    STRETCH_FRAME_MS,
    STRETCH_FRAME_MS,
    STRETCH_FRAME_MS,
    STRETCH_FRAME_MS_LONG,
    STRETCH_FRAME_MS,
    STRETCH_FRAME_MS,
    STRETCH_FRAME_MS,
    STRETCH_FRAME_MS,
    STRETCH_FRAME_MS_LONG,
};
#define STRETCH_FRAME_COUNT ((int)ARRAY_LENGTH(STRETCH_FRAMES))

// ── Vegeta: transformation phases ────────────────────────────────────────────
// No TO_SS_3 — explosion phase reuses TO_SS_2/TO_SS_1 blink.
static const Phase PHASES_VEGETA[] = {
    {
        .bitmap_res = RESOURCE_ID_IMAGE_VEGETA_STILL,
        .next_bitmap_res = 0,
        .blink_on_ms = NULL,
        .blink_off_ms = NULL,
        .blink_count = 0,
        .vibe = VIBE_TENSION,
        .vibe_count = ARRAY_LENGTH(VIBE_TENSION),
        .duration_ms = 2800,
    },
    {
        .bitmap_res = RESOURCE_ID_IMAGE_VEGETA_TO_SS_1,
        .next_bitmap_res = RESOURCE_ID_IMAGE_VEGETA_TO_SS_2,
        .blink_on_ms = BLINK_ON,
        .blink_off_ms = BLINK_OFF,
        .blink_count = ARRAY_LENGTH(BLINK_ON),
        .vibe = VIBE_BUILDING,
        .vibe_count = ARRAY_LENGTH(VIBE_BUILDING),
        .duration_ms = 300,
    },
    {
        // No TO_SS_3 — blink TO_SS_2 ↔ TO_SS_1 (reversed) for visual variation
        .bitmap_res = RESOURCE_ID_IMAGE_VEGETA_TO_SS_2,
        .next_bitmap_res = RESOURCE_ID_IMAGE_VEGETA_TO_SS_1,
        .blink_on_ms = BLINK_ON,
        .blink_off_ms = BLINK_OFF,
        .blink_count = ARRAY_LENGTH(BLINK_ON),
        .vibe = VIBE_EXPLOSION,
        .vibe_count = ARRAY_LENGTH(VIBE_EXPLOSION),
        .duration_ms = 300,
    },
    {
        .bitmap_res = RESOURCE_ID_IMAGE_VEGETA_TO_SS_2,
        .next_bitmap_res = 0,
        .blink_on_ms = NULL,
        .blink_off_ms = NULL,
        .blink_count = 0,
        .vibe = VIBE_POWER,
        .vibe_count = ARRAY_LENGTH(VIBE_POWER),
        .duration_ms = 2500,
    },
};

// ── Vegeta: punching animation ────────────────────────────────────────────────
// Sequence: 1, 2, 3, 2, 3, 1
#define PUNCH_FRAME_MS 150
#define PUNCH_FRAME_MS_LONG 350

static const uint32_t PUNCH_FRAMES[] = {
    RESOURCE_ID_IMAGE_VEGETA_PUNCHING_1,
    RESOURCE_ID_IMAGE_VEGETA_PUNCHING_2,
    RESOURCE_ID_IMAGE_VEGETA_PUNCHING_3,
    RESOURCE_ID_IMAGE_VEGETA_PUNCHING_1,
    RESOURCE_ID_IMAGE_VEGETA_PUNCHING_2,
    RESOURCE_ID_IMAGE_VEGETA_PUNCHING_3,
    RESOURCE_ID_IMAGE_VEGETA_PUNCHING_1,
};
static const uint32_t PUNCH_SS_FRAMES[] = {
    RESOURCE_ID_IMAGE_VEGETA_SS_PUNCHING_1,
    RESOURCE_ID_IMAGE_VEGETA_SS_PUNCHING_2,
    RESOURCE_ID_IMAGE_VEGETA_SS_PUNCHING_3,
    RESOURCE_ID_IMAGE_VEGETA_SS_PUNCHING_1,
    RESOURCE_ID_IMAGE_VEGETA_SS_PUNCHING_2,
    RESOURCE_ID_IMAGE_VEGETA_SS_PUNCHING_3,
    RESOURCE_ID_IMAGE_VEGETA_SS_PUNCHING_1,
};
static const uint32_t PUNCH_DURATIONS_MS[] = {
    PUNCH_FRAME_MS_LONG,
    PUNCH_FRAME_MS,
    PUNCH_FRAME_MS,
    PUNCH_FRAME_MS_LONG,
    PUNCH_FRAME_MS,
    PUNCH_FRAME_MS,
    PUNCH_FRAME_MS_LONG,
};
#define PUNCH_FRAME_COUNT ((int)ARRAY_LENGTH(PUNCH_FRAMES))

// ── State ─────────────────────────────────────────────────────────────────────
static BitmapLayer *s_layer;
static GBitmap *s_bitmap;
static AppTimer *s_timer;
static int s_phase_idx; // -1=idle, 0..N-1=active, N=complete
static int s_blink_idx;
static bool s_showing_next;
static bool s_was_super;
static bool s_vibe_on_transform = true;
static bool s_vibe_muted = false;
static AppTimer *s_stretch_timer;
static int s_stretch_idx;
static const uint32_t *s_tap_frames_normal; // tap frames for normal state
static const uint32_t *s_tap_frames_ss;     // tap frames for super state
static const uint32_t *s_stretch_frames;    // current animation frames (set in character_tap)
static const uint32_t *s_tap_durations;     // frame durations
static int s_tap_frame_count;
static const Phase *s_phases; // PHASES or PHASES_VEGETA
static bool s_is_vegeta;
static CharacterStretchListener s_stretch_listener;
static CharacterStateListener s_state_listener;
static CharacterPhaseListener s_phase_listener;

// ── Character helpers ─────────────────────────────────────────────────────────
static uint32_t idle_res(void)
{
  return s_is_vegeta ? RESOURCE_ID_IMAGE_VEGETA_STILL : RESOURCE_ID_IMAGE_GOKU_STILL;
}

static uint32_t ss_res(void)
{
  return s_is_vegeta ? RESOURCE_ID_IMAGE_VEGETA_SS_STILL : RESOURCE_ID_IMAGE_GOKU_SS_STILL;
}

static void apply_character(void)
{
  if (s_is_vegeta)
  {
    s_phases = PHASES_VEGETA;
    s_tap_frames_normal = PUNCH_FRAMES;
    s_tap_frames_ss = PUNCH_SS_FRAMES;
    s_tap_durations = PUNCH_DURATIONS_MS;
    s_tap_frame_count = PUNCH_FRAME_COUNT;
  }
  else
  {
    s_phases = PHASES;
    s_tap_frames_normal = STRETCH_FRAMES;
    s_tap_frames_ss = STRETCH_SS_FRAMES;
    s_tap_durations = STRETCH_DURATIONS_MS;
    s_tap_frame_count = STRETCH_FRAME_COUNT;
  }
}

// ── Public listener registration ─────────────────────────────────────────────
void character_set_vibe_on_transform(bool enabled)
{
  s_vibe_on_transform = enabled;
}

void character_mute_vibe(void)
{
  vibes_cancel();
  s_vibe_muted = true;
}

CharacterState character_get_state(void)
{
  if (s_phase_idx < 0)
    return CharacterStateNormal;
  if (s_phase_idx < PHASE_COUNT)
    return CharacterStateTransforming;
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

void character_set_phase_listener(CharacterPhaseListener listener)
{
  s_phase_listener = listener;
}

// ── Helpers ───────────────────────────────────────────────────────────────────
static void enter_phase(int idx); // forward declaration

static void play_vibe(const uint32_t *durations, uint32_t count)
{
  if (!s_vibe_on_transform || s_vibe_muted || battery_state_service_peek().is_plugged || quiet_time_is_active())
    return;
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
    s_vibe_muted = false;
    set_bitmap(ss_res());
    s_was_super = true;
    persist_write_int(PERSIST_KEY_SUPER, today_key());
    if (s_state_listener)
      s_state_listener(CharacterStateSuper);
  }
}

static void blink_tick(void *context)
{
  s_timer = NULL;
  const Phase *p = &s_phases[s_phase_idx];

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
  if (s_phase_listener)
    s_phase_listener(idx);
  const Phase *p = &s_phases[idx];

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
    if (s_stretch_listener)
      s_stretch_listener(true);
    s_stretch_timer = app_timer_register(s_tap_durations[0], stretch_tick, NULL);
    return;
  }
  if (s_stretch_idx >= s_tap_frame_count)
  {
    set_bitmap((s_phase_idx == PHASE_COUNT) ? ss_res() : idle_res());
    if (s_stretch_listener)
      s_stretch_listener(false);
    return;
  }
  set_bitmap(s_stretch_frames[s_stretch_idx]);
  s_stretch_timer = app_timer_register(s_tap_durations[s_stretch_idx], stretch_tick, NULL);
}

// ── Public API ────────────────────────────────────────────────────────────────
void character_create(Layer *root)
{
  apply_character();
  s_bitmap = gbitmap_create_with_resource(idle_res());
  GSize img_size = gbitmap_get_bounds(s_bitmap).size;

  s_layer = bitmap_layer_create(GRect(CHARACTER_X, CHARACTER_Y, img_size.w, img_size.h));
  bitmap_layer_set_bitmap(s_layer, s_bitmap);
  bitmap_layer_set_compositing_mode(s_layer, GCompOpSet);
  layer_add_child(root, bitmap_layer_get_layer(s_layer));

  s_phase_idx = -1; // idle
  s_timer = NULL;
  s_was_super = persist_exists(PERSIST_KEY_SUPER) &&
                persist_read_int(PERSIST_KEY_SUPER) == today_key();
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
        set_bitmap(ss_res());
        s_phase_idx = PHASE_COUNT;
        if (s_state_listener)
          s_state_listener(CharacterStateSuper);
      }
    }
    else if (s_phase_idx == -1)
    {
      if (s_state_listener)
        s_state_listener(CharacterStateTransforming);
      enter_phase(0); // first time crossing the threshold — play the full animation
    }
    // else: animation already running or complete this session — nothing to do
  }
  else
  {
    if (s_phase_idx == -1 && !s_was_super)
      return; // already in normal state — nothing to do
    if (s_timer)
    {
      app_timer_cancel(s_timer);
      s_timer = NULL;
    }
    vibes_cancel();
    s_vibe_muted = false;
    s_was_super = false;
    persist_write_int(PERSIST_KEY_SUPER, 0);
    s_phase_idx = -1;
    set_bitmap(idle_res());
    if (s_state_listener)
      s_state_listener(CharacterStateNormal);
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
    if (s_stretch_listener)
      s_stretch_listener(false);
  }

  s_stretch_frames = (s_phase_idx == PHASE_COUNT) ? s_tap_frames_ss : s_tap_frames_normal;
  s_stretch_idx = -1;
  s_stretch_timer = app_timer_register(500, stretch_tick, NULL);
}

void character_set_character(bool is_vegeta)
{
  s_is_vegeta = is_vegeta;
  apply_character();
  // If the layer exists and we're idle, update the displayed bitmap immediately
  if (s_layer && s_phase_idx == -1 && !s_stretch_timer)
    set_bitmap(idle_res());
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
