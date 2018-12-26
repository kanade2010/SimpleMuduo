OBJOUT := ./out/
EXEOUT := ./out/
INCLUDE_DIR := ../AsyncLogging/
LIB := -lpthread
LOGGER_DIR = ../AsyncLogging/

SRC := $(wildcard *.cpp)
SRC += $(wildcard $(LOGGER_DIR)*.cpp)
OBJ := $(patsubst %.cpp,%.o,$(SRC))
TARGET := test.out

#CC := arm-oe-linux-gnueabi-g++
CC := g++
CFLAGS := -I$(INCLUDE_DIR)

all: $(TARGET)
$(TARGET) : $(OBJ)
	$(info "SRC : " $(SRC))
	$(info "OBJ : " $(OBJ))
	$(info "TARGET : " $(TARGET))
	$(CC) $(CFLAGS) $^ -g -o $(TARGET) $(LIB) -std=c++11
	$(info "Compiling" $@ "end")

%.o : %.c
	$(info "Compiling" $< "...")
	$(CC) $(CFLAGS) -c $^ -o $@ $(LIB)
	$(info "Compiling" $@ "end")

%.o : %.cpp
	$(info "Compiling" $< "...")
	$(CC) $(CFLAGS) -c $^ -o $@ $(LIB) -std=c++11
	$(info "Compiling" $@ "end")

.PHONY: clean

clean:
	rm -rf $(TARGET)
	rm -rf $(LOGGER_DIR)*.o
	rm -rf ./*.o
