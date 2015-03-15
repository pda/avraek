#include "keybuffer.h"
#include "string.h"

static int8_t keybuffer_find(struct keybuffer_t *, uint8_t key);

void keybuffer_init(struct keybuffer_t * kb, uint8_t * keys) {
  memset((void *)keys, 0, KEYBUFFER_SIZE * sizeof(uint8_t));
  kb->keys = keys;
  kb->next_index = 0;
}

void keybuffer_down(struct keybuffer_t * kb, uint8_t key) {
  if (keybuffer_find(kb, key) != -1) return; // already down
  kb->keys[kb->next_index] = key;
  if (kb->next_index == (KEYBUFFER_SIZE - 1)) {
    kb->next_index = 0;
  } else {
    kb->next_index++;
  }
}

void keybuffer_up(struct keybuffer_t * kb, uint8_t key) {
  int8_t i = keybuffer_find(kb, key);
  if (i >= 0) {
    kb->keys[i] = 0;
  }
}

static int8_t keybuffer_find(struct keybuffer_t * kb, uint8_t key) {
  for (int i = 0; i < KEYBUFFER_SIZE; i++) {
    if (kb->keys[i] == key) return i;
  }
  return -1;
}
