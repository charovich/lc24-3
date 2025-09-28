import lusl.std
import gdi.gui
import gdi.font
import gdi.gdi9x

B3D = (
    b"\x0F\x0F\x10\x0F\x0F\x00\x30"
    b"\x0F\x07\x10\x07\x08\x00\x30"
    b"\x20\x0F\x07\x10\x08\x00\xF0"
    b"\x0F\x08\x10\x08\x08\x00\x30"
    b"\x00\x00\x10\x00\x00\x00\xF0"
)

WIN = (
    b"\x07\x07\x07\x10\x07\x07\x07\x00\x30"
    b"\x07\x0F\x0F\x10\x0F\x0F\x08\x00\x30"
    b"\x07\x0F\x07\x10\x07\x07\x08\x00\x30"
    b"\x20\x07\x0F\x07\x10\x07\x08\x00\xF0"
    b"\x07\x0F\x07\x10\x07\x07\x08\x00\x30"
    b"\x07\x08\x08\x10\x08\x08\x08\x00\x30"
    b"\x00\x00\x00\x10\x00\x00\x00\x00\xF0"
)

MINIMIZE_SPR = spr(
    "        \n"
    "        \n"
    "        \n"
    "####    \n"
    "        \n"
    "        \n        \n        "
)

MAXIMIZE_SPR = spr(
    "####    \n"
    "#  #    \n"
    "#  #    \n"
    "####    \n"
    "        \n"
    "        \n        \n        "
)

CLOSE_SPR = spr(
    "#  #    \n"
    " ##     \n"
    " ##     \n"
    "#  #    \n"
    "        \n"
    "        \n        \n        "
)

def draw_mouse():
  __asm__("jsr getmousepos")
  __asm__("mov %di cursor")
  __asm__("int $13")
  __asm__("int $11")
  __asm__("mov %dc 16")
  __asm__("int $22")

def window(x, y, w, h, a, n):
    border(x - 3, y - 3, w, h + 13, WIN)
    if a:
        box(x, y, w, 13, 4)
    else:
        box(x, y, w, 13, 8)
    box(x, y + 13, w, h, 7)
    vputs8t(x + 2, y + 2, 15, n)
    box(x + w - 30, y + 2, 18, 9, 7)
    border(x + w - 30, y + 2, 5, 5, B3D)
    spr(x + w - 28, y + 4, 0, MINIMIZE_SPR)
    border(x + w - 21, y + 2, 5, 5, B3D)
    spr(x + w - 19, y + 4, 0, MAXIMIZE_SPR)
    box(x + w - 11, y + 2, 9, 9, 7)
    border(x + w - 11, y + 2, 5, 5, B3D)
    spr(x + w - 9, y + 4, 0, CLOSE_SPR)


def main():
    flag = 1
    while flag:
        cls(36)
        window(10,5,360,100,9,"Explorer")
        vputs8t(10, 18, 0, "BILL GATES WANTED GNU/LINUX\nHERE'S YOUR :fig:\nHA:abaudna:\nGNU/LINUX MOTHERLAND:abaudna::fig:\nROTTEN WINDOWS WILL SOON DIE:fig:\nOUR LINUX-OID SPIRIT CANNOT BE BROKEN:strong:\nMICROSOFT WILL BE OURS :ura:")
        puts("X:")
        puti(getx());
        puts("\n")
        puts("Y:")
        puti(gety())
        puts("\n")
        puts("Clicked:")
        puti(getm())
        puts("\n")
        get1 = 360
        get2 = 7
        get3 = 2
        if scmp(getx(), get1):
            if scmp(gety(), get2):
                if scmp(getm(), get3):
                    __asm__('int $12')
                    __asm__('lda $69')
                    __asm__('trap')
        draw_mouse()
        vflush()
