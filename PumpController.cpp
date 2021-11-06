#include "PumpController.h"
#include <iostream>
#include <thread>
#include <assert.h>
#include <future>
#include <fstream>


Pump PumpController::findPumpByPumpNr(int nr) {
	int index = 0;
	for(Pump p : this->pumps) {
		if(nr == p.pumpNr) {
			return this->pumps[index];
		}
		index++;
	}
	return {};
}


Pump PumpController::findPumpByIndex(int index) {
	return this->pumps[index];
}

void PumpController::Kill() {
	this->alive = false;
}

bool PumpController::IsAlive() {
	return this->alive;
}

bool PumpController::DumpSnapShot() {
	try {
		std::string pumpCsv = "name|calibrated|timetodose100ml|pumpNr\n";
		std::string presetCsv = "name|ml|pumpNr|description\n";
		int index =0;
		for(index = 0; index < this->pumps.size(); index++) {
			Pump p = this->pumps[index];
			if(p.name != "__UNDEFINED__") {
				pumpCsv = pumpCsv + p.name + "|" + (p.calibrated == true ? "y" : "n")+ "|"+ std::to_string(p.timeToDose100Ml) + "|" + std::to_string(p.pumpNr) + std::string("\n");
			}
		}
		index =0;
		for(index = 0; index < this->presets.capacity(); index++) {
			Preset p = this->presets[index];
			if(p.name != "__UNDEFINED__") {
				presetCsv = presetCsv + p.name + "|" + std::to_string(p.ml) + "|"+ std::to_string(p.pumpNr) + "|" + p.description + std::string("\n");
			}
		}
		std::ofstream out("pumps.csv");
		out << pumpCsv;
		out.close();

		std::ofstream out2("presets.csv");
		out2 << presetCsv;
		out2.close();
		return true;
	} catch(...) {
		return false;
	}
}

bool PumpController::ParseSnapShot() {
	return false;
}

std::vector<Preset> PumpController::GetPresetsForPump() {
	std::vector<Preset> result;
	for(Preset pr : this->presets) {
		if(pr.pumpNr == this->_selectedPump.pumpNr) {
			result.push_back(pr);
		}
	}
	return result;
}

void PumpController::_dosing() {
	double timeToDose1ml = this->_selectedPump.timeToDose100Ml / 100;
	double timeToDosePreset = timeToDose1ml * this->GetPresetsForPump()[0].ml;
	auto start = std::chrono::high_resolution_clock::now();
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(100us);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end-start;
	std::cout << std::endl << "Dosing started: " << std::endl;
	*this->isDosing = true;
	int index = 0;
	while(elapsed.count() <= timeToDosePreset) {
		end = std::chrono::high_resolution_clock::now();
		elapsed = end-start;
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1ms);
		std::cout << ">";
	}

	std::cout << std::endl << "Dosing completed!" << std::endl;
}
void PumpController::startDose(int argc, char** args) {
	int pumpNr = 0;
	int ml = 0;
	if(argc == 0) {
		pumpNr = this->_fetchPumpNr();
		Pump pump = this->findPumpByPumpNr(pumpNr);
		std::cout << "You have selected pump with name [" << pump.name << "] is this correct?" << std::endl;
		if(pump.calibrated != true) {
			std::cout << "This pump is not calibrated. Please calibrate it first." << std::endl;
			return;
		}
		std::string yn = this->_fetchYesNo();
		this->backgroundWorker = new std::thread([this]() {_dosing();} );
		if(yn == "y" || yn == "yes") {
			int ml = 0;
			this->backgroundWorker->join();
		}
		std::cout << "DosingPump>" << std::endl;
	}
	
}

