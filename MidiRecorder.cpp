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
struct {
  uint8_t *start, *end, *pos;
} buf = {buffer, buffer + sizeof(buffer), buffer};

SdmmcHandler sd;
FatFSInterface fsi;

MidiUartTransport midi;

void mount(const char *path) {
  SdmmcHandler::Config sd_cfg;
  sd_cfg.Defaults();
  sd.Init(sd_cfg);
  fsi.Init(FatFSInterface::Config::MEDIA_SD);
  assert(!f_mount(&fsi.GetSDFileSystem(), path, 1));
}

int recording = 1;
void midicallback(uint8_t *data, size_t size, void *context) {
  if (recording && ((ptrdiff_t)size < buf.end - buf.pos)) {
    memcpy(buf.pos, data, size);
    buf.pos += size;
  }
}

char obuf[512];

void ledred(void) {
  hw.led1.SetRed(1);
  hw.UpdateLeds();
}

int main(void) {
  FIL file = {0};

  hw.Init();

  mount("/");
  assert(!f_open(&file, "uart.bin", FA_CREATE_ALWAYS | FA_WRITE));

  midi.Init(MidiUartTransport::Config());
  midi.StartRx(midicallback, 0);

  while (1) {
    hw.ProcessDigitalControls();

    if (recording && hw.button1.RisingEdge()) {
      uint8_t *p;
      unsigned nw;
      enum { frame = 510 };
      recording = 0;

      for (p = buf.start; buf.pos - p >= frame; p += frame) {
        obuf[0] = 512 >> 8;
        obuf[1] = 512 & 0xff;
        memcpy(obuf + 2, p, frame);
        f_write(&file, obuf, sizeof(obuf), &nw);
      }

      int rem = buf.pos - p;
      if (rem) {
        obuf[0] = rem >> 8;
        obuf[1] = rem & 0xff;
        memcpy(obuf + 2, p, rem);
        memset(obuf + 2 + rem, 0, frame - rem);
        f_write(&file, obuf, sizeof(obuf), &nw);
      }

      f_close(&file);
      ledred();
    }
  }
}
