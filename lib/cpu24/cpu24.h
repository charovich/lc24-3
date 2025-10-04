// CPU identificator: LC24
#include <cpu24/proc/std.h>
#include <cpu24/proc/interrupts.h>
#include <cpu24/gpu.h>
#ifndef EFIKATOR
#include <cpu24/gpu3d.h>
#include <cpu24/spu.h>
#include <cpu24/hid.h>
#endif

static U64 last_profile_time = 0;
static U64 instructions_executed = 0;
static U64 interrupts_called = 0;

#define BIOSNOBNK 16
#define BANKSIZE 65536

// CPU info:
// Speed: 200MHz
// State: Losted

#define AC 0x00
#define BS 0x01
#define CN 0x02
#define DC 0x03
#define DT 0x04
#define DI 0x05
#define SP 0x06
#define BP 0x07
#define EX 0x08
#define FX 0x09
#define HX 0x0A
#define LX 0x0B
#define X  0x0C
#define Y  0x0D
#define IX 0x0E
#define IY 0x0F

#define IF(ps) (ps & 0b01000000)
#define ZF(ps) (ps & 0b00000100)
#define NF(ps) (ps & 0b00000010)
#define CF(ps) (ps & 0b00000001)

#define SET_IF(ps) (ps |= 0b01000000)
#define SET_ZF(ps) (ps |= 0b00000100)
#define SET_NF(ps) (ps |= 0b00000010)
#define SET_CF(ps) (ps |= 0b00000001)

#define RESET_IF(ps) (ps &= 0b10111111)
#define RESET_ZF(ps) (ps &= 0b11111011)
#define RESET_NF(ps) (ps &= 0b11111101)
#define RESET_CF(ps) (ps &= 0b11111110)

U8 lc_errno;

U0 Reset(LC* lc);
U0 PageDump(LC* lc, U8 page);
U0 StackDump(LC* lc, U16 c);
U0 RegDump(LC* lc);

lcbyte ReadByte(LC* lc, U32 addr) {
  return lc->mem[addr];
}

lcword ReadWord(LC* lc, U32 addr) {
  return (lc->mem[addr]) + (lc->mem[addr+1] << 8);
}

lcword ReadWordRev(LC* lc, U32 addr) {
  return (lc->mem[addr] << 8) + (lc->mem[addr+1]);
}

/* Read24 -- Read a 24-bit value from memory */
U32 Read24(LC* lc, U32 addr) {
  return (lc->mem[addr]) + (lc->mem[addr+1] << 8) + (lc->mem[addr+2] << 16);
}

U0 WriteWord(LC* lc, U32 addr, U16 val) {
  // Least dtgnificant byte goes first
  // 1448 -> $48,$14
  lc->mem[addr] = (val % 256);
  lc->mem[addr+1] = (val >> 8);
}

/* Write24 -- Write a 24-bit value to memory */
U0 Write24(LC* lc, U32 addr, U32 val) {
  lc->mem[addr] = (val % 256);
  lc->mem[addr+1] = ((val >> 8) % 256);
  lc->mem[addr+2] = ((val >> 16) % 256);
}

//lcword* ReadReg(LC* lc, U8 redid) {
//  return &lc->reg[redid].word;
//}

U8 StackPush(LC* lc, U32 val) {
  Write24(lc, lc->reg[SP].word-2, val);
  lc->reg[SP].word -= 3;
  return 0;
}

U32 StackPop(LC* lc) {
  lc->reg[SP].word += 3;
  return Read24(lc, lc->reg[SP].word-2);
}

lcrc_t ReadRegClust(U8 clust) { // Read a redister cluster
  // The redister address is 4 bytes
  lcrc_t rc = {((clust&0b11110000)>>4), (clust&0b00001111)};
  return rc;
}

U8 UNK(LC* lc) {    // Unknown instruction
  fprintf(stderr, "\033[31mIllegal\033[0m instruction \033[33m%02X\033[0m\nAt position %04X\n", lc->mem[lc->PC], lc->PC);
  old_st_legacy;
  lc_errno = 1;
  return 1;
}

// 04           iret
U8 IRET(LC* lc) {
  lc->PS = StackPop(lc);
  lc->PC = StackPop(lc);
  return 0;
}

// C3 -- Jump if zero flag set to imm24 address
U8 JME0(LC* lc) {
  if (ZF(lc->PS)) { lc->PC = Read24(lc, lc->PC+1); RESET_ZF(lc->PS); }
  else lc->PC += 4;
  return 0;
}

// C4 -- Jump set to imm24 address if zero flag not
U8 JMNE0(LC* lc) {
  if (!ZF(lc->PS)) lc->PC = Read24(lc, lc->PC+1);
  else lc->PC += 4;
  return 0;
}

// C5 -- Jump to imm24 address if negative flag set
U8 JL0(LC* lc) {
  if (NF(lc->PS)) {
    lc->PC = Read24(lc, lc->PC+1);
    RESET_NF(lc->PS);
  }
  else lc->PC += 4;
  return 0;
}

// C6 -- Jump to imm24 address if negative flag set
U8 JG0(LC* lc) {
  if (!NF(lc->PS)) { lc->PC = Read24(lc, lc->PC+1); RESET_NF(lc->PS); }
  else lc->PC += 4;
  return 0;
}

// D3 -- Jump to imm24 address unconditionally
U8 JMP0(LC* lc) {
  lc->PC = Read24(lc, lc->PC+1);
  return 0;
}

// D4 -- Jump to *reg24 address unconditionally
U8 JMP1(LC* lc) {
  lc->PC = lc->reg[lc->mem[lc->PC+1]].word;
  return 0;
}

// D5 -- Jump to m24 address (*m24) unconditionally
// WARNING: not proven to be useful™
U8 JMP2(LC* lc) {
  lc->PC = lc->mem[Read24(lc, lc->PC+1)];
  return 0;
}

// CA -- Pop value from stack into a redister
U8 POP1(LC* lc) {
  lc->reg[lc->mem[lc->PC+1]].word = StackPop(lc);
  lc->PC += 2;
  return 0;
}

