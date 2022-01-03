// Copyright 2021 <lwolmer>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "lw_utils.h"


#if (defined(_WIN32) || defined(__linux__))
ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    size_t pos;
    int c;
    if (lineptr == NULL || stream == NULL || n == NULL) {
        errno = EINVAL;
        return -1;
    }
    c = getc(stream);
    if (c == EOF) {
        return -1;
    }
    if (*lineptr == NULL) {
        *lineptr = malloc(128);
        if (*lineptr == NULL) {
            return -1;
        }
        *n = 128;
    }
    pos = 0;
    while (c != EOF && c != '\n') {
        if (pos + 1 >= *n) {
            size_t new_size = *n + (*n >> 2);
            if (new_size < 128) {
                new_size = 128;
            }
            char *new_ptr = realloc(*lineptr, new_size);
            if (new_ptr == NULL) {
                return -1;
            }
            *n = new_size;
            *lineptr = new_ptr;
        }

        ((unsigned char *)(*lineptr))[pos ++] = c;
        c = getc(stream);
    }

    (*lineptr)[pos] = '\0';
    return pos;
}
#endif


list *new_node(void *data, bool is_dynamic) {
    list *new_node = malloc(sizeof(list));
    if (new_node) {
        new_node->data = data;
        new_node->is_dynamic = is_dynamic;
        new_node->next_node = NULL;
    }
    return new_node;
}

list *list_add(list **first_node, void *data, bool is_dynamic) {
    if (first_node && *first_node) {
        list *node_head = *first_node;
        while (node_head->next_node)
            node_head = node_head->next_node;
        node_head->next_node = new_node(data, is_dynamic);
    } else if (first_node) {
        *first_node = new_node(data, is_dynamic);
    }
    return *first_node;
}



int list_add_from_file(list **first_node, char *filename) {
    FILE *file = fopen(filename, "r");
    int error = 0;
    if (file) {
        ssize_t read = 0;
        size_t mem_len = 0;
        char *line = NULL;
        while ((read = getline(&line, &mem_len, file)) != -1) {
            size_t eol = strcspn(line, "\n");
            if (eol)
                line[eol] = 0;
            char *data = calloc(1, sizeof(char) * eol);
            if (data) {
                strncpy(data, line, eol);
                list_add(first_node, ((void*)data), true);
            }
        }
        if (line)
            free(line);
        fclose(file);
    } else {
        error = errno;
    }
    return error;
}

void free_node(list **node) {
    if ((*node)->is_dynamic)
        free((*node)->data);
    free(*node);
}

void list_pop(list **first_node) {
    if (first_node) {
        if ((*first_node)->next_node == NULL) {
            free_node(first_node);
            *first_node = NULL;
        } else {
            list *node_head = *first_node;
            list *prev_node = NULL;
            while (node_head->next_node != NULL) {
                prev_node = node_head;
                node_head = node_head->next_node;
            }
            free_node(&node_head);
            if (prev_node)
                prev_node->next_node = NULL;
        }
    }
}

void list_free(list **first_node) {
    while (first_node && *first_node)
        list_pop(first_node);
}

list *list_get_last(const list *first_node) {
    list *node_head = (list*)first_node;
    if (node_head)
        while (node_head->next_node)
            node_head = node_head->next_node;
    return node_head;
}

size_t list_get_length(const list *first_node) {
    list *node_head = (list*)first_node;
    size_t len = 0;
    while (node_head) {
        node_head = node_head->next_node;
        len++;
    }
    return len;
}

bool capp_assert(char *command1, char *command2, bool suppress_msg) {
    FILE *pipe1, *pipe2;
    bool error = false;
    pipe1 = popen(command1, "r");
    pipe2 = popen(command2, "r");
    if (pipe1 && pipe2) {
        size_t line_count = 1;
        ssize_t read1 = 0, read2 = 0;
        char *buf_ptr1 = NULL, *buf_ptr2 = NULL;
        size_t mem_len1 = 0, mem_len2 = 0;
        do {
            read1 = getline(&buf_ptr1, &mem_len1, pipe1);
            read2 = getline(&buf_ptr2, &mem_len2, pipe2);
            if (!(read1 < 0 && read2 < 0) && (read1 < 0 || read2 < 0 || strcmp(buf_ptr1, buf_ptr2) != 0)) {
                if (!suppress_msg) {
                    size_t eol = 0;
                    if (read1 > 0) {
                        eol = min_long(strcspn(buf_ptr1, "\n"), read1);
                        if (eol)
                            buf_ptr1[eol] = 0;
                    }
                    if (read2 > 0) {
                        eol = min_long(strcspn(buf_ptr2, "\n"), read2);
                        if (eol)
                            buf_ptr2[eol] = 0;
                    }
                    printf("=== Command output assertion FAILED ===\n");
                    printf("line: %zu\n\"%s\" != \"%s\"\n\"%s\" != \"%s\"\n", line_count, command1, \
                            command2 , buf_ptr1 ? buf_ptr1 : "NULL", buf_ptr2 ? buf_ptr2 : "NULL");
                    printf("=== END ===\n");
                }
                error = true;
            }
            line_count++;
        } while (read1 >= 0 && read2 >= 0 && !error);
        if (buf_ptr1) free(buf_ptr1);
        if (buf_ptr2) free(buf_ptr2);
    }
    pclose(pipe1);
    pclose(pipe2);
    return !error;
}

