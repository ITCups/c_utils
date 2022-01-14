// Copyright 2021 <lwolmer>
#include <check.h>
#include <stdlib.h>
#include "lw_utils.h"

START_TEST(list_add_default) {
    list *test_list = NULL;
    int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (size_t i = 0; i < 10; i++) {
        int *data = malloc(sizeof(int));
        if (data) {
            *data = arr[i];
            list_add(&test_list, data, true);
        }
    }
    list *test_head = test_list;
    for (size_t i = 0; i < 10 && test_head; i++) {
        ck_assert_int_eq(arr[i], *((int*)test_head->data));
        test_head = test_head->next_node;
    }
    list_free(&test_list);
}
END_TEST

void check_DS(dynamic_string *ds, char *str) {
    size_t str_len = strlen(str);
    ck_assert_str_eq(ds->string, str);
    ck_assert_uint_eq(ds->length, str_len);
}

START_TEST(dynamic_string_init_default) {
    dynamic_string *str = DS_init(NULL);
    dynamic_string *str2 = DS_init("some_text123 _ cjetnbem[qoetm qpbwe[[pr]23-95");
    check_DS(str, "");
    check_DS(str2, "some_text123 _ cjetnbem[qoetm qpbwe[[pr]23-95");
    DS_free(str);
    DS_free(str2);
}
END_TEST

START_TEST(dynamic_string_set_char_default) {
    dynamic_string *str = DS_init(NULL);
    char *long_str = "123456789qwoeiwfmpwe,pocjetnbem[qoetm qpbwe[[pr]23-952]lvf[2l," \
                    "fb=42ti b24[tl24,i20tibm3,t34kt p0nu34mt3p4mit3p4bik3]4[/lt4.v";
    size_t ls = strlen(long_str);
    for (size_t i = 0; i < ls; i++)
        DS_set_char(str, long_str[i], i);
    check_DS(str, long_str);
    DS_free(str);
}
END_TEST

START_TEST(dynamic_string_set_char_insert) {
    dynamic_string *str = DS_init("000000000000000000000000000000000000000");
    DS_set_char(str, '1', 3);
    check_DS(str, "000100000000000000000000000000000000000");
    DS_free(str);
}
END_TEST
START_TEST(dynamic_string_set_char_out_of_bounds) {
    dynamic_string *str = DS_init("000000000000000000000000000000000000000");
    DS_set_char(str, '1', 40);
    check_DS(str, "000000000000000000000000000000000000000");
    DS_free(str);
}
END_TEST

START_TEST(dynamic_string_set_text_default) {
    dynamic_string *str = DS_init(NULL);
    char *long_str = "123456789qwoeiwfmpwe,pocjetnbem[qoetm qpbwe[[pr]23-952]lvf[2l," \
                    "fb=42ti b24[tl24,i20tibm3,t34kt p0nu34mt3p4mit3p4bik3]4[/lt4.v";
    DS_set_text(str, long_str);
    check_DS(str, long_str);
    DS_free(str);
}
END_TEST

START_TEST(dynamic_string_set_text_const) {
    dynamic_string *str = DS_init(NULL);
    DS_set_text(str, "123456789qwoeiwfmpwe,pocjetnbem[qoetm qpbwe[[pr");
    check_DS(str, "123456789qwoeiwfmpwe,pocjetnbem[qoetm qpbwe[[pr");
    DS_free(str);
}
END_TEST

START_TEST(multiply_strings_default) {
    dynamic_string *str = DS_init(NULL);
    multiply_strings(str, "123", "55");
    check_DS(str, "6765");
    multiply_strings(str, "9999999999999999999999999999999999", "9999999999999999999999999999999999");
    check_DS(str, "99999999999999999999999999999999980000000000000000000000000000000001");
    multiply_strings(str, "11111", "0");
    check_DS(str, "0");
    multiply_strings(str, "11111", "11111");
    check_DS(str, "123454321");
    DS_free(str);
}
END_TEST

START_TEST(multiply_string_by_digit_default) {
    dynamic_string *str = DS_init(NULL);
    multiply_string_by_digit(str, "123456789", 9, 9, false);
    check_DS(str, "1111111101");
    multiply_string_by_digit(str, "123456789", 9, -9, false);
    check_DS(str, "-1111111101");
    multiply_string_by_digit(str, "-123456789", 10, 9, false);
    check_DS(str, "-1111111101");
    multiply_string_by_digit(str, "123456789", 9, 0, false);
    check_DS(str, "0");
    DS_free(str);
}
END_TEST