void PumpController::_calibrate() {
	while(this->isCalibrating) {
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1ms);
		std::cout << "=";
	}
}
void PumpController::calibrate(int argc, char **args) {
	if(argc == 0) {
		std::cout << "Welcome to the calibration mode!" << std::endl;
		int pumpNr = this->_fetchPumpNr();
		std::cout <<std::endl << "You have selected pump number " << pumpNr << std::endl << "if this is correct, type 'start' to run calibration process. hit enter to stop calibration. type 'stop' to abort calibration" << std::endl << "DosingPump>";
		std::string startStop = "";
		std::cin >> startStop;
		if(startStop == "stop") {
			std::cout << "Stopped calibration proces.";
			return;
		}
		if(startStop == "start" || startStop == "s") {
			std::string runner = "running";
			this->isCalibrating = true;
			this->backgroundWorker = new std::thread([this]() {_calibrate();} );
			auto start = std::chrono::high_resolution_clock::now();
			while(runner == std::string("running")) {
				this->backgroundWorker->detach();
				std::cout << "Hit any key and press enter to stop" << std::endl;
				std::cin >> runner;
			}
			this->isCalibrating = false;
			auto end = std::chrono::high_resolution_clock::now();	
			std::chrono::duration<double, std::milli> elapsed = end-start;
			std::cout << std::endl << "Calibration completed. It takes " << elapsed.count() << " milliseconds to dose 100ml" << std::endl;
			std::cout << "So a 1ml will take " << (elapsed.count() / 100) << " milliseconds." << std::endl;
			std::cout << "So a 10ml will take " << (elapsed.count() / 10) << " milliseconds." << std::endl;
			std::string yn = this->_fetchYesNo();
			if(yn=="y" || yn == "yes") {
				Pump pointerToPump = this->findPumpByPumpNr(pumpNr);
				pointerToPump.timeToDose100Ml = elapsed.count(); //ms
				pointerToPump.calibrated = true;
				std::cout << "Calibration result verified and saved to pump [" << pointerToPump.name << ":"<< pointerToPump.timeToDose100Ml << "ms]" << std::endl;
				std::cout << "Stopping calibration mode." << std::endl;
				int index = 0;
				int result = 0;
				for(Pump p : this->pumps) {
					if(p.pumpNr == pointerToPump.pumpNr) {
						result = index;
					}
					index++;
				}
				this->pumps[result] = pointerToPump;
			} else {
				this->calibrate(0, {});
			}
		}
	}
}

void PumpController::addPump(Pump p) {
	this->pumps.push_back(p);
}

Pump PumpController::getPump(int index) {
	return this->pumps[index];
}

void PumpController::showMainMenu(bool showWelcome, bool isHelp) {
		if(showWelcome == true) {
			if(isHelp == false) {
				std::cout << "Welcome to the interactive mode. Please enter a instruction to start" << std::endl;
				std::cout << "The following instructions are available now:" << std::endl << std::endl;
			}
			std::cout << "s/start - only when pump is stopped." << std::endl;
			std::cout << "s/stop - only when pump is started." << std::endl;
			std::cout << "p/pr/preset - to add a preset to pump x." << std::endl;
			std::cout << "c/cal/calibrate - to calibrate pump x." << std::endl;
			std::cout << "d/dose - to start dosing pump x." << std::endl;
			std::cout << "save - save snapshot of configuration" << std::endl;
			std::cout << "ls/list-presets - to list all presets." << std::endl;
			std::cout << "lp/list-pumps - to list all pumps." << std::endl;
			std::cout << "e/exit - to quit this application." << std::endl << std::endl;
			std::cout << "DosingPump>";
		} else {
			std::cout << "DosingPump>";
		}
		std::string input = "";
		std::cin >> input;
		
		startProcessByCin(input);
}

void PumpController::startProcessByCin(std::string processName) {
	if(processName == "calibrate" || processName == "c" || processName == "cal") {
		this->calibrate(0, {});
	}
	if(processName == "start" || processName == "s") {
		this->startPumping(0, {});
	}
	if(processName == "stop") {
		this->stopPumping(0, {});
	}
	if(processName == "dose"  || processName == "d") {
		this->startDose(0, {});
	}
	if(processName == "preset" || processName == "p") {
		this->preset(0, {});
	}
	if(processName == "save") {
		this->DumpSnapShot();
	}
	if(processName == "list-presets" || processName == "ls") {
		this->ListPresets();
	}
	if(processName == "list-pumps"  || processName == "lp") {
		this->ListPumps();
	}
	if(processName == "?" || processName == "help") {
		this->showMainMenu(true, true);
	}
	if(processName != "exit"  || processName == "e") {
		this->showMainMenu(false, false);
	}
}

