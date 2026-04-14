#pragma once
#include <pebble.h>

void bardock_create(Layer *root);
void bardock_set_expr(int expr); // 1, 2, or 3; 0 = clear
void bardock_destroy(void);
