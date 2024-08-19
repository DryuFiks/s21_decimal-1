#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DECIMAL powl(2, 96) - 1
#define MIN_DECIMAL -1 * powl(2, 96)
#define INTEGER_MIN -2147483648
#define INTEGER_MAX 2147483647
#define ROUND_UP   \
  {                \
    { 1, 0, 0, 0 } \
  }

// Cтруктура decimal:
typedef struct {
  unsigned int bits[4];
} s21_decimal;

typedef struct {
  unsigned int bits[7];
} s21_big_decimal;

// Арифметические операторы:
int s21_add(s21_decimal value1, s21_decimal value2, s21_decimal *result);
int s21_sub(s21_decimal value1, s21_decimal value2, s21_decimal *result);
int s21_mul(s21_decimal value1, s21_decimal value2, s21_decimal *result);
int s21_div(s21_decimal value1, s21_decimal value2, s21_decimal *result);

// Операторы сравнения:
int s21_is_less(s21_decimal value1, s21_decimal value2);
int s21_is_greater(s21_decimal value1, s21_decimal value2);
int s21_is_equal(s21_decimal value1, s21_decimal value2);
int s21_is_not_equal(s21_decimal value1, s21_decimal value2);
int s21_is_less_or_equal(s21_decimal src1, s21_decimal src2);
int s21_is_greater_or_equal(s21_decimal src1, s21_decimal src2);

// Преобразователи:
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

// Другие функции:
int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);

// Внутренние операции для арифметики:
void s21_minus_simple(s21_decimal a, s21_decimal b, s21_decimal *res);
int s21_add_simple(s21_decimal a, s21_decimal b, s21_decimal *res);
int s21_mult_simple(s21_decimal a, s21_decimal b, s21_decimal *c);
int s21_div_simple(s21_decimal a, s21_decimal b, s21_decimal *c,
                   s21_decimal *ostatok);
int s21_div_10(s21_decimal *a);
int s21_minus_scale(s21_decimal *a, int kolvo);
int s21_is_even(s21_decimal a);

// Вспомогательные функции:
int s21_get_bit(s21_decimal *number, int bit);
void s21_set_bit(s21_decimal *number, int bit, int sign);
void s21_init_decimal(s21_decimal *number);
void s21_set_sign(s21_decimal *d, int sign);
int s21_get_sign(s21_decimal *d);
int s21_get_scale(s21_decimal *d);
void s21_set_scale(s21_decimal *value, int scale);
int s21_check_mantisa(s21_decimal *num1, s21_decimal *num2);
int s21_check_sign(s21_decimal *num1, s21_decimal *num2);
int s21_scale_equalize(s21_decimal *number1, s21_decimal *number2);
int s21_scale_increase(s21_decimal *value, int shift);
int s21_left_shift(s21_decimal *number, int shift);
int s21_check_zero_mantisa(s21_decimal *value);
int s21_check_greater_mantisa(s21_decimal *value1, s21_decimal *value2);
int s21_checks(s21_decimal *dst);
void s21_copy_decimal(s21_decimal *src, s21_decimal *dst);
int s21_check_2_and_3_mantissa(s21_decimal num);
int s21_g_zeroes(char *string);
int s21_okruglenie(s21_decimal *number, s21_decimal *result);
int s21_get_bit_big(s21_big_decimal *dst, int bit);
void s21_set_bit_big(s21_big_decimal *dst, int bit, int new_bit);
void s21_left_shift_big(s21_big_decimal *number, int shift);
int s21_add_simple_big(s21_big_decimal a, s21_big_decimal b,
                       s21_big_decimal *c);
s21_big_decimal s21_copy_big_decimal(s21_decimal src);
int s21_div_10_big(s21_big_decimal *a);
int s21_get_high_bit(s21_decimal *number);
int s21_get_high_bit_big(s21_big_decimal *number);
// void s21_print_decimal(s21_decimal *decimal);
// void s21_print_binary(int dst);
// void s21_print_big_decimal(s21_big_decimal *dst);

#endif