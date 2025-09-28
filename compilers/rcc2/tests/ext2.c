#include <libc/std.h>
#include <libc/diskio.h>

typedef struct {
    U8 number;
    U32 sector_size;
    U32 inode_per_blk;
    U16 inode_size;
    U32 bgdt;
} Disk;

typedef enum {
    IFile,
    IDirectory,
    IInternal
} FileType;

typedef struct {
    Disk* dsk;
    FileType ft;
    U32 pos;
    U8 status;
    U32 len;
    U32 inode_pos;
    U32 dp;
} File;

typedef enum {
    Ok,
    Error
} ERR;

typedef enum {
    ScratchBuf = 0x1000000
} NiceAddress;

U32 breakpoint() = "trap" "rts";

ERR init_ext2(Disk* dsk) {
    {
        U16 sig;
        read_disk(dsk->number, 1080, 2, &sig);
        if (sig != 0xef53) return Error;
    }
    {
        U32 mjver;
        read_disk(dsk->number, 1100, 4, &mjver);
        if (mjver != 1) return Error;
    }
    {
        U32 flags;
        read_disk(dsk->number, 1120, 2, &flags);
        if (flags != 0) if (flags != 2) return Error;
    }
    read_disk(dsk->number, 1112, 2, &dsk->inode_size);
    {
        U32 log2_blk;
        read_disk(dsk->number, 1052, 4, &log2_blk);
        if (log2_blk > 2) return Error;
        dsk->sector_size = 1024 << log2_blk;
        if (log2_blk == 0) dsk->bgdt = 2048;
        else dsk->bgdt = dsk->sector_size;
    }
    read_disk(dsk->number, 1064, 4, &(dsk->inode_per_blk));
    return Ok;
}

ERR open_inode(Disk* dsk, File* file, U32 inode) {
    --inode;
    U32 block = inode / (dsk->inode_per_blk);
    puts("blk=%x\n", block);
    U32 table;
    read_disk(dsk->number, dsk->bgdt + block * 32 + 8, 4, &table);
    table = table * dsk->sector_size;
    table = (inode % dsk->inode_per_blk) * dsk->inode_size + table;
    file->dsk = dsk;
    file->inode_pos = table;
    file->pos = 0;

    U16 itp;
    read_disk(dsk->number, table, 2, &itp);
    itp = itp >> 12;
    if (itp == 4) file->ft = IDirectory;
    else if (itp == 8) file->ft = IFile;
    else file->ft = IInternal;

    read_disk(dsk->number, table + 4, 4, &(file->len));

    //read_disk(dsk->number, table + 40, 4, &(file->dp));
    //file->dp = file->dp * file->dsk->sector_size;
    file->dp = 1024;
    file->status = 1;
    return Ok;
}

U32 read_file(File* file, U32 count, U8* buf) {
    while (1) {
        {
            U32 tmp = file->len - file->pos;
            if (tmp < count) {
                count = tmp;
            }
        }
        if (!count) {
            return 0;
        }
        U32 ds = file->dsk->sector_size;
        U32 rem = ds - (file->pos % ds);
        if (rem == ds && file->status) {
            if (file->pos < ds * 12) {
                read_disk(file->dsk->number, (file->pos / ds) * 4 + 40 + file->inode_pos, 4, &(file->dp));
            } else if (file->pos < ds * (ds / 4 + 12)) {
                 U32 tbase;
                 read_disk(file->dsk->number, file->inode_pos + 88, 4, &tbase);
                 tbase = tbase * ds;
                 U32 tidx = ((file->pos - ds * 12) / ds) * 4;
                  read_disk(file->dsk->number, tbase + tidx, 4, &(file->dp));
            } else{
                puts("\x1b[41mУпс! Ваш говнокор устал и он решил выключится.\x1b[0m\n");
                exit(0);
            }
            file->dp = file->dp * ds;
            file->status = 0;
        }
        if (count < rem) {
            rem = count;
        }
        if (rem) {
            read_disk(file->dsk->number, file->dp, rem, buf);
            buf = buf + rem;
            count = count - rem;
            file->pos = file->pos + rem;
            file->dp = file->dp + rem;
            file->status = 1;
        }
    }
}
typedef struct {
    U32 inode;
    U16 e_size;
    U8 name_len;
    U8 type_ind;
} Ext2DirEnt;
U8 main() {
    puts("PRos Killer 2025\nЗагрузка Списка Файлов...");
    Disk dsk;
    dsk.number = 0;
    if (init_ext2(&dsk) == Ok) puts("Ура, ext2!\n");
    else {puts("Не люблю эту файловую систему...\n"); return 1;}
    // Open root directory
    File root;
    open_inode(&dsk, &root, 2);
    puts("ls -i /\n");
    while ((&root)->pos < (&root)->len) {
        read_file(&root, 8, ScratchBuf);
        U32 nl =  ((Ext2DirEnt*)ScratchBuf)->name_len;
        U32 tl =  ((Ext2DirEnt*)ScratchBuf)->e_size;
        U32 in =  ((Ext2DirEnt*)ScratchBuf)->inode;
        read_file(&root, nl, ScratchBuf);
        *(U8*)(ScratchBuf+nl) = 0;
        if (*(U8*)ScratchBuf != '.' && in) {
            puts("%d %s\n", in, ScratchBuf);
        }
        read_file(&root, tl-nl-8, ScratchBuf);
    }
}
