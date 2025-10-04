all:
	gcc utils/cpu.c -I/usr/pkg/include/SDL3 -Ilib -L/usr/pkg/lib -lSDL3 -lm -g -o builds/lc24
	gcc utils/ugovnfs1.3/main.c -Ilib -o builds/ugovnfs1.3
	gcc utils/ugovnfs2/main.c -o builds/mkfs.govnfs2
	gcc utils/ugovnfs2/gfs2.c -lm -o builds/ugovnfs2
	gcc utils/ugovnfs3/main.c -Ilib/ -lm -o builds/ugovnfs3
	gcc compilers/lcc/lcc.c -o builds/lcc
	gcc compilers/lrs/lrs.c -o builds/lrs
	gcc compilers/rcc2ge/main.c -o builds/rcc2ge
	cd compilers/rcc2 && ./build.cmd
	cp compilers/rcc2/rcc2 builds/rcc2
	cd utils/Lost86 && make
	cd utils/LostEFI && make clean ; make clean

install:
	install -m 755 builds/lc24 /usr/local/bin/
	install -m 755 utils/ugovnfs1.3/mkfs.govnfs1.3 /usr/local/bin/
	install -m 755 builds/mkfs.govnfs2 /usr/local/bin/
	install -m 755 builds/ugovnfs1.3 /usr/local/bin/
	install -m 755 builds/ugovnfs2 /usr/local/bin/
	install -m 755 builds/ugovnfs3 /usr/local/bin/
	install -m 755 compilers/las /usr/local/bin/
	install -m 755 compilers/lusl/lusl /usr/local/bin/
	install -m 755 builds/rcc2 /usr/local/bin/
	install -m 755 builds/rcc2ge /usr/local/bin/
	install -m 755 utils/govndatetime /usr/local/bin/

clean:
	rm -f builds/lc24
	rm -f lc24.efi
	rm -f builds/mkfs.govnfs2
	rm -f builds/ugovnfs1.3
	rm -f builds/ugovnfs2
	rm -f builds/ugovnfs3
	rm -f utils/LostEFI/lc24.efi
	rm -rf utils/LostEFI/Disk
	find . -name "*.bin" -type f | xargs -r rm -f
	find . -name "*.exp" -type f | xargs -r rm -f
	find . -name "*.sym" -type f | xargs -r rm -f
	find . -name "*.obj" -type f | xargs -r rm -f
	find . -name "*.b" -type f | xargs -r rm -f

efi:
	cd utils/LostEFI && mkdir -p Disk/EFI/BOOT
	cd compilers/lusl && ./run.sh run -b sh
	cp compilers/lusl/progs/compiled/sh.bin file.bin
	xxd -i file.bin > lib/lusl.h
	cd utils/LostEFI/ && make clean ; make
	mv utils/LostEFI/lc24.efi utils/LostEFI/Disk/EFI/BOOT/bootx64.efi
	rm file.bin

ovmf:
	sudo qemu-system-x86_64   -drive if=pflash,format=raw,unit=0,file=/usr/share/OVMF/x64/OVMF_CODE.4m.fd   -drive if=pflash,format=raw,unit=1,file=/usr/share/OVMF/x64/OVMF_VARS.4m.fd   -drive file=fat:rw:utils/LostEFI/Disk,format=raw,media=disk   -boot menu=on   -m 1024

ovmf9:
	sudo qemu-system-x86_64   -drive if=pflash,format=raw,unit=0,file=/usr/share/OVMF/x64/OVMF_CODE.4m.fd   -drive if=pflash,format=raw,unit=1,file=/usr/share/OVMF/x64/OVMF_VARS.4m.fd   -drive file=fat:rw:utils/LostEFI/Disk,format=raw,media=disk   -boot menu=on   -m 1024 -rtc base="2025-09-09",clock=vm

lost86:
	cd utils/Lost86 && make run

lost:
	make
	sudo make install
	make efi
	sudo make ovmf
	make lost86
