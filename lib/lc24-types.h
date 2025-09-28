/*
  This file defines LC24 keyword types to
  use in the CPU implementation (lib/cpu16/cpu16.h).
*/

// LC24 types
typedef unsigned char  lcbyte;
typedef unsigned short lcword;

// Non-LC24 types (over 24 bits)
// Xi kak obychno na solyah
typedef unsigned int           nonlcdword;
typedef unsigned long long int nonlcqword;
