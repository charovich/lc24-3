// The CLI for using the emulator

#include <cpu24/gpuh.h>
#include <cpu24/spuh.h>
#include <cpu24/cpu24h.h>
#define LC24_VERSION "3.0"
#define EXEC_START 1

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
  buf[0] = 'R';
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
  case 'i':
    if (j == 3)
      cli_InsertMem(lc, strtol(tokens[1], NULL, 16), strtol(tokens[2], NULL, 16));
    break;
  case 'h':
    printf("lc24 cli help:\n");
    printf("  c       Clear the screen\n");
    printf("  h       Show help\n");
    printf("  m <00>  Dump memory\n");
    printf("  r       Dump registers\n");
    printf("  R       Run the program\n");
    printf("  q       Quit\n");
    break;
  default:
    printf("unknown command\n");
  }
  goto execloop;
  return 0;
}
