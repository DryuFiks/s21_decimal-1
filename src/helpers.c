#include "s21_decimal.h"

int s21_get_bit(s21_decimal *dst, int bit) {
  int result = 0;
  if ((dst->bits[bit / 32] & (1 << bit)) != 0) {
    result = 1;
  }
  return result;
}

void s21_set_bit(s21_decimal *dst, int bit, int new_bit) {
  if (new_bit)
    dst->bits[bit / 32] |= (1 << bit);
  else
    dst->bits[bit / 32] &= ~(1 << bit);
}

int s21_get_sign(s21_decimal *dst) {
  int result = (dst->bits[3] >> 31) & 1;
  return result;
}

void s21_set_sign(s21_decimal *dst, int sign) {
  if (sign)
    dst->bits[3] |= (1 << 31);
  else
    dst->bits[3] &= ~(1 << 31);
}

void s21_init_decimal(s21_decimal *number) {
  for (int i = 0; i < 128; i++) {
    s21_set_bit(number, i, 0);
  }
}

void s21_set_scale(s21_decimal *value, int scale) {
  if (scale >= 0 && scale <= 28) {
    int sign = s21_get_sign(value);
    value->bits[3] &= ~(0xFF << 16);
    value->bits[3] |= scale << 16;
    if (sign) s21_set_sign(value, 1);
  }
}

int s21_get_scale(s21_decimal *d) {
  int shift = 16;
  int scale = 0, i = 0;
  while (shift <= 23) {
    int bit = ((d->bits[3] & (1 << shift)) != 0);
    scale += bit * pow(2, i);
    shift++;
    i++;
  }
  return scale;
}

int s21_check_mantisa(s21_decimal *num1, s21_decimal *num2) {
  int res = 1;
  for (int i = 0; i < 96; i++) {
    if (s21_get_bit(num1, i) != (s21_get_bit(num2, i))) res = 0;
  }
  return res;
}

int s21_check_sign(s21_decimal *num1, s21_decimal *num2) {
  int sign1 = s21_get_sign(num1);
  int sign2 = s21_get_sign(num2);
  int index = -1;
  if ((sign1 == 0) && (sign2 == 0))
    index = 0;
  else if ((sign1 == 1) && (sign2 == 1))
    index = 1;
  else if ((sign1 == 0) && (sign2 == 1))
    index = 2;
  else if ((sign1 == 1) && (sign2 == 0))
    index = 3;
  return index;
}

int s21_scale_equalize(s21_decimal *number1, s21_decimal *number2) {
  s21_decimal *big = NULL;
  s21_decimal *small = NULL;
  s21_decimal temp = {0};
  int per = 0;
  int out = 1;
  if (s21_get_scale(number1) != s21_get_scale(number2)) {
    if (s21_get_scale(number1) > s21_get_scale(number2)) {
      big = number1;
      small = number2;
      per = 1;
    } else {
      big = number2;
      small = number1;
    }
    int scaleSmall = s21_get_scale(small);
    int scaleBig = s21_get_scale(big);
    int newscale = scaleBig - scaleSmall;
    while (newscale) {
      temp = *small;
      s21_scale_increase(small, 1);
      if (!s21_get_bit(small, 96)) {
        temp = *small;
        scaleSmall++;
        newscale--;
      } else {
        *small = temp;
        break;
      }
    }
    s21_set_scale(&temp, scaleSmall);
    int newScale2 = scaleBig - scaleSmall;
    // printf("%d\n", newScale2);
    while (newScale2) {
      s21_minus_scale(big, 1);  // поменяла здесь значение
      s21_set_scale(big, scaleSmall);
      newScale2--;
    }
    number1 = (per) ? big : &temp;
    number2 = (per) ? &temp : big;
    out = 0;
  }
  return out;
}

// Выравнивание скейлов и как умножение на 10:
int s21_scale_increase(s21_decimal *value, int shift) {
  s21_decimal zapomnite = {0};
  zapomnite = *value;
  int res = 0;
  if ((s21_get_scale(value) + shift) < 29) {
    s21_set_scale(value, (s21_get_scale(value) + shift));
    s21_decimal tmp;
    tmp = *value;
    s21_decimal tmp1;
    tmp1 = *value;
    s21_left_shift(&tmp, 1);
    s21_left_shift(&tmp1, 3);
    if (s21_add_simple(tmp, tmp1, value)) {
      s21_set_bit(value, 96, 1);
      res = 1;
      if (shift == 0) *value = zapomnite;
    }
  }
  return res;
}

