#ifndef Pump_h
#define Pump_h
#include <string>
#include "Preset.h"
#include <vector>
#define MAX_PRESETS 128

class Pump {
	public:
		Preset *selectedPresetToDose; //dosing for preset.
		int pumpNr;
		double timeToDose100Ml;
		std::vector<bool> dataFrame; //data to be sended to pump
		bool calibrated;
		bool isRunning;
		std::string name;
		int lengthOfPresets = 0; //this should be private, keeps track of the nr of presets in list.

		Pump(int p_pumpNr, std::string pname, std::vector<bool> pdataFrame) {
			this->pumpNr = p_pumpNr;
			this->name = pname;
			this->calibrated = false;
			this->timeToDose100Ml = 0.0;
			this->dataFrame = pdataFrame;
		}

		Pump() {
			this->pumpNr = -1;
			this->calibrated = false;
			this->name = "__UNDEFINED__";
			this->timeToDose100Ml = 0.0;
			this->dataFrame = {false,false,false,false,false,false,false,false};
		}
	private:
};
#endif