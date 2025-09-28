// Basic processor functions
struct lostdate {
  short year;
  char  month;
  char  day;
};
typedef struct lostdate lostdate;

lostdate lostdate_convert(unsigned short date) {
  return (lostdate){
    .year = date / 372 + 1970,
    .month = (date % 372) / 31 + 1,
    .day = (date % 372) % 31 + 1
  };
}

U16 LC_LOSTDATE() {
  time_t rawtm;
  struct tm* localtm;

  time(&rawtm);
  localtm = localtime(&rawtm);
  return (localtm->tm_mday - 1) + (localtm->tm_mon * 31) + (localtm->tm_year - 70) * 372;
}

U0 fatal(char* msg) {
  printf("lc24: \033[91mcannot operate\033[0m, error:\n  %s", msg);
}
