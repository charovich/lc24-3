// GPU identificator: GovnGraphics 69-4932
#include <cpu24/gpuh.h>
#include <cpu24/gpufont.h>

ggrgb rgbv[] = {
  (ggrgb){.r = 0x00, .g = 0x00, .b = 0x00},
  (ggrgb){.r = 0xAA, .g = 0x00, .b = 0x00},
  (ggrgb){.r = 0x00, .g = 0xAA, .b = 0x00},
  (ggrgb){.r = 0xAA, .g = 0x55, .b = 0x00},
  (ggrgb){.r = 0x00, .g = 0x00, .b = 0xAA},
  (ggrgb){.r = 0xAA, .g = 0x00, .b = 0xAA},
  (ggrgb){.r = 0x00, .g = 0xAA, .b = 0xAA},
  (ggrgb){.r = 0xAA, .g = 0xAA, .b = 0xAA},

  (ggrgb){.r = 0x55, .g = 0x55, .b = 0x55},
  (ggrgb){.r = 0xFF, .g = 0x55, .b = 0x55},
  (ggrgb){.r = 0x55, .g = 0xFF, .b = 0x55},
  (ggrgb){.r = 0xFF, .g = 0xFF, .b = 0x55},
  (ggrgb){.r = 0x55, .g = 0x55, .b = 0xFF},
  (ggrgb){.r = 0xFF, .g = 0x55, .b = 0xFF},
  (ggrgb){.r = 0x55, .g = 0xFF, .b = 0xFF},
  (ggrgb){.r = 0xFF, .g = 0xFF, .b = 0xFF},
};

enum glcolors {
  BLACK    = 0, // Standard 8 colors
  RED      = 1,
  GREEN    = 2,
  YELLOW   = 3,
  BLUE     = 4,
  MAGENTA  = 5,
  CYAN     = 6,
  WHITE    = 7,

  EBLACK   = 8, // Bright 8 colors
  ERED     = 9,
  EGREEN   = 10,
  EYELLOW  = 11,
  EBLUE    = 12,
  EMAGENTA = 13,
  ECYAN    = 14,
  EWHITE   = 15,
};
#ifndef EFIKATOR
U0 GGinit(U8* mem, lc_gg32* gg, U8 scale) {
  memcpy(mem+0x4A1000, CHARSET, 2048);
  memcpy(mem+0x4A1800, CHARSET816, 4096);
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  SDL_SetAppMetadata("LostCore 24-20020", "24-3", "io.github.charovich.lc24-3");
  gg->scale = scale;
  gg->win = SDL_CreateWindow("Lost NEWVGA2", WINW * scale, WINH * scale, SDL_WINDOW_HIGH_PIXEL_DENSITY);
  gg->surf = SDL_CreateSurface(WINW, WINH, SDL_PIXELFORMAT_INDEX8);
  gg->pal = SDL_CreatePalette(256);
  SDL_SetSurfacePalette(gg->surf, gg->pal);
  gg->status = 0b00000000;
  SDL_HideCursor();
  SDL_UpdateWindowSurface(gg->win);
}

U0 GGstop(lc_gg32* gg) {
  SDL_DestroyPalette(gg->pal);
  SDL_DestroySurface(gg->surf);
  SDL_DestroyRenderer(gg->rndr);
  SDL_DestroyWindow(gg->win);
  SDL_Quit();
}

U0 GGupload(LC* lc) {
  lc->gg.surf->pixels = lc->mem + 0x400000;
  SDL_SetSurfacePalette(lc->gg.surf, lc->gg.pal);
  SDL_BlitSurfaceScaled(lc->gg.surf, 0, SDL_GetWindowSurface(lc->gg.win), 0, SDL_SCALEMODE_NEAREST);
  SDL_UpdateWindowSurface(lc->gg.win);
}
#else
U0 GGinit(U8* mem, lc_gg32* gg, U8 scale) {
  memcpy(mem+0x4A1000, CHARSET, 2048);
  memcpy(mem+0x4A1800, CHARSET816, 4096);
  efi_status_t status;
  efi_guid_t gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  efi_gop_t *gop = NULL;
  // efi_gop_mode_info_t *info = NULL;
  // uintn_t isiz = sizeof(efi_gop_mode_info_t), currentMode, i;
  status = BS->LocateProtocol(&gopGuid, NULL, (void**)&gop);
  if (
    EFI_ERROR(status)
    || !gop
    || gop->Mode->Information->PixelFormat != PixelBlueGreenRedReserved8BitPerColor
    || gop->Mode->Information->HorizontalResolution < 640
    || gop->Mode->Information->VerticalResolution < 480) {
    gg->status = 0;
    printf("GOP failure.\n");
    getchar();
    return;
  }
  gg->status = 1;
  gg->buf = (U32*)gop->Mode->FrameBufferBase;
  gg->linesz = gop->Mode->Information->PixelsPerScanLine;
  gg->pal = malloc(sizeof(struct uefi_palitro));

}
U0 GGstop(lc_gg32* gg) {
}

U0 GGupload(LC* lc) {
  if (!lc->gg.status) return;
  for (U32 i = 0; i < VGASIZE; i++) {
    lc->gg.buf[(i % WINW) + (i / WINW) * lc->gg.linesz]
    = *(U32*)(lc->gg.pal->colors + lc->mem[0x400000 + i]);
  }
}
#endif

