CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
LDFLAGS = -lcheck -lpthread -lrt -lm -lsubunit

SOURCES = arithmetics.c helpers.c other.c conversion.c comparison.c
OBJECTS = $(SOURCES:.c=.o)

HEADERS = s21_decimal.h

all: test clean

s21_decimal.a: $(OBJECTS)
	@ar rcs $@ $^
	@ranlib $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

test: s21_decimal.a 
	$(CC) $(CFLAGS)  tests/*.c s21_decimal.a $(LDFLAGS) -o $@
	./$@

gcov_report: s21_decimal.a
	$(CC) $(CFLAGS) --coverage  $(SOURCES) s21_decimal.a tests/*.c $(LDFLAGS)  -o gcov_report
	./gcov_report
	lcov -t "gcov_report" -o report.info --no-external -c -d .
	genhtml -o report/ report.info

clean:
	rm -f test main *.gcno *.gcda report.info gcov_report *.o s21_decimal.a
	rm -rf report

valgrind:
	valgrind --tool=memcheck --leak-check=yes ./test 

clang:
	clang-format -n *.c *.h -style=Google
	clang-format -n ./tests/*.c ./tests/*.h -style=Google