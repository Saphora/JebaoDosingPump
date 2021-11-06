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
		int dataFrame; //data to be sended to pump
		bool calibrated;
		bool isRunning;
		std::string name;
		int lengthOfPresets = 0; //this should be private, keeps track of the nr of presets in list.

		Pump(int p_pumpNr, std::string pname, int pdataFrame) {
			this->pumpNr = p_pumpNr;
			this->name = pname;
			this->calibrated = false;
			this->dataFrame = pdataFrame;
		}

		Pump() {
			this->pumpNr = -1;
			this->calibrated = false;
			this->name = "__UNDEFINED__";
			this->dataFrame = -1;
		}
	private:
};
#endif