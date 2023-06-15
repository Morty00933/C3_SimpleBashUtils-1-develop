CC= gcc -std=c11 -D_GNU_SOURCE
CFLAGS= -Wall -Wextra -Werror
SOURCES= s21_grep.c

all: s21_grep test

s21_grep:
	$(CC) $(CFLAGS) $(SOURCES) -o s21_grep

test:
	chmod +x test_func_grep.sh
	sh test_func_grep.sh

check:
	cp ../../materials/linters/.clang-format .
	clang-format -n s21_grep.c

clean:
	rm -rf s21_grep 
	rm -rf *.a *.out *.o .log.txt

rebuild:
	make clean
	make all
	rm -rf  *.o *.a