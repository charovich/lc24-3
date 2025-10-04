import lusl.std
import gdi.gui
import gdi.font

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
    vputs8t(x + 8, y + 56, 9, "< GovnDate=28445")
    vputs8t(x + 8, y + 64, 15, "---")
    vputs8t(x + 8, y + 72, 10, "> GovnDate=28446")

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
    #vputs8t(x + 8, y + 40, 15, st)

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
    vputs16(bx+(340-3*8)/3,by+32,0,"Фанфик про шлюкси")
    vputsas16(bx,by+48,8,"Персонажи: Люкси и Гнкуи")
    vputs8t(bx,by+64,0,"Часть 1\nПасмурдный день апреля, Гнкуи спокойно спа\nла. Но вдруг телефон завибрировал.Гнкуи н\nехотя поднялась с кровати, и проверила тел\nефон. Это был Люксидев. Гнкуи пробормотала\n под нос. -И чего ему надо. В сообщении го\nворилось.-Привет.Не хочешь прогулятся?.Гн\nкуи насупилась и напечатала.-Извини за воп\nрос.Но зачем писать в такую рань? Не долго\nдумая, Люксидев ответил.-Как можно так до\nлго спать.Здоровые люди уже на ногах, а ты\nспишь.Гнкуи не предала значения его словам,\nи быстро ответила.-Ладно, говори во скольк\nо.Люксидев быстро написал.-9:00, я буду жд\nать около твоего дома.Гнкуи посмотрела на\nчасы.8:40.")
    #vputs8t(bx,by+64,0,"Version 2")

def window(x,y,w,h,name):
    bdr = 15
    box(x,y,w+2,1,bdr)
    box(x,y+h+17,w+2,1,bdr)
    box(x,y+1,1,h+16,bdr)
    box(x+w+1,y+1,1,h+16,bdr)
    box(x+1,y+1,w,16,bdr)
    box(x+1,y+18,w,h,15)
    vputs(x+20,y+1,0,name)
    vputs(x+304,y+1,0,"- O X")

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
        gic(32+4+160*i, 480-32+8,13, max_mini)
        vputs(32+4+16+4+160*i, 480-32+8, 15, "Рика Фуруде - MAX")
    elif i == 2:
        gic(32+4+160*i, 480-32+8,13, my_pc)
        vputs(32+4+16+4+160*i, 480-32+8, 15, "ПО `мой пк`")
    else:
        box(32+4+160*i, 480-32+8, 16, 16, 3)

def draw_mouse():
  __asm__("jsr getmousepos")
  __asm__("mov %di cursor")
  __asm__("int $13")
  __asm__("int $11")
  __asm__("mov %dc 16")
  __asm__("int $22")

def main():
    flag = 1
    load_pal()
    while flag:
        gic(0,0,13,wall)
        gic(8, 480-24,13, g10)

        vputs(640-6*8,480-29,15, "00:00:00")
        vputs(640-6*8,480-16,15, "09.25.25")

        vputs(8, 8+32, 15, "GovnWeb2")
        gic(8+8, 8, 13, gweb2)
        vputs(8, 8+32+48, 15, "VKontakte")
        gic(8+8, 8+48, 13, max)

        task(0,1)
        task(1,0)
        task(2,0)

        govnweb(160,8)

        govndate_inx(640-200-8,480-100-32-8)

        draw_mouse()
