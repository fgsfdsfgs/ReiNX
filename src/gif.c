#include <stdint.h>
#include "hwinit.h"
#include "fs.h"

#include "gif.h"

static void gif_frame(void *data, GIF_WHDR *whdr) {
  u32 x, y, yoff, iter, ifin, dsrc, bgcol;
  u32 *fbuf = data;

  #define BGRA(i) \
      ((u32)(whdr->cpal[whdr->bptr[i]].R <<  0) | \
       (u32)(whdr->cpal[whdr->bptr[i]].G <<  8) | \
       (u32)(whdr->cpal[whdr->bptr[i]].B << 16) | \
                                          0xFF000000)

  if (!whdr->ifrm) {
    whdr->nfrm = ((whdr->nfrm < 0)? -whdr->nfrm : whdr->nfrm) * whdr->ydim;
    whdr->nfrm = (whdr->nfrm < 0xFFFF)? whdr->nfrm : 0xFFFF;
  }

  ifin = (!(iter = (whdr->intr)? 0 : 4))? 4 : 5; /** interlacing support **/
  bgcol = (whdr->mode == GIF_BKGD) ? BGRA(whdr->bkgd) : 0xFF000000;

  for (dsrc = (u32)-1; iter < ifin; iter++)
    for (yoff = 16U >> ((iter > 1)? iter : 1), y = (8 >> iter) & 7;
       y < (u32)whdr->fryd; y += yoff)
      for (x = 0; x < (u32)whdr->frxd; x++) {
        if (whdr->tran != (long)whdr->bptr[++dsrc])
          fbuf[768 * x + y] = BGRA(dsrc);
        else
          fbuf[768 * x + y] = bgcol;
      }

  #undef BGRA
}

void gif_show(const char *fname) {
  u8 *data = NULL;

  u32 r = fopen(fname, "rb");
  if (!r) return;
  size_t fsz = fsize();
  if (!fsz) { fclose(); return; }
  data = malloc(fsz);
  if (!data) { fclose(); return; }
  fread(data, fsz, 1);
  fclose();

  gif_load(data, (long)fsz, gif_frame, 0, (void*)0xC0000000, 0L);

  free(data);
}
