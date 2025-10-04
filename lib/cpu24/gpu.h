
#ifndef GPU_H
#define GPU_H 1

// GPU identificator: GovnGraphics 69-4932
#include <cpu24/gpuh.h>
#include <cpu24/gpufont.h>

#define LOG_FPS 1
static U64 frame_count = 0;
static U64 last_fps_time = 0;
static float current_fps = 0.0f;

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
  gg->win = SDL_CreateWindow("Lost NEWVGA2", 640, 480, 0);
  SDL_SetWindowResizable(gg->win, false);
  
  gg->rndr = SDL_CreateRenderer(gg->win, NULL);
  SDL_SetRenderVSync(gg->rndr, 0);
  SDL_SetRenderLogicalPresentation(gg->rndr, 640, 480, 
    SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
  SDL_SetRenderVSync(gg->rndr, 1);
  gg->tex = SDL_CreateTexture(
    gg->rndr,
    SDL_PIXELFORMAT_RGBA8888,
    SDL_TEXTUREACCESS_STREAMING,
    WINW, WINH
  );
  gg->pal = SDL_CreatePalette(256);
  gg->status = 0b00000000;
  SDL_HideCursor();
}

U0 GGstop(lc_gg32* gg) {
  SDL_DestroyPalette(gg->pal);
  SDL_DestroyTexture(gg->tex);
  SDL_DestroyRenderer(gg->rndr);
  SDL_DestroyWindow(gg->win);
  SDL_Quit();
}

U0 GGupload(LC* lc) {
  void* pixels;
  int pitch;
  
  SDL_LockTexture(lc->gg.tex, NULL, &pixels, &pitch);
  if (pitch == WINW * 4) {
    U8* src = lc->mem + 0x400000;
    U32* dst = (U32*)pixels;
    
    for (int i = 0; i < VGASIZE; i++) {
      dst[i] = lc->gg.pal_cache[src[i]];
    }
  } else {
    for (int y = 0; y < WINH; y++) {
      U32* row = (U32*)((U8*)pixels + y * pitch);
      U8* src = lc->mem + 0x400000 + y * WINW;
      
      for (int x = 0; x < WINW; x++) {
        row[x] = lc->gg.pal_cache[src[x]];
      }
    }
  }
  
  SDL_UnlockTexture(lc->gg.tex);
  
  if (LOG_FPS) {
    frame_count++;
    U64 current_time = SDL_GetTicks();

    if (current_time - last_fps_time >= 1000) {
        current_fps = frame_count * 1000.0f / (current_time - last_fps_time);
        
        U64 freq = SDL_GetPerformanceFrequency();
        
        char title[128];
        snprintf(title, sizeof(title), 
            "Lost NEWVGA2 - %.1f FPS", 
            current_fps);
        SDL_SetWindowTitle(lc->gg.win, title);
        
        frame_count = 0;
        last_fps_time = current_time;
    }
  }
  SDL_RenderClear(lc->gg.rndr);
  SDL_RenderTexture(lc->gg.rndr, lc->gg.tex, NULL, NULL);
  SDL_RenderPresent(lc->gg.rndr);
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
  const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888);
  
  for (i = 0; i < 256; i++) {
    lc->gg.pal->colors[i].r = rgbv[i%16].r;
    lc->gg.pal->colors[i].g = rgbv[i%16].g;
    lc->gg.pal->colors[i].b = rgbv[i%16].b;
    lc->gg.pal->colors[i].a = 0xFF;
    
    SDL_Color col = lc->gg.pal->colors[i];
    lc->gg.pal_cache[i] = SDL_MapRGBA(format, NULL, col.r, col.g, col.b, 255);
  }
  GGupload(lc);
}

U0 GGpage_RGB555LE(LC* lc) {
  U16 i;
  U16 palitro;
  
  const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888);
  
  for (i = 0; i < 256; i++) {
    palitro = (lc->mem[0x4A0000+2*i]) + (lc->mem[0x4A0001+2*i] << 8);
    lc->gg.pal->colors[i].r = ((palitro&0b0111110000000000)>>10)*8;
    lc->gg.pal->colors[i].g = ((palitro&0b0000001111100000)>>5)*8;
    lc->gg.pal->colors[i].b = ((palitro&0b0000000000011111))*8;
    lc->gg.pal->colors[i].a = 0xFF;
    
    SDL_Color col = lc->gg.pal->colors[i];
    lc->gg.pal_cache[i] = SDL_MapRGBA(format, NULL, col.r, col.g, col.b, 255);
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

#endif // GPU_H