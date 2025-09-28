#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include <holyc-types.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/* File types */
#define TFILE 0x0
#define TDIR  0x1

/* Parent dir types */
#define PROOT 0x0
#define PBAD  0xFFFF

/* Cluster types */
#define CFREE 0x0000
#define CENDC 0xFFF8

I32  ac;
I8** av;

U8* disk=0;
U8 ugfserr=0xFF;
U32 disksz;

/* сектант разрабатывает по на раст для атаки фломастера через шанхай */
typedef struct header_t {
  U16 magic;
  U8 fsname[10];
  U32 serial;
  U16 filesoff;
  U16 clustoff;
  U8 oem[8];
  U8 letter;
  U8 _;
  U16 clustmax; /* реклама мессенджера MAX */
  U16 filesmax; /* ловит даже в говне      */
  U8 vollabel[11];
  U8 resrvd[469];
} __attribute__((packed)) header;
header head={0};

int tok(buf, delim, arr, sz)
char* buf;
char delim;
char** arr;
int sz; {
  char c;
  char* b=buf;
  char* p=(char*)buf;
  int cnt=0, pt=0;
  char* p2;
  while ((c = *b) && cnt < sz) {
    if (c == delim) {
      *b = 0;
      memset(arr[cnt], ' ', 11);
      memcpy(arr[cnt], p, strlen(p));
      cnt++;
      p = b+1;
    }
    b++; /* B succesor */
  }
  *b = 0;
  memset(arr[cnt], ' ', 11);
  p2=p;
  while (((c = *p) != '.') && (*p)) {
    arr[cnt][pt++] = c;
    p++;
  }
  if (strchr(p2, '.')) memcpy(arr[cnt]+8, p+1, 3);
  arr[0][0] = '/';
  return cnt;
}

U8 dirpath(I8* str, U16 strsz, U16 dirind) {
  U32 addr;
  U32 len;
  U8 type;
  I8* s=str;
  U8 ind=0;
  strsz--;
  s += strsz;
  memset(str, 0x00, 256);
  while (1) {
    addr = (head.clustoff*0x800)+(dirind*0x020);
    type = disk[addr+0xB];
    if (type != 0x01) {
      fprintf(stderr, "Error! Entry %X is not a directory\n", dirind);
      return -1;
    }
    while (ind < 11) {
      if (disk[addr+ind] == ' ') {
        disk[addr+ind] = '\0';
        break;
      }
      ind++;
    }
    len = strlen((I8*)disk+addr);
    s -= len+1;
    strcpy(s, (I8*)disk+addr);
    disk[addr+ind] = ' ';
    ind = 0;

    if (disk[addr] == '/') { /* root reached */
      break;
    }
    s[len] = '/';
    dirind = *(U16*)(disk+addr+0xE);
  }
  return 0;
}

U16 inodeidx(I8* name, U16 parent, U8 t) {
  U32 addr, cnt=0, total=head.clustmax;
  U8 type;
  t = !!t;

  addr = head.clustoff*0x800;
  if (name[0] == '/') return 0x0000;
  while (1) {
    type = disk[addr+0xB];
    if ((type == t) && (!memcmp(name, (I8*)disk+addr, 11)) && ((*(U16*)(disk+addr+0xE)) == parent))
      return (addr - head.clustoff*0x800)/0x020;
    else if ((cnt+1) == total) return 0xFFFF;
    addr += 0x020;
    cnt++;
  }
}

U32 rdfile(I8* name, U16 parent, U8* buf, U32 sz) {
  U8* b = buf;
  U16 inode = inodeidx(name, parent, 0x0);
  U32 addr = (head.clustoff*0x800)+(inode*0x020);
  U16 clust = *(U16*)(disk+addr+0xC);
  U16* caddr = (U16*)(disk+1*0x800);
  U32 faddr = 0;
  U32 read = 0;
  if (clust == CFREE) {
    fprintf(stderr, "error: file owns 0 clusters\n");
    return 0;
  }
  while (1) {
    puts((I8*)(disk+faddr+(clust*0x800)));
    memcpy(b, disk+faddr+(clust*0x800), 2048);
    b += 2048;
    sz -= 2048;
    read += 2048;
    // printf("$%04X -> $%04X\n", clust, caddr[clust]);
    if ((clust = caddr[clust]) == CENDC) return read;
  }
}

