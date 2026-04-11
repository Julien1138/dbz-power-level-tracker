#pragma once
#include <pebble.h>

void time_display_create(Layer *root);
void time_display_destroy(void);
void time_display_update(struct tm *t);
