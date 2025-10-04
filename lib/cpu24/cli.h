#ifndef CLI_H
#define CLI_H 1

// The CLI for using the emulator
#ifndef EFIKATOR
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#endif

#include <cpu24/gpuh.h>
#ifndef EFIKATOR
#include <cpu24/spuh.h>
#endif
#include <cpu24/cpu24h.h>
#include <cpu24/gpu.h>
#ifndef EFIKATOR
#include <cpu24/gpu3d.h>
#endif
#define LC24_VERSION "3.0"
#define EXEC_START 1

#ifndef EFIKATOR
unsigned long long perf_counter_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (unsigned long long)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000ULL;
}
#endif

typedef struct LC24 LC;
char* regf[16] = {
  "AC", "BS", "CN", "DC", "DT", "DI", "SP", "BP",
  "EX", "FX", "HX", "LX", "X ", "Y ", "IX", "IY",
};

U0 cli_DisplayReg(LC* lc) {
  for (U8 i = 0; i < 16; i++) {
    if ((i != 0) && !(i%4)) putchar(10);
    printf("%s:\033[93m$%06X\033[0m  ", regf[i], lc->reg[i]);
  }
  printf("\nPS:\033[93m%08b\033[0m ", lc->PS);
  printf("PC:\033[93m$%06X\033[0m  ", lc->PC);
  printf("\n   -I---ZNC\n");
}

U8 putmc(U8 c) {
  switch (c) {
  case 0x00 ... 0x1F:
    putchar('.');
    break;
  case 0x20 ... 0x7E:
    putchar(c);
    break;
  case 0x80 ... 0xFF:
    putchar('~');
    break;
  }
  return 0;
}

U8 cli_DisplayMem(LC* lc, U8 page) {
  printf("\033[A\n");
  for (U32 i = page*256; i < page*256+256; i++) {
    if (!(i % 16)) {
      printf("\n%04X  ", i);
    }
    printf("%02X ", lc->mem[i]);
  }
  putchar(10);
  return 0;
}

U8 cli_DisplayMemX(LC* lc, U8 page) {
  printf("\033[A\n");
  for (U32 i = page*256; i < page*256+256; i++) {
    if (!(i % 16)) {
      printf("\n%04X  ", i);
    }
    putmc(lc->mem[i]);
  }
  putchar(10);
  return 0;
}

U8 cli_InsertMem(LC* lc, U16 addr, U8 byte) {
  lc->mem[addr] = byte;
  return 0;
}

#ifndef EFIKATOR
int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
#endif

