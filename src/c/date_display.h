#pragma once
#include <pebble.h>

void date_display_update(struct tm *t);
const char *date_display_get_text(void);