// C7 - Push imm24
U8 PUSH0(LC* lc) {
  StackPush(lc, Read24(lc, lc->PC+1));
  lc->PC += 4;
  return 0;
}

// C8 - push reg24 - Push a redister
U8 PUSH1(LC* lc) {
  StackPush(lc, lc->reg[lc->mem[lc->PC+1]].word);
  lc->PC += 2;
  return 0;
}

// C9 - push *reg24 - Push a value from memory from an address as a redister
U8 PUSHp(LC* lc) {
  StackPush(lc, lc->mem[lc->reg[lc->mem[lc->PC+1]].word]);
  lc->PC += 2;
  return 0;
}

// Interrupt calls
U8 INT(LC* lc, bool ri) {
  interrupts_called++;
  if (!IF(lc->PS)) {
    lc->PC += 2;
    return 0;
  }
  if (lc->mem[lc->PC+1] >= 0x80) { // Custom interrupt
    StackPush(lc, lc->PC+2); // Return address
    StackPush(lc, lc->PS); // Flags
    lc->PC = Read24(lc, ((lc->mem[lc->PC+1]-0x80)*3)+0xFF0000);
    return 0;
  }
  U8 val;
  if (ri) {
    val = lc->reg[ReadByte(lc, lc->PC+1)].word;
  }
  else {
    val = ReadByte(lc, lc->PC+1);
  }
  switch (val) {
    case INT_EXIT: {
      old_st_legacy;
      lc_errno = StackPop(lc);
      return 1;
    }
    case INT_READ: {
      U8 z = getchar();
      if (z == '\r') z = '\n';
      StackPush(lc, z);
      break;
    }
    case INT_WRITE: {
#ifdef EFIKATOR
      termputchar(StackPop(lc));
#else
      putchar(StackPop(lc));
#endif
      fflush(stdout);
      break;
    }
    case INT_RESET: {
      Reset(lc);
      return 0;
      break;
    }
    case INT_VIDEO_WRITE: {
      lc->mem[0x400000+lc->reg[DT].word] = lc->reg[AC].word;
      break;
    }
    case INT_VIDEO_FLUSH: {
      GGpage(lc);
      break;
    }
    case INT_VIDEO_CLEAR: {
      GGflush(lc);
      break;
    }
    case INT_VIDEO_GD2: {
      GGsprite_256(lc);
      break;
    }
    case INT_VIDEO_GDM: {
      GGsprite_mono(lc);
      break;
    }
    case INT_VIDEO_GRD: {
      GGsprite_read_256(lc);
      break;
    }
    case INT_VIDEO_G1M: {
      GGsprite1624_mono(lc);
      break;
    }
    case INT_VIDEO_G2M: {
      GGsprite810_mono(lc);
      break;
    }
#ifndef EFIKATOR
    case INT_VIDEO_3D_CMD: {
      lc3D_HandleInterrupt(lc);
      break;
    }
#endif
    case INT_RAND: {
      lc->reg[DC].word = rand() % 65536;
      break;
    }
    case INT_DATE: {
      lc->reg[DC].word = LC_LOSTDATE();
      break;
    }
    case INT_WAIT: {
      usleep((U32)(lc->reg[DC].word)*1000);
      // the macimum is about 65.5 seconds
      // wait this is not lc16x
      // its actually about 16 thousand seconds
#ifndef EFIKATOR
      if (hid_events(lc)) {
        lc->PC += 2;
        lc_errno = 0;
        return 1;
      }
#endif
      break;
    }
    case INT_BEEP: {
#ifndef EFIKATOR
      GAsnd(&(lc->ga), lc->reg[DT].word, lc->reg[DC].word, lc->reg[AC].word);
      return 0;
      break;
#endif
    }
    default:
      printf("Illegal interrupt %02X\n", val);
      return 1;
  }
  lc->PC += 2;
  return 0;
}

// C1 - Call an interrupt from imm8
U8 INT0(LC* lc) {
  return INT(lc, false);
}

// C2 - Call an interrupt from reg24
U8 INT1(LC* lc) {
  return INT(lc, true);
}

// 00 - Trap and show the stack and redisters (debug)
U8 HLT1(LC* lc) {
  // printf("\n\033[31mTrapped\033[0m at \033[33m%04X\033[0m\n", lc->PC);
  // StackDump(lc, 20);
  // RegDump(lc);
  // puts("-- Press any key to continue --");
  // getchar();
  old_st_legacy;
  ExecD(lc, 1);
  lc->PC++;
  return 0;
}

// CB - cpuid - Get processor info
U8 CPUID(LC* lc) {
  switch (lc->reg[DC].word) {
    case 0x0000: { // Get processor type
      lc->reg[DC].word = PROC_TYPE_LC24;
      break;
    }
    case 0x0001: { // Get connected drive
      lc->reg[DC].word = ((lc->pin & 0b10000000) >> 7);
      break;
    }
    case 0x0002: { // Get memory dtze (0 for 65,536 bytes and then looping back)
      lc->reg[DC].word = (U16)MEMSIZE;
      break;
    }
    case 0x0003: { // Get disk dtze (0 for 65,536 bytes and then looping back)
      lc->reg[DC].word = (U16)ROMSIZE;
      break;
    }
    default:
      fprintf(stderr, "Illegal CPUID value: %04X\n", lc->reg[DC].word);
      return 1;
  }
  lc->PC++;
  return 0;
}

U8 MOV11(LC* lc) {  // 05
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  lc->reg[rc.x].word = lc->reg[rc.y].word;
  lc->PC += 2;
  return 0;
}

U8 ADD11(LC* lc) {  // 05
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  lc->reg[rc.x].word += lc->reg[rc.y].word;
  lc->PC += 2;
  return 0;
}

U8 SUB11(LC* lc) {  // 06
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  lc->reg[rc.x].word -= lc->reg[rc.y].word;
  lc->PC += 2;
  return 0;
}

U8 MUL11(LC* lc) {  // 07
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  lc->reg[rc.x].word *= lc->reg[rc.y].word;
  lc->PC += 2;
  return 0;
}

