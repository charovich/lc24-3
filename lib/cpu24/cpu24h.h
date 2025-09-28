// Header include file for lib/cpu/cpu16.h
#ifndef CPU16H_H
#define CPU16H_H 1

#define ROMSIZE 16777216 // Maximum for a 16-bit adress line
#define MEMSIZE 16777216 // Maximum for a 16-bit adress line

union lcreg {
  U32 word : 24;
};
typedef union lcreg lcreg;

// Register cluster
struct lcrc {
  lcbyte x;
  lcbyte y;
};
typedef struct lcrc lcrc_t;

struct LC24 {
  lcreg reg[16];

  lcbyte PS;   // -I---ZNC                 Unaddressable
  uint32_t PC; // Program counter          Unaddressable

  // Memory and ROM
  lcbyte mem[MEMSIZE];
  lcbyte rom[ROMSIZE];
  lcbyte pin;

  U8 io_free;

  // GPU
  lc_gg32 gg;
#ifndef EFIKATOR
  // SPU
  lc_ga32 ga;
#endif
};
typedef struct LC24 LC;
void PlayBeep(double frequency);
lcrc_t ReadRegClust(U8 clust);

U0 WriteByte(LC* lc, U32 addr, U8 val);
U0 WriteWord(LC* lc, U32 addr, U16 val);
U0 Write24(LC* lc, U32 addr, U32 val);

#endif
