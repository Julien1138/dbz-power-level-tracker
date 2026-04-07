#pragma once
#include <pebble.h>

void battery_icon_set_state(BatteryChargeState state);
void battery_icon_draw(GContext *ctx, GRect r);
