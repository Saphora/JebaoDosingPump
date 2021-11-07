SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)
HEADER	= Preset.h PresetLoader.h Pump.h PumpController.h
OUT	= dosingpump
CC	 = g++
FLAGS	 = -g -c -Wall  -std=c++11
LFLAGS	 = -pthread

all: $(OBJS)
	$(CC) -g $(OBJ) -o $(OUT) $(LFLAGS)

clean:
	rm -f $(OBJ) $(OUT)