#include "s21_grep.h"

int main(int argc, char *argv[]) {
  unsigned short flags = 0;
  if (argc >= 3) {
    get_flags(&flags, argv, argc);
    if (flags != USHRT_MAX) {
      regex_list *head = NULL;
      short regex_position = get_regular_expressions(&head, argc, argv, &flags);
      count_files(&flags, argc, argv, regex_position);
      for (int i = 1; i < argc; i++) {
        if (!is_flag(argv[i], &flags) && is_file(argv[i]) &&
            (!check_for_flag("ef", flags, argv, i))) {
          int match = 0;
          int v_flag_counter = 0;
          int line_number = 1;
          match = process_file(head, &flags, argv[i], &v_flag_counter,
                               &line_number);
          if (flags & C_FLAG)
            print_count_of_lines(match, v_flag_counter, flags, argv[i]);
          if (flags & L_FLAG && match > 0) printf("%s\n", argv[i]);
        } else {
          print_error_msg(argv, i, &flags, regex_position);
        }
      }
      destroy_linked_list(&head);
    } else {
      printf("error: Invalid argument\n");
    }
  } else {
    printf("error: not enough arguments \n");
  }
  return 0;
}

void get_flags(unsigned short *flags, char *argv[], int argc) {
  for (int i = 1; i < argc && *flags != USHRT_MAX; i++) {
    if (is_flag(argv[i], flags)) {
      for (int j = 1; argv[i][j] != '\0'; j++) {
        switch (argv[i][j]) {
          case 'e':
            *flags |= E_FLAG;
            break;
          case 'i':
            *flags |= I_FLAG;
            break;
          case 'v':
            *flags |= V_FLAG;
            break;
          case 'c':
            *flags |= C_FLAG;
            break;
          case 'l':
            *flags |= L_FLAG;
            break;
          case 'n':
            *flags |= N_FLAG;
            break;
          case 'h':
            *flags |= H_FLAG;
            break;
          case 's':
            *flags |= S_FLAG;
            break;
          case 'f':
            *flags |= F_FLAG;
            break;
          case 'o':
            *flags |= O_FLAG;
            break;
          default:
            *flags = USHRT_MAX;
        }
      }
    }
  }
}

void count_files(unsigned short *flags, int argc, char *argv[],
                 short regex_position) {
  int count = 0;
  for (int i = 1; i < argc; i++) {
    if (i != regex_position) {
      if (is_file(argv[i]) &&
          (!(*flags & F_FLAG) || !check_for_flag("f", *flags, argv, i)) &&
          (!(*flags & E_FLAG) || !check_for_flag("e", *flags, argv, i))) {
        count++;
      }
      if (count > 1) {
        *flags |= MULTIPLE_FILES;
      }
    }
  }
}

bool is_flag(const char *argument, unsigned short *flags) {
  bool flag = false;
  if (argument[0] == '-' && argument[1] != '\0') {
    if (strspn(argument + 1, "eivclnhsfo")) {
      flag = true;
    } else {
      *flags = USHRT_MAX;
      flag = false;
    }
  } else if (argument[0] == '-' && argument[1] == '\0') {
    flag = false;
  }
  return flag;
}

bool is_file(char *argv_name) {
  bool f = false;
  FILE *checker;
  checker = fopen(argv_name, "r");
  if (checker == NULL) {
  } else {
    f = true;
    fclose(checker);
  }
  return f;
}

short get_regular_expressions(regex_list **head, int argc, char *argv[],
                              unsigned short *flags) {
  *head = (regex_list *)malloc(sizeof(regex_list));
  bool regex_argument_found = 0;
  short regex_position = -1;
  int f = 0;
  if (head == NULL) {
    printf("memory allocation failed\n");
    free(head);
    exit(1);
  }
  (*head)->link = NULL;
  if (*flags & F_FLAG) {
    get_regex_from_file(*head, argc, argv, *flags);
    f++;
    regex_argument_found = 1;
  }
  regex_t regex;
  for (int j = 1; j < argc; j++) {
    if ((*flags & E_FLAG) && (check_for_flag("e", *flags, argv, j))) {
      compile_regular_expression(&regex, argv[j], *flags);
      if (!f) {
        (*head)->regular_expression = regex;
        f++;
      } else {
        list_add_at_tail(regex, *head);
      }
      regex_argument_found = 1;
    } else if (!(*flags & E_FLAG) && !regex_argument_found &&
               !is_flag(argv[j], flags)) {
      compile_regular_expression(&regex, argv[j], *flags);
      regex_argument_found = 1;
      regex_position = j;
      if (!f) {
        (*head)->regular_expression = regex;
        *flags |= REGEX_FOUND;
        f++;
      } else {
        list_add_at_tail(regex, *head);
      }
    }
  }
  return regex_position;
}

