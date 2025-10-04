#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>
#include <poll.h>

#include <holyc-types.h>
#include <lc24-types.h>
#include <sterm-control.h>
#include <cpu24/bpf.h>
#include <cpu24/cli.h>
#include <cpu24/cpu24.h>

U8 usage() {
  puts("lc24: a Lost Core 24 emulator");
  puts("Syntax:");
  puts("  lc24        [file]  Load the file directly to memory");
  puts("  lc24 bios   [file]  Load the file as BIOS (beta)");
  puts("  lc24 cli    [file]  Start the emulator in CLI mode");
  puts("  lc24 disk   [file]  Load the file as ROM");
  puts("  lc24 help           Show help");
  return 0;
}

/* loadBootSector -- loads a block of data from one pointer to
   another, until it finds an $AA$55 */
U8 loadBootSector(U8* drive, U8* mem, U32 start, U32 to) {
  U32 oto = to;
  while (1) {
    if ((*(drive+start) == 0xAA) && (*(drive+start+1) == 0x55)) break;
    *(mem+to) = *(drive+start);
    to++;
    start++;
  }
  printf("lc24: read %d bytes from ROM\n", to-oto+1);
  return 0;
}

U8 OLDloadBootSector(U8* drive, U8* mem) {
  U8* odrive = drive;
  while (1) {
    if ((*(drive+0x91EE) == 0xAA) && (*(drive+0x91EF) == 0x55)) break;
    *(mem++) = *(drive+0x91EE);
    drive++;
  }
  return 0;
}


U8 main(I32 argc, I8** argv) {
  srand(time(NULL));
  new_st;
  U16 driveboot;
  U8 climode = 0;
  U8 argp = 1; // 256 arguments is enough for everyone
  U8 scale = 1;
  U8* filename = NULL;
  U8* biosfile = NULL;

  driveboot = 0x000000;
  parseArgs:
  if (argc == 1) {
    old_st;
    usage();
    fprintf(stderr, "lc24: fatal error: no arguments given\n");
    return 1;
  }
  while (argp < argc) {
    // Load from the disk
    if ((!strcmp(argv[argp], "disk")) || (!strcmp(argv[argp], "-d")) || (!strcmp(argv[argp], "--disk"))) {
      driveboot = 0x0091EE;
      argp++;
    }
    else if ((!strcmp(argv[argp], "bios")) || (!strcmp(argv[argp], "-b")) || (!strcmp(argv[argp], "--bios"))) {
      biosfile = argv[argp+1];
      argp += 2;
    }
    else if ((!strcmp(argv[argp], "cli")) || (!strcmp(argv[argp], "-c")) || (!strcmp(argv[argp], "--cli"))) {
      climode = 1;
      argp++;
    }
    else if ((!strcmp(argv[argp], "help")) || (!strcmp(argv[argp], "-h")) || (!strcmp(argv[argp], "--help"))) {
      old_st;
      usage();
      exit(1);
    }
    else {
      filename = argv[argp];
      break;
    }
  }

  U8 biosbuf[BIOSNOBNK * BANKSIZE];
  biosbuf[0x000000] = 0xAA;
  biosbuf[0x000001] = 0x55;
  // Create a virtual CPU
  LC* lc = malloc(sizeof(LC));
  // lc->pin = 0b00000000; // Reset the pin
  Reset(lc);

  if (!driveboot) { // Load a memory dump
    FILE* fl = fopen(filename, "rb");
    if (fl == NULL) {
      fprintf(stderr, "\033[31mError\033[0m while opening %s\n", filename);
      old_st;
      return 1;
    }
    fread(lc->mem, 1, 16777216, fl);
    fclose(fl);
    // Disk signaures for LostFS (without them, fs drivers would not work)
    lc->rom[0x00] = 0x63;
    lc->rom[0x11] = '#';
    lc->rom[0x21] = 0xF7;
    lc->pin &= 0b01111111;
  }
  else { // Load a disk
    FILE* fl = fopen(filename, "rb");
    if (fl == NULL) {
      fprintf(stderr, "\033[31mError\033[0m while opening %s\n", filename);
      old_st;
      return 1;
    }
    fread(lc->rom, 1, ROMSIZE, fl);
    fclose(fl);
    // Load the boot sector from $C00000 into RAM ($030000)
    OLDloadBootSector(lc->rom, lc->mem);
    // Setup the pin bit 7 to 1 (drive)
    lc->pin |= 0b10000000;
  }
  if (biosfile != NULL) { // BIOS provided
    FILE* fl = fopen(biosfile, "rb");
    if (fl == NULL) {
      fprintf(stderr, "\033[31mError\033[0m while opening %s\n", biosfile);
      old_st;
      return 1;
    }
    fread(biosbuf, 1, BIOSNOBNK*BANKSIZE, fl);
    fclose(fl);
    loadBootSector(biosbuf, lc->mem, 0x000000, 0x700000);
  }

  // GPU
  GGinit(lc->mem, &(lc->gg), scale);
#ifndef EFIKATOR
  lc3D_Init(lc);
#endif
  GAinit(&(lc->ga));

  int runcode = 0xFF;
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  if (climode) runcode = ExecD(lc, 0);
  if (runcode == 0) {
    return 0;
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  U8 exec_errno = Exec(lc, 0);
  GGstop(&(lc->gg));
  old_st;
  if (driveboot) { // Save the modified disk back
    FILE* fl = fopen(argv[2], "wb");
    if (fl == NULL) {
      fprintf(stderr, "\033[31mError\033[0m while opening %s\n", filename);
      old_st;
      return 1;
    }
    fwrite(lc->rom, 1, 16777216, fl);
    fclose(fl);
  }
  return exec_errno;
}
