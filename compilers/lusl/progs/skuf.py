import lusl.std
buf = bytes(64)
def input(prompt):
  puts(prompt)
  gets(buf)
  return buf
def write(title, s, duration, autonext):
  ptr = title;
  while ptr[0]:
    #__asm__(ptr[0], "int $2 push 0");
    putc(ptr[0]);
    msleep(0)#28);
    ptr = ptr + 1;

  msleep(28);
  ptr = s;
  while ptr[0]:
    #__asm__(ptr[0], "int $2 push 0");
    putc(ptr[0]);
    if ptr[0] & 128 == 128:
      if ptr[0] & 192 == 128:
        msleep(duration);
    else:
      msleep(duration);
    ptr = ptr + 1;
  if (autonext):
    msleep(200);
    puts("\n");
  else:
    input("");

innv = "ВНУТРЕННИЙ ГОЛОС:"
nSkoof = "???:"
skoof = "СКУФ:"
dc = "ДИКТОР:"
game = "ИГРА:"
alt0 = "КСЮША:"
alt1 = "НАСТЯ:"
alt2 = "АЛИСА:"

press_enter = ">> Нажмите <Enter>, чтобы продолжить <<\n";
pwds = b"\x01\x01\x01\x01\x00"
def main():
  tsp = 0#45;
  write(innv, "Доброе утро, СКУФЫ!", tsp, False);
  write(nSkoof, "Ещё один день.", tsp, False);
  write(dc, "Где я? Кто я?", tsp, False);
  write(game, "Как тебя зовут?", tsp, True);
  temp = input(">> ");
  if not scmp(temp, "скуф"):
    write(innv, "Кого ты пытаешься надуть!? Все знают, что ты просто СКУФ!", tsp, False);
  write(skoof, "Ещё один день.", tsp, False);
  write(skoof, "Ещё один бессмысленный день.", tsp, False);
  write(innv, "Дружище, хорош киснуть! Всё не может быть настолько плохо!", tsp, False);
  write(skoof, "Как я дошёл до того, что мой единственный собеседник, это мой внутренний голос?", tsp, False);
  write(innv, "Проснись и ПОЙ!", tsp, False);
  write(skoof, "В одном он прав.", tsp, False);
  write(skoof, "Конечно, хорошо было бы провести в постели весь день...", tsp, False);
  write(skoof, "А впрочем, почему бы и нет?", tsp, False);
  write(game, "Провести в постели весь день?", tsp, True);
  puts("0 -- Да, с удовольствием!\n1 -- Конечно нет!\nНичего -- 0\n");
  temp = input(">> ");
  if temp[0]:
    temp = s2i(temp) % 2;
  else: temp = 0;
  # Спать или встать
  if (temp == 0):
    write(dc, "Ты переворачиваешься на другой бок, уютно укутавшись старым добрым одеялом.", tsp, False);
    write(dc, "Удобно устариваясь на матрасе среднего уровня качества, ты погружаешься в него, словно нож в масло.", tsp, False);
    write(dc, "По твоему телу пробегает лёгкая дрожь. Ты знаешь, что за этим последует.", tsp, False);
    write(dc, "Твои веки тяжелеют, а мысли путаются.", tsp, False);
    write(dc, "Ты засыпаешь.", tsp, False);
    msleep(600);
    write(innv, "Добрый вечер!", tsp, False);
    write(skoof, "В смысле... Вечер?", tsp, False);
    write(dc, "Ты видишь, как свет уличного фонаря проникает сквозь зазор между шторами.", tsp, False);
    write(dc, "Сочные городские сумерки проглотили твою типовую панельку и, переварив, изрыгнули в промозглый провинциальный вечер.", tsp, False);
    write(dc, "День остался позади.", tsp, False);
    write(game, "Встать с кровати.", tsp, True);
    input(press_enter);
  else:
    write(dc, "Легко и непринуждённо с грацией, которой позавидовала бы косуля (по крайней мере, тебе так кажется)...", tsp, False);
    write(dc, "...Ты соскакиваешь с повидавшей виды, но всё ещё добротной полутораспальной кровати.", tsp, False);
  msleep(300);
  write(innv, "БАБАХ!", tsp, False);
  write(dc, "Ты грузно и неуклюже падаешь на пол, неудобно подвернув руку под свой месистый живот.", tsp, False);
  write(skoof, "Сука!", tsp, False);
  write(dc, "Этот грохот, наверное, соседи на три этажа вниз слышали.", tsp, False);
  write(innv, "Тебя ждут великие дела!", tsp, False);
  write(skoof, "Окей, окей, уже встаю, товарищ начальник.", tsp, False);
  write(innv, "Я не начальник, я твой лучший ДРУУУГ!", tsp, False);
  write(dc, "Смурной и сонный, не найдя под кроватью шлёпанцы (чёрт бы их побрал), ты выходишь в коридор.", tsp, False);
  write(skoof, "Какого чёрта здесь так темно!?", tsp, False);
  write(innv, "Чего ты ещё ждал? Что на самом дне человеческого существования всё будет ярко, как в жаркий полдень из рассказа Рэя Бредберри?", tsp, False);
  write(skoof, "Заткнись, я не с тобой разговариваю!", tsp, False);
  write(innv, "А с кем тогда?", tsp, False);
  write(skoof, "Сам с собой, вот с кем!", tsp, False);
  write(innv, "Но ведь я и есть ты!", tsp, False);
  write(skoof, "Заткнись, заткнись!", tsp, False);
  write(innv, "А вдруг во тьме нас ждут ЧУДОВИЩА?", tsp, False);
  write(innv, "Которые только и мечтают, чтобы затянуть тебя вглубь своими склизкими щупальцами?", tsp, False);
  write(dc, ".............", tsp, False);
  write(skoof, "Ты дурак?", tsp, False);
  write(dc, "Диалог занимает в твоей голове не более полутора секунд, а рука сама привычно нашаривает рельеф выключателя на обшарпанной стене.", tsp, False);
  write(game, "Включить свет.", tsp, True);
  input(press_enter);
  write(skoof, "Теперь, когда фронт работ освещён, нам предстоит нелёгкий выбор.", tsp, False);
  write(skoof, "Куда мне отправиться дальше, дабы смахнуть остатки сна с моих нежных ресниц...", tsp, False);
  write(innv, "...Напоминающих обувную щётку?", tsp, False);
  write(skoof, "Цыц, здесь я делаю выбор, данный мне столь щедро создателем.", tsp, True);
  puts("0 -- Пойти в ванную\n1 -- Пойти в туалет\nНичего -- 0\n");
  current_place = input(">> ");
  if current_place[0]: current_place = s2i(current_place) % 2;
  else: current_place = 0;
  work_list = 3;
  # Ванная и туалет
  while (work_list):
    if ((current_place == 0) and (work_list & 2)):
      write(dc, "Грязная ванная встречает тебя с радостью похороонного служащего.", tsp, False);
      write(dc, "Совершенно очевидно, что вы друг другу чужие и ваши отношения -- в лучшем случае деловые.", tsp, False);
      write(dc, "В худшем -- токсичные созависимые.", tsp, False);
      write(skoof, "Так. Ты не нравишься мне, а я не нравлюсь тебе, леди ванная комната.", tsp, False);
      write(skoof, "Давай решим это по-быстрому.", tsp, False);
      write(skoof, "Все всё понимают!", tsp, False);
      write(skoof, "Мы взрослые люди.", tsp, False);
      write(innv, "Разговариваешь с ванной комнатой? Да ты совсем с катушек слетел, дружище.", tsp, False);
      write(skoof, "Не я, а мы. Ты же часть меня, разве забыл?", tsp, False);
      write(game, "Посмотреть в зеркало", tsp, True);
      input(press_enter);
      write(skoof, "Нет. Нет. Точно нет.", tsp, False);
      write(innv, "Откуда такая категоричность?", tsp, False);
      write(skoof, "Оно... Оно просто грязное. Там ничего не видно.", tsp, False);
      write(innv, "Так протри его, всего делов!", tsp, False);
      write(skoof, "Ну...", tsp, False);
      write(game, "Протереть зеркало", tsp, True);
      input(press_enter);
      write(dc, "Зажав в руке то, что когда-то называлось \"Полотенце Вафельное Гост 11027\", ты производишь несколько характерных вращательных движений.", tsp, False);
      write(dc, "Старое стекло приятно поскрипыывает под волокнами ткани.", tsp, False);
      write(dc, "Грязь отступает, давая зеркальной поверхности ещё один шанс засиять.", tsp, False);
      write(dc, "(Конечно же, при наличии источника направленного света).", tsp, False);
      write(innv, "Узри же, шакал!", tsp, False);
      write(skoof, "НЕТ!", tsp, False);
      write(skoof, "Я не хочу!", tsp, False);
      write(skoof, "Мне точно не понравится то, что я там увижу.", tsp, False);
      write(skoof, "Я же только проснулся и не хочу расстраиваться на весь день вперёд.", tsp, False);
      write(skoof, "Когда-то отражение в этом зеркале было словно репродукция картины эпохи Ренессанса...", tsp, False);
      write(skoof, "...Ну, одной из тех, на античную тематику.", tsp, False);
      write(skoof, "Но теперь...", tsp, False);
      write(skoof, "Чудовище, живущее там, может запросто выбраться наружу и убить нас всех!", tsp, False);
      write(innv, "Ты здесь один?", tsp, False);
      write(skoof, "КХАААА....", tsp, False);
      write(dc, "Хрип в твоей груди вот-вот сорвётся на сдавленные рыдания, но помощь приходит оттуда, откуда ты её вовсе не ждал.", tsp, False);
      write(innv, "Тише, тише, дружище.", tsp, False);
      write(innv, "Мы уже поняли, ванная команата -- не лучшее место для тебя.", tsp, False);
      write(innv, "Давай пойдём куда-нибудь ещё.", tsp, False);
      if (work_list & 1):
        write(game, "Пойти в туалет", tsp, True);
        input(press_enter);
        current_place = 1;
      else:
        write(game, "Выйти в коридор", tsp, True);
        input(press_enter);
      work_list = work_list - 2
    elif ((current_place == 1) and (work_list & 1)):
      write(skoof, "Пойти в туалет? Что ещё? Может просраться в сортире?", tsp, False);
      write(innv, "Ой, простите, ваше величество!", tsp, False);
      write(game, "Посетить уборную.", tsp, True);
      input(press_enter);
      write(skoof, "Самый важный ритуал за день.", tsp, False);
      write(skoof, "И по совместитьельству -- самый приятный!", tsp, False);
      write(game, "Просраться как следует?", tsp, True);
      puts("0 -- Да\n1 -- Да\n2 -- Da\n3 -- Чёрт возьми да!\nНичего -- 0\n");
      temp = input(">> ");
      if temp[0]: temp = s2i(temp) % 4
      else: temp = 0
      msleep(600);
      write(innv, "Поздравляю, ты получил +2 к здоровью и +3 к харизме!", tsp, False);
      write(skoof, "Правда? Было бы неплохо! Со здорозвьем у меня как раз проблемы.", tsp, False);
      write(innv, "Неправда!", tsp, False);
      write(skoof, "Чёрт.", tsp, False);
      write(skoof, "Кажется, все дела в коридоре мы закончили.", tsp, False);
      write(skoof, "Тьфу ты! Какие ещё \"дела\" могут быть в коридоре?..", tsp, False);
      write(skoof, "Ладно, не хочу об этом думать.", tsp, False);
      work_list = work_list - 1
      if work_list & 2:
        write(game, "Пойти в ванную", tsp, True);
        input(press_enter);
        current_place = 0;

  write(game, "Отправиться на кухню", tsp, True);
  input(press_enter);
  write(innv, "Ты имел в виду, <<Отправиться на скуфню>>?", tsp, False);
  write(skoof, "Нет, на кухню", tsp, False);
  write(game, "Отправиться на кухню", tsp, True);
  input(press_enter);
  write(innv, "<<Отправиться?>> да ты у нас не иначе как знаменитый путешественник Фернан Магеллан!", tsp, False);
  write(innv, "Который <<отправился>> в плавание вокруг света!", tsp, False);
  write(skoof, "Я не собираюсь никак это комментировать.", tsp, False);
  # Кухня
  write(dc, "Кухня встречает тебя привычным пейзажем.", tsp, False);
  write(dc, "Не то, чтобы ты хотел проводить здесь много времени.", tsp, False);
  write(dc, "Но будучи примерно половиной жилого пространства в типовой однокомнатной квартире...", tsp, False);
  write(dc, "...Эти квадратные метры играют не последнюю роль в твоей жизни.", tsp, False);
  write(skoof, "Есть хочется.", tsp, False);
  write(skoof, "Что у нас на зватрак?", tsp, False);
  write(innv, "Внутренний голос подсказывает тебе, что лучше даже не соваться в холодильник!", tsp, False);
  write(innv, "Просто поверь!", tsp, False);
  write(innv, "(Кстати, внутренний голос -- это я).", tsp, False); # Спасибо, Капитан Очевидность.
  write(dc, "Ты привык делить на шестнадцать любое мнение и заявление этого назойливого наглеца в твоей голове.", tsp, False);
  write(dc, "Но в этот раз ты почему-то склонен ему поверить.", tsp, False);
  write(dc, "Быть может, всё дело в том, что рецепторная память играет не последнюю роль в твоём теле?", tsp, False);
  write(dc, "Страшные, невыносимые по своей мерзости запахи вспыхивают в твоём разуме.", tsp, False);
  write(dc, "Гниль, только глубокая гниль, вызванная разве что разложением белковых форм жизни, может быть причиной такого уровня вони.", tsp, False);
  write(skoof, "Пожалуй, ты прав.", tsp, False);
  write(dc, "Твой взгляд скользит по обшарпан.. кхм...", tsp, False); # хахаха
  write(dc, "Видавшему виды, но всё ещё старому доброму столу", tsp, False);
  write(skoof, "Остатки былого праздника.", tsp, False);
  write(innv, "Интересно, есть ли пицца внутри коробки с пиццей?", tsp, False);
  write(skoof, "Есть только один способ узнать об этом.", tsp, False);
  write(game, "Открыть коробку", tsp, True);
  input(press_enter);
  write(dc, "Коробка открывается с лёгким хрустом гофрокартона, и твоему взору предстаёт он.", tsp, False);
  write(skoof, "Последний кусок.", tsp, False);
  write(innv, "Вчерашний!", tsp, False);
  write(dc, "Его величество вчерашний кусок пиццы.", tsp, False);
  write(dc, "Королевский завтрак.", tsp, False);
  write(dc, "Спонсор гастрита™", tsp, False);
  write(skoof, "Нас ждёт вопрос похлеще Гамлетовского.", tsp, False);
  write(game, "Разогреть или съесть так?", tsp, True);
  puts("0 -- Греть, греть!\n1 -- Сэкономим электричество, силы и время\nНичего -- 0\n");
  temp = input(">> ");
  if temp[0]: temp = s2i(temp) % 2
  else: temp = 0
  msleep(300);
  if (temp == 0):
    write(dc, "Величественно вращаясь, вчерашний кусок пиццы позволяет своим молекулам двигаться быстрее.", tsp, False);
    write(dc, "Тем самым делая свою структуру горячее, а твою жизнь -- чуточку приятнее.", tsp, False);
    msleep(600);
    write(innv, "ДИННЬ!!", tsp, False);
    write(skoof, "Ну, приятного аппетита!", tsp, False);
  write(dc, "Твои зубы вонзаются в упругую вчерашнюю хлебную плоть.", tsp, False);
  write(dc, "Челюсти мерно двигаются, перемалывая нехитрую начинку.", tsp, False);
  write(dc, "Если бы ты был в настольной ролевой игре, то это действие восстановило бы твои очки здоровья.", tsp, False); # отсылка на майнкрафт пре-беты 1.8
  write(dc, "Или дало заряд бодрости.", tsp, False); # отсылка на майнкрафт после беты 1.8
  write(dc, "Но ты не в настольной ролевой игре.", tsp, False);
  write(dc, "Ты на кухне в России.", tsp, False);
  write(skoof, "*Глоть*", tsp, False);
  write(dc, "Кусок камнем падает на дно твоего желудка.", tsp, False);
  write(skoof, "Доброе утро, пищеварительный тракт.", tsp, False);
  write(skoof, "Надеюсь, ты доволен.", tsp, False);
  write(skoof, "Завтрак окончен, пора приступать к делам.", tsp, False);
  write(innv, "Ха-ха, да какие у тебя могут быть дела!?", tsp, False);
  write(innv, "Скажи уж честно: иду играть в танки!", tsp, False);
  write(game, "Перейти в комнату", tsp, True);
  input(press_enter);
  write(skoof, "Ну вот мы и дома.", tsp, False);
  write(dc, "Пропёрженный стул смиренно принимает на себя твою увесистую душу.", tsp, False);
  write(dc, "Рука привычно нащупывает кнопку запуска.", tsp, False);
  write(dc, "Родной логотип компании, производящей программное обеспечение. СТАВЬТЕ ЛИНУКС!!!", tsp, False); # Линукс топ.
  write(dc, "Тёплый свет загрузочного экрана.", tsp, False);
  write(dc, "И вот ты готов отправиться в лучший из миров.", tsp, False);
  msleep(600);
  write(innv, "Танки!", tsp, False);
  write(innv, "ТАНКИ!", tsp, False);
  write(dc, "Привычным росчерком руки с зажатой в ней мышью ты подводишь курсор к столь милому сердцу ярлыку...", tsp, False);
  write(innv, "Жми!", tsp, False);
  write(skoof, "Что-то не то.", tsp, False);
  write(skoof, "Танки, танки, танки!", tsp, False);
  write(skoof, "Каждый день одни танки!", tsp, False);
  write(skoof, "У меня там уже ранг мастера, что дальше?", tsp, False);
  write(skoof, "Катка за каткой, куда ещё-то?", tsp, False);
  write(skoof, "Мля, как же скучно...", tsp, False);
  write(dc, "Твой разум затуманивает сигнал откуда-то снизу.", tsp, False);
  write(dc, "Расслабившись в тепле и уюте привычной берлоги...", tsp, False);
  write(dc, "Ты чувствуешь странный позыв, звучащий даже не у тебя в мозгу, но в крови", tsp, False);
  write(dc, "\"Тяночку бы......\"", tsp, False);
  write(innv, "Не забывай про танки!", tsp, False);
  write(innv, "Они даруют тебе привычное спокойствие обыденности!", tsp, False);
  write(innv, "ЖМИ!", tsp, False);
  write(dc, "Ты берёшь себя в руки, и наводишь курсор на заветную иконку.", tsp, False);
  write(game, "Запустить танки?", tsp, False);
  puts("0 -- Скучно\n1 -- Скучно\n2 -- Скучно\n3 -- Скучно\nНичего -- 0\n");
  temp = input(">> ");
  if temp[0]: temp = s2i(temp) % 4
  else: temp = 0
  msleep(300);
  write(innv, "СКУФНО!", tsp, False);
  write(innv, "ХА-ХА-ХА-ХА-ХА!", tsp, False);
  write(innv, "Кстати, а другие скуфы тоже знают, что они скуфы?", tsp, False); # момент, который поменял весь сюжет игры
  write(skoof, "Конечно, даже есть такой сервис государственный.", tsp, False);
  write(skoof, "Скуфуслуги!", tsp, False);
  write(innv, "Ты, должно быть, издеваешься?", tsp, False);
  write(innv, "Прикалываешься надо мной?!", tsp, False);
  write(skoof, "Да нет, смотри сам!", tsp, False);
  write(dc, "Пара кликов на мониторе, и волшебство широкополосного соединения переносит тебя...", tsp, False);
  write(dc, "...В бескрайний океан интернета", tsp, False);
  write(dc, "На экране загорается логотип, а после прогружается весь сайт полностью.", tsp, False);
  msleep(600);
  write(innv, "Вот это да! Ты не врал!", tsp, False);
  write(skoof, "Ага, между прочим, очень удобно, и пиндосов такого нет...", tsp, False);
  write(skoof, "Можно получить справки, выписки, проверить штрафы... Так, падажжи...", tsp, False);
  write(dc, "Твой взгляд внезапно падает на баннер, который ты раньше не видел.", tsp, False);
  write(dc, "Твоё сердце начинает биться чаще, словно карбюратор хорошо смазанной Лады 2104", tsp, False); # не понял шутку
  write(dc, "(Классическая машина, лучшая в серии)", tsp, False); # а теперь понял
  write(skoof, "Альтушка?", tsp, False); # ооо щас самая интересная часть начнётся
  write(skoof, "Это же тяночка, верно!", tsp, False);
  write(skoof, "Которая как её... гот... эмо?..", tsp, False);
  write(skoof, "А, неважно.", tsp, False);
  write(skoof, "Бесили меня всегда эти альтушки.", tsp, False);
  write(skoof, "Как вижу такую, так сразу хочется...", tsp, False);
  write(skoof, "Ну вы понимаете...", tsp, False); # я понял
  write(skoof, "Хочется такую...", tsp, False);
  write(innv, "Поняшить под пледиком!", tsp, False);
  write(skoof, "При этом понимаю, что такой девчонке навряд ли понравится сорокалетний мужик...", tsp, False);
  write(skoof, "К тому же жир... ОБЫЧНОГО телосложения..", tsp, False);
  write(skoof, "С короткой стрижкой.", tsp, False);
  write(skoof, "Дабы не получать урона самолюбию, я предпочитаю при встрече с такими не заявлять о себе.", tsp, False);
  write(skoof, "И от этого бешусь ещё больше.", tsp, False);
  write(skoof, "Эх...", tsp, False);
  write(skoof, "Эх, если бы такая девчонка полюбила меня просто так...", tsp, False);
  write(skoof, "Не за деньги, красоту, ум, или чувство юмора...", tsp, False);
  write(skoof, "А просто так!", tsp, False);
  write(dc, "Сладостная пелена застилает твой взор, эфирные грёзы затмевают твой разум.", tsp, False);
  write(dc, "Ты резко встряхиваешь головой, чтобы сбросить с себя наваждение.", tsp, False);
  msleep(300);
  write(skoof, "Так, какой у меня там пароль от Скуфуслуг?!", tsp, False);
  while pwds[0] or pwds[1] or pwds[2] or pwds[3]:
    write(game, "Введите пароль", tsp, True);
    temp = 4 # secret
    while pwds[temp] == 0:
      if pwds[0]: puts("0 -- skoof69\n")
      if pwds[1]: puts("1 -- i_wannafuckvirai_1\n")
      if pwds[2]: puts("2 -- sobakistan4\n")
      if pwds[3]: puts("3 -- Я не помню пароль\n")
      temp = input(">> ");
      if temp[0]: temp = s2i(temp) % 4
      else: temp = 0
    if ((temp == 0)):
      write(game, "Неверный пароль... Попробуйте ещё раз, уважаемый скуф!", tsp, False);
      pwds[0] = 0;
    elif ((temp == 1)):
      write(game, "Неверный пароль... Попробуйте ещё раз, уважаемый скуф!", tsp, False);
      pwds[1] = 0;
    elif ((temp == 2)):
      write(game, "Неверный пароль... Попробуйте ещё раз, уважаемый скуф!", tsp, False);
      pwds[2] = 0;
    elif ((temp == 3)):
      write(skoof, "Чёрт! Чёрт! Чёрт!", tsp, False);
      write(skoof, "Какой же там пароль!?", tsp, False);
      write(skoof, "Зачем я убрал этот стикер с монитора...", tsp, False);
      write(innv, "Дружище, не кипятись!", tsp, False);
      write(innv, "Может, я помогу тебе покопаться в глубинах твоего сознания?", tsp, False);
      write(game, "Согласиться на помощь внутреннего голоса?", tsp, False);
      input(press_enter);
      write(skoof, "Окей, терять уже нечего.", tsp, False);
      write(dc, "Ты закрываешь глаза.", tsp, False); # ооо щас долго будет
      pwds[0] = 0;
      pwds[1] = 0;
      pwds[2] = 0;
      pwds[3] = 0;
  write(dc, "Ты ныряешь в океан собственных мыслей, путешествуя по складу памяти.", tsp, False);
  write(dc, "Здесь не очень опрятно и много всякого хлама.", tsp, False);

  write(game, "ПРОДОЛЖЕНИЕ СЛЕДУЕТ... v0.1.3", tsp, False);

  return 0;
