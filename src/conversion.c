#include <stdint.h>

#include "s21_decimal.h"

int s21_from_decimal_to_int(s21_decimal d, int *n) {
  int error = 0;
  if (n == NULL) error = 1;
  if (s21_check_2_and_3_mantissa(d) == 1 || s21_checks(&d) == 1) {
    error = 1;
  } else {
    int scale = s21_get_scale(&d);
    s21_set_scale(&d, 0);
    int sign = s21_get_sign(&d);
    s21_set_sign(&d, 0);
    while (scale > 0) {
      d.bits[0] = d.bits[0] / 10;
      scale--;
    }
    *n = d.bits[0];
    if (sign == 1) {
      *n = -*n;
    }
  }
  return error;
}

int s21_from_int_to_decimal(int n, s21_decimal *d) {
  int error = 0;
  if (d == NULL)
    error = 1;
  else {
    s21_init_decimal(d);
    if (n >= 0) {
      d->bits[0] = n;
    } else {
      d->bits[0] = -n;
    }
    s21_set_sign(d, n < 0);
  }
  return error;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int error = 0;
  if (!dst)
    error = 1;
  else
    s21_init_decimal(dst);
  int exp = 0, sign = 0;
  int exp_sign = 1;
  int sign_id = src / fabs(src);
  if (dst == NULL)
    error = 1;
  else if ((fabs(src) < powl(10.0, -1 * 28)) && (fabs(src) > 0)) {
    error = 1;
    s21_init_decimal(dst);
  } else if (src >= MAX_DECIMAL || src <= MIN_DECIMAL || isnan(src)) {
    error = 1;
  } else {
    error = 0;
    if (sign_id < 0) {
      sign = 1;
      src = fabs(src);
    }
    char float_as_str[16] = {'\0'};
    sprintf(float_as_str, "%.6e", src);
    int i = 0;
    for (; float_as_str[i] != 'e'; i++) {
      if (float_as_str[i] == '.') continue;
      dst->bits[0] *= 10;
      dst->bits[0] += float_as_str[i] - '0';
    }
    i++;
    if (float_as_str[i++] == '+') exp_sign = -1;
    for (; float_as_str[i] != '\0'; i++) {
      exp *= 10;
      exp += float_as_str[i] - '0';
    }
    int zeroes = s21_g_zeroes(float_as_str);
    exp = exp * exp_sign + 6 - zeroes;
    if (zeroes) dst->bits[0] /= pow(10, zeroes);
    if (exp < 0) {
      dst->bits[0] *= pow(10, -exp);
      exp = 0;
    }
    s21_set_scale(dst, exp);
    if (sign == 1) s21_set_sign(dst, 1);
  }
  return error;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int error = 0;
  if (dst == NULL)
    error = 1;
  else if (s21_checks(&src) == 1) {
    error = 1;
  } else {
    double tmp = 0.0;
    int sign = s21_get_sign(&src);
    s21_set_sign(&src, 0);
    int scale = s21_get_scale(&src);
    s21_set_scale(&src, 0);
    for (int i = 0; i < 96; i++) {
      tmp = tmp + pow(2.0, i) * s21_get_bit(&src, i);
    }
    tmp = tmp / pow(10, scale);
    tmp = tmp * pow(-1, sign);
    *dst = (float)tmp;
  }
  return error;
}
