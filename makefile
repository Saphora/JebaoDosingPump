OBJS	= main.o CalibrationResult.o Preset.o PresetLoader.o Pump.o PumpController.o
SOURCE	= main.cpp CalibrationResult.cpp Preset.cpp PresetLoader.cpp Pump.cpp PumpController.cpp
HEADER	= CalibrationResult.h Preset.h PresetLoader.h Pump.h PumpController.h
OUT	= dosingpump.exe
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

clean:
	rm -f $(OBJS) $(OUT)