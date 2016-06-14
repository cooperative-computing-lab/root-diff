.PHONY: clean

NAME=root_diff
BIN_DIR=bin
SRC_DIR=src
CC=g++
CFLAGS=-std=c++0x -lrt -w
PRE_PROC=root-config --cflags --glibs

OBJS=$(SRC_DIR)/$(NAME).cpp\
	 $(SRC_DIR)/root_file_comparator.cpp\
	 $(SRC_DIR)/root_obj_comparator.cpp\
	 $(SRC_DIR)/timer.cpp

all: $(BIN_DIR)/$(NAME)

$(BIN_DIR)/$(NAME):$(OBJS) 
	@if [ ! -d "$(BIN_DIR)" ]; then mkdir $(BIN_DIR); fi
	$(CC) -I$(SRC_DIR) $(CFLAGS) $^ -o $@ `$(PRE_PROC)` 

clean:
	rm $(BIN_DIR)/$(NAME)