int s21_left_shift(s21_decimal *number, int shift) {
  int ow = 0;
  if (s21_get_high_bit(number) != 95) {
    for (int i = 0; i < shift; i++) {
      int bits_32 = s21_get_bit(number, 31);
      int bits_64 = s21_get_bit(number, 63);
      int bits_95 = s21_get_bit(number, 95);
      number->bits[0] <<= 1;
      number->bits[1] <<= 1;
      number->bits[2] <<= 1;
      if (bits_32) s21_set_bit(number, 32, 1);
      if (bits_64) s21_set_bit(number, 64, 1);
      if (bits_95) s21_set_bit(number, 95, 1);
    }
  } else
    ow = 1;
  return ow;
}

int s21_check_zero_mantisa(s21_decimal *value) {
  int k = 0;
  if (value->bits[0] == 0 && value->bits[1] == 0 && value->bits[2] == 0) k = 1;
  return k;
}

int s21_check_greater_mantisa(s21_decimal *value1, s21_decimal *value2) {
  // Проверка мантисс от старших к младшим.
  int k = 0;
  if (value1->bits[2] > value2->bits[2]) {
    k = 1;
  } else if (value1->bits[2] < value2->bits[2]) {
    k = 0;
  } else {
    if (value1->bits[1] > value2->bits[1]) {
      k = 1;
    } else if (value1->bits[1] < value2->bits[1]) {
      k = 0;
    } else {
      k = value1->bits[0] > value2->bits[0] ? 1 : 0;
    }
  }
  return k;
}

// Внутренние операции для арифметики:

int s21_add_simple(s21_decimal a, s21_decimal b, s21_decimal *c) {
  int carry = 0;
  for (int i = 0; i < 96; i++) {
    int bit_a = s21_get_bit(&a, i);
    int bit_b = s21_get_bit(&b, i);
    int sum = bit_a + bit_b + carry;
    if (sum == 1) {
      carry = 0;
      sum = 1;
    } else if (sum == 2) {
      carry = 1;
      sum = 0;
    } else if (sum == 3) {
      sum = 1;
      carry = 1;
    }
    s21_set_bit(c, i, sum);
  }
  return carry;
}

void s21_minus_simple(s21_decimal a, s21_decimal b, s21_decimal *res) {
  for (int i = 0; i < 96; i++) {
    int bit_a = s21_get_bit(&a, i);
    int bit_b = s21_get_bit(&b, i);

    s21_set_bit(res, i, bit_a ^ bit_b);

    if (bit_a == 0 && bit_b == 1) {
      int k = i + 1;
      while (s21_get_bit(&a, k) == 0) {
        s21_set_bit(&a, k, 1);
        k++;
      }
      s21_set_bit(&a, k, 0);
    }
  }
}

int s21_mult_simple(s21_decimal a, s21_decimal b, s21_decimal *c) {
  s21_decimal one = ROUND_UP;
  int ostatok, res = 0;
  for (int i = 0; i < 96 && res != 1; i++) {
    s21_decimal temp = {0};
    if (s21_get_bit(&b, i) == 1) {
      temp = a;
      s21_left_shift(&temp, i);
      res = s21_add_simple(temp, *c, c);
    }
  }
  if (res == 1) {
    s21_init_decimal(c);
    s21_big_decimal tmp_c = {0};
    s21_big_decimal tmp_a = {0};
    tmp_a = s21_copy_big_decimal(a);
    for (int i = 0; i < 96; i++) {
      if (s21_get_bit(&b, i) == 1) {
        tmp_a = s21_copy_big_decimal(a);
        s21_left_shift_big(&tmp_a, i);
        res = s21_add_simple_big(tmp_a, tmp_c, &tmp_c);
      }
    }
    int scale = 0;
    while (s21_get_high_bit_big(&tmp_c) > 95) {
      ostatok = s21_div_10_big(&tmp_c);
      scale++;
    }
    c->bits[0] = tmp_c.bits[0];
    c->bits[1] = tmp_c.bits[1];
    c->bits[2] = tmp_c.bits[2];
    if (ostatok >= 5) s21_add_simple(*c, one, c);
    s21_set_scale(c, (s21_get_scale(&a) + s21_get_scale(&b) - scale));
  }
  return res;
}

