// General & I/O
#define INT_EXIT         0x00 // exit code from stack
#define INT_READ         0x01 // push char to stack
#define INT_WRITE        0x02 // write char from stack
#define INT_DATE         0x03 // get date to dx register
#define INT_RESET        0x04 // reset the CPU

// Videobuffer
#define INT_VIDEO_WRITE  0x0C // set vga[si] to ax
#define INT_VIDEO_FLUSH  0x11 // flush the videobuffer
#define INT_VIDEO_CLEAR  0x12 // flush the videobuffer
#define INT_VIDEO_GD2    0x13 // flush the videobuffer
#define INT_VIDEO_GDM    0x14 // flush the videobuffer
#define INT_VIDEO_GRD    0x15 // flush the videobuffer
#define INT_VIDEO_G1M    0x16 // flush the videobuffer
#define INT_VIDEO_G2M    0x17 // flush the videobuffer
#define INT_VIDEO_3D_CMD 0x45 // send command to 3d pipeline
#define INT_RAND         0x21 // get random number to dx
#define INT_WAIT         0x22 // wait dx milliseconds
#define INT_BEEP         0x23 // beep
