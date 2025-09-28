# GovnPy shell example
# Copyright (C) 2025 t.me/pyproman
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
import lusl.std
import gdi.gui
import gdi.font

buf = bytes(32)

def draw_mouse():
  __asm__("jsr getmousepos")
  __asm__("mov %di cursor")
  __asm__("int $13")
  __asm__("int $11")
  __asm__("mov %dc 16")
  __asm__("int $22")
def rfib(x):
    if x == 1 or x == 0:
        return 1
    return rfib(x-1) + rfib(x-2)
def sfib(x):
    a = 1
    b = 1
    while x:
        a = b + a

        b = b + a
        a = b - a
        b = b - a

        x = x - 1
    return a
def getint(bf):
    c = 0
    i = 0
    while bf[i]:
        c = c * 10 + (bf[i] - 0x30)
        i = i + 1
    return c
def fibcalc():
    puts("Number> ")
    gets(buf)
    puts("Result: ")
    puti(rfib(getint(buf)))
    puts("\n")
lost_cpu_mem = bytes(400)
def lost_editor():
    puts('Lost Core Input Mode\nType code, end with . on a seperate line\n')
    ptr = lost_cpu_mem
    flag = True
    while flag:
        auxptr = ptr
        ptr = gets(ptr)
        ptr[0] = 0x0a
        if ptr == auxptr + 1:
            if auxptr[0] == 0x2e:
                auxptr[0] = 0
                flag = False
        ptr = ptr + 1
def lost_emulator():
    puts("\x1b[9mLox\x1b[0m Lost Core Starting...\n") #]
    ptr = lost_cpu_mem
    aif = False
    while ptr[0]:
        if ptr[0] == 0xa:
            ptr = ptr + 1
        elif memcmp(6, ptr, "print "):
            auxptr = ptr + 6
            while auxptr[0] != 0x0a:
                auxptr = auxptr + 1
            auxptr[0] = 0
            puts(ptr + 6)
            puts("\n")
            if aif:
                puts("[SYS] Condition met for print: ")
                puts(ptr+6)
                puts("\n")
                aif = False
            ptr = auxptr + 1
        elif memcmp(6, ptr, "print\n"):
            puts("\n")
            ptr = ptr + 6
        elif memcmp(12, ptr, "input_string"):
            puts("Lost -> ")
            gets(buf)
            ptr = ptr + 13
        elif memcmp(3, ptr, "if "):
            auxptr = ptr + 3
            while auxptr[0] != 0x0a:
                auxptr = auxptr + 1
            auxptr[0] = 0
            if scmp(buf, ptr + 3) == 0:
                ptr = auxptr
                while ptr[0] != 0x0a:
                    ptr = ptr + 1
                ptr = ptr + 1
            else:
                ptr = auxptr + 1
                aif = True
        else:
            puts("\x1b[31m ERROR: UNKNOWN SHIT \x1b[0m") #]
            puts(ptr)
            return 0
def gui():
    vputs(0,0,15,"123321")
    vflush()
    msleep(2000)
    cls(0)
    vflush()
    vputs8(0,10,15,"123321")
    vflush()
    msleep(2000)
    cls(0)
    vflush()
    vputs8t(0,20,15,"123321")
    vflush()
    msleep(2000)
    cls(0)
    vflush()
    vputs16(0,30,15,"123321")
    vflush()
    msleep(2000)
    cls(0)
    vflush()
    vputsas16(0,40,15,"123321")
    vflush()
    msleep(2000)
    cls(0)
    vflush()
    vputsgui(0,50,15,"123321")
    vflush()
    msleep(2000)
    cls(0)
    vflush()
    puts("\n123321 <- Must output this\n")
def presa():
    cls(15)
    tmp = vputs8t(10, 10, 3, "Govn")
    vputs8t(tmp, 0, 0, "Py Conf 2025")
    vputs8t(10, 20, 0, "Версия 1")
    box(10, 44, 8 * 15, 8 * 15, 7)
    vputs8t(10, 44, 0, "push 2\npush 2\npush 2\npop %bx\npop %ax\nmul %ax %bx\npush %ax\npop %bx\npop %ax\nadd %ax %bx\npush %ax")
    vputs8t(140, 20, 0, "Version 2\n11 lines")
    vputs8t(140, 36, 0, "50% shorter")
    box(140, 44, 8 * 15, 8 * 15, 7)
    vputs8t(140, 44, 0, "lda 2\nldb 2\nmul %bx 2\nadd %ax %bx\npush %ax")
    vflush()
