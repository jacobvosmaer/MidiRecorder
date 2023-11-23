#include <stdio.h>

unsigned char buf[512];

int main(void) {
  while (fread(buf, sizeof(buf), 1, stdin)) {
    unsigned char *p = buf + 2;
    int n = (int)buf[0] << 8 | (int)buf[1];
    if (n < 2 || n > sizeof(buf)) {
      fprintf(stderr, "invalid header: %d\n", n);
      return 1;
    }
    n -= 2;
    while (n--)
      putchar(*p++);
  }
}
