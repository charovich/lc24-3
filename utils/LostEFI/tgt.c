#include <uefi.h>
#include <holyc-types.h>
#include <efikator_support.h>
#include <uefi-ansi.h>
#include <lc24-types.h>
#include <sterm-control.h>
#include <cpu24/bpf.h>
#include <cpu24/cli.h>
#include <cpu24/cpu24.h>
#include <lusl.h>

void puts(const char* str)
{
    while(*str)
    {
        termputchar(*str);
        str++;
    }
}

void dates() {
    unsigned short date_encoded = LC_LOSTDATE();
    lostdate date = lostdate_convert(date_encoded);

    if (date.month == 9 && date.day == 9) {
        puts("\x1b[96mWarning: CPU Temp below 0 degrees Celcius\x1b[0m\n\n");
    } else if (date.month == 6 && date.day == 2) {
        puts("\x1b[93mreturn()\x1b[0m\n\n");
    } else if (date.month == 9 && date.day == 27) {
        puts("\x1b[93m:molniya: GC16X -- V S E.\x1b[0m\n\n");
    } else if (date.month == 3 && date.day == 1) {
        puts("\x1b[93mDeleting UEFI...\x1b[0m\n\n");
    } else if (date.month == 9 && date.day == 21) {
        puts("\x1b[93m:molniya: Shluxidev -- V S E.\x1b[0m\n\n");
    } else if (date.month == 9 && date.day == 2) {
        puts("\x1b[93mHappy Death Day!\x1b[0m\n\n");
    }
}

int main() {
    LC* lc = malloc(sizeof(LC));
    Reset(lc);
    memset(lc->mem, 0, MEMSIZE);
    GGinit(lc->mem, &(lc->gg), 1);
    puts("\033[2J\033[H");
    dates();
    for (U32 i = 0; i < file_bin_len; i++) {
        lc->mem[i] = file_bin[i];
    }
    Exec(lc, 0);
    return 0;
}
