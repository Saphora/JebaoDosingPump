SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)
OBJS = Preset.o PresetLoader.o Pump.o PumpController.o main.o
HEADER	= Preset.h PresetLoader.h Pump.h PumpController.h
OUT	= dosingpump
CC	 = g++
FLAGS	 = -g -c -Wall  -std=c++17
LFLAGS	 = -pthread -lwiringPi

all: $(OBJS)
	$(CC) -g $(OBJ) -o $(OUT) $(LFLAGS)

clean:
	rm -f $(OBJ) $(OUT)