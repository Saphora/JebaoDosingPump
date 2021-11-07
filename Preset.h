#ifndef Preset_h
#define Preset_h
#include <iostream>
class Preset {
	public:
		int ml;
		int pumpNr;
		std::string name;
		std::string description;
		Preset() {
			ml = -1;
			name = "";
			description = "";
			pumpNr = 0;
		}
};
#endif