int s21_div_simple(s21_decimal a, s21_decimal b, s21_decimal *c,
                   s21_decimal *ostatok) {
  s21_decimal zapomnite = {0};
  s21_decimal one = {{1, 0, 0, 0}};
  s21_decimal res = {{1, 0, 0, 0}};
  int sdvig = 0;
  zapomnite = b;
  if (s21_get_high_bit(&a) > s21_get_high_bit(&b)) {
    sdvig = s21_get_high_bit(&a) - s21_get_high_bit(&b);
  }
  s21_left_shift(&b, sdvig);
  if (s21_check_greater_mantisa(&a, &b) == 1) {
    s21_sub(a, b, ostatok);
    s21_left_shift(&res, sdvig);
  } else {
    if (sdvig > 0) {
      sdvig = sdvig - 1;
      s21_left_shift(&res, sdvig);
      b = zapomnite;
      s21_left_shift(&b, sdvig);
      s21_sub(a, b, ostatok);
    } else {
      s21_init_decimal(&res);
      *ostatok = a;
    }
  }
  *c = res;
  s21_decimal new_res = {0};
  s21_decimal new_ostatok = {0};
  if (s21_is_greater(*ostatok, zapomnite) == 1) {
    s21_div_simple(*ostatok, zapomnite, &new_res, &new_ostatok);
    *ostatok = new_ostatok;
  } else if (s21_is_equal(*ostatok, zapomnite) == 1) {
    s21_add(*c, one, c);
    s21_init_decimal(ostatok);
  }
  s21_add(*c, new_res, c);
  return 0;
}

// Доп. арифметика:

int s21_minus_scale(s21_decimal *a, int kolvo) {
  s21_decimal ost = {0};
  s21_decimal one = ROUND_UP;
  int j = 0;
  int ostatok = 0;
  while (j < kolvo) {
    ostatok = s21_div_10(a);
    j++;
  }
  s21_from_int_to_decimal(ostatok, &ost);
  s21_set_scale(a, (s21_get_scale(a) - kolvo));
  if (ostatok >= 5 && s21_get_scale(a) != 0) s21_add_simple(*a, one, a);
  return ostatok;
}

int s21_div_10(s21_decimal *a) {
  unsigned long long overflow;
  int ostatok = 0;
  int tmp = 0;
  overflow = a->bits[2];
  int i = 2;
  while (i > 0) {
    tmp = overflow % 10;
    a->bits[i] = overflow / 10;
    i--;
    overflow = tmp * (4294967296) + a->bits[i];
    if (i == 0) {
      a->bits[i] = overflow / 10;
      ostatok = overflow % 10;
    }
  }
  return ostatok;
}

// Проверяет последний бит числа (который нулевой), если 0, то четное:
int s21_is_even(s21_decimal a) {
  int even = 0;
  if (!(a.bits[0] & 1)) even = 1;
  return even;
}

int s21_checks(s21_decimal *dst) {
  int result = 0;
  for (int i = 0; i < 127; i++) {
    if ((i > 119 && i < 127) || (i > 95 && i < 112)) {
      if (s21_get_bit(dst, i) == 1) result = 1;
    }
  }
  if ((s21_get_scale(dst) > 28)) {
    result = 1;
  }
  return result;
}

void s21_copy_decimal(s21_decimal *src, s21_decimal *dst) {
  for (int i = 0; i < 128; i++) {
    s21_set_bit(dst, i, s21_get_bit(src, i));
  }
}

int s21_check_2_and_3_mantissa(s21_decimal num) {
  int res = 0;
  s21_decimal vr1 = {{2147483647, 0, 0, 0}};
  s21_decimal vr2 = {{0x80000000, 0, 0, 0x80000000}};
  if ((s21_is_less(num, vr2) == 1) || (s21_is_greater(num, vr1) == 1)) res = 1;

  return res;
}

int s21_g_zeroes(char *string) {
  int zeroes = 0;
  for (int i = 0; string[i] != 'e'; i++) {
    if (string[i] == '0')
      zeroes++;
    else
      zeroes = 0;
  }
  return zeroes;
}

int s21_okruglenie(s21_decimal *number, s21_decimal *result) {
  int res = 0;
  int da = 0;
  s21_decimal five = {{5, 0, 0, 0}};
  s21_decimal one = {{1, 0, 0, 0}};
  s21_set_scale(&five, 1);
  s21_set_sign(number, 0);
  if (s21_is_greater(*number, five) == 1) {
    da = 1;
  } else if (s21_is_equal(*number, five) == 1) {
    if (s21_is_even(*result) == 0) da = 1;
  }
  if (da == 1) {
    int scale = s21_get_scale(result);
    s21_set_scale(result, 0);
    res = s21_add_simple(*result, one, result);
    s21_set_scale(result, scale);
  }
  return res;
}

