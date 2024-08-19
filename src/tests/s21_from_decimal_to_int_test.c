#include "../s21_decimal.h"
#include "s21_decimal_test.h"

static s21_decimal array[] = {
    {{0x00000001, 0x00000000, 0x00000000, 0x00000000}},  // 1
    {{0x0000006E, 0x00000000, 0x00000000, 0x00020000}},  // 1.10
    {{0x00000457, 0x00000000, 0x00000000, 0x00030000}},  // 1.111
    {{0x773593FF, 0x00000000, 0x00000000, 0x00090000}},  // 1.999999999
    {{0x0000007B, 0x00000000, 0x00000000, 0x00000000}},  // 123
    {{0x0001E078, 0x00000000, 0x00000000, 0x00030000}},  // 123.000
    {{0x0001E45F, 0x00000000, 0x00000000, 0x00030000}},  // 123.999
    {{0x000003E7, 0x00000000, 0x00000000, 0x80030000}},  // -0.999
    {{0x00000001, 0x00000000, 0x00000000, 0x80000000}},  // -1
    {{0x80000000, 0x00000000, 0x00000000, 0x80000000}},  // -2147483648
    {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},  // 0
    {{0x00000097, 0x00000000, 0x00000000, 0x00010000}},  // 15.1
    {{0x0000060F, 0x00000000, 0x00000000, 0x00020000}},  // 15.51
    {{0x00000097, 0x00000000, 0x00000000, 0x80010000}},  // -15.1
    {{0x0000060F, 0x00000000, 0x00000000, 0x80020000}},  // -15.51
    {{0x00000619, 0x00000000, 0x00000000, 0x00010000}},  // 156.1
    {{0x00003D23, 0x00000000, 0x00000000, 0x00020000}},  // 156.51
    {{0x00000619, 0x00000000, 0x00000000, 0x80010000}},  // -156.1
    {{0x00003D23, 0x00000000, 0x00000000, 0x80020000}},  // -156.51
};

static int result[] = {
    1, 1,  1,  1,   123, 123, 123, 0,    -1,   -2147483648,
    0, 15, 15, -15, -15, 156, 156, -156, -156,
};

static s21_decimal error_array[] = {
    {{0x80000000, 0x00000000, 0x00000000, 0x00000000}},  // 2147483648
    {{0x80000001, 0x00000000, 0x00000000, 0x80000000}},  // -2147483649
    {{0x000F4240, 0x00002710, 0x00002710,
      0x00000000}},  // 184467440780045190120000
    {{0x000F4240, 0x00002710, 0x00002710,
      0x80000000}},  // -184467440780045190120000
    {{0x00002F59, 0x0CA55839, 0x00000000, 0x00000000}},  // 911231501448785753
    {{0x000004C2, 0x01518D64, 0x00000000, 0x80000000}},  // -95012527787738306
    {{0x00000000, 0x00000000, 0x00000000, 0x81001100}}};

static int error_result[] = {1, 1, 1, 1, 1, 1, 1};

START_TEST(test_from_decimal_to_int) {
  int tmp = 0;
  int code = s21_from_decimal_to_int(array[_i], &tmp);
  ck_assert_int_eq(tmp, result[_i]);
  ck_assert_int_eq(code, 0);
}
END_TEST

START_TEST(test_from_decimal_to_int_with_error) {
  int empty = 0;
  int code = s21_from_decimal_to_int(error_array[_i], &empty);
  ck_assert_int_eq(code, error_result[_i]);
}
END_TEST

Suite *suite_from_decimal_to_int(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("FROM_DECIMAL_TO_INT");
  tc_core = tcase_create("Core");

  tcase_add_loop_test(tc_core, test_from_decimal_to_int, 0,
                      sizeof(array) / sizeof(s21_decimal));
  tcase_add_loop_test(tc_core, test_from_decimal_to_int_with_error, 0,
                      sizeof(error_array) / sizeof(s21_decimal));

  suite_add_tcase(s, tc_core);

  return s;
}
