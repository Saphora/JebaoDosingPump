#include "PumpController.h"
#include <iostream>
#include <thread>
#include <assert.h>
#include <future>
#include <fstream>
#include <sstream>
#include <wiringPi.h>

//STCP = Storage Register Clock Input = 
//SHCP = Shift Register Clock Input
//DA = Serial data 

//STORAGEKLOK = GRIJS = 24
//SHIFT IN KLOK = WIT = 22
//Serial data = paars
//storage klok is laag wanneer shift klok aan het klokken is.
//128 = 8
//64 =  7
//32 =  6
//16 =  5
//8  =  4
//4  =  3
//2  =  2
//1  =  1
int STORAGE_CLOCK = 29; //CH3
int SHIFT_CLOCK = 28; //CH2
int SERIAL_DATA = 27; //CH1

void WritePumpData(std::vector<bool> databyte) {  
	digitalWrite(STORAGE_CLOCK, false);
	for(int i = 0; i<8; i++) {
		//bitbang
		if(i == 0) {
			digitalWrite(SHIFT_CLOCK, false);
			delayMicroseconds(67);
			digitalWrite(SHIFT_CLOCK, false);
			delayMicroseconds(67);
		}
		digitalWrite(SHIFT_CLOCK, false);
		delayMicroseconds(67);
		digitalWrite(SERIAL_DATA, databyte[i]);
		delayMicroseconds(134);
		digitalWrite(SHIFT_CLOCK, true);  
		delayMicroseconds(268);
	}
	digitalWrite(STORAGE_CLOCK, true);
	digitalWrite(SERIAL_DATA, false);
}

void DisablePumps(std::string whereCalled) {  
	std::cout << whereCalled << std::endl;
	std::vector<bool> dataByte = {false,false,false,false,false,false,false,false};
	digitalWrite(STORAGE_CLOCK, false);
	for(int i = 0; i<8; i++) {
	//bitbang
	if(i == 0) {
		digitalWrite(SHIFT_CLOCK, false);
		delayMicroseconds(67);
		digitalWrite(SHIFT_CLOCK, false);
		delayMicroseconds(67);
	}
	digitalWrite(SHIFT_CLOCK, false);
	delayMicroseconds(67);
	digitalWrite(SERIAL_DATA, dataByte[i]);
	delayMicroseconds(134);
	digitalWrite(SHIFT_CLOCK, true);  
	delayMicroseconds(268);
	}
	digitalWrite(STORAGE_CLOCK, true);
	digitalWrite(SERIAL_DATA, false);
}

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

	
		std::string pumpCsv = "name|calibrated|timetodose100ml|pumpNr|dataFrame\n";
		std::string presetCsv = "name|description|ml|pumpNr\n";
		int index =0;
		for(index = 0; index < this->pumps.size(); index++) {
			Pump p = this->pumps[index];
			if(p.name != "__UNDEFINED__") {
				std::string df = "";
				for(bool bit : p.dataFrame) {
					if(bit == true) {
						df = df + "1";
					} else {
						df = df + "0";
					}
				}
				pumpCsv = pumpCsv + p.name + "|" + (p.calibrated == true ? "y" : "n")+ "|"+ std::to_string(p.timeToDose100Ml) + "|" + std::to_string(p.pumpNr) + "|" + df + "|" +std::string("\n");
			}
		}
		index =0;
		for(index = 0; index < this->presets.size(); index++) {
			Preset p = this->presets[index];
			if(p.name != "__UNDEFINED__") {
				presetCsv = presetCsv + p.name + "|"+ p.description+"|" + std::to_string(p.ml) + "|"+ std::to_string(p.pumpNr)+"|" + std::string("\n");
			}
		}
		std::ofstream out("/home/pi/DosingPump/pumps.csv");
		out << pumpCsv.c_str();
		out.close();

		std::ofstream out2("/home/pi/DosingPump/presets.csv");
		out2 << presetCsv.c_str();
		out2.close();
		return true;
	
	
}

std::vector<std::string> split(std::string s, std::string delimiter) { 
	std::vector<std::string> result;
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		result.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	return result;
}

