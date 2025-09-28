U8 ret_i[1] = {0};
U8 my_char[2] = {0, 0};
U0 geti() {
    ret_i = 0;
    my_char = '0';
    while(*my_char - '\n') {
        ret_i = (*my_char - '0') + 10 * *ret_i;
        my_char = getchar();
        puts(my_char);
    }
}
U0 main() {
    puts("GovnDate: ");
    puti(date());
    puts("\n");
    cls();
    ret_i = 0;
    while(*ret_i - 255) {
        0x400000 + rand() = 0x0F;
        ret_i = 1+*ret_i;
    }
    flip();
    puts("Press any key (NOT THE SHUTDOWN ONE)...");
    getchar();
    puts("\nFreq/10? ");
    geti();
    puts("Beep in 3");
    sleep(500);
    puts("\b2");
    sleep(500);
    puts("\b1");
    sleep(500);
    puts("\b\b\b\bNOW with ");
    puti(10* *ret_i);
    puts("Hz\n");
    beep(10* *ret_i);
}
