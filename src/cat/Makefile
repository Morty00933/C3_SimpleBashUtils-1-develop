CC= gcc -std=c11 -D_GNU_SOURCE
CFLAGS= -Wall -Wextra -Werror
SOURCES= s21_cat.c

all: s21_cat test

s21_cat:
	$(CC) $(CFLAGS) $(SOURCES) -o s21_cat

test:
	chmod +x test_func_cat.sh
	sh test_func_cat.sh

check:
	cp ../../materials/linters/.clang-format .
	clang-format -n s21_cat.c

clean:
	rm -rf s21_cat
	rm -rf *.a *.out *.o .log.txt

rebuild:
	make clean
	make all
	rm -rf  *.o *.a