bool PumpController::ParseSnapShot() {
	try {
		std::cout << "Trying to parse snapshot" << std::endl;
		std::ifstream csv("pumps.csv");
		std::stringstream stringStream;
		stringStream << csv.rdbuf();
		
		
		std::cout << "Pumps csv readed." << std::endl;

		std::ifstream csvPresets("presets.csv");
		std::stringstream stringStreamPresets;
		stringStreamPresets << csvPresets.rdbuf();
		
		std::cout << "Presets csv readed." << std::endl;

		std::vector<std::string> csvPumpLines = split(stringStream.str(), "\n");
		std::vector<std::string> csvPresetLines = split(stringStreamPresets.str(), "\n");
		if(csvPresetLines.size() > 0 ) {
			this->presets.clear();
		}
		if(csvPumpLines.size() > 0) {
			this->pumps.clear();
		}
		int Lindex = 0;

		std::cout << "Parsing lines for pumps.csv" << std::endl;
		for(std::string line : csvPumpLines) {
			if(Lindex > 0) {
				std::vector<std::string> csvPumpColData = split(line, "|");
				Pump p;
				p.calibrated = (csvPumpColData[1] == "y" ? true:false);
				std::vector<bool> df;
				int index = 0;
				for(char bit : csvPumpColData[4]) {
					if(bit == '1') {
						df.push_back(true);
					} else {
						df.push_back(false);
					}
					++index;
				}
				p.dataFrame = df;
				p.name = csvPumpColData[0];
				p.pumpNr = atoi(csvPumpColData[3].c_str());
				p.timeToDose100Ml = atof(csvPumpColData[2].c_str());
				this->pumps.push_back(p);
			}
			++Lindex;
		}
		Lindex = 0;

		
		std::cout << "Parsing lines for presets csv" << std::endl;
		for(std::string line : csvPresetLines) {
			if(Lindex > 0) {
				std::vector<std::string> csvData = split(line, "|");
				Preset p;
				p.name = csvData[0];
				p.description = csvData[1];
				p.ml = atoi(csvData[2].c_str());
				p.pumpNr = atoi(csvData[3].c_str());
				this->presets.push_back(p);
			}
			++Lindex;
		}
		if(csvPumpLines.size() > 0) {
			return true;
		} else {
			return false;
		}
	} catch(...) {
		std::cout << "EXCEPTION CAUGHT!" <<std::endl;
		throw "There was an error reading the csv files.";
	}
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
	int ml = this->GetPresetsForPump()[0].ml;
	double timeToDose1ml = this->_selectedPump.timeToDose100Ml / 100;
	double timeToDosePreset = timeToDose1ml * ml;

	std::cout << std::endl << std::endl << "Dosing started: " << std::endl;
	std::cout << "Time to dose 100ml: " << this->_selectedPump.timeToDose100Ml << std::endl;
	std::cout << "Pump: " << this->_selectedPump.name << std::endl;
	std::cout << "Time to dose preset: " << timeToDosePreset << std::endl;
	std::cout << "Ammount dosing (ml): " << ml << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(100us);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end-start;
	std::cout << "Initial elapsed time: " << elapsed.count() << std::endl << std::endl;
	int index = 0;
	assert(elapsed.count() <= timeToDosePreset);
	while(elapsed.count() <= timeToDosePreset) {
		WritePumpData(this->_selectedPump.dataFrame);
		end = std::chrono::high_resolution_clock::now();
		elapsed = end-start;
		
		using namespace std::chrono_literals;
		std::cout << ">";
		//std::cout << timeToDosePreset << std::endl;
		//std::cout << elapsed.count() << std::endl << std::endl;
	}
	
	//DisablePumps("[_dosing] - Disable pumps called from inside the thread.");
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
			this->backgroundWorker->join();
			DisablePumps("[startDose] - Disable pumps called from outside the thread.");
		}
		std::cout << "DosingPump>" << std::endl;
	}
	
}