bool check_for_flag(char *ef_flags, unsigned short flags, char *argv[], int j) {
  bool output = false;
  if ((flags & F_FLAG || (flags & E_FLAG)) && (is_flag(argv[j - 1], &flags))) {
    output =
        (strcspn(argv[j - 1] + 1, ef_flags) - strlen(argv[j - 1] + 1)) != 0;
  }
  return output;
}

void compile_regular_expression(regex_t *regex, char *argv,
                                unsigned short flags) {
  if (flags & I_FLAG)
    regcomp(regex, argv, REG_EXTENDED | REG_ICASE);
  else
    regcomp(regex, argv, REG_EXTENDED);
}

void get_regex_from_file(regex_list *head, int argc, char *argv[],
                         unsigned short flags) {
  int f = 0;
  size_t buffsize = 0;
  FILE *file_ptr = NULL;
  for (int k = 1; k < argc; k++) {
    if (check_for_flag("f", flags, argv, k)) {
      char *line_buffer = NULL;
      if ((file_ptr = fopen(argv[k], "r"))) {
        while (getline(&line_buffer, &buffsize, file_ptr) > 0) {
          regex_t regex;
          replace_cr(line_buffer, '\n');
          compile_regular_expression(&regex, line_buffer, flags);
          if (!f) {
            head->regular_expression = regex;
            f++;
          } else {
            list_add_at_tail(regex, head);
          }
        }
        free(line_buffer);
      }
      fclose(file_ptr);
    }
  }
}

int process_file(regex_list *head, unsigned short *flags, char *filename,
                 int *v_flag_counter, int *line_number) {
  char *buffer = NULL;
  size_t buffsize = 0;
  FILE *file_stream;
  int match_exists = 0;
  file_stream = fopen(filename, "r");
  *line_number = 1;
  *v_flag_counter = 0;
  while (getline(&buffer, &buffsize, file_stream) != -1) {
    if (!(*flags & O_FLAG)) {
      match_exists += print_matched_line(head, buffer, *flags, filename,
                                         line_number, v_flag_counter);
    } else {
      if (!(*flags & V_FLAG)) {
        match_exists += print_only_matched_parts(head, buffer, *flags, filename,
                                                 line_number);
      } else {
        match_exists += print_matched_line(head, buffer, *flags, filename,
                                           line_number, v_flag_counter);
      }
    }
    *v_flag_counter = match_exists;
  }
  free(buffer);
  fclose(file_stream);
  return match_exists;
}

bool print_matched_line(regex_list *head, char *buffer, unsigned short flags,
                        char *filename, int *line_number, int *v_flag_counter) {
  bool f = 0;
  char multi = 1;
  regex_list *tmp = head;
  while (tmp != NULL && !f) {
    int checker = regexec(&(tmp->regular_expression), buffer, 0, NULL, 0);
    if (flags & V_FLAG) {
      multi *= checker;
      checker = multi == 0 ? 1 : 0;
      if (tmp->link != NULL) {
        tmp = tmp->link;
        continue;
      }
    }
    if (!checker) {
      if (!(flags & C_FLAG) && !(flags & L_FLAG)) {
        if ((flags & MULTIPLE_FILES) && !(flags & H_FLAG)) {
          printf("%s:", filename);
          if (flags & N_FLAG) printf("%d:", *line_number);
          printf("%s", buffer);
          if (!strchr(buffer, '\n')) printf("\n");
        } else {
          if (flags & N_FLAG) printf("%d:", *line_number);
          printf("%s", buffer);
          if (!strchr(buffer, '\n')) printf("\n");
        }
        f = 1;
      } else {
        *v_flag_counter += 1;
        f = 1;
      }
    }
    tmp = tmp->link;
  }
  (*line_number)++;
  return f;
}

