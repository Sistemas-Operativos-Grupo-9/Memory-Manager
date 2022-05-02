

BUILD_DIR=build
SRC_FILES=$(shell find src -name "*.c")
TEST_FILES=$(shell find test -name "*.c")
SRC_OBJ=$(SRC_FILES:src/%.c=$(BUILD_DIR)/%.o)
TEST_OBJ=$(TEST_FILES:test/%.c=$(BUILD_DIR)/test/%.o)

CFLAGS=-Iinclude

$(BUILD_DIR)/libmemory_manager.a: $(SRC_OBJ)
	ar rcs $@ $^

all: $(BUILD_DIR)/libmemory_manager.a test
	
test: $(BUILD_DIR)/run-test

$(BUILD_DIR)/run-test: $(BUILD_DIR)/libmemory_manager.a $(TEST_OBJ)
	$(CC) -o $@ $(TEST_OBJ) -L$(BUILD_DIR) -lmemory_manager



$(BUILD_DIR)/test/%.o: test/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD_DIR)/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -nostdlib -c -o $@ $^



clean:
	rm -rf build

.PHONY: test all clean
