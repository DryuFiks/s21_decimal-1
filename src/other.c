#include "s21_decimal.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
  int error = s21_checks(&value);
  if (error != 1 && result != NULL) {
    s21_decimal buffer = value;
    int scale = s21_get_scale(&buffer);
    if (scale) {
      s21_set_scale(&buffer, 0);
      int remainder = s21_minus_scale(&buffer, scale);
      if (s21_get_sign(&buffer) && (remainder || scale > 7)) buffer.bits[0]++;
    }
    s21_copy_decimal(&buffer, result);
  } else
    error = 1;
  return error;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  int error = s21_checks(&value);
  if (error != 1 && result != NULL) {
    s21_decimal buffer = value;
    if (s21_get_scale(&value)) {
      s21_decimal dot,
          half_dec = {{0x00000005, 0x00000000, 0x00000000, 0x00010000}};  // 0.5
      s21_truncate(value, &buffer);
      s21_sub(value, buffer, &dot);
      s21_set_sign(&dot, 0);
      if (s21_is_greater_or_equal(dot, half_dec)) buffer.bits[0]++;
    }
    s21_copy_decimal(&buffer, result);
  } else
    error = 1;
  return error;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int error = s21_checks(&value);
  if (error != 1 && result != NULL) {
    s21_decimal buffer = value;
    if (s21_get_scale(&value)) s21_minus_scale(&buffer, s21_get_scale(&buffer));
    s21_copy_decimal(&buffer, result);
  } else
    error = 1;
  return error;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  int error = 1;
  if (result) {
    *result = value;
    s21_set_sign(result, !s21_get_sign(result));
    if (s21_checks(&value) != 1) error = 0;
  }
  return error;
}