long max_long(long a, long b) {
    return a > b ? a : b;
}

long min_long(long a, long b) {
    return a < b ? a : b;
}

long clamp_long(long d, long min, long max) {
    const long t = d < min ? min : d;
    return t > max ? max : t;
}

bool is_number(const char *str) {
    bool error = 0;
    size_t index = 0;
    while (str && *str && !error) {
        if (!isdigit(*str)) {
            if (!(index == 0 && (*str == '-' || *str == '+')))
                error = 1;
        }
        str++;
    }
    return !error;
}

void str_reverse(char *str) {
    size_t len = strlen(str);
    for (size_t i = 0; i < len / 2; i++) {
        char t = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = t;
    }
}

dynamic_string *DS_realloc(dynamic_string *dest, const size_t mem_size) {
    dest->string = realloc(dest->string, mem_size);
    if (dest->string == NULL)
        fprintf(stderr, "ERROR (dynamic_string): Cant realloc.\n");
    else
        dest->mem_size = mem_size;
    return dest;
}

void DS_free(dynamic_string *ds) {
    free(ds->string);
    free(ds);
}

dynamic_string *DS_set_text(dynamic_string *dest, char *src) {
    if (dest) {
        size_t src_len = strlen(src);
        size_t new_mem_size = max_long((src_len + 1), dest->mem_size);
        if (new_mem_size > dest->mem_size)
            dest = DS_realloc(dest, new_mem_size);
        if (dest) {
            dest->length = src_len;
            dest->string = strncpy(dest->string, src, dest->mem_size);
        }
    }
    return dest;
}

dynamic_string *DS_init(char *str) {
    dynamic_string *string = malloc(sizeof(dynamic_string));
    if (string) {
        string->mem_size = 20;
        string->string = malloc(string->mem_size);
        if (string->string == NULL) {
            fprintf(stderr, "ERROR (dynamic_string): Couldnt malloc.\n");
            free(string);
            string = NULL;
        } else {
            if (str)
                string = DS_set_text(string, str);
            else
                string = DS_set_text(string, "");
        }
    }
    return string;
}

dynamic_string *DS_insert_text(dynamic_string *dest, const char *src, const size_t pos) {
    size_t src_len = strlen(src);
    if (dest && src_len) {
        size_t new_mem_size = src_len + dest->length + 1;
        if (new_mem_size > dest->mem_size)
            dest = DS_realloc(dest, new_mem_size);
        memmove(dest->string + pos + src_len, dest->string + pos, dest->length - pos + 1);
        memcpy(dest->string + pos, src, src_len);
        dest->length = new_mem_size - 1;
    } else if (dest) {
        if (!src_len) {
            fprintf(stderr, "ERROR (dynamic_string):[%s][%zu] trying to insert string '%s'" \
            " that is empty.\n", dest->string, dest->length, src);
        } else {
            fprintf(stderr, "ERROR (dynamic_string):[%s][%zu] trying to insert string '%s'" \
            " at position '%zu' that is out of bounds.\n", dest->string, dest->length, src, pos);
        }
    }
    return dest;
}

dynamic_string *DS_append_char(dynamic_string *dest, const char src) {
    return DS_set_char(dest, src, dest->length);
}

dynamic_string *DS_set_char(dynamic_string *dest, const char src, const size_t pos) {
    if (dest) {
        // Trying to put char inside string
        if (pos < dest->length) {
            dest->string[pos] = src;
            if (src == 0)
                dest->length = pos;
        // Trying to put char at end of the string and have enough memory
        } else if (pos == dest->length && dest->length < dest->mem_size - 1) {
            if (!(src == 0 && dest->string[pos] == 0)) {
                dest->string[pos] = src;
                dest->length++;
            }
        // Trying to put char at end of the string and need to allocate more memory
        } else if (pos == dest->length && dest->length == dest->mem_size - 1) {
            if (!(src == 0 && dest->string[pos] == 0)) {
                dest = DS_realloc(dest, dest->mem_size + sizeof(char));
                if (dest) {
                    dest->string[pos] = src;
                    dest->length++;
                    dest->string[dest->length] = 0;
                }
            }
        } else if (pos > dest->length) {
            fprintf(stderr, "ERROR (dynamic_string):[%s][%zu] trying to set character '%c'" \
            " at position '%zu' that is out of bounds.\n", dest->string, dest->length, src, pos);
        }
    }
    return dest;
}


