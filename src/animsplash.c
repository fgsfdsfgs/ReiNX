/*
* Copyright (c) 2018 Reisyukaku
*
* This program is free software; you can redistribute it and/or modify it
* under the terms and conditions of the GNU General Public License,
* version 2, as published by the Free Software Foundation.
*
* This program is distributed in the hope it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>

#include "hwinit.h"
#include "fs.h"

#include "animsplash.h"

static spl_frame_t *frame = NULL;
static u8* spldata = NULL;
static u8* splptr = NULL;
static u64 splsize = 0;

int splash_init(const char *fname) {
  u32 r = fopen(fname, "rb");
  if (!r) return -2;

  splsize = fsize();
  if (!splsize) { fclose(); return -3; }

  spldata = malloc(splsize);
  if (!spldata) { fclose(); return -4; }

  if (!fread(spldata, splsize, 1)) { fclose(); free(spldata); return -5; }

  fclose();

  splptr = spldata;

  return 0;
}

int splash_draw_frame(void) {
  spl_delta_t dline, dcol;
  u32 *tile = NULL, *pixel = NULL;
  u32 *line = NULL, *base = NULL;
  u32 y = 0, tx = 0, ty = 0;

  if (!splptr) return -1;

  if (splptr >= spldata + splsize) {
    splptr = NULL;
    free(spldata);
    return -2;
  }

  frame = (spl_frame_t *)splptr;
  splptr += sizeof(spl_frame_t) + frame->numtiles * SPL_TILEW * SPL_TILEH * sizeof(u32);

  tile = frame->data;
  line = (u32*)0xC0000000;

  for (ty = 0; ty < SPL_YTILES; ++ty) {
    dline = frame->delta[ty];
    dcol = dline;
    base = line;
    for (tx = 0; tx < SPL_XTILES; dcol >>= 1, ++tx) {
      if (dcol & 1) {
        pixel = base;
        for (y = 0; y < SPL_TILEH; ++y) {
          memcpy(pixel, tile, SPL_TILEW * sizeof(u32));
          tile += SPL_TILEW;
          pixel += SPL_STRIDE;
        }
      }
      base += SPL_TILEW;
    }
    line += SPL_TILEH * SPL_STRIDE;
  }

  return 0;
}
