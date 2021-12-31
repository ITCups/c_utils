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
    while (str && *str && !error) {
        if (!isdigit(*str))
            error = 1;
        str++;
    }
    return !error;
}

void reverse_str(char *str) {
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

dynamic_string *DS_insert_char(dynamic_string *dest, const char src, const size_t pos) {
    if (dest && pos < dest->length) {
        dest = DS_realloc(dest, dest->mem_size + sizeof(char));
        dest->length++;
        memmove(dest->string + pos + 1, dest->string + pos, dest->length - pos);
        dest->string[pos] = src;
    } else if (dest) {
        fprintf(stderr, "ERROR (dynamic_string):[%s][%zu] trying to insert character '%c'" \
        " at position '%zu' that is out of bounds.\n", dest->string, dest->length, src, pos);
    }
    return dest;
}

dynamic_string *DS_set_char(dynamic_string *dest, const char src, const size_t pos) {
    if (dest) {
        if (pos < dest->length) {
            dest->string[pos] = src;
        } else if (pos == dest->length && dest->length < dest->mem_size - 1) {
            dest->string[pos] = src;
            dest->length++;
        } else if (pos == dest->length && dest->length == dest->mem_size - 1) {
            dest = DS_realloc(dest, dest->mem_size + sizeof(char));
            if (dest) {
                dest->string[pos] = src;
                dest->length++;
                dest->string[dest->length] = 0;
            }
        } else if (pos > dest->length) {
            fprintf(stderr, "ERROR (dynamic_string):[%s][%zu] trying to set character '%c'" \
            " at position '%zu' that is out of bounds.\n", dest->string, dest->length, src, pos);
        }
    }
    return dest;
}

dynamic_string *multiply_string_by_digit(dynamic_string *result, const char *str1, const size_t len,
                                const size_t digit, bool keep_reversed) {
    if (digit <= 9) {
        int carryover = 0;
        for (int j = len - 1; j >= 0; j--) {
            int index_j = len - 1 - j;
            int d = (str1[j] - '0') * digit + carryover;
            result = DS_set_char(result, d % 10 + '0', index_j);
            carryover = d / 10;
        }
        if (carryover)
            result = DS_set_char(result, carryover + '0', len);
    } else {
        fprintf(stderr, "ERROR (multiply_string): digit must be in range (0-9)\n");
        errno = EINVAL;
    }
    if (!keep_reversed)
        reverse_str(result->string);
    return result;
}

dynamic_string *sum_strings(dynamic_string *result, const char *str1, const size_t len1,
                            const char *str2, const size_t len2, const size_t offset) {
    int carryover = 0;
    for (int j = len2 - 1; j >= 0; j--) {
        int index = len2 - 1 - j;
        int d = (str2[index] - '0') + carryover;
        if (index + offset < len1) {
            d += str1[index + offset] - '0';
        }
        result = DS_set_char(result, d % 10 + '0', index + offset);
        carryover = d / 10;
    }
    if (carryover)
        result = DS_set_char(result, carryover + '0', len2 + offset);
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
                             buffers[i]->string, buffers[i]->length, i);
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
        reverse_str(result_str->string);
    for (size_t i = 0; i < len2; i++)
        DS_free(buffers[i]);
    free(buffers);
    return result_str;
}
