# Daisy Pod MIDI recorder

When the Daisy Pod powers up, it will start recording incoming MIDI
data (i.e. UART bytes) into memory, up to 64MB. When you press button
"SW 1" the data is saved to a file `uart.bin` on the MicroSD slot. Upon success LED 1 lights up red.

Due to some sort of bug I must write exactly 512 bytes at a time to
SD. To deal with this we use a length prefix and zero padding. The
`decode` executable decodes the format `uart.bin` to a pure stream of
bytes.

The `print` executable is a simple hex dump program that puts each
MIDI status byte on a new line.