U32 irdfile(U16 inode, U8* buf, U32 sz) {
  U8* b = buf;
  U32 addr = (head.clustoff*0x800)+(inode*0x020);
  U16 clust = *(U16*)(disk+addr+0xC);
  U16* caddr = (U16*)(disk+1*0x800);
  U32 faddr = 0;
  U32 read = 0;
  if (clust == CFREE) {
    fprintf(stderr, "error: file owns 0 clusters\n");
    return 0;
  }
  while (1) {
    memcpy(b, disk+faddr+(clust*0x800), 2048);
    b += 2048;
    sz -= 2048;
    read += 2048;
    // printf("$%04X -> $%04X\n", clust, caddr[clust]);
    if ((clust = caddr[clust]) >= CENDC) return read;
  }
}

U16 pathidx(I8* path, U8 file) {
  #define siz 12
  I8* p[siz];
  U8 i, cnt;
  U16 parent;
  for (i=0; i<siz; i++) {
    p[i] = malloc(12);
  }
  if (path[0] != '/') {
    fputs("error: path given to pathidx does not start from root\n", stderr);
    for (i=0; i<siz; i++) {
      free(p[i]);
    }
    return 0xFFFF;
  }
  if ((path[strlen(path)-1] == '/') && file) {
    fputs("error: pathidx says that no file was given\n", stderr);
    for (i=0; i<siz; i++) {
      free(p[i]);
    }
    return 0xFFFF;
  }
  cnt = tok(path, '/', &p, 12);
  for (i=0; i<cnt; i++) {
    // printf("%s\t", p[i]);
    parent = inodeidx(p[i], parent, TDIR);
    if (parent == PBAD) {
      fprintf(stderr, "error: directory %s does not exist\n", p[i]);
      for (i=0; i<siz; i++) {
        free(p[i]);
      }
      return 0xFFFF;
    }
  }
  if (file) {
    parent = inodeidx(p[cnt], parent, TFILE);
    if (parent == PBAD) {
      fprintf(stderr, "error: file %s does not exist\n", p[cnt]);
      for (i=0; i<siz; i++) {
        free(p[i]);
      }
      return 0xFFFF;
    }
  }
  for (i=0; i<siz; i++) {
    free(p[i]);
  }
  // printf("entry id: $%04X, first cluster: $%04X\n", parent, (*(U16*)(disk+(head.clustoff*0x800+parent*0x020)+0xC))); /* puts(p[cnt]); */
  return parent;
}

U16 freclust() {
  U16* d = (U16*)(disk+0x860);
  U16 i = 48;
  while (*d) {
    d++;
    i++;
  }
  return i;
}

U16 freinode() {
  U8* d = disk+(head.clustoff*0x800);
  U16 i = 0;
  while (*d) {
    d += 32;
    i++;
  }
  return i;
}

U32 dircr(U8* dpath, U8* name) {
  U16 free = freinode();
  U32 inode = (head.clustoff * 0x800) + (free * 0x020);
  U16 parent = pathidx((I8*)dpath, 0);
  if (parent == 0xFFFF) {
    fprintf(stderr, "error: fuck you!!\n");
    return 0xFFFF;
  }

  memset(disk + inode, 0x00, 32);
  memset(disk + inode, 0x20, 11);
  memcpy(disk + inode, name, MIN(strlen((I8*)name), 11));
  disk[inode + 0xB] = 0x01;
  *(U16*)(disk + inode + 0xE) = parent;
  *(U32*)(disk + inode + 0x10) = 0;
  return free;
}

