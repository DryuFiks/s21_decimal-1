#include "s21_decimal.h"

int s21_is_equal(s21_decimal n1, s21_decimal n2) {
  int res = 1;
  int check_sign = s21_check_sign(&n1, &n2);
  int check_mantisa = s21_check_mantisa(&n1, &n2);
  int diff_scale = s21_get_scale(&n1) - s21_get_scale(&n2);
  if (check_sign == 0 || check_sign == 1) {
    if (diff_scale == 0) {
      if (check_mantisa == 1)
        res = 1;
      else
        res = 0;
    } else if (diff_scale != 0) {
      s21_scale_equalize(&n1, &n2);
      if (s21_check_mantisa(&n1, &n2) == 1)
        res = 1;  // проверка мантисы
      else
        res = 0;
    }
  } else if (s21_check_zero_mantisa(&n1) == 1 &&
             s21_check_zero_mantisa(&n2) == 1)
    res = 1;
  else
    res = 0;
  return res;
}

int s21_is_greater(s21_decimal src1, s21_decimal src2) {
  int res = 1;
  int check_sign = s21_check_sign(&src1, &src2);
  int check_mantisa = s21_check_mantisa(&src1, &src2);
  int diff_scale = s21_get_scale(&src1) - s21_get_scale(&src2);

  if (check_sign == 0) {
    if (diff_scale == 0) {
      if (check_mantisa == 1) {
        res = 0;
      } else
        res = s21_check_greater_mantisa(&src1, &src2);
    } else if (diff_scale != 0) {
      s21_scale_equalize(&src1, &src2);
      if (s21_check_mantisa(&src1, &src2) == 1)
        res = 0;  // проверка мантисы
      else
        res = s21_check_greater_mantisa(&src1, &src2);
    }
  } else if (check_sign == 2) {  //+-
    if (s21_check_zero_mantisa(&src1) == 1 &&
        s21_check_zero_mantisa(&src2) == 1)
      res = 0;
    else
      res = 1;
  } else if (check_sign == 3) {  //-+
    res = 0;
  }
  if (check_sign == 1) {
    if (diff_scale == 0) {
      if (check_mantisa == 1) {
        res = 0;
      } else
        res = !s21_check_greater_mantisa(&src1, &src2);
    } else if (diff_scale != 0) {
      s21_scale_equalize(&src1, &src2);
      if (s21_check_mantisa(&src1, &src2) == 1)
        res = 0;  // проверка мантисы
      else
        res = !s21_check_greater_mantisa(&src1, &src2);
    }
  }
  return res;
}

int s21_is_greater_or_equal(s21_decimal src1, s21_decimal src2) {
  int result = 0;
  if ((s21_is_greater(src1, src2) == 1) || (s21_is_equal(src1, src2) == 1))
    result = 1;

  return result;
}

int s21_is_not_equal(s21_decimal n1, s21_decimal n2) {
  int res = 1;
  if (s21_is_equal(n1, n2) == 1) res = 0;
  return res;
}

int s21_is_less(s21_decimal src1, s21_decimal src2) {
  int res = 1;
  if (s21_is_greater_or_equal(src1, src2) == 1) res = 0;
  return res;
}

int s21_is_less_or_equal(s21_decimal src1, s21_decimal src2) {
  int res = 0;
  if ((s21_is_less(src1, src2) == 1) || (s21_is_equal(src1, src2) == 1))
    res = 1;
  return res;
}
