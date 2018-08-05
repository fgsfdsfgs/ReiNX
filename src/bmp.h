#pragma once

#include <string.h>
#include "hwinit.h"

static int bmp_load(const char *fname, u32 *out, u32 out_pitch) {
  if (!fopen(fname, "rb")) return 1;
  u8 *filedata = malloc(fsize());
  if (!filedata) { fclose(); return 2; }

  int res = fread(filedata, fsize(), 1);
  fclose();
  if (!res) { free(filedata); return 3; }

  // first 30 bytes is bmp headers (up to and including bpp)

  // extract width and height (both s32) from ihdr
  s32 w = filedata[18] +
    (((int)filedata[19]) << 8)  + 
    (((int)filedata[20]) << 16) +
    (((int)filedata[21]) << 24);
  s32 h = filedata[22] +
    (((int)filedata[23]) << 8)  +
    (((int)filedata[24]) << 16) +
    (((int)filedata[25]) << 24);

  // ignore non-24bit bmps
  u16 bpp = filedata[28] + (((int)filedata[29]) << 8);
  if (bpp != 24) { free(filedata); return 4; }

  // in BMPs row size is rounded up to nearest 4 bytes
  // because fuck you
  s32 pitch = (bpp / 8) * w;
  s32 pad = ((pitch % 4) ? (4 - (pitch % 4)) : 0);

  // extract pixel data offset
  u32 off = filedata[10] + 
    (((int)filedata[11]) << 8)  +
    (((int)filedata[12]) << 16) +
    (((int)filedata[13]) << 24);

  u32 in_pitch = pitch + pad;
  u32 in_size = h * in_pitch;

  u32 *destline = out + out_pitch * (h - 1), *dest;
  u8 *srcline = filedata + off, *src;
  for (s32 y = 0; y < h; ++y, srcline += in_pitch, destline -= out_pitch) {
    dest = destline;
    src = srcline;
    for (s32 x = 0; x < w; ++x) {
      *(dest++) = 0xFF000000 |
        (src[0] << 16) |
        (src[1] << 8 ) |
        (src[2] << 0 ) ;
      src += 3;
    }
  }

  free(filedata);
  return 0;
}
