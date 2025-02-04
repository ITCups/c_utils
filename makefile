# compiler
CC = gcc
CFLAGS =-Werror -Wall -Wextra -Wshadow -g
OS = $(shell uname)
ifeq ($(OS), Darwin)
CFLAGS +=-std=c11
endif
# directories
BUILD_DIR = ./src
LDFLAGS = -L./ -L./lib
INCLUDE_DIR = -I./include

# programm dependant parameters
LIB_NAME = lw_utils
SRCS = ./src/lw_utils.c
OBJS = $(SRCS:.c=.o) # autogenerated obj names
LDLIBS =

# directory where html tests are created
TEST_HTML_DIR = ./report_lw_utils

.PHONY: all clean check
all: lib$(LIB_NAME).a

clean: 
# clean for compiler
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.a $(BUILD_DIR)/*.so $(BUILD_DIR)/.graf $(BUILD_DIR)/*.dSYM $(BUILD_DIR)/$(EXEC_NAME).exe $(BUILD_DIR)/$(LIB_NAME)
# clean lib and include
	rm -rf ./lib ./include
# clean for tests
	rm -rf *.gcda *.gcno $(BUILD_DIR)/*.gcov $(BUILD_DIR)/*.info $(TEST_HTML_DIR) $(BUILD_DIR)/$(TEST_NAME)

# Overriding impicit rule because fuck dem rules, they doesnt work properly
%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^ $(LDFLAGS) $(LDLIBS)

#############################################################
#															#
#					Library compilation						#
#															#
#############################################################
# compiles static library
lib$(LIB_NAME).a: $(OBJS)
	cd $(BUILD_DIR)/
	mkdir -p ./lib/
	ar rcs ./lib/lib$(LIB_NAME).a $(OBJS)
	@echo "library "lib$(LIB_NAME).a" compiled successfully!"
	mkdir -p ./include/
	cp ./src/lw_utils.h ./include/lw_utils.h

#############################################################
#															#
#					Testing and Covering					#
#															#
#############################################################
# check library and linux directives
TEST_LDLIBS = -lcheck -coverage -fprofile-arcs -ftest-coverage -llw_utils

# name of the test executable
TEST_NAME = test
TEST_SRCS = ./src/test.c
ifeq ($(OS), Linux)
TEST_LDLIBS +=
endif

# compiles test programm with gcov flags
build_test: lib$(LIB_NAME).a
	$(CC) $(CFLAGS) $(INCLUDE_DIR) -o $(BUILD_DIR)/$(TEST_NAME) $(TEST_SRCS) $(LDFLAGS) $(TEST_LDLIBS)

# runs tests for gdna files
test: build_test
	$(BUILD_DIR)/$(TEST_NAME)

# collects info file for genhtml
lcov: test
	lcov -t $(BUILD_DIR)/$(TEST_NAME) -o $(BUILD_DIR)/$(TEST_NAME).info -c -d $(BUILD_DIR)/

# generates html report
gcov_report: lcov
	genhtml $(BUILD_DIR)/$(TEST_NAME).info -o $(TEST_HTML_DIR)

# location of python script for cpplint style check
CPPLINT = ../../materials/linters/cpplint.py
#style check for all code in relative folder
check: test
ifeq ($(OS), Linux)
	$(MAKE) -i valgrind
else
	$(MAKE) leaks
endif
	python3 $(CPPLINT) --extensions=c $(BUILD_DIR)/**.c
	python3 $(CPPLINT) --extensions=h $(BUILD_DIR)/**.h
	cppcheck --enable=all --suppress=missingIncludeSystem $(BUILD_DIR)/**.c $(BUILD_DIR)/**.h

valgrind:
	valgrind --leak-check=yes $(BUILD_DIR)/$(TEST_NAME)

leaks:
	CK_FORK=no leaks --atExit -- $(BUILD_DIR)/$(TEST_NAME)

