#ifndef GPU32H_H
#define GPU32H_H 1

#define WINW 640
#define WINH 480
#define VGASIZE WINW*WINH // 307,200

#ifdef EFIKATOR
struct uefi_color {
  U8 b;
  U8 g;
  U8 r;
  U8 a;
};
struct uefi_palitro {
  struct uefi_color colors[256];
};
struct lc_gg32 {
  U8 status; // besplatno
  U8 scale; // not used
  struct uefi_palitro* pal;
  U32* buf;
  U32 linesz;
};
#else
#include <SDL3/SDL.h>
struct lc_gg32 {
  U8 status; // besplatno
  U8 scale; // platno
  SDL_Window* win;
  SDL_Renderer* rndr;
  SDL_Texture* tex;
  SDL_Palette* pal;
  U32 pal_cache[256];
};
typedef struct lc_gg32 lc_gg32;
#endif
typedef struct lc_gg32 lc_gg32;

struct ggrgb {
  U8 r;
  U8 g;
  U8 b;
};
typedef struct ggrgb ggrgb;

#endif