def meme():
    cls(15)
    i = 0
    while i != 0x10:
        box(i*8, 0, 8, 8, i)
        vputs(i*8, 0, 15-i, "0\x001\x002\x003\x004\x005\x006\x007\x008\x009\x00a\x00b\x00c\x00d\x00e\x00f"+i*2)
        i = i + 1
    vputs8t(10, 10, 2, "Geraldip Pavlov")
    vputs8t(10, 18, 0,
          "Коллега, подскажите, пожалуйста, когда будет "
          "завершен этот\nэксперимент? В том плане, что, я "
          "полагаю, изучение ОС началось\nиз-за некого рода "
          "интереса к теме, была ли поставлена цель,\nкакое-то "
          "логическое завершение, или это просто будет длиться\n"
          "до тех пор, пока энтузиазм не угаснет?"
        )

    vputs8t(0, 182, 0, "LostGram. Powered by LUSL(TM)")
    vflush()
def geometry():
    cls(1)
    box(0,0,340,190,11)
    r = 35
    x = 0-r
    y = 0-r
    while (y != r):
      while (x != r):
        if (((x*x) + (y*y)) <= (r*r)):
          box(x+270,y+85,1,1,5)
        x = x + 1
      x = 0-r
      y = y + 1
    box(10, 50, 70, 70, 5)
    x = 160;
    y = 50;
    w = 1;    
    while (w <= 120):
      box(x, y, w, 1, 5);
      y = y + 1;
      x = x - 1;
      w = w + 2;
    vflush()
def govndate_inx(x, y):
    box(x, y, 200, 100, 8)
    box(x + 200 - 1, y, 1, 100, 8)
    box(x, y + 100 - 1, 200, 1, 8)
    box(x, y, 1, 100, 8)

    box(x + 1, y + 1, 198, 98, 0)

    vputsas16(x + 8, y + 8, 15, "GovnDate by Ксэ816")
    vputs(x + 8, y + 24, 15, "С инкрементацией!")
    vputs8t(x + 8, y + 40, 15, "$ diff gd.old gd.new")
    vputs8t(x + 8, y + 48, 6, "1c1")
    vputs8t(x + 8, y + 56, 9, "< GovnDate=28429")
    vputs8t(x + 8, y + 64, 15, "---")
    vputs8t(x + 8, y + 72, 10, "> GovnDate=28430")

    box(x + 200 - 9 - 49 - 8, y + 100 - 9 - 16, 49 + 8, 16, 8)
    vputs(x + 200 - 9 - 49 - 8 + 4, y + 100 - 9 - 16 + 1, 15, "Скотчать")

def notify(x, y, t, st, btn):
    box(x, y, 200, 100, 8)
    box(x + 200 - 1, y, 1, 100, 8)
    box(x, y + 100 - 1, 200, 1, 8)
    box(x, y, 1, 100, 8)

    box(x + 1, y + 1, 198, 98, 0)

    vputsas16(x + 8, y + 8, 15, t)
    vputs(x + 8, y + 24, 15, st)

    box(x + 200 - 9 - 49 - 8, y + 100 - 9 - 16, 49 + 8, 16, 8)
    vputs(x + 200 - 9 - 49 - 8 + 4, y + 100 - 9 - 16 + 1, 15, btn)

def govnweb(x,y):
    window(x,y,340,190,"GovnWeb 2 (Alpha)")
    gic(x+2,y+1,13,gweb2_mini)
    bx=x+1
    by=y+17

    box(bx,by,8,8,6)
    vputs8t(bx,by,15,"1")
    box(bx+8,by,8,8,8)
    vputs8t(bx+8,by,7,"2")
    box(bx+16,by,340-16,8,0)
    box(bx,by+8,340,8,12)
    vputs8t(bx+340-8*24,by,15,"  100kg/32MiB|08-23-2025")

    vputs8t(bx,by+8,15,"GovnWeb 2")
    box(bx,by+16,7*8,8,15)
    vputs8t(bx,by+16,0,"Gnkui")
    vputs8t(bx+6*8,by+16,8,"x")
    box(bx+7*8,by+16,340-7*8,8,7)
    box(bx,by+24,340,8,15)
    vputs8t(bx,by+24,0,"gnkui.github.io")
    vputs8t(bx+15*8,by+24,8,"/")
    box(bx,by+32,340,190-32,15)
    #vputs16(bx+(340-3*8)/2,by+32,0,"404")
    #vputsas16(bx+(340-6*33)/2,by+48,8,"There isn't a GovnWeb site here.")
    #vputs(bx+48,by+64,7,"If you're trying to publish one, just don't pls.")
    vputs16(bx+(340-3*8)/3,by+32,0,"Фанфик про шлюкси")
    vputsas16(bx,by+48,8,"Персонажи: Люкси и Гнкуи")
    vputs8t(bx,by+64,0,"Часть 1\nПасмурдный день апреля, Гнкуи спокойно спа\nла. Но вдруг телефон завибрировал.Гнкуи н\nехотя поднялась с кровати, и проверила тел\nефон. Это был Люксидев. Гнкуи пробормотала\n под нос. -И чего ему надо. В сообщении го\nворилось.-Привет.Не хочешь прогулятся?.Гн\nкуи насупилась и напечатала.-Извини за воп\nрос.Но зачем писать в такую рань? Не долго\nдумая, Люксидев ответил.-Как можно так до\nлго спать.Здоровые люди уже на ногах, а ты\nспишь.Гнкуи не предала значения его словам,\nи быстро ответила.-Ладно, говори во скольк\nо.Люксидев быстро написал.-9:00, я буду жд\nать около твоего дома.Гнкуи посмотрела на\nчасы.8:40.")