dynamic_string *multiply_string_by_digit(dynamic_string *result, const char *str1, const size_t len, \
                                int digit, bool keep_reversed) {
    if (digit <= 9 && digit >= -9) {
        if (digit == 0) {
            result = DS_set_text(result, "0");
            return result;
        }
        result = DS_set_text(result, "");
        bool is_negative = digit >= -9 && digit < 0;
        if (is_negative)
            digit *= -1;
        int carryover = 0;
        for (int j = len - 1; j >= 0 && isdigit(str1[j]); j--) {
            int index_j = len - 1 - j;
            int d = (str1[j] - '0') * digit + carryover;
            result = DS_set_char(result, d % 10 + '0', index_j);
            carryover = d / 10;
        }
        if (carryover)
            result = DS_set_char(result, carryover + '0', result->length);
        if (str1[0] == '-') {
            if (is_negative)
                is_negative = false;
            else
                is_negative = true;
        }
        if (is_negative)
            DS_set_char(result, '-', result->length);
    } else {
        fprintf(stderr, "ERROR (multiply_string): digit must be in range (-9 - 9)\n");
        errno = EINVAL;
    }
    if (!keep_reversed)
        str_reverse(result->string);

    return result;
}

dynamic_string *sum_strings(dynamic_string *result, const char *str1, const size_t len1,
                    const char *str2, const size_t len2, size_t offset, bool using_reversed) {
    if (offset > len1 - 1) {
        fprintf(stderr, "ERROR (sum_strings): offset cant be more than str1 lenght.\n");
        return result;
    }
    char *buf1 = malloc(len1 + 1);
    char *buf2 = malloc(len2 + 1);
    if (!buf1 || !buf2) {
        fprintf(stderr, "ERROR (sum_strings): error during malloc\n");
        return result;
    }
    buf1 = strncpy(buf1, str1, len1 + 1);
    buf2 = strncpy(buf2, str2, len2 + 1);
    if (!using_reversed) {
        str_reverse(buf1);
        str_reverse(buf2);
    }
    size_t index = 0;
    int carryover = 0;
    char *str1_p = buf1;
    char *str2_p = buf2;
    while (*str1_p || *str2_p) {
        if (offset) {
            DS_set_char(result, *str1_p, index);
            index++;
            str1_p++;
            offset--;
        } else {
            int d = carryover;
            if (*str1_p) {
                d += *str1_p - '0';
                str1_p++;
            }
            if (*str2_p) {
                d += *str2_p - '0';
                str2_p++;
            }
            result = DS_set_char(result, d % 10 + '0', index);
            carryover = d / 10;
            index++;
        }
    }
    if (carryover) {
        result = DS_set_char(result, carryover + '0', index);
        index++;
    }
    result = DS_set_char(result, 0, index);
    if (!using_reversed) {
        str_reverse(result->string);
    }
    free(buf1);
    free(buf2);
    return result;
}

dynamic_string **compute_multi_rows(dynamic_string **buffers, const char *str1, const size_t len1,
                                 const char *str2, const size_t len2) {
     // long multiplying and storing in reverse order
    for (int i = len2 - 1; i >= 0; i--) {
        int index_i = len2 - 1 - i;
        buffers[index_i] = multiply_string_by_digit(buffers[index_i], str1, len1, str2[i] - '0', true);
    }
    return buffers;
}

dynamic_string *sum_multi_rows(dynamic_string *result_str, dynamic_string **buffers, \
                         const size_t buf_size) {
    result_str = DS_set_text(result_str, buffers[0]->string);
    for (size_t i = 1; i < buf_size; i++) {
        sum_strings(result_str, result_str->string, result_str->length,
                             buffers[i]->string, buffers[i]->length, i, true);
    }
    return result_str;
}

dynamic_string *multiply_strings(dynamic_string *result_str, const char *str1, const char *str2) {
    if (!result_str || !is_number(str1) || !is_number(str2))
        return result_str;
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    dynamic_string **buffers = calloc(len2, sizeof(dynamic_string*));
    for (size_t i = 0; i < len2; i++)
        buffers[i] = DS_init(NULL);
    buffers = compute_multi_rows(buffers, str1, len1, str2, len2);
    result_str = sum_multi_rows(result_str, buffers, len2);

    if (result_str->string[0] == '0')
        DS_set_text(result_str, "0");
    else
        str_reverse(result_str->string);
    for (size_t i = 0; i < len2; i++)
        DS_free(buffers[i]);
    free(buffers);
    return result_str;
}
