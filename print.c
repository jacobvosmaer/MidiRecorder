#include <stdio.h>

int main(void) {
  int ch, n;
  for (n = 0; (ch = getchar()) != EOF; n++) {
    if (n && (ch & 0x80))
      putchar('\n');
    printf("%02x ", ch);
  }
  if (n)
    putchar('\n');
}
