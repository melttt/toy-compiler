DIR := $(shell pwd)
SOURCE_DIR := $(DIR)/source
TEST_INPUT_FILE := $(DIR)/source/testfile
TEST_INPUT_FILE2 := $(DIR)/source/test_1.xasm
C_OBJS := $(shell find $(SOURCE_DIR) -name "*.c")

xas : $(C_OBJS) 
	gcc $^ -o $@ -g


r : xas
	./xas $(TEST_INPUT_FILE)

clean: 
	rm -f ./xas ./xas.out

