#include "daisy_pod.h"
#include "fatfs.h"
#include <string.h>

#undef assert
#define assert(x)                                                              \
  if (!(x))                                                                    \
  asm("bkpt 255")

using namespace daisy;

DaisyPod hw;
SdmmcHandler sd;
FatFSInterface fsi;
MidiUartTransport midi;

uint8_t DSY_SDRAM_BSS buffer[(1 << 26)]; /* Use all 64MB of SDRAM */
struct {
  uint8_t *start, *end, *pos;
} buf = {buffer, buffer + sizeof(buffer), buffer};

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

void ledred(void) {
  hw.led1.SetRed(1);
  hw.UpdateLeds();
}

uint8_t obuf[512];
int writeframe(FIL *f, char *data, int n) {
  unsigned nw;
  enum { headersize = 2, maxpayload = sizeof(obuf) - headersize };
  assert(n >= 0);
  if (n > maxpayload)
    n = maxpayload;
  int header = n + headersize;
  obuf[0] = (header >> 8);
  obuf[1] = header;
  memmove(obuf + headersize, data, n);
  memset(obuf + headersize + n, 0, maxpayload - n);
  assert(!f_write(f, obuf, sizeof(obuf), &nw));
  return n;
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
      recording = 0;

      uint8_t *p = buf.start;
      while (p < buf.pos)
        p += writeframe(&file, (char *)p, buf.pos - p);

      f_close(&file);
      ledred();
    }
  }
}
