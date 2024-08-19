#include "s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal v1 = value_1;
  s21_decimal v2 = value_2;
  s21_decimal one = ROUND_UP;
  int ostatok1, ostatok2, res = 0;
  s21_init_decimal(result);

  if (s21_get_scale(&value_1) != s21_get_scale(&value_2)) {
    s21_scale_equalize(&value_1, &value_2);
  }
  int sign_1 = s21_get_sign(&value_1);
  int sign_2 = s21_get_sign(&value_2);
  if (sign_1 == sign_2) {
    if (s21_add_simple(value_1, value_2, result) == 1) {
      if ((s21_get_scale(&value_1) == 0 || s21_get_scale(&value_2) == 0) &&
          sign_1 == 0) {
        res = 1;
      } else if ((s21_get_scale(&value_1) == 0 ||
                  s21_get_scale(&value_2) == 0) &&
                 sign_1 == 1) {
        res = 2;
      } else if (s21_get_scale(&value_1) > 0 && s21_get_scale(&value_2) > 0) {
        ostatok1 = s21_minus_scale(&value_1, 1);
        ostatok2 = s21_minus_scale(&value_2, 1);
        s21_add(value_1, value_2, result);
        if (ostatok1 + ostatok2 >= 5)
          res = s21_add_simple(*result, one, result);
      }
    } else {
      if (s21_check_zero_mantisa(&value_1) == 1) {
        res = s21_okruglenie(&v1, result);
      } else if (s21_check_zero_mantisa(&value_2) == 1) {
        res = s21_okruglenie(&v2, result);
      }
      if (res > 0) res += sign_1;
      s21_set_scale(result, s21_get_scale(&value_1));
      s21_set_sign(result, sign_1);
    }
  }
  if (sign_1 ^ sign_2) {
    if (s21_check_greater_mantisa(&value_1, &value_2) == 1) {
      s21_minus_simple(value_1, value_2, result);
      s21_set_sign(result, sign_1);
    } else if (s21_check_greater_mantisa(&value_1, &value_2) == 0) {
      s21_minus_simple(value_2, value_1, result);
      s21_set_sign(result, sign_2);
    }
  }
  s21_set_scale(result, s21_get_scale(&value_1));
  return res;
}

int s21_mul(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  int res = 0;
  int sign_id = s21_check_sign(&value1, &value2);
  s21_decimal max_check = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0x00000000}};
  s21_decimal one = {{0x00000001, 0x00000000, 0x00000000, 0x00000000}};
  if (result != NULL) {
    if (s21_checks(&value1) || s21_checks(&value2))
      res = -1;
    else if ((s21_is_greater_or_equal(value1, max_check) &&
              !s21_get_scale(&value2)) ||
             (s21_is_greater_or_equal(value2, max_check) &&
              !s21_get_scale(&value1)))
      res = 1;
    else if ((s21_check_greater_mantisa(&value1, &max_check) == 1 &&
              s21_is_greater(value1, one) &&
              (s21_get_scale(&value1) > 1 || s21_get_scale(&value2) > 1) &&
              (sign_id == 2 || sign_id == 3)) ||
             ((s21_check_greater_mantisa(&value2, &max_check) == 1 &&
               s21_is_greater(value2, one) &&
               (s21_get_scale(&value1) > 1 || s21_get_scale(&value2) > 1) &&
               (sign_id == 2 || sign_id == 3))))
      res = 2;
    else {
      s21_init_decimal(result);
      res = s21_mult_simple(value1, value2, result);
      if (sign_id == 0 || sign_id == 1)
        s21_set_sign(result, 0);
      else if (sign_id == 2 || sign_id == 3) {
        s21_set_sign(result, 1);
      }
    }
  } else
    res = -1;
  return res;
}

int s21_sub(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  int res = (s21_checks(&value1) || s21_checks(&value2));
  if (res != 1 && result != NULL) {
    int sign_1 = s21_get_sign(&value1);
    int sign_2 = s21_get_sign(&value2);
    s21_set_sign(&value1, 0);
    s21_set_sign(&value2, 0);
    if (sign_1 == sign_2) {
      if (s21_is_equal(value1, value2) == 1) {
        s21_init_decimal(result);
      } else if (s21_is_greater(value1, value2) == 1) {
        s21_set_sign(&value1, sign_1);
        s21_set_sign(&value2, !sign_2);
        res = s21_add(value1, value2, result);
        s21_set_sign(result, sign_1);
      } else if (s21_is_less(value1, value2) == 1) {
        s21_set_sign(&value2, sign_2);
        s21_set_sign(&value1, !sign_1);
        res = s21_add(value1, value2, result);
        s21_set_sign(result, !sign_1);
      }
    } else {
      s21_set_sign(&value2, !sign_2);
      s21_set_sign(&value1, sign_1);
      res = s21_add(value1, value2, result);
    }
  } else
    res = -1;
  return res;
}

int s21_div(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  int res = 0;
  if (!s21_check_zero_mantisa(&value2)) {
    if (!s21_check_zero_mantisa(&value1)) {
      int check = 0;
      s21_decimal res = {0};
      s21_decimal ost = {0};
      s21_decimal five = {{5, 0, 0, 0}};
      s21_decimal one = {{1, 0, 0, 0}};
      int diff = s21_get_scale(&value1) - s21_get_scale(&value2);
      int scale = diff;
      if (diff > 0) {
        scale = 0;
        diff = 0;
        s21_scale_equalize(&value1, &value2);
      }
      if (diff < 0) {
        s21_scale_increase(&value2, 0);
        scale--;
      }
      s21_scale_equalize(&value1, &value2);
      int sign1 = s21_get_sign(&value1);
      int sign2 = s21_get_sign(&value2);
      s21_set_sign(&value1, 0);
      s21_set_sign(&value2, 0);
      if (s21_is_greater(value1, value2) == 0) {
        while (s21_check_greater_mantisa(&value2, &value1) == 1 && scale < 28) {
          s21_scale_increase(&value1, 0);
          scale++;
        }
      }
      s21_div_simple(value1, value2, &res, &ost);
      *result = res;
      while (ost.bits[0] != 0 && scale < 28 && check == 0) {
        if (!s21_scale_increase(result, 0)) {
          scale++;
          s21_scale_increase(&ost, 0);
          value1 = ost;
          s21_div_simple(value1, value2, &res, &ost);
          s21_add(*result, res, result);
          if (scale - diff == 28) {
            s21_scale_increase(&ost, 0);
            value1 = ost;
            s21_div_simple(value1, value2, &res, &ost);
            if (s21_is_greater_or_equal(res, five)) {
              s21_add(*result, one, result);
            }
            check = 1;
          }
        } else {
          s21_scale_increase(&ost, 0);
          value1 = ost;
          s21_div_simple(value1, value2, &res, &ost);
          if (s21_is_greater_or_equal(res, five)) {
            s21_add(*result, one, result);
          }
          check = 1;
        }
      }
      s21_set_scale(result, scale - diff);
      if (sign1 ^ sign2) s21_set_sign(result, 1);
    } else
      s21_init_decimal(result);
  } else
    res = 3;
  return res;
}
