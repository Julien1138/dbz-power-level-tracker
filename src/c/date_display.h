#pragma once
#include <pebble.h>

void date_display_create(Layer *root);
void date_display_destroy(void);
void date_display_update(struct tm *t);
