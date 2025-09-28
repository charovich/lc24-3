import lusl.std
import gdi.gui
import gdi.vfont

def govndate_inx(x, y):
    box(x, y, 200, 100, 8)
    box(x + 200 - 1, y, 1, 100, 8)
    box(x, y + 100 - 1, 200, 1, 8)
    box(x, y, 1, 100, 8)

    box(x + 1, y + 1, 198, 98, 0)

    vputs(x + 8, y + 8, 15, "GovnDate by Ксэ816")
    vputs(x + 8, y + 24, 15, "С инкрементацией!")
    vputs(x + 8, y + 40, 15, "$ diff gd.old gd.new")
    vputs(x + 8, y + 48, 6, "1c1")
    vputs(x + 8, y + 56, 9, "< GovnDate=28426")
    vputs(x + 8, y + 64, 15, "---")
    vputs(x + 8, y + 72, 10, "> GovnDate=28427")

    box(x + 200 - 9 - 49 - 8, y + 100 - 9 - 16, 49 + 8, 16, 8)
    vputs(x + 200 - 9 - 49 - 8 + 4, y + 100 - 9 - 16 + 1, 15, "Скотчать")

def notify(x, y, t, st, btn):
    box(x, y, 200, 100, 8)
    box(x + 200 - 1, y, 1, 100, 8)
    box(x, y + 100 - 1, 200, 1, 8)
    box(x, y, 1, 100, 8)

    box(x + 1, y + 1, 198, 98, 0)

    vputs(x + 8, y + 8, 15, t)
    vputs(x + 8, y + 24, 15, st)

    box(x + 200 - 9 - 49 - 8, y + 100 - 9 - 16, 49 + 8, 16, 8)
    vputs(x + 200 - 9 - 49 - 8 + 4, y + 100 - 9 - 16 + 1, 15, btn)

def window(x,y,w,h,bdr):
    box(x,y,w+2,1,bdr)
    box(x,y+h+17,w+2,1,bdr)
    box(x,y+1,1,h+16,bdr)
    box(x+w+1,y+1,1,h+16,bdr)
    box(x+1,y+1,w,16,bdr)
    box(x+1,y+18,w,h,15)

def task(i,s):
    if s:
        box(32+160*i, 480-32, 160,32-2, 8)
        box(32+160*i, 480-2, 160, 2, 10)
    else:
        box(34+160*i, 480-2, 156, 2, 7)
    if i == 0:
        gic(32+4+160*i, 480-32+8,13, gweb2_mini)
        vputs(32+4+16+4+160*i, 480-32+8, 15, "GovnWeb 2")
    elif i == 1:
        gic(32+4+160*i, 480-32+8,13, shutapp_mini)
        vputs(32+4+16+4+160*i, 480-32+8, 15, "Чара - ShutApp")
    elif i == 2:
        gic(32+4+160*i, 480-32+8,13, my_pc)
        vputs(32+4+16+4+160*i, 480-32+8, 15, "помой пк")
    else:
        box(32+4+160*i, 480-32+8, 16, 16, 3)

def govnweb(x,y):
    window(x,y,340,190,15)
    gic(x+2,y+1,13,gweb2_mini)
    vputs(x+5,y+1,0,"   GovnWeb 2 (Alpha)")
    bx=x+1
    by=y+17

    box(bx,by,8,8,6)
    vputs(bx,by,15,"1")
    box(bx+8,by,8,8,8)
    vputs(bx+8,by,7,"2")
    box(bx+16,by,340-16,8,0)
    box(bx,by+8,340,8,12)
    vputs(bx+340-8*24,by,15,"          100kg/16MiB|08-23-2025")

    vputs(bx,by+8,15,"GovnWeb 2")
    box(bx,by+16,7*8,8,15)
    vputs(bx,by+16,0,"Xi816")
    vputs(bx+6*8,by+16,8,"x")
    box(bx+7*8,by+16,340-7*8,8,7)
    box(bx,by+24,340,8,15)
    vputs(bx,by+24,0,"xi816.github.io")
    vputs(bx+11*8,by+24,8,"/")
    box(bx,by+32,340,190-32,15)
    vputs(bx+(340-3*8)/2,by+32,0,"404")
    vputs(bx+(340-6*33)/2,by+48,8,"There isn't a GovnWeb site here.")
    vputs(bx+48,by+64,7,"If you're trying to publish one, just don't pls.")

def main():
    cls(34)
    box(0, 480-32, 640, 32, 0)

    vputs(640-6*8,480-29,15, "00:00:00")
    vputs(640-6*8,480-16,15, "09.13.25")

    gic(8, 480-24,13, g10)

    vputs(8, 8+32, 15, "GovnWeb2")
    gic(8+8, 8, 13, gweb2)
    vputs(8, 8+32+48, 15, "ShutApp")
    gic(8+8, 8+48, 13, shutapp)

    task(0,1)
    task(1,0)
    task(2,0)

    govnweb(160,8)

    govndate_inx(640-200-8,480-100-32-8)
    notify(640-200-8,480-100-32-8-105,"Ldt Lost      MAX", "GF2 Ported with bugs.","Close")

    vflush()
    gets("")
    exit(0)
