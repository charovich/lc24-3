#pragma once

#include <stdint.h>

typedef void U0;
typedef uint8_t U8;
typedef int8_t I8;
typedef uint16_t U16;
typedef int16_t I16;
typedef uint32_t U32;
typedef int32_t I32;

typedef enum {
    Ok,
    Error,
    // BadApplePortWhen
    // DoomPortWhen
} ERR;

#define error(msg, ...) (fprintf(stderr, "%s:%d says:\n\x1b[31m" msg "\x1b[0m\n", __FILE__, __LINE__, ## __VA_ARGS__), 1)
//#define error(msg, ...) fprintf(stderr, "\x1b[31m%s\x1b[0m\n", msg)
