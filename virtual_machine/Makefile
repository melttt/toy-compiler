DIR := $(shell pwd)
SOURCE_DIR := $(DIR)/source
TEST_INPUT_FILE := ../assembler/xas.out
C_OBJS := $(shell find $(SOURCE_DIR) -name "*.c")
MEM_FILE := $(DIR)/memwatch-2.71/

vm : $(C_OBJS) $(MEM_FILE)/memwatch.c
	gcc $^ -o $@ -g -lm 

r : vm $(TEST_INPUT_FILE)
	./vm $(TEST_INPUT_FILE)

clean: 
	rm -f ./vm

