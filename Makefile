NAME=root_cmp
BIN_DIR=bin
SRC_DIR=src
CC=g++
PRE_PROC=root-config --cflags --glibs

OBJS=$(SRC_DIR)/$(NAME).cpp\
	 $(SRC_DIR)/root_file_comparator.cpp\
	 $(SRC_DIR)/root_obj_comparator.cpp


all: $(BIN_DIR)/$(NAME)

$(BIN_DIR)/$(NAME):$(OBJS) 
	@if [ ! -d "$(BIN_DIR)" ]; then mkdir $(BIN_DIR); fi
	$(CC) -I$(SRC_DIR) $^ -o $@ `$(PRE_PROC)` 

.PHONY: clean
clean:
	@rm $(BIN_DIR)/$(NAME)