bool print_only_matched_parts(regex_list *head, char *buffer,
                              unsigned short flags, char *filename,
                              int *line_number) {
  regmatch_t pmatch;
  regex_list *tmp = head;
  bool match = 0;
  while (tmp != NULL) {
    size_t offset = 0;
    while (offset < strlen(buffer)) {
      int checker =
          regexec(&(tmp->regular_expression), buffer + offset, 1, &pmatch, 0);
      if (checker != 0) {
        break;
      } else if (pmatch.rm_so != -1) {
        char *p = getsubstr(buffer + offset, &pmatch);
        if (!(flags & C_FLAG) && !(flags & L_FLAG)) {
          if ((flags & MULTIPLE_FILES) && !(flags & H_FLAG)) {
            if (!(flags & EXTRA_DATA)) printf("%s:", filename);
            if (!(flags & EXTRA_DATA) && flags & N_FLAG)
              printf("%d:", *line_number);
            printf("%s", p);
            flags |= EXTRA_DATA;
            if (!strchr(p, '\n')) printf("\n");
          } else {
            if (!(flags & EXTRA_DATA) && flags & N_FLAG)
              printf("%d:", *line_number);
            printf("%s", p);
            flags |= EXTRA_DATA;
            if (!strchr(p, '\n')) printf("\n");
          }
        }
        match = 1;
      }
      offset += pmatch.rm_eo;
    }
    tmp = tmp->link;
  }
  (*line_number)++;
  return match;
}

void print_count_of_lines(int match, int vflag_counter,
                          unsigned short int flags, char *filename) {
  if (flags & L_FLAG) vflag_counter = match > 0 ? 1 : 0;
  if (((flags & C_FLAG) && !(flags & MULTIPLE_FILES)) || (flags & H_FLAG))
    printf("%d\n", vflag_counter);
  else if ((flags & C_FLAG) && (flags & MULTIPLE_FILES))
    printf("%s:%d\n", filename, vflag_counter);
  else if ((flags & C_FLAG) && !(flags & MULTIPLE_FILES))
    printf("%d\n", vflag_counter);
}

void destroy_linked_list(regex_list **head) {
  regex_list *tmp = *head;
  while (tmp != NULL) {
    regfree(&(*head)->regular_expression);
    tmp = tmp->link;
    free(*head);
    *head = tmp;
  }
}

void list_add_at_tail(regex_t regex, regex_list *head) {
  regex_list *new_node = (regex_list *)malloc(sizeof(regex_list));
  if (new_node == NULL) {
    printf("memory allocation failed");
    free(new_node);
    exit(1);
  }
  new_node->regular_expression = regex;
  regex_list *tmp = head;
  while (tmp->link != NULL) {
    tmp = tmp->link;
  }
  tmp->link = new_node;
  new_node->link = NULL;
}

void replace_cr(char *line, char delim) {
  char *ptr_to_cr = memchr(line, delim, strlen(line));
  if (ptr_to_cr) *ptr_to_cr = '\0';
}

void print_error_msg(char *argv[], int i, unsigned short *flags,
                     short regex_position) {
  if (!(*flags & E_FLAG) && regex_position != i) {
    if (!(is_file(argv[i]))) {
      if (!(is_flag(argv[i], flags))) {
        *flags |= MULTIPLE_FILES;
        if (!(*flags & S_FLAG)) {
          printf("grep: %s: No such file or directory\n", argv[i]);
        }
      }
    }
  }
  if (*flags & E_FLAG) {
    if (!(is_file(argv[i]))) {
      if (!check_for_flag("ef", *flags, argv, i)) {
        if (!(is_flag(argv[i], flags))) {
          *flags |= MULTIPLE_FILES;
          if (!(*flags & S_FLAG)) {
            printf("grep: %s: No such file or directory\n", argv[i]);
          }
        }
      }
    }
  }
}

char *getsubstr(char *s, regmatch_t *pmatch) {
  static char buf[100] = {0};
  memset(buf, 0, sizeof(buf));
  memcpy(buf, s + pmatch->rm_so, pmatch->rm_eo - pmatch->rm_so);
  return buf;
}

void print_list(regex_list *head) {
  regex_list *tmp = head;
  short n_nodes = 0;
  while (tmp != NULL) {
    n_nodes += 1;
    tmp = tmp->link;
  }
  printf("%d\n", n_nodes);
}