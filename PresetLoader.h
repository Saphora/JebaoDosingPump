#ifndef PresetLoader_h
#define PresetLoader_h = "PresetLoader_h";
#include "Preset.h"
class PresetLoader {
	public:
		PresetLoader();
		
		Preset* getAllPresets();
		Preset getPresetForPump(int pump);
};
#endif