U8 ExecD(LC* lc, U8 trapped) {
  char* inpt;
  char* tokens[10];
  size_t bufsize = 25;
  uint8_t j = 0;
  char* buf = (char*)malloc(bufsize);

  if (trapped) printf("\n\033[91mlost\033[0m at PC$%06X\n", lc->PC);
  else printf("lc24 emu %s\n", LC24_VERSION);

  execloop:
  printf(": ");
#ifdef EFIKATOR
  buf[0] = ':';
  buf[1] = 0;
#else
  getline(&buf, &bufsize, stdin);
#endif
  j = 0;
  tokens[j] = strtok(buf, " ");
  while (tokens[j] != NULL) {
    tokens[++j] = strtok(NULL, " ");
  }
  switch ((*tokens)[0]) {
  case 'q':
    if (trapped) exit(0);
    return 0;
  case 'R':
    return EXEC_START;
  case 'r':
    cli_DisplayReg(lc);
    break;
  case 'c':
    printf("\033[H\033[2J");
    break;
  case 'm':
    if (j == 2)
      cli_DisplayMem(lc, strtol(tokens[1], NULL, 16));
    break;
  case 'M':
    if (j == 2)
      cli_DisplayMemX(lc, strtol(tokens[1], NULL, 16));
    break;
#ifndef EFIKATOR
  case 'i':
    if (j == 3)
      cli_InsertMem(lc, strtol(tokens[1], NULL, 16), strtol(tokens[2], NULL, 16));
    break;
    // ТЕСТЫ ПИСАЛА НЕЙРОНКА, КУБИК НЕ РАБОТАЕТ!!!!!!!!
  case 't': {
    printf("--- Cube render test: manual vs Display List ---\n\n");
    // Координаты вершин куба
    float vertices[8][3] = {
        {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f},
        {0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f},
        {-0.5f, -0.5f,  0.5f}, {0.5f, -0.5f,  0.5f},
        {0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}
    };
    int faces[6][4] = {
        {0, 1, 2, 3}, // back
        {4, 5, 6, 7}, // front
        {0, 1, 5, 4}, // bottom
        {2, 3, 7, 6}, // top
        {0, 4, 7, 3}, // left
        {1, 5, 6, 2}  // right
    };
    U8 colors[6][3] = {
        {255,0,0}, {0,255,0}, {0,0,255}, {255,255,0}, {0,255,255}, {255,0,255}
    };

    // PROJECTION: Стартовая ортографическая
    lc3D_MatrixMode(LC3D_PROJECTION);
    lc3D_LoadIdentity();
    lc3D_Ortho(-1.8f, 1.8f, -1.8f, 1.8f, -3.0f, 3.0f);
    lc3D_MatrixMode(LC3D_MODELVIEW);
    lc3D_Enable(LC3D_DEPTH_TEST);

    unsigned long long t0 = perf_counter_us();
    float angle = 0.0f; int frame = 0;
    printf("[immediate] Rendering 180 frames. Press key to skip...\n");
    while (frame++ < 180 && !kbhit()) {
        lc3D_Clear(lc, LC3D_COLOR_BUFFER_BIT | LC3D_DEPTH_BUFFER_BIT);
        lc3D_LoadIdentity();
        lc3D_Rotatef(angle, 1.0f, 1.0f, 0.0f);

        for (int f = 0; f < 6; f++) {
            lc3D_Begin(lc, LC3D_MODE_QUADS);
            lc3D_Color4ub(lc, colors[f][0], colors[f][1], colors[f][2], 255);
            for (int v = 0; v < 4; v++) {
                int idx = faces[f][v];
                lc3D_Vertex3f(lc, vertices[idx][0], vertices[idx][1], vertices[idx][2]);
            }
            lc3D_End(lc);
        }
        angle += 2.0f;
    }
    unsigned long long t1 = perf_counter_us();
    printf("[immediate] %d frames: %llu us\n", frame-1, t1-t0);

    // --- Теперь Display List ---
    printf("[Display List] Recording cube...\n");
    int list_id = lc3D_GenLists(1);
    lc3D_NewList(list_id, LC3D_COMPILE);
    for (int f = 0; f < 6; f++) {
        lc3D_Begin(lc, LC3D_MODE_QUADS);
        lc3D_Color4ub(lc, colors[f][0], colors[f][1], colors[f][2], 255);
        for (int v = 0; v < 4; v++) {
            int idx = faces[f][v];
            lc3D_Vertex3f(lc, vertices[idx][0], vertices[idx][1], vertices[idx][2]);
        }
        lc3D_End(lc);
    }
    lc3D_EndList(lc);

    printf("[Display List] Rendering 180 frames. Press key to skip...\n");
    angle = 0; frame = 0;
    unsigned long long t2 = perf_counter_us();
    while (frame++ < 180 && !kbhit()) {
        lc3D_Clear(lc, LC3D_COLOR_BUFFER_BIT | LC3D_DEPTH_BUFFER_BIT);
        lc3D_LoadIdentity();
        lc3D_Rotatef(angle, 1.0f, 1.0f, 0.0f);
        lc3D_CallList(lc, list_id);
        angle += 2.0f;
    }
    unsigned long long t3 = perf_counter_us();
    printf("[Display List] %d frames: %llu us\n", frame-1, t3-t2);

    getchar();
    break;
}
case 'a': {
    printf("--- Assembly Triangle Test (C version) ---\n\n");
    
    // Setup как в ассемблере
    lc3D_MatrixMode(LC3D_PROJECTION);
    lc3D_LoadIdentity();
    lc3D_Ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    lc3D_MatrixMode(LC3D_MODELVIEW);
    lc3D_Enable(LC3D_DEPTH_TEST);
    lc3D_ClearColor(0, 0, 0);
    
    float angle = 0.0f;
    int frame = 0;
    
    printf("Choose triangle type:\n");
    printf("  1 - Right-angled (current assembly version)\n");
    printf("  2 - Equilateral (fixed version)\n");
    printf("Choice: ");
    
    char choice;
    scanf(" %c", &choice);
    
    float v1_x = 0.0f,    v1_y = 0.75f,   v1_z = 0.0f;
    float v2_x, v2_y = -0.75f, v2_z = 0.0f;
    float v3_x, v3_y, v3_z = 0.0f;
    
    if (choice == '2') {
        // Equilateral triangle
        v2_x = -0.6495f;
        v2_y = -0.375f;
        v3_x = 0.6495f;
        v3_y = -0.375f;
        printf("[Equilateral Triangle] All sides equal\n");
    } else {
        // Right-angled triangle (original)
        v2_x = 0.0f;
        v2_y = -0.75f;
        v3_x = 0.75f;
        v3_y = -0.75f;
        printf("[Right-Angled Triangle] Like current assembly\n");
    }
    
    printf("Rendering rotating triangle. Press key to exit...\n");
    
    while (frame++ < 1) {
        lc3D_Clear(lc, LC3D_COLOR_BUFFER_BIT | LC3D_DEPTH_BUFFER_BIT);
        lc3D_LoadIdentity();
        lc3D_Rotatef(angle, 0.0f, 1.0f, 0.0f);
        
        lc3D_Begin(lc, LC3D_MODE_TRIANGLES);
        
        lc3D_Color4ub(lc, 255, 0, 0, 255);
        lc3D_Vertex3f(lc, v1_x, v1_y, v1_z);
        
        lc3D_Color4ub(lc, 0, 255, 0, 255);
        lc3D_Vertex3f(lc, v2_x, v2_y, v2_z);
        
        lc3D_Color4ub(lc, 0, 0, 255, 255);
        lc3D_Vertex3f(lc, v3_x, v3_y, v3_z);
        
        lc3D_End(lc);
        
        angle += 2.0f;
        usleep(16000);
    }
    
    printf("Rendered %d frames\n", frame-1);
    getchar();
    break;
}
#endif
  case 'h':
    printf("lc24 cli help:\n");
    printf("  c       Clear the screen\n");
    printf("  h       Show help\n");
    printf("  m <00>  Dump memory\n");
    printf("  r       Dump registers\n");
    printf("  R       Run the program\n");
    printf("  t       Run 3D pipeline test\n");
    printf("  q       Quit\n");
    break;
  default:
    printf("unknown command\n");
  }
  goto execloop;
  return 0;
}

#endif // CLI_H
