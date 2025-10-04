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
    vputs5(x + 2, y + 2, 15, n)
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
    cls(36)
    i = 1
    while i != 10:
        window(10*i,5*i,100,100,i==9, "Explorer")
        i = i + 1
    vflush()
    while flag:
        msleep(1)
