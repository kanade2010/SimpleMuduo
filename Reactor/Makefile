OBJOUT := ./out/
EXEOUT := ./out/
INCLUDE_DIR := ../SyncLogging/
LIB := -lpthread
LOGGER_DIR = ../SyncLogging/

SRC := $(wildcard *.cpp)
SRC += $(wildcard $(LOGGER_DIR)*.cpp)
OBJ := $(patsubst %.cpp,%.o,$(SRC))
TARGET := test.out

#CC := arm-oe-linux-gnueabi-g++
CC := g++
CFLAGS := -I$(INCLUDE_DIR)

all: $(TARGET)
$(TARGET) : $(OBJ)
	@echo "SRC : " $(SRC) 
	@echo "OBJ : " $(OBJ) 
	@echo "TARGET : " $(TARGET) 
	#$(CC) -C $(LOGGER_DIR)
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LIB) -std=c++11
	#mv $^ $(OBJOUT)
	@echo "Compiling" $@ "end\n"

%.o : %.c
	@echo "Compiling" $< "..."
	$(CC) $(CFLAGS) -c $^ -o $@ $(LIB)
	@echo "Compiling" $@ "end\n"

%.o : %.cpp
	@echo "Compiling" $< "..."
	$(CC) $(CFLAGS) -c $^ -o $@ $(LIB) -std=c++11
	@echo "Compiling" $@ "end\n"

.PHONY: clean

clean:
	rm -rf $(TARGET)
	rm -rf $(LOGGER_DIR)*.o
	rm -rf ./*.o