void PumpController::_calibrate() {
	while(this->isCalibrating) {
		WritePumpData(this->_selectedPump.dataFrame);
		using namespace std::chrono_literals;
		std::cout << "=";
	}
	DisablePumps("[_calibrate] - Disable pumps called from inside the thread.");
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
				std::cout << "The folfalseing instructions are available now:" << std::endl << std::endl;
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
	if(processName == "exit"  || processName == "e") {
		std::terminate();
	} else {
		this->showMainMenu(false, false);
	}
	if(processName == "kill") {
		DisablePumps("[KILL] - Disable pumps from kill command.");
	}
	if(processName == "?" || processName == "help") {
		this->showMainMenu(true, true);
	}

}

void PumpController::setup() {
	pinMode(STORAGE_CLOCK, OUTPUT);
	pinMode(SHIFT_CLOCK, OUTPUT);  
	pinMode(SERIAL_DATA, OUTPUT);
	digitalWrite(SERIAL_DATA, false); // do this first
	digitalWrite(SHIFT_CLOCK, true); // do this first
	digitalWrite(STORAGE_CLOCK, true); // do this first
	delay(2);
	std::cout << "Setup started." << std::endl;
	bool calRequired = false;
	if(this->ParseSnapShot()==true) {
		std::cout << "Predefined setup found. Validating.." << std::endl;
		if(this->pumps.size() > 0) {
			std::cout << "We have found " << this->pumps.size() << " pumps!" << std::endl;;
		}
		if(this->presets.size() > 0) {
			std::cout << "We have found " << this->presets.size() << " presets!" << std::endl;
		}
		std::cout << "Validation completed." << std::endl;
	} else {
		std::cout << "No predefined data was found. Please setup pumps in pumps.csv. " << std::endl <<"and make sure that you save those pump with the save instruction." << std::endl;
		Pump p1(1, "Demo 1", {false,false,false,false,false,false,false,true});
		this->addPump(p1);
		Pump p2(2, "Demo 2", {false,false,false,false,false,false,true,false});
		this->addPump(p2);
		Pump p3(3, "Demo 3", {false,false,false,false,false,true,false,false});
		this->addPump(p3);
		Pump p4(4, "Demo 4", {false,false,false,false,true,false,false,false});
		this->addPump(p4);
		Pump p5(5, "Demo 5", {false,false,false,true,false,false,false,false});
		this->addPump(p5);
		Pump p6(6, "Demo 6", {false,false,true,false,false,false,false,false});
		this->addPump(p6);
		Pump p7(7, "Demo 7", {false,true,false,false,false,false,false,false});
		this->addPump(p7);
		Pump p8(8, "Demo 8", {true,false,false,false,false,false,false,false});
		this->addPump(p8);

		Preset presetDefault;
		presetDefault.ml = 50;
		presetDefault.description = "This is a default preset.";
		presetDefault.name = "test preset";
		presetDefault.pumpNr = 1;
		this->presets.push_back(presetDefault);
		
		Preset presetDefault2;
		presetDefault2.ml = 75;
		presetDefault2.description = "This is a default preset.";
		presetDefault2.name = "test preset 2";
		presetDefault2.pumpNr = 2;
		this->presets.push_back(presetDefault2);

		Preset presetDefault3;
		presetDefault3.ml = 75;
		presetDefault3.description = "This is a default preset.";
		presetDefault3.name = "test preset 2";
		presetDefault3.pumpNr = 2;
		this->presets.push_back(presetDefault3);
		this->DumpSnapShot();
	}
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

void PumpController::_startPumping() {
	while(this->isRunning) {
		WritePumpData(this->_selectedPump.dataFrame);
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1ms);
		std::cout << ">";
	}
}

void PumpController::startPumping(int argc, char**args) {
	if(argc==0) {
		int pumpNr = this->_fetchPumpNr();
		std::cout << "Pump [" << this->_selectedPump.name << " running." << std::endl;
		this->backgroundWorker = new std::thread([this]() {_startPumping();} );
		auto start = std::chrono::high_resolution_clock::now();
		std::string runner = "running";
		this->isRunning = true;
		while(runner == std::string("running")) {
			this->backgroundWorker->detach();
			std::cout << "Hit any key and press enter to stop" << std::endl;
			std::cin >> runner;
			
			DisablePumps("[startPumping] - Disable pumps called from outside the thread.");
			this->isRunning = false;
		}
	}
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