U32 writef(U8* dpath, U8* name, U8* buf, U32 sz) {
  U16 free = freinode();
  U8 i=0;
  if (free == 0xFFFF) {
    fprintf(stderr, "error: ur disk is full\n");
    return 0xFFFF;
  }
  /* printf("heres your free inode: $%04X\n", free); */

  U32 inode = (head.clustoff * 0x800) + (free * 0x020);
  memset(disk + inode, 0x00, 32);
  while ((name[i]) && (name[i] != '.') && (i <= 8)) {
    i++;
  } i--;
  memset(disk + inode, 0x20, 11);
  memcpy(disk + inode, name, MIN(i+1, 8));
  memcpy(disk + inode + 8, name+i+2, MIN(strlen((I8*)(name+i+1)), 3));
  disk[inode + 0xB] = 0x00;
  *(U32*)(disk + inode + 0x10) = sz;

  U16 parent = pathidx((I8*)dpath, 0);
  if (parent == 0xFFFF) {
    fprintf(stderr, "error: fuck you!!\n");
    return 0xFFFF;
  }

  *(U16*)(disk + inode + 0xE) = parent;

  U32 amount = 0;
  U16 clust1 = 0xFFFF;
  U16 clust2 = 0xFFFF;

  while (amount < sz) {
    U16 fclust = freclust();
    if (fclust == 0xFFFF) {
      fprintf(stderr, "error: fuck you!!!\n");
      return 0xFFFF;
    }

    *(U16*)(disk + 0x800 + fclust * 2) = 0xFFFF;

    if (clust1 == 0xFFFF) {
      clust1 = fclust;
      *(U16*)(disk + inode + 0xC) = clust1;
    } else {
      *(U16*)(disk + 0x800 + clust2 * 2) = fclust;
    }

    U32 csz = (sz - amount) > 2048 ? 2048 : (sz - amount);
    memcpy(disk + fclust * 0x800, buf + amount, csz);

    amount += csz;
    clust2 = fclust;
  }
  *(U16*)(disk + 0x800 + clust2 * 2) = 0xFFF8;

  return free;
}

U32 ulist() {
  U32 addr = head.clustoff*0x800, cnt=0, total=head.filesmax;
  U8 type;
  U8 ind=0;
  puts("\tname            clust# inode# size#");
  while (1) {
    type = disk[addr+0xB];
    if ((cnt+1) == total) break;
    if ((type == 0x00) && (disk[addr] != '\0')) {
      printf("\t%.11s     $%04X  $%04X  %u\n", disk+addr, *(U16*)(disk+addr+0xC), (addr-0x8000)/0x020, *(U32*)(disk+addr+0x10));
    }
    else if ((type == 0x01) && (disk[addr] != '/') && (disk[addr] != '\0')) {
      while (ind < 11) {
        if (disk[addr+ind] == ' ') {
          disk[addr+ind] = '\0';
          break;
        }
        ind++;
      }
      printf("\t%.11s/\n", disk+addr);
      disk[addr+ind] = ' ';
    }

    addr += 0x020;
    cnt++;
  }
  return 0;
}

U32 uread() {
  U8 b[102400] = {0};
  I8* av3 = strdup((I8*)av[3]);
  U16 s = pathidx(av[3], 1);
  if (s == 0xFFFF) {
    fprintf(stderr, "error reading file: `%s' not found\n", av3);
    return 0;
  }
  U32 r = irdfile(s, b, 102400);
  write(1, (I8*)b, r);
  fflush(stdout);
  return 0;
}