START_TEST(sum_strings_default) {
    dynamic_string *str = DS_init(NULL);
    sum_strings(str, "10", 2, "1", 1, 0, false);
    check_DS(str, "11");
    sum_strings(str, "1000", 4, "1", 1, 1, false);
    check_DS(str, "1010");
    sum_strings(str, "99999999999999999999999", 23, "99999999999999999999999", 23, 0, false);
    check_DS(str, "199999999999999999999998");
    sum_strings(str, "10", 2, "0", 1, 0, false);
    check_DS(str, "10");
    sum_strings(str, "10", 2, "0", 1, 1, false);
    check_DS(str, "10");
    DS_free(str);
}
END_TEST

START_TEST(DS_append_char_default) {
    dynamic_string *str = DS_init(NULL);
    DS_append_char(str, 'a');
    check_DS(str, "a");
    DS_append_char(str, 'b');
    check_DS(str, "ab");
    DS_set_text(str, "rqwrfqrvqwqwvqwrbf");
    DS_append_char(str, '1');
    check_DS(str, "rqwrfqrvqwqwvqwrbf1");
    DS_free(str);
}
END_TEST

START_TEST(DS_insert_text_default) {
    dynamic_string *str = DS_init("0");
    DS_insert_text(str, "1", 0);
    check_DS(str, "10");
    DS_insert_text(str, "0", 1);
    check_DS(str, "100");
    DS_insert_text(str, "10", str->length);
    check_DS(str, "10010");
    DS_insert_text(str, "30", 1);
    check_DS(str, "1300010");
    DS_insert_text(str, "abc1241fwf3434", 0);
    check_DS(str, "abc1241fwf34341300010");
    DS_insert_text(str, "", 0);
    check_DS(str, "abc1241fwf34341300010");
    DS_insert_text(str, "123", 2);
    check_DS(str, "ab123c1241fwf34341300010");
    DS_free(str);
}
END_TEST

START_TEST(print_binary_default) {
    char d = CHAR_MAX;
    print_binary(&d, 3);
    int i = INT_MAX;
    print_binary(&i, 32);
    unsigned int ui = UINT_MAX;
    print_binary(&ui, 32);
    unsigned long long ull = UINT_MAX;
    print_binary(&ull, sizeof(unsigned long long) * CHAR_BIT);
}
END_TEST

// Функция создания набора тестов.
Suite *s21_string_suite(void) {
    Suite *suite = suite_create("Testing liblw_utils.a");
    // Набор разбивается на группы тестов, разделённых по каким-либо критериям.
    TCase *LST = tcase_create("List");
    tcase_add_test(LST, list_add_default);

    TCase *STRMULT = tcase_create("String multiplication");
    tcase_add_test(STRMULT, multiply_strings_default);
    tcase_add_test(STRMULT, multiply_string_by_digit_default);
    TCase *STRSUM = tcase_create("String additions");
    tcase_add_test(STRSUM, sum_strings_default);
    TCase *DYNSTR = tcase_create("Dynamic string");
    tcase_add_test(DYNSTR, dynamic_string_init_default);
    tcase_add_test(DYNSTR, dynamic_string_set_char_default);
    tcase_add_test(DYNSTR, dynamic_string_set_char_insert);
    tcase_add_test(DYNSTR, dynamic_string_set_char_out_of_bounds);
    tcase_add_test(DYNSTR, dynamic_string_set_text_default);
    tcase_add_test(DYNSTR, dynamic_string_set_text_const);
    tcase_add_test(DYNSTR, DS_append_char_default);
    tcase_add_test(DYNSTR, DS_insert_text_default);
    TCase *MISC = tcase_create("Misc");
    tcase_add_test(MISC, print_binary_default);
    // Добавление теста в тестовый набор.
    suite_add_tcase(suite, LST);
    suite_add_tcase(suite, STRMULT);
    suite_add_tcase(suite, STRSUM);
    suite_add_tcase(suite, DYNSTR);
    suite_add_tcase(suite, MISC);

    return suite;
}

int main(void) {
    Suite *suite = s21_string_suite();
    SRunner *suite_runner = srunner_create(suite);
    srunner_set_fork_status(suite_runner, CK_NOFORK);

    srunner_run_all(suite_runner, CK_NORMAL);
    // Получаем количество проваленных тестов.
    int failed_count = srunner_ntests_failed(suite_runner);
    srunner_free(suite_runner);

    return failed_count ? EXIT_FAILURE : EXIT_SUCCESS;
}

