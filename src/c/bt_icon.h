#pragma once
#include <pebble.h>

void bt_icon_set_connected(bool connected);
bool bt_icon_get_connected(void);
void bt_icon_draw(GContext *ctx, GRect r);
