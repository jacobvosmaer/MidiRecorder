#include "daisy_pod.h"

using namespace daisy;

DaisyPod hw;

uint8_t DSY_SDRAM_BSS
    buffer[(1 << 26)  ]; /* Use all 64MB of sample RAM */
  
int main(void) {
  hw.Init();

  while (1)
    ;
}
