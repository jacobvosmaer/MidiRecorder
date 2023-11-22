#include <string.h>
#include "daisy_pod.h"
#include "fatfs.h"
#include <string.h>

#undef assert
#define assert(x)                                                              \
  if (!(x))                                                                    \
  asm("bkpt 255")

using namespace daisy;

DaisyPod hw;

uint8_t DSY_SDRAM_BSS buffer[(1 << 26)]; /* Use all 64MB of sample RAM */

SdmmcHandler sd;
FatFSInterface fsi;

void mount(const char *path) {
  SdmmcHandler::Config sd_cfg;
  sd_cfg.Defaults();
  sd.Init(sd_cfg);
  fsi.Init(FatFSInterface::Config::MEDIA_SD);
  assert(!f_mount(&fsi.GetSDFileSystem(), path, 1));
}

char msg[512];

int main(void) {
  FIL file = {0};
  unsigned nw;

  hw.Init();
  mount("/");
  assert(!f_open(&file, "uart.bin", FA_CREATE_ALWAYS | FA_WRITE));

  memset(msg, '\n', sizeof(msg));
  sprintf(msg, "hello world!\n");
  assert(!f_write(&file, msg, sizeof(msg), &nw));
  assert(!f_close(&file));

  int recording = 1;
  while (1) {
    hw.ProcessDigitalControls();
    if (recording && hw.button1.RisingEdge()) {
      recording = 0;
    }
  }
}