U0 GGflush(LC* lc) {
  U8 byte = lc->mem[0x450000];
  //U16 palitro = (lc->mem[0x4A0000+2*byte]) + (lc->mem[0x4A0001+2*byte] << 8);
  byte = lc->mem[0x450000];
  // SDL_SetRenderDrawColor(lc->renderer,
  //   ((palitro&0b0111110000000000)>>10)*8, // R
  //   ((palitro&0b0000001111100000)>>5)*8,  // G
  //   ((palitro&0b0000000000011111))*8,     // B
  //   0xFF);
  memset(lc->mem+0x400000, byte, VGASIZE);
  // SDL_RenderPresent(lc->renderer);
}

U0 GGpage_CGA16(LC* lc) {
  U16 i;
  for (i = 0; i < 256; i++) {
    lc->gg.pal->colors[i].r = rgbv[i%16].r;
    lc->gg.pal->colors[i].g = rgbv[i%16].g;
    lc->gg.pal->colors[i].b = rgbv[i%16].b;
    lc->gg.pal->colors[i].a = 0xFF;
  }
  GGupload(lc);
}

U0 GGpage_RGB555LE(LC* lc) {
  U16 i;
  U16 palitro;
  for (i = 0; i < 256; i++) {
    palitro = (lc->mem[0x4A0000+2*i]) + (lc->mem[0x4A0001+2*i] << 8);
    lc->gg.pal->colors[i].r = ((palitro&0b0111110000000000)>>10)*8;
    lc->gg.pal->colors[i].g = ((palitro&0b0000001111100000)>>5)*8;
    lc->gg.pal->colors[i].b = ((palitro&0b0000000000011111))*8;
    lc->gg.pal->colors[i].a = 0xFF;
  }
  GGupload(lc);

}

U0 GGpage_text(LC* lc) {
  U8* b = lc->mem + 0x4F0000;
  U8* sb = lc->mem + 0x400000;
  U8 x, y, ch, col, fg, bg, r, c;
  U8* fontdata;
  for (y = 0; y < 60; y++) {
    for (x = 0; x < 80; x++) {
      ch = b[(y * 80 + x) * 2];
      col = b[(y * 80 + x) * 2 + 1];
      fg = col & 0x0F;
      bg = (col >> 4) & 0x0F;
      fontdata = lc->mem+0x4A1000+(ch*8);
      for (r = 0; r < 8; r++) for (c = 0; c < 8; c++)
        sb[(y * 8 + r) * 640 + (x * 8 + c)] = ((fontdata[r] >> (7 - c)) & 1) ? fg : bg;
    }
  }
  GGpage_RGB555LE(lc);
}

U0 GGpage_text816(LC* lc) {
  U8* b = lc->mem + 0x004F0000;
  U8* sb = lc->mem + 0x00400000;
  U8 x, y, ch, col, fg, bg, r, c;
  U8* fontdata;
  for (y = 0; y < 30; y++) {
    for (x = 0; x < 80; x++) {
      ch = b[(y * 80 + x) * 2];
      col = b[(y * 80 + x) * 2 + 1];
      fg = col & 0x0F;
      bg = (col >> 4) & 0x0F;
      fontdata = lc->mem+0x4A1800+(ch*16);
      for (r = 0; r < 16; r++) for (c = 0; c < 8; c++)
        sb[(y * 16 + r) * 640 + (x * 8 + c)] = ((fontdata[r] >> (7 - c)) & 1) ? fg : bg;
    }
  }
  GGpage_RGB555LE(lc);
}

U0 (*GGPAGE[4])(LC*) = {&GGpage_CGA16, &GGpage_RGB555LE, &GGpage_text, &GGpage_text816};
U0 GGpage(LC* lc) {
  GGPAGE[lc->mem[0x49FF00]%4](lc);
}

// PPU functions
U0 GGsprite_256(LC* lc) {
  U32 to = lc->reg[0x04].word;   // ESI
  U32 from = lc->reg[0x05].word; // EGI
  U8 x,y;
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      if ((lc->mem[from+(y*8)+x])) lc->mem[to+(y*640)+x] = (lc->mem[from+(y*8)+x]);
    }
  }
}

U0 GGsprite_read_256(LC* lc) {
  U32 to = lc->reg[0x05].word;   // EGI
  U32 from = lc->reg[0x04].word; // ESI
  U8 x,y;
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      lc->mem[to+(y*8)+x] = lc->mem[from+(y*640)+x];
    }
  }
}

U0 GGsprite_mono(LC* lc) {
  U32 to = lc->reg[0x04].word;   // ESI
  U32 from = lc->reg[0x05].word; // EGI
  U8 color = lc->reg[0x00].word; // EAX
  U8 x,y;
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      lc->mem[to+(y*640)+x] = color*((lc->mem[from+y] & (1<<(7-x))) >> (7-x));
    }
  }
}

U0 GGsprite1624_mono(LC* lc) {
  U32 to = lc->reg[0x04].word;   // ESI
  U32 from = lc->reg[0x05].word; // EGI
  U8 color = lc->reg[0x00].word; // EAX
  U8 x,y;
  for (y = 0; y < 24; y++) {
    for (x = 0; x < 16; x++) {
      lc->mem[to+(y*640)+x] = color*((((lc->mem[from+y*2]<<8)|lc->mem[from+y*2+1]) & (1<<(15-x))) >> (15-x));
    }
  }
}

U0 GGsprite810_mono(LC* lc) {
  U32 to = lc->reg[0x04].word;   // ESI
  U32 from = lc->reg[0x05].word; // EGI
  U8 color = lc->reg[0x00].word; // EAX
  U8 x,y;
  for (y = 0; y < 10; y++) {
    for (x = 0; x < 8; x++) {
      if ((lc->mem[from+y] & (1<<(7-x)))) lc->mem[to+(y*640)+x] = color*((lc->mem[from+y] & (1<<(7-x))) >> (7-x));
    }
  }
}

