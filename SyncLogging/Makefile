SRC := $(wildcard *.cpp)
OBJ := $(patsubst %.cpp,%.o,$(SRC))
TARGET := logtest

OBJOUT := ./out/
EXEOUT := ./out/
INCLUDE_DIR := ./
LIB := -lpthread

CC := g++
CFLAGS := -I$(INCLUDE_DIR)

all: $(TARGET)
$(TARGET) : $(OBJ)
	@echo "SRC : " $(SRC) 
	@echo "OBJ : " $(OBJ) 
	@echo "TARGET : " $(TARGET) 
	$(CC) $(CFLAGS) $^ -o $(EXEOUT)$@ $(LIB)
	mv $^ $(OBJOUT)
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
	rm -rf $(OBJOUT)*.o $(EXEOUT)$(TARGET)
	rm -rf test.log*
	rm -rf *.log
	rm -rf *.log.*
