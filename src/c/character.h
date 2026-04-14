#pragma once
#include <pebble.h>

typedef void (*CharacterStretchListener)(bool stretching);

void character_create(Layer *root);
void character_set_super(bool super);
void character_set_stretch_listener(CharacterStretchListener listener);
void character_tap(void);
void character_destroy(void);
