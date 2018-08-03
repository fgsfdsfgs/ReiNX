#pragma once

#include "hwinit/types.h"

#define SPL_W 720
#define SPL_H 1280
#define SPL_STRIDE (720 + 48)
#define SPL_XTILES 15
#define SPL_YTILES 20
#define SPL_TILEW 48
#define SPL_TILEH 64

typedef u16 spl_delta_t;

#pragma pack(push, 1)

typedef struct spl_frame_s {
  u32 numtiles;
  spl_delta_t delta[SPL_YTILES];
  u32 data[];
} spl_frame_t;

#pragma pack(pop)

int splash_init(const char *fname);
int splash_draw_frame(void);
