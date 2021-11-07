#ifndef PumpController_h

#define PumpController_h

#include <array>
#include <vector>
#include "Pump.h"
#include <thread>
#include <future>
#include <atomic>

#define MAX_PUMPS_COUNT 128

class PumpController {
	public:
		Pump findPumpByPumpNr(int nr);
		Pump findPumpByIndex(int index);
		std::vector<Preset> GetPresetsForPump();
		void Kill();
		bool IsAlive();
		void ListPresets();
		void ListPumps();
		void addPump(Pump p);
		Pump getPump(int index);
		void calibrate(int argc, char **args);
		void startPumping(int argc, char **args);
		void stopPumping(int argc, char** args);
		void startDose(int argc, char** args);
		void preset(int argc, char** args);
		void setup();
		void printNonCalibratedPumps();
		bool allPompsCalibrated();
		void showMainMenu(bool showWelcome, bool isHelp);
		void startProcessByCin(std::string processName);

		bool DumpSnapShot();
		bool ParseSnapShot();
		
		PumpController() {
			alive = true;
			this->calibrationCounter = 0;
		}
	private:
		int _fetchPumpNr();
		Pump _selectedPump;
		std::string _fetchYesNo();
		std::vector<Pump> pumps;
		std::vector<Preset> presets;
		void _calibrate();
		void _dosing();
		void _startPumping();
		bool alive;
		bool _isDosing = false;
		bool isCalibrating = false;
		bool isRunning = false;
		std::thread *backgroundWorker;
		long calibrationCounter;
		
};
#endif