U32 uformat(U8 letter) {
  U32 val=0;
  /* Sector 0 */
  memset(&head, 0x00, 512);
  head.magic    = 0xAE42;
  memcpy(head.fsname, "GOVNFS3.0 ", 10);
  val = rand()<<17;
  val |= rand();
  head.serial   = val;
  head.filesoff = 0x0030;
  head.clustoff = 0x0010;
  memcpy(head.oem, "ugovnfs ", 8);
  head.letter   = letter;
  head.clustmax = 0x3C00;
  head.filesmax = 0x0800;
  memcpy(head.vollabel, "GOVNDISK   ", 11);

  /* GCAT */
  memcpy(disk, &head, 512);
  memset(disk+512, 0x00, disksz-512);
  memcpy(disk+0x0800, "Sorry, but these govnclusters are not accessible in the UK. Choose the way to confirm your age:\0", 96);
  // memcpy(disk+0x0860, (char[2]){0xFF,0xFF}, 2);

  /* Directory entries */
  memset(disk+0x8000, 0x00, 32*head.clustmax);
  memcpy(disk+0x8000, "/          \x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 32);

  return 0;
}

U32 uwrite() {
  FILE* f;
  U8 dpath[11];
  strcpy((I8*)dpath, av[3]);
  if (!(f = fopen(av[5], "rb"))) {
    fprintf(stderr, "couldn't open file `%s'\n", av[5]);
    return 0xFFFF;
  }
  fseek(f, 0, SEEK_END);
  U32 rsz = ftell(f);
  U32 sz = (rsz+2047)/2048*2048;
  fseek(f, 0, SEEK_SET);
  U8* buf = malloc(sz);
  fread(buf, rsz, 1, f);
  fclose(f);
  printf("writing %d bytes ", sz);
  if (writef(dpath, (U8*)av[4], buf, rsz)) return 1;
  free(buf);
  return 0;
}

U32 umkdir() {
  U8 dpath[11];
  strcpy((I8*)dpath, av[3]);
  dircr(dpath, (U8*)av[4]);
  return 0;
}

I32 main(I32 argc, I8** argv) {
  ac = argc;
  av = argv;
  srand(time(NULL));
  /* puts("ugovnfs3 for GovnFS 3.0");
  puts("© 7E9, All rights and lefts keked\n"); */
  if (argc == 1) {
    puts("ugovnfs3: no arguments given");
    return 1;
  }
  if (argc == 2) {
    puts("ugovnfs3: no disk/flag given");
    return 1;
  }
  FILE* fl = fopen(argv[2], "rb+");
  if (fl == NULL) {
    printf("ugovnfs3: \033[91mfatal error:\033[0m file `%s` not found\n", argv[1]);
    return 1;
  }

  fseek(fl, 0, SEEK_END);
  disksz = ftell(fl);
  disk = malloc(disksz);
  fseek(fl, 0, SEEK_SET);
  fread(disk, 1, disksz, fl);
  fseek(fl, 0, SEEK_SET);

  memcpy(&head, disk, 512);

  /* Format before disk checking :D */
  if (!strcmp(argv[1], "format")) {
    ugfserr = uformat(argv[3]? argv[3][0]: 'Z');
    printf("\033[92mdone\033[0m\n");
    fwrite(disk, 1, disksz, fl);
    goto next;
  }
  /* Check the disk */
  if (((U16*)disk)[0x000000] != 0xAE42) {
    printf("ugovnfs3: \033[91mdisk corrupted:\033[0m expected magic `$42AE`, got `$%04X`\n", ((U16*)disk)[0x000000]);
    goto err;
  }

  if (!strcmp(argv[1], "list")) {
    printf("Listing %c/\n", head.letter);
    ugfserr = ulist();
    putchar(10);
  }
  else if (!strcmp(argv[1], "read")) {
    ugfserr = uread();
  }
  else if (!strcmp(argv[1], "write")) {
    ugfserr = uwrite();
    printf("\033[92msuccess\033[0m\n");
    fwrite(disk, 1, disksz, fl);
  }
  else if (!strcmp(argv[1], "mkdir")) {
    ugfserr = umkdir();
    fwrite(disk, 1, disksz, fl);
  }
  else {
    ugfserr = 0;
    fprintf(stderr, "error: unknown argument `%s'\n", argv[1]);
  }
next:
  fclose(fl);
  if (ugfserr == 0xFF) {
    puts("ugovnfs3: no arguments given");
    goto end;
  }
end:
  free(disk);
  return 0;
err:
  free(disk);
  return 0;
}

