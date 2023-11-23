#include <stdio.h>

int main(void) {
  int ch, first = 1;
  while ((ch = getchar()) != EOF) {
    if (!first && (ch & 0x80))
      putchar('\n');
    first = 0;
    printf("%02x ", ch);
  }
  if (!first)
    putchar('\n');
}
