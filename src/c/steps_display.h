#pragma once
#include <pebble.h>

typedef void (*StepsCallback)(int steps);

void steps_display_set_callback(StepsCallback cb);
void steps_display_create(Layer *root);
void steps_display_destroy(void);
void steps_display_update(void);
