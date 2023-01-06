To export a preset, go to MainComponent.cpp and comment out "#define FACTORY_PRESET_BUTTON", then rebuild. A button appears that will save your current settings as a file called preset_.xml in your Documents folder. 

This file can then be renamed to something more descriptive and added in the Projucer. Then update preset_names, preset_filenames and NUM_PROGRAMMES in PluginProcessor.h and .cpp and rebuild again to add it as a factory preset.
