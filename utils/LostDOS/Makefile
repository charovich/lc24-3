all:
	@touch disk.img
	@truncate disk.img -s 64k
	@mkfs.govnfs1.3 disk.img -n Z
	@las boot/boot.asm boot/boot.bin
	@gboot disk.img boot/boot.bin
