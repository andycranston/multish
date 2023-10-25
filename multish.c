static char *version = "@(!--#) @(#) multish.c, sversion 0.1.0, fversion 008, 25-october-2023";

/*
 *  multish.c
 *
 *
 *  generate custom lines of shell based on content in the 00multi.conf file
 *  
 */

/* ******************************************************************* */
/*
 *  includes
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

/* ******************************************************************* */

/*
 *  defines
 */

/* #define DEBUG */

#ifndef TRUE
#define TRUE
#endif

#ifndef FALSE
#define FALSE
#endif

#define DEFAULT_MULTISH_FILENAME "00Multish.conf"

#define DEFAULT_PADDING_CHARACTER '\0'

#define MAX_LINE_LENGTH 8192

#define EXIT_LINE "<<exit>>"

/* ******************************************************************* */

char *progname;

/* ******************************************************************* */

void rtrim(s)
  char *s;
{
  int   i;

  i = strlen(s);

  while (i >= 1) {
    i--;

    if (isspace(s[i])) {
      s[i] = '\0';
    } else {
      break;
    }
  }

  return;
}

/* ******************************************************************* */

void outputchar(c, p)
  char  c;
  char  p;
{
  if (c == p) {
    putchar(' ');
  } else {
    putchar(c);
  }

  return;
}

/* ******************************************************************* */

void outputstr(s, p)
  char *s;
  char  p;
{
  while (*s != '\0') {
    outputchar(*s, p);

    s++;
  }

  return;
}

/* ******************************************************************* */

void outputmidstr(s, p, start, length)
  char *s;
  char  p;
  int   start;
  int   length;
{
  s = s + start;

  while (length > 0) {
    if (*s == '\0') {
      break;
    }

    outputchar(*s, p);

    s++;

    length--;
  }

  return;
}

/* ******************************************************************* */

void outputarg(a, p, n, firstarg)
  char *a;
  char  p;
  int   n;
  char *firstarg;
{
  while (*a != '\0') {
    if (*a != '\\') {
      outputchar(*a, p);
    } else {
      a++;

      if (*a != '\0') {
        switch(*a) {
          case '#':
            printf("%04d", n);
            break;
          case '@':
            outputstr(firstarg, p);
            break;
          default:
            outputchar(*a, p);
            break;
        }
      }
    }

    a++;
  }

  return;
}

/* ******************************************************************* */

int main(argc, argv)
  int   argc;
  char *argv[];
{
  char  paddingchar;
  int   arg;
  char *multishfilename;
  FILE *multishfile;
  int   rawlinenum;
  int   logicallinenum;
  char  line[MAX_LINE_LENGTH];
  char *split[MAX_LINE_LENGTH];
  int   nsplits;
  char *option[MAX_LINE_LENGTH];
  int   noptions;
  char *defarg[MAX_LINE_LENGTH];
  int   ndefargs;
  char *scopy;
  int   i;
  char *thisopt;
  char *thisarg;

  progname = argv[0];

  multishfilename = DEFAULT_MULTISH_FILENAME;

  paddingchar = DEFAULT_PADDING_CHARACTER;

  if (argc >= 3) {
    if (strncmp(argv[1], "-f", MAX_LINE_LENGTH) == 0) {
      multishfilename = argv[2];
      argv = argv + 2;
      argc = argc - 2;
    }
  }

  if (argc >= 3) {
    if (strncmp(argv[1], "-p", MAX_LINE_LENGTH) == 0) {
      paddingchar = argv[2][0];
      argv = argv + 2;
      argc = argc - 2;
    }
  }

#ifdef DEBUG
  printf("*** Padding char [%c]\n", paddingchar);
#endif

  if ((multishfile = fopen(multishfilename, "r")) == NULL) {
    fprintf(stderr, "%s: unable to open multish file \"%s\" for reading\n", progname, multishfilename);
    exit(1);
  }

  rawlinenum = 0;
  logicallinenum = 0;

  while (fgets(line, MAX_LINE_LENGTH - 1, multishfile) != NULL) {
    rawlinenum++;

    rtrim(line);

    if (strlen(line) == 0) {
      continue;
    }

    if (line[0] == '#') {
      continue;
    }

    if (strncmp(line, EXIT_LINE, MAX_LINE_LENGTH - 1) == 0) {
      break;
    }

#ifdef DEBUG
    printf("[%s] [%d]\n", line, strlen(line));
#endif

    nsplits = 0;

    split[0] = strtok(line, " \t");

    if (split[0] != NULL) {
      nsplits++;

      while ((split[nsplits] = strtok(NULL, " \t")) != NULL) {
        nsplits++;
      }
    }

#ifdef DEBUG
    printf("nsplits=%d\n", nsplits);

    for (i=0; i<nsplits; i++) {
      printf("Split %d = [%s]\n", i, split[i]);
    }
#endif

    logicallinenum++;

    if (logicallinenum == 1) {
      for (i = 0; i < nsplits; i++) {
        scopy = strndup(split[i], MAX_LINE_LENGTH);

        if (scopy == NULL) {
          fprintf(stderr, "%s: out of memory on strndup call when copying options\n", progname);
          exit(1);
        }

        option[i] = scopy;
      }

      noptions = nsplits;

      continue;
    }

    if (logicallinenum == 2) {
      if (noptions != nsplits) {
        fprintf(stderr, "%s: first argument line %d in multish file \"%s\" must have same number of arguments as options line\n", progname, rawlinenum, multishfilename);
        exit(1);
      }

      for (i = 0; i < nsplits; i++) {
        if (strncmp(split[i], "\"", MAX_LINE_LENGTH) == 0) {
          fprintf(stderr, "%s: argument line %d in multish file \"%s\" cannot have a ditto (\") argument\n", progname, rawlinenum, multishfilename);
          exit(1);
        }
      }

      for (i = 0; i < nsplits; i++) {
        scopy = strndup(split[i], MAX_LINE_LENGTH);

        if (scopy == NULL) {
          fprintf(stderr, "%s: out of memory on strndup call when copy default arguments\n", progname);
          exit(1);
        }

        defarg[i] = scopy;
      }

      ndefargs = nsplits;

    }

    if (nsplits > noptions) {
      fprintf(stderr, "%s: argument line %d in multish file \"%s\" has too many arguments\n", progname, rawlinenum, multishfilename);
      exit(1);
    }

    while (nsplits < noptions) {
      split[nsplits] = "\"";

      nsplits++;
    }

    for (arg = 1; arg < argc; arg++) {
      printf("%s ", argv[arg]);
    }

    for (i = 0; i < noptions; i++) {
      thisopt = option[i];
      thisarg = split[i];

      if (strncmp(thisopt, "\"\"", MAX_LINE_LENGTH) != 0) {
        printf("%s ", thisopt);
      }

      if (strncmp(thisarg, "\"", MAX_LINE_LENGTH) == 0) {
        thisarg = defarg[i];
      } else {
        free(defarg[i]);
        scopy = strndup(thisarg, MAX_LINE_LENGTH);
        if (scopy == NULL) {
          fprintf(stderr, "%s: out of memory on strndup call when replacing default argument\n", progname);
          exit(1);
        }
        defarg[i] = scopy;
      }

      outputarg(thisarg, paddingchar, logicallinenum - 1, split[0]);

      if (i != (nsplits - 1)) {
        printf(" ");
      }
    }

    printf("\n");
  }

  fclose(multishfile);

  return 0;
}

/* ******************************************************************* */

/* end of file */
