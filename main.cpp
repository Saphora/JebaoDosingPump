#include <iostream>
#include <wiringPi.h>
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

int main(int argc, char** args) {
	wiringPiSetup();
	std::cout << "Dosing Pump Controller v1.0, this software is very specific for jebao pumps" << std::endl;
	setup();
	controller.showMainMenu(true, false);
	
	return 0;
}