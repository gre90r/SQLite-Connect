###########################################
# @author: gre90r                         #
# builds application and test             #
###########################################

# application name
EXECUTABLE_NAME = sqliteApp

# compiler options
CC = g++
CCFLAGS = -Wall -std=c++11

# includes
INCLUDE_PATH = -I/usr/include

# linking libraries for production
LIBS = -lsqlite3

# folders
BUILD_DIR = build
SRC_FOLDER = src
TEST_FOLDER = test

# files
SRC_FILES = $(SRC_FOLDER)/Sqlite.cpp $(SRC_FOLDER)/main.cpp

# optimize level for production
OPTI = 2


	###########
	# recipes #
	###########

# build application for debug
all:
	$(CC) $(CCFLAGS) -g -O0 -o $(BUILD_DIR)/$(EXECUTABLE_NAME) $(SRC_FILES) $(LIBS)

# build application for production
production:
	$(CC) $(CCFLAGS) -O$(OPTI) -o $(BUILD_DIR)/$(EXECUTABLE_NAME) $(SRC_FILES) $(LIBS)

# runs the application
run:
	./$(BUILD_DIR)/$(EXECUTABLE_NAME)

# this is automatically called by 'make test' to compile the test code
build-test:
	$(CC) $(CCFLAGS) -g -o maintest $(TEST_FOLDER)/main_test.cpp $(SRC_FOLDER)/Sqlite.cpp \
  -L /usr/lib $(INCLUDE_PATH) -pthread -lgtest $(LIBS)

# runs the tests
test: build-test
	./maintest

# remove created files
clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf coverage/
	rm -f maintest
	rm -f maintest_coverage
	rm -f test.db
	rm -f my_res.info
	rm -f *.gcda
	rm -f *.gcno


		########################
		# code coverage report #
		########################

# this will generate an executable with coverage flag enabled.
# this will be called by 'make lcov'.
maintest_coverage: $(SRC_FOLDER)/Sqlite.cpp $(SRC_FOLDER)/Sqlite.h $(TEST_FOLDER)/main_test.cpp
	$(CC) $(CCFLAGS) -o maintest_coverage -fprofile-arcs -ftest-coverage \
	$(TEST_FOLDER)/main_test.cpp $(SRC_FOLDER)/Sqlite.cpp \
	-L /usr/lib $(INCLUDE_PATH) -pthread -lgtest $(LIBS)

# create code coverage report
lcov: maintest_coverage
	./maintest_coverage
	gcov $(TEST_FOLDER)/main_test.cpp
	lcov -t "BranchCoverage" -o my_res.info -c --rc lcov_branch_coverage=1 -d .
	genhtml -o coverage --rc lcov_branch_coverage=1 my_res.info
	firefox coverage/index.html &
