#pragma once
#include <pebble.h>

void bt_layer_create(Layer *root);
void bt_layer_set_connected(bool connected);
void bt_layer_destroy(void);
void bt_layer_mark_dirty(void);
