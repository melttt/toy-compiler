DIR := $(shell pwd)
SOURCE_DIR := $(DIR)/source
TEST_INPUT_FILE := $(DIR)/source/testfile
C_OBJS := $(shell find $(SOURCE_DIR) -name "*.c")

xas : $(C_OBJS) 
	gcc $^ -o $@	


r : xas
	./xas $(TEST_INPUT_FILE)

clean: 
	rm -f ./xas

