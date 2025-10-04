U0 main() {
	flip();
	while (1) {
		puts("HID: ");
		puti(0x480005);
		puts(" ");
		puti(0x480006);
		puts(" ");
		puti(0x480007);
		puts(" ");
		puti(0x480008);
		puts(" ");
		puti(0x480009);
		puts(" ");
	}
}
