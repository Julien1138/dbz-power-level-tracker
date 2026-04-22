#pragma once
#include <pebble.h>

typedef void (*CharacterStretchListener)(bool stretching);

typedef enum {
  CharacterStateNormal,
  CharacterStateTransforming,
  CharacterStateSuper,
} CharacterState;

typedef void (*CharacterStateListener)(CharacterState state);
typedef void (*CharacterPhaseListener)(int phase); // 0..PHASE_COUNT-1 during transformation

void character_create(Layer *root);
void character_set_character(bool is_vegeta);
void character_set_super(bool super);
CharacterState character_get_state(void);
void character_set_stretch_listener(CharacterStretchListener listener);
void character_set_state_listener(CharacterStateListener listener);
void character_set_phase_listener(CharacterPhaseListener listener);
void character_tap(void);
void character_destroy(void);