void PumpController::setup() {
	bool calRequired = false;
	std::cout << "Check if calibration is required." << std::endl << std::endl;
	for(Pump p : this->pumps) {
		if(p.calibrated == true) {
	 		std::cout << "Pump " << p.name << " calibrated!" << std::endl;
		} else {
			calRequired = true;
			std::cout << "Pump " << p.name << " not calibrated!" << std::endl;
		}
	}
}

void PumpController::preset(int argc, char** args) {
	Preset p;
	if(argc == 0) {
		std::cout << "Welcome to the preset mode. Please enter the pump nr for this preset." << std::endl << "Pump number: ";
		int pumpNr = this->_fetchPumpNr();
		p.pumpNr = pumpNr;
		std::cout << std::endl << "Now enter milliliters for the preset." << std::endl << "ml: ";
		int ml = 0;
		std::string mlStr = "0";
		std::cin >> mlStr;
		p.ml = atoi(mlStr.c_str());
		std::cout << std::endl << "Please enter a name for the preset." << std::endl << "name: ";
		std::string presetName = "";
		std::cin >> presetName;
		p.name = presetName;
		std::cout << std::endl << "Please enter a description for the preset." << std::endl << "description: " << std::endl;
		std::string desc = "";
		std::cin >> desc;
		p.description = desc;
		this->presets.push_back(p);
		std::cout << "Preset added..." << std::endl;
	}
}
void PumpController::ListPresets() {
	std::cout << "listing presets here..." << std::endl << std::endl;
	for(Pump pump : this->pumps) {
		std::cout << "Presets for pump["<< pump.pumpNr << "] " << pump.name << std::endl << std::endl;
		for(Preset p : this->presets) {
			if(p.pumpNr == pump.pumpNr) {
				std::cout << "Preset name: " << p.name << std::endl;
				std::cout << "ml: " << p.ml << std::endl;
				std::cout << "Pump" << p.pumpNr << std::endl;
				std::cout << "description: " << p.description << std::endl;
				std::cout << "pump: " << p.pumpNr << std::endl;
			}
		}
	}
	std::cout << "";
}


void PumpController::ListPumps() {
	std::cout << "--------------------------------------LIST PUMPS--------------------------------------------------" << std::endl;
	for(Pump pump : this->pumps) {
		std::cout << std::endl << "Pump name: " << pump.name << std::endl;
		std::cout << "Time to dose 100ml: " << pump.timeToDose100Ml << std::endl;
		std::cout << "Calibrated: " << pump.calibrated << std::endl;
	}
	std::cout << "--------------------------------------------------------------------------------------------------" << std::endl;
}

void PumpController::stopPumping(int argc, char**args) {
	
}

void PumpController::startPumping(int argc, char**args) {
	
}

/**
 * fetch the pumpnr from console
 */
int PumpController::_fetchPumpNr() {
	std::cout << "Please enter the pump number that you want to select: (1-"<< this->pumps.capacity() << ")" << std::endl;
	std::cout << "DosingPump>";
	std::string prstr = "";
	int result = 0;
	std::cin >> prstr;
	result = atoi(prstr.c_str());
	this->_selectedPump = this->findPumpByPumpNr(result);
	return result;
}
/**
 * fetch the yes/no from console
 */
std::string PumpController::_fetchYesNo() {
	std::cout << "Please type ((y)es/(n)o) to confirm." << std::endl;
	std::cout << "DosingPump>";
	std::string prstr = "";
	std::cin >> prstr;
	return prstr.c_str();
}



// void Key::key_update (char userKeyChar, KeyState userState, boolean userStatus) {
// 	kchar = userKeyChar;
// 	kstate = userState;
// 	stateChanged = userStatus;
// }Target all failed to build.

