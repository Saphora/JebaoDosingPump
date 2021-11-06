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
	Pump p1(1, "Vega A", 1);
	controller.addPump(p1);
	Pump p2(2, "Vega B", 2);
	controller.addPump(p2);
	Pump p3(3, "Flores A", 4);
	controller.addPump(p3);
	Pump p4(4, "Flores B", 8);
	controller.addPump(p4);
	Pump p5(5, "Rhizotonic", 16);
	controller.addPump(p5);
	Pump p6(6, "CannaZym", 32);
	controller.addPump(p6);
	Pump p7(7, "Boost", 64);
	controller.addPump(p7);
	Pump p8(8, "PH-", 128);
	controller.addPump(p8);
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
		// if(input == "start") {
		// 	p.start();
		// } else if(input == "stop") {
		// 	p.stop();
		// } else {

		// 	printLine("Invalid argument detected. Stopping calibration.");
		// }
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