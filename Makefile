

BUILD_DIR=build
SRC_FILES=$(shell find src -name "*.c")
TEST_FILES=$(shell find test -name "*.c")
CUTEST_FILES=cutest/CuTest.c
SRC_OBJ=$(SRC_FILES:src/%.c=$(BUILD_DIR)/%.o)
TEST_OBJ=$(TEST_FILES:test/%.c=$(BUILD_DIR)/test/%.o)
CUTEST_OBJ=$(CUTEST_FILES:cutest/%.c=$(BUILD_DIR)/cutest/%.o)

CFLAGS=-Iinclude -Icutest -g

$(BUILD_DIR)/libmemory_manager.a: $(SRC_OBJ)
	ar rcs $@ $^

$(BUILD_DIR)/libcutest.a: $(CUTEST_OBJ)
	ar rcs $@ $^


all: $(BUILD_DIR)/libmemory_manager.a test
	
test: $(BUILD_DIR)/run-test

$(BUILD_DIR)/run-test: $(BUILD_DIR)/libcutest.a $(BUILD_DIR)/libmemory_manager.a $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJ) -L$(BUILD_DIR) -lmemory_manager -lcutest



$(BUILD_DIR)/cutest/%.o: cutest/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD_DIR)/test/%.o: test/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD_DIR)/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -nostdlib -c -o $@ $^



clean:
	rm -rf build

.PHONY: test all clean
