#pragma once

#include <stdint.h>

#define KEYBUFFER_SIZE 6

struct keybuffer_t {
  uint8_t * keys;
  uint8_t next_index;
};

void keybuffer_init(struct keybuffer_t *, uint8_t * keys);
void keybuffer_down(struct keybuffer_t *, uint8_t key);
void keybuffer_up(struct keybuffer_t *, uint8_t key);
