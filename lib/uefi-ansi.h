U8 termputchar_state = 0; // 0 - normal, 1 - wait for [, 2 - parsing
U8 termputchar_arg = 0;
U8 termputchar_color = EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY;
U0 termputchar(U8 byte) {
  if(!byte) return;
  switch (termputchar_state) {
    case 0:
      switch (byte) {
        case '\e':
          termputchar_state = 1;
          break;
        default:
          putchar(byte);
      }
      break;
    case 1:
      if (byte != '[') {
        putchar(byte);
        termputchar_state = 0;
      } else
        termputchar_state = 2;
      break;
    case 2:
      if (byte == 'J') {
        if (termputchar_arg == 2) {
          ST->ConOut->ClearScreen(ST->ConOut);
        }
        termputchar_arg = 0;
        termputchar_state = 0;
      } else if (byte == 'H') {
        ST->ConOut->SetCursorPosition(ST->ConOut, 0, 0);
        termputchar_arg = 0;
        termputchar_state = 0;
      } else if (byte == 'm') {
        switch (termputchar_arg) {
          case 0:
            termputchar_color = EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY;
            break;
          case 1:
            termputchar_color |= EFI_BRIGHT;
            break;
          case 7:
            termputchar_color = (termputchar_color >> 4) | ((termputchar_color & 0xF) << 4);
            break;
#define MAPFG(uefi) termputchar_color &= 0xF0; termputchar_color |= uefi; break
#define MAPBG(uefi) termputchar_color &= 0xF; termputchar_color |= uefi; break
          case 30: MAPFG(EFI_BLACK);
          case 31: MAPFG(EFI_RED);
          case 32: MAPFG(EFI_GREEN);
          case 33: MAPFG(EFI_BROWN);
          case 34: MAPFG(EFI_BLUE);
          case 35: MAPFG(EFI_MAGENTA);
          case 36: MAPFG(EFI_CYAN);
          case 37: MAPFG(EFI_LIGHTGRAY);
          /* background */
          case 40: MAPBG(EFI_BACKGROUND_BLACK);
          case 41: MAPBG(EFI_BACKGROUND_RED);
          case 42: MAPBG(EFI_BACKGROUND_GREEN);
          case 43: MAPBG(EFI_BACKGROUND_BROWN);
          case 44: MAPBG(EFI_BACKGROUND_BLUE);
          case 45: MAPBG(EFI_BACKGROUND_MAGENTA);
          case 46: MAPBG(EFI_BACKGROUND_CYAN);
          case 47: MAPBG(EFI_BACKGROUND_LIGHTGRAY);
          /* Bright */
          case 90: MAPFG(EFI_DARKGRAY);
          case 91: MAPFG(EFI_LIGHTRED);
          case 92: MAPFG(EFI_LIGHTGREEN);
          case 93: MAPFG(EFI_YELLOW);
          case 94: MAPFG(EFI_LIGHTBLUE);
          case 95: MAPFG(EFI_LIGHTMAGENTA);
          case 96: MAPFG(EFI_LIGHTCYAN);
          case 97: MAPFG(EFI_WHITE);
#undef MAPFG
          //default:
            // ?
        }
        ST->ConOut->SetAttribute(ST->ConOut, termputchar_color);
        termputchar_arg = 0;
        termputchar_state = 0;
      } else if ('0' <= byte && byte <= '9') {
        termputchar_arg = termputchar_arg * 10 + (byte - '0');
      } else {
        // APrint("[%c", byte);
        termputchar_arg = 0;
        termputchar_state = 0;
      }
  }
}
