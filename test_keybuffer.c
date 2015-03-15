#include <stdio.h>
#include <stdlib.h>

#include "keybuffer.h"

static void test_down(struct keybuffer_t *, uint8_t);
static void test_up(struct keybuffer_t *, uint8_t);
static void test_print(struct keybuffer_t *);

static struct keybuffer_t kb;

int main() {
  uint8_t keys[KEYBUFFER_SIZE];
  keybuffer_init(&kb, &keys[0]);

  test_down(&kb, 0x11);
  test_down(&kb, 0x11);
  test_up(&kb,   0x11);
  test_down(&kb, 0x11);
  test_down(&kb, 0x22);
  test_down(&kb, 0x33);
  test_up(&kb,   0x22);
  test_down(&kb, 0x44);
  test_down(&kb, 0x55);
  test_down(&kb, 0x66);
  test_down(&kb, 0x77);
  test_down(&kb, 0x88);
  test_down(&kb, 0x99);
  test_down(&kb, 0xAA);
  test_up(&kb,   0x99);
  test_up(&kb,   0x55);
  test_up(&kb,   0x66);
  test_up(&kb,   0x88);
  test_up(&kb,   0x77);
  test_up(&kb,   0xAA);

  return EXIT_SUCCESS;
}

static void test_down(struct keybuffer_t * kb, uint8_t key) {
  keybuffer_down(kb, key);
  printf("↓ %02X :", key);
  test_print(kb);
}

static void test_up(struct keybuffer_t * kb, uint8_t key) {
  keybuffer_up(kb, key);
  printf("↑ %02X :", key);
  test_print(kb);
}

static void test_print(struct keybuffer_t * kb) {
  for (int i = 0; i < KEYBUFFER_SIZE; i++) {
    if (kb->keys[i]) {
      printf(" %c%02X", kb->next_index == i ? '>' : ' ', kb->keys[i]);
    } else {
      printf(" %c__", kb->next_index == i ? '>' : ' ');
    }
  }
  printf("\n");
}
