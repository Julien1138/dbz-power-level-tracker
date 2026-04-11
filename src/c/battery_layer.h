#pragma once
#include <pebble.h>

void battery_layer_create(Layer *root);
void battery_layer_destroy(void);
void battery_layer_mark_dirty(void);
