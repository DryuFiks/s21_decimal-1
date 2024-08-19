#include "s21_decimal_test.h"

int main(void) {
  Suite *list_cases[] = {suite_is_less(),
                         suite_is_less_or_equal(),
                         suite_is_greater(),
                         suite_is_greater_or_equal(),
                         suite_is_equal(),
                         suite_is_not_equal(),

                         suite_from_int_to_decimal(),
                         suite_from_float_to_decimal(),
                         suite_from_decimal_to_int(),
                         suite_from_decimal_to_float(),

                         suite_floor(),
                         suite_negate(),
                         suite_round(),
                         suite_truncate(),

                         suite_add(),
                         suite_mul(),
                         suite_sub(),
                         suite_div(),

                         NULL};

  for (int i = 0; list_cases[i]; i++) {
    fprintf(stderr,
            "------------------------------------------------------------------"
            "------------\n");
    SRunner *runner = srunner_create(list_cases[i]);
    srunner_run_all(runner, CK_NORMAL);
    srunner_free(runner);
  }

  return 0;
}
