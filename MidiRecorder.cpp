#include "daisy_pod.h"
#include "fatfs.h"

#undef assert
#define assert(x)                                                              \
  if (!(x))                                                                    \
  asm("bkpt 255")

int ferr;
#define assert_ff(x) assert((ferr = (x)) == FR_OK)

using namespace daisy;

DaisyPod hw;

uint8_t DSY_SDRAM_BSS buffer[(1 << 26)]; /* Use all 64MB of sample RAM */

SdmmcHandler sd;
SdmmcHandler::Config sd_cfg;
FatFSInterface fsi;

void mount(const char *path) {
  sd_cfg.Defaults();
  sd.Init(sd_cfg);
  fsi.Init(FatFSInterface::Config::MEDIA_SD);
  assert_ff(f_mount(&fsi.GetSDFileSystem(), path, 1));
}

int main(void) {
  FIL file = {0};
  unsigned nw;

  hw.Init();
  mount("/");
  assert_ff(f_open(&file, "uart.bin", FA_CREATE_ALWAYS | FA_WRITE));

  assert_ff(f_write(&file, "hello world\n", 12, &nw));
  assert_ff(f_sync(&file));
  int recording = 1;
  while (1) {
    hw.ProcessDigitalControls();
    if (recording && hw.button1.RisingEdge()) {
      recording = 0;
      f_close(&file);
    }
  }
}
