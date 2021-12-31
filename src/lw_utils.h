// Copyright 2021 <lwolmer>
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
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
#endif
#if defined(_WIN32)
typedef intptr_t ssize_t;
#endif

typedef struct {
    char *string;
    size_t mem_size;
    size_t length;
} dynamic_string;


/*
    Structure for node in liked list
*/
typedef struct list {
    void *data;
    bool is_dynamic;
    struct list *next_node;
} list;

/*
    Creates or adds to linkied list containing data
    args: 
        first_node: pointer to a first element of param list to add to, if NULL creates new list
        flag: data
        is_dynamic: if true then on list_free() data will be free'd
    returns: pointer to first element in list
*/
list *list_add(list **first_node, void *data, bool is_dynamic);
/*
    Creates or adds to linked list form a file one line at the time
    args:
        first_node: pointer to a first element of param list to add to, if NULL creates new list
        filename: path to filename
    returns: zero if success or error code
*/
int list_add_from_file(list **first_node, char *filename);
/*
    Removes last element in linked list
    args: 
        first_node: pointer to a first element of param list
*/
void list_pop(list **first_node);
/*
    Removes all elements in linked list
*/
void list_free(list **first_node);

/*
    Gets the pointer to the last node in list
    args:
        first_node: pointer to a first element of param list
    returns: pointer to the last node in first_node or NULL if first_node is NULL
*/
list *list_get_last(const list *first_node);

/*
    Gets length of the list
    args:
        first_node: pointer to a first element of param list
    returns: length of the list
*/
size_t list_get_length(const list *first_node);

/*
    Gets length of the list
    args:
        command1: first command which output we compare to second command
        command2: second command
        suppress_msg: if true suppresses error msg that prints output and tells its not equal
    returns: true if output of both commands are the same
*/
bool capp_assert(char *command1, char *command2, bool suppress_msg);

/*
    Returns the maximum of given numbers
    @param a first number to compare
    @param b second number to compare
    @return The largest number from numbers passed as args
*/
long max_long(long a, long b);

/*
    Returns the minimum of given numbers
    @param a first number to compare
    @param b second number to compare
    @return The smallest number from numbers passed as args
*/
long min_long(long a, long b);
/*
    Clamps the number in given limits
    @param d number that going to be clamped
    @param min minimum limit
    @param max maximum limit
    @return Clamped number
*/
long clamp_long(long d, long min, long max);

bool is_number(const char *str);

void reverse_str(char *str);

dynamic_string *DS_realloc(dynamic_string *dest, const size_t mem_size);

void DS_free(dynamic_string *ds);

dynamic_string *DS_set_text(dynamic_string *dest, char *src);

dynamic_string *DS_init(char *str);

dynamic_string *DS_set_char(dynamic_string *dest, const char src, const size_t pos);

dynamic_string *multiply_strings(dynamic_string *result_str, const char *str1, const char *str2);

#endif  // SRC_LW_UTILS_H_
