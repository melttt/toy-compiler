DIR := $(shell pwd)
SOURCE_DIR := $(DIR)/source
TEST_INPUT_FILE := ../assembler/xas.out
C_OBJS := $(shell find $(SOURCE_DIR) -name "*.c")
MEM_FILE := $(DIR)/memwatch-2.71/

vm : $(C_OBJS) 
	gcc $^ -o $@ -g -lm 

r : vm $(TEST_INPUT_FILE)
	./vm $(TEST_INPUT_FILE)

test :
	valgrind --tool=memcheck  --leak-check=full  --show-leak-kinds=all --log-file=./txt ./vm ../assembler/xas.out
clean: 
	rm -f ./vm ./txt


