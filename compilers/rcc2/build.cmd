: << batch
@echo off
D:\Toolkit\ash "%~f0"
exit /b %ERRORLEVEL%
batch

LC=~/lc/lc24/

build_c() {
    gcc $1 -o $2
}

build_mainc() {
    if [ "$DEBUG" = "1" ]; then
        gcc $1 -g -O1 -fno-omit-frame-pointer  -o $2
        # -fsanitize=address
    else
        gcc $1 -o $2
    fi
}

rcc() {
    if [ "$DEBUG" = "1" ]; then
        gdb -args ./rcc2 $1 -o $2
    else
        ./rcc2 $1 -o $2
    fi
}

kasm() {
    $LC/asm/las $1 $2_
    # cat $2_ data.bin > $2
    # rm $2_
    $LC/asm/las -e $1 $2.exp || true
}

kasm_bios() {
    $LC/asm/las -o 700000 $1 $2 || return $?
    # Govncrypt-2034
    # $LC/asm/las -o 700000 ../coms.s ../coms.bin || return $?
    # python3 ../govunix.py $2_ $2 || return $?
    # $LC/asm/las -o 700022 -e $1 $2.exp || true
}
gen_ascii() {
    ./$1 > $2
}

fail() {
    echo Error while executing $1: $3

    rm $2 && echo Removed $2 as it is probably broken
    exit 1
}

upd() {
    local src
    local target
    local tool

    src="$3"
    target="$1"
    tool="$2"

    shift
    shift
    local i
    for i in "$@"; do
        if [ "$i" -nt "$target" ] || [ ! -f "$target" ]; then
            printf "Building %s...\n" "$1"
            $tool $src $target || fail $tool $target $?
            break
        fi
    done
}

upd rcc2-asciitabl build_c lib/asciitabl.c
upd lib/tokenizer2.h gen_ascii rcc2-asciitabl

upd rcc2 build_mainc main.c $(grep "#include \"" main.c | cut -d \" -f 2)