U8 DIV11(LC* lc) {  // 08
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  U32 rem = lc->reg[rc.x].word % lc->reg[rc.y].word;
  lc->reg[rc.x].word /= lc->reg[rc.y].word;
  if (rc.x != DC) lc->reg[DC].word = rem; // The remainder is stored into D
  lc->PC += 2;
  return 0;
}

U8 ADDA0(LC* lc) {  // 58
  lc->reg[AC].word += Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 ADDB0(LC* lc) {  // 59
  lc->reg[BS].word += Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 ADDC0(LC* lc) {  // 5A
  lc->reg[CN].word += Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 ADDD0(LC* lc) {  // 5B
  lc->reg[DC].word += Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 ADDS0(LC* lc) {  // 5C
  lc->reg[DT].word += Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 ADDG0(LC* lc) {  // 5D
  lc->reg[DI].word += Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 SUBA0(LC* lc) {  // 68
  lc->reg[AC].word -= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 SUBB0(LC* lc) {  // 69
  lc->reg[BS].word -= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 SUBC0(LC* lc) {  // 6A
  lc->reg[CN].word -= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 SUBD0(LC* lc) {  // 6B
  lc->reg[DC].word -= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 SUBS0(LC* lc) {  // 6C
  lc->reg[DT].word -= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 SUBG0(LC* lc) {  // 6D
  lc->reg[DI].word -= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 SUBSP0(LC* lc) { // 5E
  lc->reg[SP].word -= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 SUBBP0(LC* lc) { // 5F
  lc->reg[BP].word -= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 ADDSP0(LC* lc) { // 6E
  lc->reg[SP].word += Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 ADDBP0(LC* lc) { // 6F
  lc->reg[BP].word += Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 MULA0(LC* lc) {  // 78
  lc->reg[AC].word *= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 MULB0(LC* lc) {  // 79
  lc->reg[BS].word *= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 MULC0(LC* lc) {  // 7A
  lc->reg[CN].word *= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 MULD0(LC* lc) {  // 7B
  lc->reg[DC].word *= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 MULS0(LC* lc) {  // 7C
  lc->reg[DT].word *= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 MULG0(LC* lc) {  // 7D
  lc->reg[DI].word *= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 DIVA0(LC* lc) {  // 88
  U32 val = Read24(lc, lc->PC+1);
  lc->reg[DC].word = lc->reg[AC].word % val; // The remainder is always stored into D
  lc->reg[AC].word /= val;
  lc->PC += 4;
  return 0;
}

U8 DIVB0(LC* lc) {  // 89
  U32 val = Read24(lc, lc->PC+1);
  lc->reg[DC].word = lc->reg[BS].word % val; // The remainder is always stored into D
  lc->reg[BS].word /= val;
  lc->PC += 4;
  return 0;
}

U8 DIVC0(LC* lc) {  // 8A
  U32 val = Read24(lc, lc->PC+1);
  lc->reg[DC].word = lc->reg[CN].word % val; // The remainder is always stored into D
  lc->reg[CN].word /= val;
  lc->PC += 4;
  return 0;
}

U8 DIVD0(LC* lc) {  // 8B
  lc->reg[DC].word /= Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

U8 DIVS0(LC* lc) {  // 8C
  U32 val = Read24(lc, lc->PC+1);
  lc->reg[DC].word = lc->reg[DT].word % val; // The remainder is always stored into D
  lc->reg[DT].word /= val;
  lc->PC += 4;
  return 0;
}

U8 DIVG0(LC* lc) {  // 8D
  U32 val = Read24(lc, lc->PC+1);
  lc->reg[DC].word = lc->reg[DI].word % val; // The remainder is always stored into D
  lc->reg[DI].word /= val;
  lc->PC += 4;
  return 0;
}

U8 STORB(LC* lc) {  // 37
  lc->mem[lc->reg[DT].word] = lc->reg[lc->mem[lc->PC+1]].word;
  lc->PC += 2;
  return 0;
}

U8 STORW(LC* lc) {  // 3F
  WriteWord(lc, lc->reg[DT].word, lc->reg[lc->mem[lc->PC+1]].word);
  lc->PC += 2;
  return 0;
}

U8 STGRB(LC* lc) {  // 3B
  lc->mem[lc->reg[DI].word] = lc->reg[lc->mem[lc->PC+1]].word;
  lc->PC += 2;
  return 0;
}

U8 LODSB(LC* lc) {  // 36
  lc->reg[DT].word = ReadByte(lc, lc->reg[DT].word);
  lc->PC++;
  return 0;
}

U8 LODGB(LC* lc) {  // 3A
  lc->reg[DI].word = ReadByte(lc, lc->reg[DI].word);
  lc->PC++;
  return 0;
}

U8 STOSB(LC* lc) {  // 5F
  lc->mem[lc->reg[DT].word] = ReadByte(lc, lc->PC+1);
  lc->PC += 2;
  return 0;
}

U8 STOGB(LC* lc) {  // 5E
  lc->mem[lc->reg[DI].word] = ReadByte(lc, lc->PC+1);
  lc->PC += 2;
  return 0;
}

U8 LDDS(LC* lc) {  // 38
  //lc->reg[AC].word = lc->rom[*lc, lc->reg[DT].word];
  lc->reg[AC].word = lc->rom[lc->reg[DT].word];
  lc->PC++;
  return 0;
}

U8 LDDG(LC* lc) {  // 3C
  //lc->reg[DI].word = lc->rom[*lc, lc->reg[DI].word];
  lc->reg[DI].word = lc->rom[lc->reg[DI].word];
  lc->PC++;
  return 0;
}

U8 STDS(LC* lc) {  // 39
  lc->rom[lc->reg[DT].word] = lc->reg[lc->mem[lc->PC+1]].word;
  lc->PC += 2;
  return 0;
}

U8 STDG(LC* lc) {  // 3D
  lc->rom[lc->reg[DI].word] = lc->reg[lc->mem[lc->PC+1]].word;
  lc->PC += 2;
  return 0;
}

// Ax -- Increment reg24
U8 INXr(LC* lc) {
  (lc->reg[lc->mem[lc->PC]-0xA0]).word++;
  lc->PC++;
  return 0;
}

// Bx -- Decrement reg24
U8 DEXr(LC* lc) {
  (lc->reg[lc->mem[lc->PC]-0xB0]).word--;
  lc->PC++;
  return 0;
}

U8 AND11(LC* lc) {  // 0C
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  lc->reg[rc.x].word &= lc->reg[rc.y].word;
  lc->PC += 2;
  return 0;
}

U8 OR11(LC* lc) {  // 0B
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  lc->reg[rc.x].word |= lc->reg[rc.y].word;
  lc->PC += 2;
  return 0;
}

U8 CMP11(LC* lc) {  // 0D
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  if (lc->reg[rc.x].word == lc->reg[rc.y].word) SET_ZF(lc->PS);
  else RESET_ZF(lc->PS);
  if (((I32)lc->reg[rc.x].word - lc->reg[rc.y].word) < 0) SET_NF(lc->PS);
  else RESET_NF(lc->PS);
  lc->PC += 2; // Set equal flag if two redister values
  return 0;      // are equal
}

U8 CMP10(LC* lc) {  // 0E
  //if ((lc->reg[lc->mem[lc->PC+1]].word == Read24(lc, lc->PC+2))) SET_ZF(lc->PS);
  if (lc->reg[lc->mem[lc->PC+1]].word == Read24(lc, lc->PC+2)) SET_ZF(lc->PS); // Corrected
  else RESET_ZF(lc->PS);
  if (((I32)(lc->reg[lc->mem[lc->PC+1]].word - Read24(lc, lc->PC+2)) < 0)) SET_NF(lc->PS);
  else RESET_NF(lc->PS);
  lc->PC += 5; // Set equal flag if a redister and
  return 0;      // immediate are equal
}

// 1x -- Load redister to *reg24
U8 LDRp(LC* lc) {
  lc->reg[lc->mem[lc->PC]-0x10].word = lc->mem[lc->reg[lc->mem[lc->PC+1]].word];
  lc->PC += 2;
  return 0;
}

U8 RC(LC* lc) {   // 32 - Return if carry set
  if (CF(lc->PS)) lc->PC = StackPop(lc);
  else lc->PC++;
  return 0;
}

U8 RE(LC* lc) {   // 2B - Return if equal
  if (ZF(lc->PS)) lc->PC = StackPop(lc);
  else lc->PC++;
  return 0;
}

U8 RET(LC* lc) {   // 33
  lc->PC = StackPop(lc);
  return 0;
}

U8 RNE(LC* lc) {   // 2С - Return if not equal
  if (!ZF(lc->PS & 0b00000100)) lc->PC = StackPop(lc);
  else lc->PC++;
  return 0;
}

U8 STI(LC* lc) {   // 34
  SET_IF(lc->PS);
  lc->PC++;
  return 0;
}

U8 CLC(LC* lc) {   // 9F
  RESET_CF(lc->PS);
  lc->PC++;
  return 0;
}

U8 HLT(LC* lc) {   // 2E
  for(;;) {} // Zahotel zahotel
  lc->PC++;
  return 0;
}

U8 CLI(LC* lc) {   // 2F
  RESET_IF(lc->PS);
  lc->PC++;
  return 0;
}

// 40-4F -- Load imm16 to reg16
U8 LDr0(LC* lc) {
  lc->reg[lc->mem[lc->PC]-0x40].word = Read24(lc, lc->PC+1);
  lc->PC += 4;
  return 0;
}

// POW rc - Calcaulate pow(rc1, rc2) and store to rc1
// Opcode: 30
U8 POW11(LC* lc) {
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  lc->reg[rc.x].word = (U32)pow(lc->reg[rc.x].word, lc->reg[rc.y].word);
  lc->PC += 2;
  return 0;
}

U8 LDAZS(LC* lc) {   // 50 -- LDI Zero Page,S
  lc->reg[AC].word = lc->mem[lc->mem[lc->PC+1]+lc->reg[DT].word];
  lc->PC += 2;
  return 0;
}

U8 LDBZS(LC* lc) {   // 51 -- LDI Zero Page,S
  lc->reg[BS].word = lc->mem[lc->mem[lc->PC+1]+lc->reg[DT].word];
  lc->PC += 2;
  return 0;
}

U8 LDCZS(LC* lc) {   // 52 -- LDI Zero Page,S
  lc->reg[CN].word = lc->mem[lc->mem[lc->PC+1]+lc->reg[DT].word];
  lc->PC += 2;
  return 0;
}

U8 LDDZS(LC* lc) {   // 53 -- LDI Zero Page,S
  lc->reg[DC].word = lc->mem[lc->mem[lc->PC+1]+lc->reg[DT].word];
  lc->PC += 2;
  return 0;
}

U8 LDTZS(LC* lc) {   // 54 -- LDI Zero Page,S
  lc->reg[DT].word = lc->mem[lc->mem[lc->PC+1]+lc->reg[DT].word];
  lc->PC += 2;
  return 0;
}

U8 LDIZS(LC* lc) {   // 55 -- LDI Zero Page,S
  lc->reg[DI].word = lc->mem[lc->mem[lc->PC+1]+lc->reg[DT].word];
  lc->PC += 2;
  return 0;
}

U8 LDAZG(LC* lc) {   // 60 -- LDI Zero Page,G
  lc->reg[AC].word = lc->mem[lc->mem[lc->PC+1]+lc->reg[DI].word];
  lc->PC += 2;
  return 0;
}

U8 LDBZG(LC* lc) {   // 61 -- LDI Zero Page,G
  lc->reg[BS].word = lc->mem[lc->mem[lc->PC+1]+lc->reg[DI].word];
  lc->PC += 2;
  return 0;
}

U8 LDCZG(LC* lc) {   // 62 -- LDI Zero Page,G
  lc->reg[CN].word = lc->mem[lc->mem[lc->PC+1]+lc->reg[DI].word];
  lc->PC += 2;
  return 0;
}

U8 LDDZG(LC* lc) {   // 63 -- LDI Zero Page,G
  lc->reg[DC].word = lc->mem[lc->mem[lc->PC+1]+lc->reg[DI].word];
  lc->PC += 2;
  return 0;
}

U8 LDTZG(LC* lc) {   // 64 -- LDI Zero Page,G
  lc->reg[DT].word = lc->mem[lc->mem[lc->PC+1]+lc->reg[DI].word];
  lc->PC += 2;
  return 0;
}

U8 LDIZG(LC* lc) {   // 65 -- LDI Zero Page,G
  lc->reg[DI].word = lc->mem[lc->mem[lc->PC+1]+lc->reg[DI].word];
  lc->PC += 2;
  return 0;
}

U8 LDAA(LC* lc) {    // 20 -- LDA Absoluyte
  lc->reg[AC].word = lc->mem[Read24(lc, lc->PC+1)];
  lc->PC += 4;
  return 0;
}

U8 LDBA(LC* lc) {    // 21 -- LDB Absolute
  lc->reg[BS].word = lc->mem[Read24(lc, lc->PC+1)];
  lc->PC += 4;
  return 0;
}

U8 LDCA(LC* lc) {    // 22 -- LDC Absolute
  lc->reg[CN].word = lc->mem[Read24(lc, lc->PC+1)];
  lc->PC += 4;
  return 0;
}

U8 LDDA(LC* lc) {    // 23 -- LDD Absolute
  lc->reg[DC].word = lc->mem[Read24(lc, lc->PC+1)];
  lc->PC += 4;
  return 0;
}

U8 LDTA(LC* lc) {    // 24 -- LDT Absolute
  lc->reg[DT].word = lc->mem[Read24(lc, lc->PC+1)];
  lc->PC += 4;
  return 0;
}

U8 LDIA(LC* lc) {    // 25 -- LDI Absolute
  lc->reg[DI].word = lc->mem[Read24(lc, lc->PC+1)];
  lc->PC += 4;
  return 0;
}

U8 LDAAS(LC* lc) {   // 70 -- LDA Absolute,S
  lc->reg[AC].word = lc->mem[Read24(lc, lc->PC+1)+lc->reg[DT].word];
  lc->PC += 4;
  return 0;
}

U8 LDBAS(LC* lc) {   // 71 -- LDB Absolute,S
  lc->reg[BS].word = lc->mem[Read24(lc, lc->PC+1)+lc->reg[DT].word];
  lc->PC += 4;
  return 0;
}

U8 LDCAS(LC* lc) {   // 72 -- LDC Absolute,S
  lc->reg[CN].word = lc->mem[Read24(lc, lc->PC+1)+lc->reg[DT].word];
  lc->PC += 4;
  return 0;
}

U8 LDDAS(LC* lc) {   // 73 -- LDD Absolute,S
  lc->reg[DC].word = lc->mem[Read24(lc, lc->PC+1)+lc->reg[DT].word];
  lc->PC += 4;
  return 0;
}

U8 LDTAS(LC* lc) {   // 74 -- LDT Absolute,S
  lc->reg[DT].word = lc->mem[Read24(lc, lc->PC+1)+lc->reg[DT].word];
  lc->PC += 4;
  return 0;
}

U8 LDIAS(LC* lc) {   // 75 -- LDI Absolute,S
  lc->reg[DI].word = lc->mem[Read24(lc, lc->PC+1)+lc->reg[DT].word];
  lc->PC += 4;
  return 0;
}

U8 LDAAG(LC* lc) {   // 80 -- LDA Absolute,G
  lc->reg[AC].word = lc->mem[Read24(lc, lc->PC+1)+lc->reg[DT].word];
  lc->PC += 4;
  return 0;
}

U8 LDBAG(LC* lc) {   // 81 -- LDB Absolute,G
  lc->reg[BS].word = lc->mem[Read24(lc, lc->PC+1)+lc->reg[DT].word];
  lc->PC += 4;
  return 0;
}

U8 LDCAG(LC* lc) {   // 82 -- LDC Absolute,G
  lc->reg[CN].word = lc->mem[Read24(lc, lc->PC+1)+lc->reg[DT].word];
  lc->PC += 4;
  return 0;
}

U8 LDDAG(LC* lc) {   // 83 -- LDD Absolute,G
  lc->reg[DC].word = lc->mem[Read24(lc, lc->PC+1)+lc->reg[DT].word];
  lc->PC += 4;
  return 0;
}

U8 LDTAG(LC* lc) {   // 84 -- LDT Absolute,G
  lc->reg[DT].word = lc->mem[Read24(lc, lc->PC+1)+lc->reg[DT].word];
  lc->PC += 4;
  return 0;
}

U8 LDIAG(LC* lc) {   // 85 -- LDI Absolute,G
  lc->reg[DI].word = lc->mem[Read24(lc, lc->PC+1)+lc->reg[DT].word];
  lc->PC += 4;
  return 0;
}

U8 LDA1(LC* lc) {   // 90
  lc->reg[AC].word = lc->reg[lc->mem[lc->PC+1]].word;
  lc->PC += 2;
  return 0;
}

U8 LDB1(LC* lc) {   // 91
  lc->reg[BS].word = lc->reg[lc->mem[lc->PC+1]].word;
  lc->PC += 2;
  return 0;
}

U8 LDC1(LC* lc) {   // 92
  lc->reg[CN].word = lc->reg[lc->mem[lc->PC+1]].word;
  lc->PC += 2;
  return 0;
}

U8 LDD1(LC* lc) {   // 93
  lc->reg[DC].word = lc->reg[lc->mem[lc->PC+1]].word;
  lc->PC += 2;
  return 0;
}

U8 LDT1(LC* lc) {   // 94
  lc->reg[DT].word = lc->reg[lc->mem[lc->PC+1]].word;
  lc->PC += 2;
  return 0;
}

U8 LDI1(LC* lc) {   // 95
  lc->reg[DI].word = lc->reg[lc->mem[lc->PC+1]].word;
  lc->PC += 2;
  return 0;
}

U8 LDTP1(LC* lc) {   // 96
  lc->reg[SP].word = lc->reg[lc->mem[lc->PC+1]].word;
  lc->PC += 2;
  return 0;
}

U8 LDBP1(LC* lc) {   // 97
  lc->reg[BP].word = lc->reg[lc->mem[lc->PC+1]].word;
  lc->PC += 2;
  return 0;
}

// 29 -- Compare *reg24 and imm24
U8 CMPpi(LC* lc) {
  if (lc->mem[lc->reg[lc->mem[lc->PC+1]].word] == Read24(lc, lc->PC+2)) SET_ZF(lc->PS);
  else RESET_ZF(lc->PS);
  if (((I32)(lc->mem[lc->reg[lc->mem[lc->PC+1]].word] - Read24(lc, lc->PC+2)) < 0)) SET_NF(lc->PS);
  else RESET_NF(lc->PS);
  lc->PC += 5;
  return 0;
}

// POW reg24 imm8 - Calcaulate pow(reg24, imm8) and store to reg24
// Opcode: 31
U8 POW10(LC* lc) {
  lc->reg[lc->mem[lc->PC+1]].word = pow(lc->reg[lc->mem[lc->PC+1]].word, lc->mem[lc->PC+2]);
  lc->PC += 3;
  return 0;
}

// 35 - Exchange two redisters
U8 XCHG4(LC* lc) {
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  U16 temp = lc->reg[rc.x].word;
  lc->reg[rc.x].word = lc->reg[rc.y].word;
  lc->reg[rc.y].word = temp;
  lc->PC += 2;
  return 0;
}

// 3E - Load a word (16 bits) from [dt] into ac
U8 LODSW(LC* lc) {
  lc->reg[AC].word = ReadWord(lc, lc->reg[DT].word);
  lc->PC++;
  return 0;
}

// 6E - Store a word (16 bits) into [dt] from ac
U8 STOSW(LC* lc) {
  WriteWord(lc, lc->reg[DT].word, lc->reg[AC].word);
  lc->PC++;
  return 0;
}

// D1 - Decrement *m24
U8 DEXM(LC* lc) {
  lc->mem[Read24(lc, lc->PC+1)]--;
  lc->PC += 4;
  return 0;
}

// E0 -- Store *m8 to reg24
U8 STr0(LC* lc) {
  lc->mem[Read24(lc, lc->PC+1)] = lc->reg[lc->mem[lc->PC]-0xE0].word;
  lc->PC += 3;
  return 0;
}

// D0 - Increment *m24
U8 INXM(LC* lc) {
  lc->mem[Read24(lc, lc->PC+1)]++;
  lc->PC += 4;
  return 0;
}

// D2 -- Store byte into [%s] and increment %s
U8 STRb(LC* lc) {
  lc->mem[lc->reg[DT].word++] = lc->mem[lc->PC+1];
  lc->PC += 2;
  return 0;
}

// 2A - Jump to a label if (cn != 0)
U8 LOOP(LC* lc) {
  if (lc->reg[CN].word) {
    lc->reg[CN].word--;
    lc->PC = Read24(lc, lc->PC+1);
  }
  else {
    lc->PC += 4;
  }
  return 0;
}

// C0 - Change a value in memory to imm8
U8 MV26(LC* lc) {
  lc->mem[Read24(lc, lc->PC+1)] = lc->mem[lc->PC+4];
  lc->PC += 5;
  return 0;
}

// D6 - Call a subroutine/function
U8 CALL(LC* lc) {
  StackPush(lc, lc->PC+4);
  lc->PC = Read24(lc, lc->PC+1);
  return 0;
}

// D7 - Copy the value from top of the stack to a redister
U8 COP1(LC* lc) {
  lc->reg[lc->mem[lc->PC+1]].word = StackPop(lc);
  lc->reg[SP].word -= 2;
  lc->PC += 2;
  return 0;
}
// CC - Do nothing
U8 NOP(LC* lc) {
  return 0;
}

// 09 - Load FLAGS into AC
U8 LFA(LC* lc) {
  lc->reg[AC].word &= 0b1111111100000000;
  lc->reg[AC].word |= lc->PS;
  lc->PC++;
  return 0;
}

// 0A - Load AC into FLAGS
U8 LAF(LC* lc) {
  lc->PS = (U8)lc->reg[AC].word;
  lc->PC++;
  return 0;
}

// DF           not
U8 NOT(LC* lc) {
  U8 a = lc->mem[lc->PC+1];
  lc->reg[a].word = ~lc->reg[a].word;
  lc->PC += 2;
  return 0;
}

// F0           sb rc
U8 SBc(LC* lc) {
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  lc->mem[lc->reg[rc.x].word] = lc->reg[rc.y].word;
  lc->reg[rc.x].word++;
  lc->PC += 2;
  return 0;
}

// F1           lb rc
U8 LBc(LC* lc) {
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  lc->reg[rc.y].word = lc->mem[lc->reg[rc.x].word];
  lc->reg[rc.x].word++;
  lc->PC += 2;
  return 0;
}


// F2           sw rc
U8 SWc(LC* lc) {
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  WriteWord(lc, lc->reg[rc.x].word, lc->reg[rc.y].word);
  lc->reg[rc.x].word += 2;
  lc->PC += 2;
  return 0;
}

// F3           lw rc
U8 LWc(LC* lc) {
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  lc->reg[rc.x].word += 2;
  lc->reg[rc.y].word = ReadWord(lc, lc->reg[rc.x].word-2);
  lc->PC += 2;
  return 0;
}

// F4           sh rc
U8 SHc(LC* lc) {
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  Write24(lc, lc->reg[rc.x].word, lc->reg[rc.y].word);
  lc->reg[rc.x].word += 3;
  lc->PC += 2;
  return 0;
}

// F5           lh rc
U8 LHc(LC* lc) {
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  lc->reg[rc.y].word = Read24(lc, lc->reg[rc.x].word);
  lc->reg[rc.x].word += 3;
  lc->PC += 2;
  return 0;
}

// F6           xor rc
U8 XORrc(LC* lc) {
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  lc->reg[rc.x].word ^= lc->reg[rc.y].word;
  lc->PC += 2;
  return 0;
}

// F7           lobb rc
U8 LOBBg(LC* lc) {
  lc->reg[(lc->mem[lc->PC+1]&0b11100000)>>5].word = lc->mem[lc->reg[BP].word + (lc->mem[lc->PC+1]&0b00011111)];
  lc->PC += 2;
  return 0;
}

// D8           lobh rg
U8 LOBHg(LC* lc) {
  lc->reg[(lc->mem[lc->PC+1]&0b11100000)>>5].word = Read24(lc, lc->reg[BP].word + (lc->mem[lc->PC+1]&0b00011111));
  lc->PC += 2;
  return 0;
}

#ifdef FPU
/* FPU instructions start */

// F8           cif
U8 CIF(LC* lc) {
  F32 reg = lc->reg[(lc->mem[lc->PC+1])].word;
  lc->reg[lc->mem[lc->PC+1]].word = *(U32*)&reg;
  lc->PC += 2;
  return 0;
}

// F9           cfi
U8 CFI(LC* lc) {
  U32 reg = *(F32*)(lc->reg + lc->mem[lc->PC+1]);
  lc->reg[lc->mem[lc->PC+1]].word = reg;
  lc->PC += 2;
  return 0;
}

// FA           addf32
U8 ADDF(LC* lc) {
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  F32 r1 = *(F32*)(lc->reg+rc.x);
  F32 r2 = *(F32*)(lc->reg+rc.y);
  F32 res = r1 + r2;
  lc->reg[rc.x].word = *(U32*)& res;
  lc->PC += 2;
  return 0;
}

// FB           subf32
U8 SUBF(LC* lc) {
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  F32 r1 = *(F32*)(lc->reg+rc.x);
  F32 r2 = *(F32*)(lc->reg+rc.y);
  F32 res = r1 - r2;
  lc->reg[rc.x].word = *(U32*)& res;
  lc->PC += 2;
  return 0;
}

// FC           mulf32
U8 MULF(LC* lc) {
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  F32 r1 = *(F32*)(lc->reg+rc.x);
  F32 r2 = *(F32*)(lc->reg+rc.y);
  F32 res = r1 * r2;
  lc->reg[rc.x].word = *(U32*)& res;
  lc->PC += 2;
  return 0;
}

// FD           divf32
U8 DIVF(LC* lc) {
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  F32 r1 = *(F32*)(lc->reg+rc.x);
  F32 r2 = *(F32*)(lc->reg+rc.y);
  F32 res = r1 / r2;
  lc->reg[rc.x].word = *(U32*)& res;
  lc->PC += 2;
  return 0;
}

// FE           negf32
U8 NEGF(LC* lc) {
  U32 r1 = lc->reg[lc->mem[lc->PC+1]].word;
  r1 ^= 0x80000000; // Flip the sign bit
  lc->reg[lc->mem[lc->PC+1]].word = r1;
  lc->PC += 2;
  return 0;
}

// FF           powf32
U8 POWF(LC* lc) {
  lcrc_t rc = ReadRegClust(lc->mem[lc->PC+1]);
  F32 r1 = *(F32*)(lc->reg+rc.x);
  F32 r2 = *(F32*)(lc->reg+rc.y);
  F32 res = pow(r1, r2);
  lc->reg[rc.x].word = *(U32*)& res;
  lc->PC += 2;
  return 0;
}

/* FPU instructions end */
#else
#define CIF UNK
#define CFI UNK
#define ADDF UNK
#define SUBF UNK
#define MULF UNK
#define DIVF UNK
#define NEGF UNK
#define POWF UNK
#endif
/*
U8 (*INSTS[256])(LC*) = {
  &HLT1 , &UNK  , &UNK  , &UNK  , &IRET , &ADD11, &SUB11, &MUL11, &DIV11, &LFA  , &LAF  , &OR11 , &AND11, &CMP11, &CMP10, &UNK  ,
  &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp ,
  &LDAA , &LDBA , &LDCA , &LDDA , &LDTA , &LDIA , &UNK  , &UNK  , &UNK  , &CMPpi, &LOOP , &RE   , &RNE  , &UNK  , &HLT  , &CLI  ,
  &POW11, &POW10, &RC   , &RET  , &STI  , &XCHG4, &LODSB, &STORB, &LDDS , &STDS , &LODGB, &STGRB, &LDDG , &STDG , &LODSW, &STORW,
  &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 ,
  &LDAZS, &LDBZS, &LDCZS, &LDDZS, &LDTZS, &LDIZS, &STOGB, &STOSB, &ADDr0, &ADDr0, &ADDr0, &ADDr0, &ADDr0, &ADDr0, &ADDr0, &ADDr0,
  &LDAZG, &LDBZG, &LDCZG, &LDDZG, &LDTZG, &LDIZG, &STOSW, &UNK  , &SUBr0, &SUBr0, &SUBr0, &SUBr0, &SUBr0, &SUBr0, &SUBr0, &SUBr0,
  &LDAAS, &LDBAS, &LDCAS, &LDDAS, &LDTAS, &LDIAS, &UNK  , &UNK  , &MULr0, &MULr0, &MULr0, &MULr0, &MULr0, &MULr0, &MULr0, &MULr0,
  &LDAAG, &LDBAG, &LDCAG, &LDDAG, &LDTAG, &LDIAG, &UNK  , &UNK  , &DIVr0, &DIVr0, &DIVr0, &DIVr0, &DIVr0, &DIVr0, &DIVr0, &DIVr0,
  &MOV11, &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &CLC  ,
  &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr ,
  &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr ,
  &MV26 , &INT0 , &INT1 , &JME0 , &JMNE0, &JL0  , &JG0  , &PUSH0, &PUSH1, &PUSHp, &POP1 , &CPUID, &NOP  , &UNK  , &UNK  , &UNK  ,
  &INXM , &DEXM , &STRb , &JMP0 , &JMP1 , &JMP2 , &CALL , &COP1 , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  ,
  &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 ,
  &SBc  , &LBc  , &SWc  , &LWc  , &SHc  , &LHc  , &XORrc, &LOBBg, &CIF  , &CFI  , &ADDF , &SUBF , &MULF , &DIVF , &NEGF , &POWF
};
*/

U8 (*INSTS[256])(LC*) = {
  &HLT1 , &UNK  , &UNK  , &UNK  , &IRET , &ADD11, &SUB11, &MUL11, &DIV11, &LFA  , &LAF  , &OR11 , &AND11, &CMP11, &CMP10, &UNK  ,
  &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp , &LDRp ,
  &LDAA , &LDBA , &LDCA , &LDDA , &LDTA , &LDIA , &UNK  , &UNK  , &UNK  , &CMPpi, &LOOP , &RE   , &RNE  , &UNK  , &HLT  , &CLI  ,
  &POW11, &POW10, &RC   , &RET  , &STI  , &XCHG4, &LODSB, &STORB, &LDDS , &STDS , &LODGB, &STGRB, &LDDG , &STDG , &LODSW, &STORW,
  &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 , &LDr0 ,
  &LDAZS, &LDBZS, &LDCZS, &LDDZS, &LDTZS, &LDIZS, &UNK  , &UNK  , &ADDA0, &ADDB0, &ADDC0, &ADDD0, &ADDS0, &ADDG0, &ADDSP0, &ADDBP0,
  &LDAZG, &LDBZG, &LDCZG, &LDDZG, &LDTZG, &LDIZG, &UNK  , &UNK  , &SUBA0, &SUBB0, &SUBC0, &SUBD0, &SUBS0, &SUBG0, &SUBSP0, &SUBBP0,
  &LDAAS, &LDBAS, &LDCAS, &LDDAS, &LDTAS, &LDIAS, &UNK  , &UNK  , &MULA0, &MULB0, &MULC0, &MULD0, &MULS0, &MULG0, &STOGB, &STOSB,
  &LDAAG, &LDBAG, &LDCAG, &LDDAG, &LDTAG, &LDIAG, &UNK  , &UNK  , &DIVA0, &DIVB0, &DIVC0, &DIVD0, &DIVS0, &DIVG0, &STOSW, &UNK  ,
  &MOV11, &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &CLC  ,
  &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr , &INXr ,
  &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr , &DEXr ,
  &MV26 , &INT0 , &INT1 , &JME0 , &JMNE0, &JL0  , &JG0  , &PUSH0, &PUSH1, &PUSHp, &POP1 , &CPUID, &UNK  , &UNK  , &UNK  , &UNK  ,
  &INXM , &DEXM , &STRb , &JMP0 , &JMP1 , &JMP2 , &CALL , &COP1 , &LOBHg, &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &UNK  , &NOT  ,
  &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 , &STr0 ,
  &SBc  , &LBc  , &SWc  , &LWc  , &SHc  , &LHc  , &XORrc, &LOBBg, &CIF  , &CFI  , &ADDF , &SUBF , &MULF , &DIVF , &NEGF , &POWF
};



U0 Reset(LC* lc) {
  lc->reg[SP].word = 0xFEFFFF;
  lc->reg[BP].word = 0xFEFFFF;
  lc->PC = 0x00000000;

  lc->PS = 0b01000000;
}

U0 PageDump(LC* lc, U8 page) {
  for (U16 i = (page*256); i < (page*256)+256; i++) {
    if (!(i % 16)) putchar(10);
    printf("%02X ", lc->mem[i]);
  }
}

U0 StackDump(LC* lc, U16 c) {
  printf("SP: %04X\n", lc->reg[SP].word);
  for (U16 i = 0xF000; i > 0xF000-c; i--) {
    printf("%04X: %02X\n", i, lc->mem[i]);
  }
}

U0 RegDump(LC* lc) {
  printf("\033[21A\033[10CAC %04X\n",   lc->reg[AC].word);
  printf("\033[10CBS %04X\n",           lc->reg[BS].word);
  printf("\033[10CCN %04X\n",           lc->reg[CN].word);
  printf("\033[10CDC %04X\n",           lc->reg[DC].word);
  printf("\033[10CDT %04X ASCII: %c\n", lc->reg[DT].word, lc->reg[DT].word);
  printf("\033[10CDI %04X ASCII: %c\n", lc->reg[DI].word, lc->reg[DI].word);
  printf("\033[10CEX %04X\n",           lc->reg[EX].word);
  printf("\033[10CFX %04X\n",           lc->reg[FX].word);
  printf("\033[10CHX %04X\n",           lc->reg[HX].word);
  printf("\033[10CLX %04X\n",           lc->reg[LX].word);
  printf("\033[10CN  %04X\n",           lc->reg[X].word);
  printf("\033[10CY  %04X\n",           lc->reg[Y].word);
  printf("\033[10CIX %04X ASCII: %c\n", lc->reg[IX].word, lc->reg[IX].word);
  printf("\033[10CIY %04X ASCII: %c\n", lc->reg[IY].word, lc->reg[IY].word);
  printf("\033[10CSP %04X\n",           lc->reg[SP].word);
  printf("\033[10CBP %04X\n",           lc->reg[BP].word);
  printf("\033[10CPC %08X\n",           lc->PC);
  printf("\033[10CPS %08b\n",           lc->PS);
  printf("\033[10C   -I---ZNC\033[0m\n");
}

U8 Exec(LC* lc, const U32 memsize) {
  U8 exc = 0;
  #define RECOVERY_SIZE 32
  U32 govnu_recovery[RECOVERY_SIZE];
  U32 ptr = 0;
  U8 opcode;
  
  execloop:
  instructions_executed++;
  
  opcode = lc->mem[lc->PC];
  
  if (opcode == 0 || INSTS[opcode] == &UNK) {
    govnu_recovery[ptr] = lc->PC + (opcode << 24);
    printf("\x1b#3\x1b[32m[ UPLOAD MODE ]\x1b[0m\n\x1b#4\x1b[32m[ UPLOAD MODE ]\x1b[0m\n\x1b#3\x1b[31mKernel Panic!\x1b[0m\n\x1b#4\x1b[31mKernel Panic!\x1b[0m\nDDR Vendor: GovnIndustries\n\x1b[33mPanic caller: Exec\nPanic Msg: TRAP\x1b[0m\n\n\x1b[32m");
    for (U8 i = 0; i < RECOVERY_SIZE; i++) {
      printf("%08X ", govnu_recovery[(RECOVERY_SIZE-i+ptr)%RECOVERY_SIZE]);
      if (i%32==31) printf("\n");
    }
  }
  
  exc = (INSTS[opcode])(lc);
  
  if (exc != 0) return lc_errno;
  
  goto execloop;
  return exc;
}