// Вспомогательные фукнции под биг децимал:

int s21_get_bit_big(s21_big_decimal *dst, int bit) {
  int result = 0;
  if ((dst->bits[bit / 32] & (1 << bit)) != 0) {
    result = 1;
  }
  return result;
}

void s21_set_bit_big(s21_big_decimal *dst, int bit, int new_bit) {
  if (new_bit) {
    dst->bits[bit / 32] |= (1 << bit);
  } else {
    dst->bits[bit / 32] &= ~(1 << bit);
  }
}

void s21_left_shift_big(s21_big_decimal *number, int shift) {
  for (int i = 0; i < shift; i++) {
    int bits_32 = s21_get_bit_big(number, 31);
    int bits_64 = s21_get_bit_big(number, 63);
    int bits_96 = s21_get_bit_big(number, 95);
    int bits_128 = s21_get_bit_big(number, 127);
    int bits_160 = s21_get_bit_big(number, 159);
    int bits_192 = s21_get_bit_big(number, 191);
    number->bits[0] <<= 1;
    number->bits[1] <<= 1;
    number->bits[2] <<= 1;
    number->bits[3] <<= 1;
    number->bits[4] <<= 1;
    number->bits[5] <<= 1;
    number->bits[6] <<= 1;
    if (bits_32) s21_set_bit_big(number, 32, 1);
    if (bits_64) s21_set_bit_big(number, 64, 1);
    if (bits_96) s21_set_bit_big(number, 96, 1);
    if (bits_128) s21_set_bit_big(number, 128, 1);
    if (bits_160) s21_set_bit_big(number, 160, 1);
    if (bits_192) s21_set_bit_big(number, 192, 1);
  }
}
int s21_add_simple_big(s21_big_decimal a, s21_big_decimal b,
                       s21_big_decimal *c) {
  int carry = 0;
  for (int i = 0; i < 192; i++) {
    int bit_a = s21_get_bit_big(&a, i);
    int bit_b = s21_get_bit_big(&b, i);

    int sum = bit_a + bit_b + carry;
    if (sum == 1) {
      carry = 0;
      sum = 1;
    } else if (sum == 2) {
      carry = 1;
      sum = 0;
    } else if (sum == 3) {
      sum = 1;
      carry = 1;
    }
    s21_set_bit_big(c, i, sum);
  }
  return carry;
}

s21_big_decimal s21_copy_big_decimal(s21_decimal src) {
  s21_big_decimal res = {0};
  for (int i = 0; i < 96; i++) {
    s21_set_bit_big(&res, i, s21_get_bit(&src, i));
  }
  return res;
}

int s21_div_10_big(s21_big_decimal *a) {
  unsigned long long overflow;
  int ostatok = 0;
  int tmp = 0;
  overflow = a->bits[6];
  int i = 6;
  while (i > 0) {
    tmp = overflow % 10;
    a->bits[i] = overflow / 10;
    i--;
    overflow = tmp * (4294967296) + a->bits[i];
    if (i == 0) {
      a->bits[i] = overflow / 10;
      ostatok = overflow % 10;
    }
  }
  return ostatok;
}

int s21_get_high_bit_big(s21_big_decimal *number) {
  int bit_pos = 0;
  int k = 0;
  for (int i = 196; i >= 0 && k == 0; i--) {
    if (s21_get_bit_big(number, i) == 1) {
      bit_pos = i;
      k = 1;
    }
  }
  return bit_pos;
}

int s21_get_high_bit(s21_decimal *number) {
  int bit_pos = 0;
  int k = 0;
  for (int i = 95; i >= 0 && k == 0; i--) {
    if (s21_get_bit(number, i) == 1) {
      bit_pos = i;
      k = 1;
    }
  }
  return bit_pos;
}

// Функции для дебага:

// void s21_print_binary(int dst) {
//   for (int i = 31; i >= 0; i--) {
//     printf("%d", (dst >> i) & 1);
//     if (i == 8 || i == 16 || i == 24) printf(" ");
//   }
//   printf("\n");
// }

// void s21_print_decimal(s21_decimal *dst) {
//   for (int i = 3; i >= 0; i--) {
//     s21_print_binary(dst->bits[i]);
//   }
// }

// void s21_print_big_decimal(s21_big_decimal *dst) {
//   for (int i = 6; i >= 0; i--) {
//     s21_print_binary(dst->bits[i]);
//   }
// }
