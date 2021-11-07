#include <iostream>
#include "PumpController.h"

PumpController controller;

void fetchArgs(int argc, char** args) {

}

void printLine(std::string txt, bool doubleEnd=false) {
	if(doubleEnd) {
		std::cout << txt << std::endl << std::endl;
	} else {
		std::cout << txt << std::endl;
	}
}
void setup() {


	controller.setup();

}

void startCalibrationProcess(int argc, char **args) {
	if(argc == 3) { // ./dosingpump calibrate 2
		int pumpNr = atoi(args[2]);
		Pump p = controller.getPump(pumpNr);
		std::cout << std::endl << "Calibrating " << p.name << std::endl << std::endl;
		printLine("Please take 2x100ML of water and 2 empty measurementcups.", true);
		printLine("Type start and press enter to start calibrating. When 100ml is reached, type stop and hit enter.");
		std::string input = "";
		std::cin >> input;
		printLine(input);
	} else {
		printLine("Invalid argument count detected!");
		printLine("");
		printLine("Use: ./dosingpump calibrate x");
	}
}

int main(int argc, char** args) {
	std::cout << "Dosing Pump Controller v1.0, this software is very specific for jebao pumps" << std::endl;

	setup();
	controller.showMainMenu(true, false);
	
	return 0;
}