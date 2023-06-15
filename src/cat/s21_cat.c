#include "s21_cat.h"

int main(int argc, char **argv) {
  if (argc > 1) {
    Flags flags;
    s21_init(&flags);
    if (s21_parse_flags(argc, argv, &flags) == 0) {
      for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') s21_output_result(argv[i], flags);
      }
    }
  }
  return 0;
}

void s21_init(Flags *flags) {
  flags->b = 0;
  flags->e = 0;
  flags->n = 0;
  flags->s = 0;
  flags->t = 0;
  flags->v = 0;
}

int s21_parse_flags(int argc, char **argv, Flags *flags) {
  int fail = 0;
  for (int i = 1; i < argc; i++) {
    int j = 1;
    if (argv[i][0] == '-' && argv[i][1] != '-') {
      while (argv[i][j] != '\0') {
        if (argv[i][j] == 'b') {
          flags->b = 1;
        } else if (argv[i][j] == 'e') {
          flags->e = 1;
          flags->v = 1;
        } else if (argv[i][j] == 'n') {
          flags->n = 1;
        } else if (argv[i][j] == 's') {
          flags->s = 1;
        } else if (argv[i][j] == 't') {
          flags->t = 1;
          flags->v = 1;
        } else if (argv[i][j] == 'v') {
          flags->v = 1;
        } else if (argv[i][j] == 'E') {
          flags->e = 1;
        } else if (argv[i][j] == 'T') {
          flags->t = 1;
        } else {
          fail = 1;
          break;
        }
        j++;
      }
    } else if (argv[i][0] == '-' && argv[i][1] == '-' && argv[i][2] != '-') {
      if (strcmp(argv[i], "--number-nonblank") == 0) {
        flags->b = 1;
      } else if (strcmp(argv[i], "--number") == 0) {
        flags->n = 1;
      } else if (strcmp(argv[i], "--squeeze-blank") == 0) {
        flags->s = 1;
      } else {
        fail = 1;
      }
      j++;
    } else if (argv[i][0] == '-' && argv[i][1] == '-' && argv[i][2] == '-') {
      fail = 1;
    }
  }
  return fail;
}

void s21_output_result(char *argv, Flags flags) {
  FILE *file;
  int c;
  if ((file = fopen(argv, "r")) != NULL) {
    int before_char = '\n';
    int count = 0;
    int i = 1;
    while ((c = fgetc(file)) != EOF) {
      if (flags.s && c == '\n' &&
          (before_char == '\n' || before_char == '\0')) {
        count++;
      } else {
        count = 0;
      }
      if (count <= 1) {
        if (((flags.n && before_char == '\n') && !flags.b) ||
            (flags.b && before_char == '\n' && c != '\n')) {
          printf("%6d\t", i++);
        }
        if ((!flags.t && !flags.e) && flags.v) {
          s21_flag_v(&c);
        }
        s21_flag_e(flags, &c);
        s21_flag_t(flags, &c);
        putchar(c);
        before_char = c;
      }
    }
    fclose(file);
  } else {
    fprintf(stderr, "s21_cat: %s: No such file or directory\n", argv);
  }
}

void s21_flag_e(Flags flags, int *c) {
  if (flags.e) {
    if (*c == '\n') {
      printf("$");
    }
    if (flags.v) {
      s21_flag_v(c);
    }
  }
}

void s21_flag_t(Flags flags, int *c) {
  if (*c == '\t' && flags.t) {
    *c = '^';
    putchar(*c);
    *c = 'I';
  }
  if (flags.v) {
    s21_flag_v(c);
  }
}

void s21_flag_v(int *c) {
  if ((*c >= 0 && *c <= 31 && *c != 9 && *c != 10)) {
    *c += 64;
    printf("^");
  } else if (*c == 127) {
    *c -= 64;
    printf("^");
  }
}