def window(x,y,w,h,name):
    bdr = 15
    box(x,y,w+2,1,bdr)
    box(x,y+h+17,w+2,1,bdr)
    box(x,y+1,1,h+16,bdr)
    box(x+w+1,y+1,1,h+16,bdr)
    box(x+1,y+1,w,16,bdr)
    box(x+1,y+18,w,h,15)
    vputs(x+20,y+1,0,name)

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
        vputs(32+4+16+4+160*i, 480-32+8, 15, "Рика Фуруде - ShutApp")
    elif i == 2:
        gic(32+4+160*i, 480-32+8,13, my_pc)
        vputs(32+4+16+4+160*i, 480-32+8, 15, "ПО `мой пк`")
    else:
        box(32+4+160*i, 480-32+8, 16, 16, 3)

def lost10():
    flag = 1
    load_pal()
    while flag:
        gic(0,0,13,wall)
        gic(8, 480-24,13, g10)

        vputs(640-6*8,480-29,15, "00:00:00")
        vputs(640-6*8,480-16,15, "09.19.25")

        vputs(8, 8+32, 15, "GovnWeb2")
        gic(8+8, 8, 13, gweb2)
        vputs(8, 8+32+48, 15, "ShutApp")
        gic(8+8, 8+48, 13, shutapp)

        task(0,1)
        task(1,0)
        task(2,0)

        govnweb(160,8)

        govndate_inx(640-200-8,480-100-32-8)

        draw_mouse()
def shell(nest):
    flag = 1
    puts("GovnShell.py [Version 25.2.8]\n(c) 2025 Govn Industries & t.me/pyproman\n\nYou are ")
    puti(nest)
    puts(" levels away from boot\n")
    while flag:
        puts("\x1b[33msh.py>\x1b[0m ") #]
        gets(buf)
        if buf[0] == 0:
            # Ok
            pass
        elif scmp(buf, "help"):
            puts("Welcome to a shell compiled with Lusl 🤑!\n"
                 "help             Show help\n"
                 "dir              List tags\n"
                 "fib              Calculate fibbonaci\n"
                 "newsh            Start a new shell\n"
                 "shutdown         int$0(0)\n"
                 "exit             Exit shell\n"
                 "scls             Clear Screen\n"
                 "lost             Enter shedevrolostcore emulator\n"
                 "graphics         XOR Graphics demo\n"
                 "render meme      Render meme to screen\n"
                 "render presa     Render presa to screen\n"
                 "render geometry  Render geometry to screen\n"
                 "render lost10    Render lost10 to screen\n")
        elif scmp(buf, "dir"):
            puts("Your disk is cooked bro\n")
        elif scmp(buf, "fib"):
            fibcalc()
        elif scmp(buf, "scls"):
            cls(0)
            vflush()
        elif scmp(buf, "render meme"):
            meme()
        elif scmp(buf, "render presa"):
            presa()
        elif scmp(buf, "render geometry"):
            geometry()
        elif scmp(buf, "render lost10"):
            lost10()
        elif scmp(buf, "graphics"):
            gui()
        elif scmp(buf, "lost"):
            lost_editor()
            lost_emulator()
        elif scmp(buf, "newsh"):
            shell(nest + 1)
        elif scmp(buf, "shutdown"):
            exit(0)
        elif scmp(buf, "exit"):
            puts("Deleting C:/System32...\nok\n")
            flag = 0
        else:
            puts("Your command is bad... just like that apple\n")
    puts("Goodbye!\n")
def main():
    shell(0)
