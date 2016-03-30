NAME=root_cmp
DEP_CLASS=root_comparator
BIN_DIR=bin
SRC_DIR=src
CC=g++
PRE_PROC=root-config --cflags --glibs

OBJS=$(SRC_DIR)/$(NAME).cpp\
	 $(SRC_DIR)/$(DEP_CLASS).cpp

all: $(BIN_DIR)/$(NAME)
	
$(BIN_DIR)/$(NAME):$(OBJS) 
	$(CC) -I$(SRC_DIR) $^ -o $@ `$(PRE_PROC)` 

.PHONY: clean
clean:
	@rm $(BIN_DIR)/$(NAME)
