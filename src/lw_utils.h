// Copyright 2021 <lwolmer>
/// @file
#ifndef SRC_LW_UTILS_H_
#define SRC_LW_UTILS_H_
#include <stdbool.h>
#include <stdio.h>
#if defined(__linux__)
#include <sys/types.h>
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#if defined(_WIN32) || defined(__linux__)
/// @brief Crossplatform plug for getline
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
#endif
#if defined(_WIN32)
/// @brief Crossplatform plug for windows
typedef intptr_t ssize_t;
#endif

/**
    @brief Structure for holding dynamic string
*/
typedef struct {
    char *string;  /**< raw string, !DO NOT MODIFY DIRECTLY!*/
    size_t mem_size;  /**< size of the string array*/
    size_t length;  /**< length of the string without terminating zero*/
} dynamic_string;


/**
    @brief Structure for node in liked list
*/
typedef struct list {
    void *data;
    bool is_dynamic;
    struct list *next_node;
} list;

/**
    @brief Creates or adds to linkied list containing data

    @param first_node pointer to a first element of param list to add to, if NULL creates new list
    @param flag data to put
    @param is_dynamic if true then on list_free() data will be free'd
    @return list : pointer to first element in list
*/
list *list_add(list **first_node, void *data, bool is_dynamic);
/**
    @brief Creates or adds to linked list form a file one line at the time

    @param first_node pointer to a first element of param list to add to, if NULL creates new list
    @param filename path to filename
    @return int : zero if success or error code
*/
int list_add_from_file(list **first_node, char *filename);
/**
    @brief Removes last element in linked list

    @param first_node: pointer to a first element of param list
*/
void list_pop(list **first_node);
/**
    @brief Removes all elements in linked list
*/
void list_free(list **first_node);

/**
    @brief Gets the pointer to the last node in list

    @param first_node pointer to a first element of param list
    @return list : pointer to the last node in first_node or NULL if first_node is NULL
*/
list *list_get_last(const list *first_node);

/**
    @brief Gets length of the list

    @param first_node pointer to a first element of param list
    @return size_t : length of the list
*/
size_t list_get_length(const list *first_node);

/**
    @brief Gets length of the list

    @param command1 first command which output we compare to second command
    @param command2 second command
    @param suppress_msg: if true suppresses error msg that prints output and tells its not equal
    @return bool : true if output of both commands are the same
*/
bool capp_assert(char *command1, char *command2, bool suppress_msg);

/**
    @brief Returns the maximum of given numbers

    @param a first number to compare
    @param b second number to compare
    @return long : The largest number from numbers passed as args
*/
long max_long(long a, long b);

/**
    @brief Returns the minimum of given numbers

    @param a first number to compare
    @param b second number to compare
    @return long : smallest number from numbers passed as args
*/
long min_long(long a, long b);
/**
    @brief Clamps the number in given limits

    @param d number that going to be clamped
    @param min minimum limit
    @param max maximum limit
    @return long : Clamped number
*/
long clamp_long(long d, long min, long max);

/**
    @brief Checks if str consist only form digits

    @param str string that will be checked
    @return bool : True if str is a number
*/
bool is_number(const char *str);

/**
    @brief Reverses string

    @param str string that will reversed
*/
void str_reverse(char *str);

dynamic_string *DS_init(char *str);

/**
    @brief Reallocs dynamic_string using new size

    @param dest Dynamic string that will be resized
    @return dynamic_string* : Resized dest
*/
dynamic_string *DS_realloc(dynamic_string *dest, const size_t mem_size);

/**
    @brief Frees memory for dynamic string

    @param ds Dynamic string that will free'd
*/
void DS_free(dynamic_string *ds);

/**
    @brief Puts text into dynamic_string and resizes it accordingly

    @param dest Dynamic string that will be modified
    @param src Raw text that will be used
    @return dynamic_string* Modified dynamic string
*/
dynamic_string *DS_set_text(dynamic_string *dest, char *src);

/**
    @brief Puts char into dynamic_string and resizes it accordingly (note that if char is placed in postion of terminating zero it will put char there and resize string)

    @param dest Dynamic string that will be modified
    @param src Char that will be used
    @param pos Position in which char is placed
    @return dynamic_string* : Modified dynamic string
*/
dynamic_string *DS_set_char(dynamic_string *dest, const char src, const size_t pos);
/**
    @brief Appends char at the end of dynamic string
    
    @param dest Dynamic string that will be modified
    @param src Char to append
    @return dynamic_string* : Modified dynamic string
 */
dynamic_string *DS_append_char(dynamic_string *dest, const char src);
/**
    @brief Miltiplies two strings and returns result as dynamic string

    @param result_str Dynamic string that will hold results of multiplication
    @param str1 First string containing number
    @param str2 Second string containing number
    @return dynamic_string* : result_str
 */
dynamic_string *multiply_strings(dynamic_string *result_str, const char *str1, const char *str2);

/**
    @brief Multiplies string by single digit
    
    @param result Dynamic string that will hold results of multiplication
    @param str1 First string containing number
    @param len length of the str1
    @param digit integer in range (-9 - 9)
    @param keep_reversed if true then result will be returned in reverse order
    @return dynamic_string* : result of multiplication that is reversed if keep_reversed set to true
 */
dynamic_string *multiply_string_by_digit(dynamic_string *result, const char *str1, const size_t len, \
                                             int digit, bool keep_reversed);
/**
    @brief Sums two strings (uses buffers, so you can pass the string from 'result', DOES NOT SUPPORT NEGATIVE NUMBERS YET)
    
    @param result Dynamic string that will hold results of addition
    @param str1 First string containing number
    @param len1 Length of the str1
    @param str2 Second string containing number
    @param len2 Length of the str2
    @param offset Starts adding str2 to str1 from this postion from the right
    @param using_reversed if true then result will be returned in reverse order
    @return dynamic_string* : result of addition that is reversed if keep_reversed set to true
 */
dynamic_string *sum_strings(dynamic_string *result, const char *str1, const size_t len1,
                            const char *str2, const size_t len2, size_t offset, bool using_reversed);

#endif  // SRC_LW_UTILS_H_
