#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include "headers/defines.h"

#define NoPositions /*---------------------------*/ 0
#define LinesPosition /*-------------------------*/ (1 << 0)
#define XPosition /*-----------------------------*/ (1 << 1)
#define YPosition /*-----------------------------*/ (1 << 2)
#define WidthPosition /*-------------------------*/ (1 << 3)
#define HeightPosition /*------------------------*/ (1 << 4)
#define BorderPosition /*------------------------*/ (1 << 5)
#define BorderColorPosition /*-------------------*/ (1 << 6)
#define BackgroundColorPosition /*---------------*/ (1 << 7)
#define GlobalMenuBorderColorPosition /*---------*/ (1 << 8)
#define GlobalMenuBackgroundColorPosition /*-----*/ (1 << 9)
#define GlobalBoxBorderColorPosition /*----------*/ (1 << 10)
#define GlobalBoxBackgroundColorPosition /*------*/ (1 << 11)
#define GlobalInnerBoxBorderColorPosition /*-----*/ (1 << 12)
#define GlobalInnerBoxBackgroundColorPosition /*-*/ (1 << 13)
#define HideKeyPosition /*-----------------------*/ (1 << 14)
#define TextPosition /*--------------------------*/ (1 << 15)
#define TextColorPosition /*---------------------*/ (1 << 16)
#define GlobalTextColorPosition /*---------------*/ (1 << 17)
#define CommandPosition /*-----------------------*/ (1 << 18)
#define DrawableCommandPosition /*---------------*/ (1 << 19)
#define ButtonPosition /*------------------------*/ (1 << 20)
#define MenuPosition /*--------------------------*/ (1 << 21)
#define BoxPosition /*---------------------------*/ (1 << 22)
#define InnerBoxPosition /*----------------------*/ (1 << 23)

#define NoOperation /*---------------------------*/ ((unsigned int)0)
#define OperationAddition /*---------------------*/ ((unsigned int)1)
#define OperationSubtraction /*------------------*/ ((unsigned int)2)
#define OperationMultiplication /*---------------*/ ((unsigned int)3)
#define OperationDivision /*---------------------*/ ((unsigned int)4)

extern const char *restrict configPath;
extern Display *restrict display;
extern const XRRMonitorInfo *restrict monitorInfo;
extern unsigned int totalBoxAmount;
extern char *line;
extern unsigned int currentMonitor;

static FILE *getConfigFile(void);
static bool pushSpaces(const char *const restrict lineArray, unsigned int *const restrict element);
static bool isVariable(const char *const restrict variableArray, const char *const restrict lineArray, unsigned int *const restrict element);
static unsigned int getUnsignedDecimalNumber(const Window *const restrict window, const unsigned int *const restrict currentLine, const char *const restrict lineArray, unsigned int *const restrict element);
static int getDecimalNumber(const Window *const restrict window, const char *const restrict lineArray, unsigned int *const restrict element);
static bytes4 getARGB(const char *const restrict lineArray, unsigned int *const restrict element);
static void getKeys(const Window *const restrict window, const unsigned int *const restrict currentLine, const char *const restrict lineArray, unsigned int *const restrict element, unsigned int *const restrict key, int *const restrict masks);
static char *getText(const char *const restrict lineArray, unsigned int *const restrict element, const unsigned int addAmpersand);
static bool printLineError(const char *const restrict lineArray, const unsigned int *const restrict element, const unsigned int *const restrict currentLine);

bool readConfigScan(const Window *const restrict parentWindow){
	bool value = 0;
	FILE *restrict config = getConfigFile();
	if(config){
		totalBoxAmount = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		size_t characters = DefaultCharactersCount;
		bytes4 hasReadVariable = NoPositions;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			element = 0;
			getline(&line, &characters, config);
			pushSpaces(line, &element);
			if(!isVariable("#", line, &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("Lines", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								maxLinesCount = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HideKeyPosition)){
						if(isVariable("HideKey", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								unsigned int key;
								int masks;
								getKeys(parentWindow, &currentLine, line, &element, &key, &masks);
								XGrabKey(display, key, masks, *parentWindow, True, GrabModeAsync, GrabModeAsync);
								hasReadVariable |= HideKeyPosition;
							}
							continue;
						}
					}
					if(isVariable("Menu", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							hasReadVariable |= MenuPosition;
						}
						continue;
					}
					if(!isVariable("Lines",                     line, &element) &&
					   !isVariable("X",                         line, &element) &&
					   !isVariable("Y",                         line, &element) &&
					   !isVariable("Width",                     line, &element) &&
					   !isVariable("Height",                    line, &element) &&
					   !isVariable("Border",                    line, &element) &&
					   !isVariable("BorderColor",               line, &element) &&
					   !isVariable("BackgroundColor",           line, &element) &&
					   !isVariable("GlobalMenuBorderColor",     line, &element) &&
					   !isVariable("GlobalMenuBackgroundColor", line, &element) &&
					   !isVariable("HideKey",                   line, &element) &&
					   !isVariable("Menu",                      line, &element) &&
					   !isVariable("}",                         line, &element)){
						printLineError(line, &element, &currentLine);
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("Box", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							++totalBoxAmount;
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", line, &element)){
						hasReadVariable ^= MenuPosition;
						continue;
					}
					if(!isVariable("X",                        line, &element) &&
					   !isVariable("Y",                        line, &element) &&
					   !isVariable("Width",                    line, &element) &&
					   !isVariable("Height",                   line, &element) &&
					   !isVariable("Border",                   line, &element) &&
					   !isVariable("BorderColor",              line, &element) &&
					   !isVariable("BackgroundColor",          line, &element) &&
					   !isVariable("GlobalBoxBorderColor",     line, &element) &&
					   !isVariable("GlobalBoxBackgroundColor", line, &element) &&
					   !isVariable("GlobalTextColor",          line, &element) &&
					   !isVariable("Box",                      line, &element) &&
					   !isVariable("}",                        line, &element)){
						printLineError(line, &element, &currentLine);
						continue;
					}
				}else if(!(hasReadVariable & InnerBoxPosition)){
					if(isVariable("InnerBox", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", line, &element)){
						hasReadVariable ^= BoxPosition;
						continue;
					}
					if(!isVariable("X",               line, &element) &&
					   !isVariable("Y",               line, &element) &&
					   !isVariable("Width",           line, &element) &&
					   !isVariable("Height",          line, &element) &&
					   !isVariable("Border",          line, &element) &&
					   !isVariable("BorderColor",     line, &element) &&
					   !isVariable("BackgroundColor", line, &element) &&
					   !isVariable("Text",            line, &element) &&
					   !isVariable("TextColor",       line, &element) &&
					   !isVariable("Command",         line, &element) &&
					   !isVariable("DrawableCommand", line, &element) &&
					   !isVariable("Button",          line, &element) &&
					   !isVariable("InnerBox",        line, &element) &&
					   !isVariable("}",               line, &element)){
						printLineError(line, &element, &currentLine);
						continue;
					}
				}else{
					if(isVariable("}", line, &element)){
						hasReadVariable ^= InnerBoxPosition;
						continue;
					}
					if(!isVariable("X",               line, &element) &&
					   !isVariable("Y",               line, &element) &&
					   !isVariable("Width",           line, &element) &&
					   !isVariable("Height",          line, &element) &&
					   !isVariable("Border",          line, &element) &&
					   !isVariable("BorderColor",     line, &element) &&
					   !isVariable("BackgroundColor", line, &element) &&
					   !isVariable("}",               line, &element)){
						printLineError(line, &element, &currentLine);
						continue;
					}
				}
			}
		}
		fclose(config);
		value = 1;
	}
	return value;
}
bool readConfigTopLevelWindow(const Window *const restrict parentWindow, int *const restrict x, int *const restrict y, unsigned int *const restrict width, unsigned int *const restrict height, unsigned int *const restrict border, bytes4 *const restrict borderColor, bytes4 *const restrict backgroundColor, bytes4 *const restrict globalMenuBorderColor, bytes4 *const restrict globalMenuBackgroundColor, unsigned int *const restrict menuAmount){
	bool value = 0;
	FILE *restrict config = getConfigFile();
	if(config){
		*x = 0;
		*y = 0;
		*width = 0;
		*height = 0;
		*border = 0;
		*borderColor = 0x00000000;
		*backgroundColor = 0x00000000;
		*globalMenuBorderColor = 0x00000000;
		*globalMenuBackgroundColor = 0x00000000;
		*menuAmount = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		size_t characters = DefaultCharactersCount;
		bytes4 hasReadVariable = NoPositions;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			element = 0;
			getline(&line, &characters, config);
			pushSpaces(line, &element);
			if(!isVariable("#", line, &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("Lines", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								maxLinesCount = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & XPosition)){
						if(isVariable("X", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*x = getDecimalNumber(parentWindow, line, &element);
								hasReadVariable |= XPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & YPosition)){
						if(isVariable("Y", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*y = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= YPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & WidthPosition)){
						if(isVariable("Width", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*width = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= WidthPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HeightPosition)){
						if(isVariable("Height", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*height = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= HeightPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderColorPosition)){
						if(isVariable("BorderColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*borderColor = getARGB(line, &element);
								hasReadVariable |= BorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderPosition)){
						if(isVariable("Border", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*border = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= BorderPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BackgroundColorPosition)){
						if(isVariable("BackgroundColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*backgroundColor = getARGB(line, &element);
								hasReadVariable |= BackgroundColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalMenuBorderColorPosition)){
						if(isVariable("GlobalMenuBorderColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*globalMenuBorderColor = getARGB(line, &element);
								hasReadVariable |= GlobalMenuBorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalMenuBackgroundColorPosition)){
						if(isVariable("GlobalMenuBackgroundColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*globalMenuBackgroundColor = getARGB(line, &element);
								hasReadVariable |= GlobalMenuBackgroundColorPosition;
							}
							continue;
						}
					}
					if(isVariable("Menu", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							++(*menuAmount);
							hasReadVariable |= MenuPosition;
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("Box", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", line, &element)){
						hasReadVariable ^= MenuPosition;
						continue;
					}
				}else if(!(hasReadVariable & InnerBoxPosition)){
					if(isVariable("InnerBox", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", line, &element)){
						hasReadVariable ^= BoxPosition;
						continue;
					}
				}else{
					if(isVariable("}", line, &element)){
						hasReadVariable ^= InnerBoxPosition;
						continue;
					}
				}
			}
		}
		fclose(config);
		value = 1;
	}
	return value;
}
bool readConfigMenuWindow(const Window *const restrict parentWindow, const unsigned int *const restrict currentMenu, int *const restrict x, int *const restrict y, unsigned int *const restrict width, unsigned int *const restrict height, unsigned int *const restrict border, bytes4 *const restrict borderColor, bytes4 *const restrict backgroundColor, bytes4 *const restrict globalBoxBorderColor, bytes4 *const restrict globalBoxBackgroundColor, unsigned int *const restrict boxAmount){
	bool value = 0;
	FILE *restrict config = getConfigFile();
	if(config){
		*x = 0;
		*y = 0;
		*width = 0;
		*height = 0;
		*border = 0;
		*borderColor = 0x00000000;
		*backgroundColor = 0x00000000;
		*globalBoxBorderColor = 0x00000000;
		*globalBoxBackgroundColor = 0x00000000;
		*boxAmount = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		size_t characters = DefaultCharactersCount;
		bytes4 hasReadVariable = NoPositions;
		unsigned int menuAmountRead = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			element = 0;
			getline(&line, &characters, config);
			pushSpaces(line, &element);
			if(!isVariable("#", line, &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("Lines", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								maxLinesCount = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("Menu", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							if(menuAmountRead == *currentMenu){
								hasReadVariable |= MenuPosition;
							}else{
								++menuAmountRead;
							}
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(!(hasReadVariable & XPosition)){
						if(isVariable("X", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*x = getDecimalNumber(parentWindow, line, &element);
								hasReadVariable |= XPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & YPosition)){
						if(isVariable("Y", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*y = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= YPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & WidthPosition)){
						if(isVariable("Width", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*width = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= WidthPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HeightPosition)){
						if(isVariable("Height", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*height = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= HeightPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderColorPosition)){
						if(isVariable("BorderColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*borderColor = getARGB(line, &element);
								hasReadVariable |= BorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderPosition)){
						if(isVariable("Border", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*border = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= BorderPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BackgroundColorPosition)){
						if(isVariable("BackgroundColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*backgroundColor = getARGB(line, &element);
								hasReadVariable |= BackgroundColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalBoxBorderColorPosition)){
						if(isVariable("GlobalBoxBorderColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*globalBoxBorderColor = getARGB(line, &element);
								hasReadVariable |= GlobalBoxBorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalBoxBackgroundColorPosition)){
						if(isVariable("GlobalBoxBackgroundColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*globalBoxBackgroundColor = getARGB(line, &element);
								hasReadVariable |= GlobalBoxBackgroundColorPosition;
							}
							continue;
						}
					}
					if(isVariable("Box", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							++(*boxAmount);
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", line, &element)){
						break;
					}
				}else if(!(hasReadVariable & InnerBoxPosition)){
					if(isVariable("InnerBox", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", line, &element)){
						hasReadVariable ^= BoxPosition;
						continue;
					}
				}else{
					if(isVariable("}", line, &element)){
						hasReadVariable ^= InnerBoxPosition;
						continue;
					}
				}
			}
		}
		fclose(config);
		value = 1;
	}
	return value;
}
bool readConfigBoxWindow(const Window *const restrict parentWindow, const unsigned int *const restrict currentMenu, const unsigned int *const restrict currentBox, int *const restrict x, int *const restrict y, unsigned int *const restrict width, unsigned int *const restrict height, unsigned int *const restrict border, bytes4 *const restrict borderColor, bytes4 *const restrict backgroundColor, bytes4 *const restrict globalInnerBoxBorderColor, bytes4 *const restrict globalInnerBoxBackgroundColor, unsigned int *const restrict innerBoxAmount){
	bool value = 0;
	FILE *restrict config = getConfigFile();
	if(config){
		*x = 0;
		*y = 0;
		*width = 0;
		*height = 0;
		*border = 0;
		*borderColor = 0x00000000;
		*backgroundColor = 0x00000000;
		*globalInnerBoxBorderColor = 0x00000000;
		*globalInnerBoxBackgroundColor = 0x00000000;
		*innerBoxAmount = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		size_t characters = DefaultCharactersCount;
		bytes4 hasReadVariable = NoPositions;
		unsigned int menuAmountRead = 0;
		unsigned int boxAmountRead = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			element = 0;
			getline(&line, &characters, config);
			pushSpaces(line, &element);
			if(!isVariable("#", line, &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("Lines", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								maxLinesCount = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("Menu", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							if(menuAmountRead == *currentMenu){
								hasReadVariable |= MenuPosition;
							}else{
								++menuAmountRead;
							}
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("Box", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							if(boxAmountRead == *currentBox){
								hasReadVariable |= BoxPosition;
							}else{
								++boxAmountRead;
							}
						}
						continue;
					}
				}else if(!(hasReadVariable & InnerBoxPosition)){
					if(!(hasReadVariable & XPosition)){
						if(isVariable("X", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*x = getDecimalNumber(parentWindow, line, &element);
								hasReadVariable |= XPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & YPosition)){
						if(isVariable("Y", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*y = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= YPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & WidthPosition)){
						if(isVariable("Width", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*width = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= WidthPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HeightPosition)){
						if(isVariable("Height", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*height = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= HeightPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderColorPosition)){
						if(isVariable("BorderColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*borderColor = getARGB(line, &element);
								hasReadVariable |= BorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderPosition)){
						if(isVariable("Border", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*border = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= BorderPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BackgroundColorPosition)){
						if(isVariable("BackgroundColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*backgroundColor = getARGB(line, &element);
								hasReadVariable |= BackgroundColorPosition;
							}
							continue;
						}
					}



					if(!(hasReadVariable & GlobalInnerBoxBorderColorPosition)){
						if(isVariable("GlobalInnerBoxBorderColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*globalInnerBoxBorderColor = getARGB(line, &element);
								hasReadVariable |= GlobalInnerBoxBorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalInnerBoxBackgroundColorPosition)){
						if(isVariable("GlobalInnerBoxBackgroundColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*globalInnerBoxBackgroundColor = getARGB(line, &element);
								hasReadVariable |= GlobalInnerBoxBackgroundColorPosition;
							}
							continue;
						}
					}



					if(isVariable("InnerBox", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							++(*innerBoxAmount);
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", line, &element)){
						break;
					}
				}else{
					if(isVariable("}", line, &element)){
						hasReadVariable ^= InnerBoxPosition;
						continue;
					}
				}
			}
		}
		fclose(config);
		value = 1;
	}
	return value;
}
bool readConfigInnerBoxWindow(const Window *const restrict parentWindow, const unsigned int *const restrict currentMenu, const unsigned int *const restrict currentBox, const unsigned int *const restrict currentInnerBox, int *const restrict x, int *const restrict y, unsigned int *const restrict width, unsigned int *const restrict height, unsigned int *const restrict border, bytes4 *const restrict borderColor, bytes4 *const restrict backgroundColor){
	bool value = 0;
	FILE *restrict config = getConfigFile();
	if(config){
		*x = 0;
		*y = 0;
		*width = 0;
		*height = 0;
		*border = 0;
		*borderColor = 0x00000000;
		*backgroundColor = 0x00000000;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		size_t characters = DefaultCharactersCount;
		bytes4 hasReadVariable = NoPositions;
		unsigned int menuAmountRead = 0;
		unsigned int boxAmountRead = 0;
		unsigned int innerBoxAmountRead = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			element = 0;
			getline(&line, &characters, config);
			pushSpaces(line, &element);
			if(!isVariable("#", line, &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("Lines", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								maxLinesCount = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("Menu", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							if(menuAmountRead == *currentMenu){
								hasReadVariable |= MenuPosition;
							}else{
								++menuAmountRead;
							}
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("Box", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							if(boxAmountRead == *currentBox){
								hasReadVariable |= BoxPosition;
							}else{
								++boxAmountRead;
							}
						}
						continue;
					}
				}else if(!(hasReadVariable & InnerBoxPosition)){
					if(isVariable("InnerBox", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							if(innerBoxAmountRead == *currentInnerBox){
								hasReadVariable |= InnerBoxPosition;
							}else{
								++innerBoxAmountRead;
							}
						}
						continue;
					}
				}else{
					if(!(hasReadVariable & XPosition)){
						if(isVariable("X", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*x = getDecimalNumber(parentWindow, line, &element);
								hasReadVariable |= XPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & YPosition)){
						if(isVariable("Y", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*y = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= YPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & WidthPosition)){
						if(isVariable("Width", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*width = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= WidthPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HeightPosition)){
						if(isVariable("Height", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*height = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= HeightPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderColorPosition)){
						if(isVariable("BorderColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*borderColor = getARGB(line, &element);
								hasReadVariable |= BorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderPosition)){
						if(isVariable("Border", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*border = getUnsignedDecimalNumber(parentWindow, &currentLine, line, &element);
								hasReadVariable |= BorderPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BackgroundColorPosition)){
						if(isVariable("BackgroundColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*backgroundColor = getARGB(line, &element);
								hasReadVariable |= BackgroundColorPosition;
							}
							continue;
						}
					}
					if(isVariable("}", line, &element)){
						break;
					}
				}
			}
		}
		fclose(config);
		value = 1;
	}
	return value;
}
bool readConfigTextCommands(const Window *const restrict window, const unsigned int *const restrict currentBox, char **const restrict textPointerArray, bytes4 *const restrict textColor, char **const restrict commandPointerArray, char **const restrict drawableCommandPointerArray){
	bool value = 0;
	FILE *restrict config = getConfigFile();
	if(config){
		*textPointerArray = NULL;
		*textColor = 0x00000000;
		*commandPointerArray = NULL;
		*drawableCommandPointerArray = NULL;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		size_t characters = DefaultCharactersCount;
		bytes4 hasReadVariable = NoPositions;
		unsigned int boxAmountRead = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			element = 0;
			getline(&line, &characters, config);
			pushSpaces(line, &element);
			if(!isVariable("#", line, &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("Lines", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								maxLinesCount = getUnsignedDecimalNumber(window, &currentLine, line, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("Menu", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							hasReadVariable |= MenuPosition;
						}
						continue;
					}
				}else if(boxAmountRead <= *currentBox){
					if(!(hasReadVariable & GlobalTextColorPosition)){
						if(isVariable("GlobalTextColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*textColor = getARGB(line, &element);
								hasReadVariable |= GlobalTextColorPosition;
							}
							continue;
						}
					}
					if(isVariable("Box", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							++boxAmountRead;
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("InnerBox", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", line, &element)){
						if(hasReadVariable & InnerBoxPosition){
							hasReadVariable ^= InnerBoxPosition;
						}else if(hasReadVariable & BoxPosition){
							hasReadVariable ^= BoxPosition;
						}else{
							if(hasReadVariable & GlobalTextColorPosition){
								*textColor = 0x00000000;
								hasReadVariable ^= GlobalTextColorPosition;
							}
							hasReadVariable ^= MenuPosition;
						}
						continue;
					}
				}else if(!(hasReadVariable & InnerBoxPosition)){
					if(!(hasReadVariable & TextPosition)){
						if(isVariable("Text", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*textPointerArray = getText(line, &element, 0);
								hasReadVariable |= TextPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & TextColorPosition)){
						if(isVariable("TextColor", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*textColor = getARGB(line, &element);
								hasReadVariable |= TextColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & CommandPosition)){
						if(isVariable("Command", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*commandPointerArray = getText(line, &element, 1);
								hasReadVariable |= CommandPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & DrawableCommandPosition)){
						if(isVariable("DrawableCommand", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								*drawableCommandPointerArray = getText(line, &element, 0);
								hasReadVariable |= DrawableCommandPosition;
							}
							continue;
						}
					}
					if(isVariable("InnerBox", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", line, &element)){
						break;
					}
				}else{
					if(isVariable("}", line, &element)){
						hasReadVariable ^= InnerBoxPosition;
						continue;
					}
				}
			}
		}
		fclose(config);
		value = 1;
	}
	return value;
}
bool readConfigButton(const Window *const restrict window, const unsigned int *const restrict currentBox){
	bool value = 0;
	FILE *restrict config = getConfigFile();
	if(config){
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		size_t characters = DefaultCharactersCount;
		bytes4 hasReadVariable = NoPositions;
		unsigned int boxAmountRead = 0;
		unsigned int button = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			element = 0;
			getline(&line, &characters, config);
			pushSpaces(line, &element);
			if(!isVariable("#", line, &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("Lines", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								maxLinesCount = getUnsignedDecimalNumber(window, &currentLine, line, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("Menu", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							hasReadVariable |= MenuPosition;
						}
						continue;
					}
				}else if(boxAmountRead <= *currentBox){
					if(isVariable("Box", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							++boxAmountRead;
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("InnerBox", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", line, &element)){
						if(hasReadVariable & InnerBoxPosition){
							hasReadVariable ^= InnerBoxPosition;
						}else if(hasReadVariable & BoxPosition){
							hasReadVariable ^= BoxPosition;
						}else{
							hasReadVariable ^= MenuPosition;
						}
						continue;
					}
				}else if(!(hasReadVariable & InnerBoxPosition)){
					if(!(hasReadVariable & ButtonPosition)){
						if(isVariable("Button", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("=", line, &element)){
								pushSpaces(line, &element);
								button = getUnsignedDecimalNumber(window, &currentLine, line, &element);
								hasReadVariable |= ButtonPosition;
							}
							continue;
						}
					}
					if(isVariable("InnerBox", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", line, &element)){
						break;
					}
				}else{
					if(isVariable("}", line, &element)){
						hasReadVariable ^= InnerBoxPosition;
						continue;
					}
				}
			}
		}
		if(hasReadVariable & ButtonPosition){
			if(button < 6){
				XGrabButton(display, button, AnyModifier, *window, True, NoEventMask, GrabModeAsync, GrabModeAsync, None, None);
			}
		}
		fclose(config);
		value = 1;
	}
	return value;
}
static FILE *getConfigFile(void){
	FILE *restrict config = fopen(configPath, "r");
	if(!config){
		if((config = fopen(configPath, "w"))){
			fprintf(config, "# configuration file for\n\n");
			fprintf(config, "# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
			fprintf(config, "#                                                                                               #\n");
			fprintf(config, "#   #           #   #     # # #     # # # #       # # #     # # # #       # # #     # # # #     #\n");
			fprintf(config, "#   # #       # #   #   #       #   #       #   #       #   #       #   #       #   #       #   #\n");
			fprintf(config, "#   #   #   #   #   #   #           #       #   #       #   #       #   #       #   #       #   #\n");
			fprintf(config, "#   #     #     #   #   #           # # # #     #       #   # # # #     # # # # #   # # # #     #\n");
			fprintf(config, "#   #     #     #   #   #           #   #       #       #   #       #   #       #   #   #       #\n");
			fprintf(config, "#   #     #     #   #   #       #   #     #     #       #   #       #   #       #   #     #     #\n");
			fprintf(config, "#   #     #     #   #     # # #     #       #     # # #     # # # #     #       #   #       #   #\n");
			fprintf(config, "#                                                                                               #\n");
			fprintf(config, "# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n\n\n\n");
			fprintf(config, "# # # # #\n");
			fprintf(config, "# rules #\n");
			fprintf(config, "# # # # #\n\n");
			fprintf(config, "# this file needs to be user-specified when launched\n");
			fprintf(config, "# max line character length is %u\n", DefaultCharactersCount);
			fprintf(config, "# comments are signified by a \'#\' at the beginning of the line\n");
			fprintf(config, "# one variable per line, followed by \'=\' and it\'s value\n");
			fprintf(config, "# all spaces and tabs are ignored\n");
			fprintf(config, "# all variables are valued 0 or undefined by default unless stated otherwise\n");
			fprintf(config, "# colors are in argb format\n");
			fprintf(config, "# all variables, argb values and macros can be written with random capitalization\n");
			fprintf(config, "# argb do not require a \'#\' before the value\n");
			fprintf(config, "# text requires the same quote character before and after it\n");
			fprintf(config, "# text quotation is variable, the first character after \'=\' is the quote character\n\n\n\n");
			fprintf(config, "# # # # # # #\n");
			fprintf(config, "# variables #\n");
			fprintf(config, "# # # # # # #\n\n");
			fprintf(config, "# global object: lines, x, y, width, height, border, borderColor, backgroundColor, globalMenuBorderColor, globalMenuBackgroundColor, hideKey, menu{}\n");
			fprintf(config, "# menu object: x, y, width, height, border, borderColor, backgroundColor, globalBoxBorderColor, globalBoxBackgroundColor, globalTextColor, box{}\n");
			fprintf(config, "# box object: x, y, width, height, border, borderColor, backgroundColor, text, textColor, command, drawableCommand, button, innerBox{}\n");
			fprintf(config, "# innerBox object: x, y, width, height, border, borderColor, backgroundColor\n");
			fprintf(config, "# decimal operands: +, -, *, /\n");
			fprintf(config, "# decimal macros: ParentWidth, ParentHeight\n\n\n\n");
			fprintf(config, "# # # # # # # # # # # #\n");
			fprintf(config, "# variable definition #\n");
			fprintf(config, "# # # # # # # # # # # #\n\n");
			fprintf(config, "# lines: config lines to be read\n");
			fprintf(config, "# x: x axis position of object\n");
			fprintf(config, "# y: y axis position of object\n");
			fprintf(config, "# width: size of object\'s width, excluding border\n");
			fprintf(config, "# height: size of object\'s height, excluding border\n");
			fprintf(config, "# border: size of object\'s border\n");
			fprintf(config, "# borderColor: color of object\'s border\n");
			fprintf(config, "# backgroundColor: color of object\'s background\n");
			fprintf(config, "# globalMenuBorderColor: color of all menus\' border\n");
			fprintf(config, "# globalMenuBackgroundColor: color of all menus\' background\n");
			fprintf(config, "# globalBoxBorderColor: color of all boxes\' border\n");
			fprintf(config, "# globalBoxBackgroundColor: color of all boxes\' background\n");
			fprintf(config, "# globalTextColor: color of all boxes\' text\n");
			fprintf(config, "# hideKey: combination of keycode and modifiers to hide the bar\n");
			fprintf(config, "# text: text label of box\n");
			fprintf(config, "# textColor: color of box\'s text\n");
			fprintf(config, "# command: command executed on interaction with box (followed by control operator \'&\')\n");
			fprintf(config, "# drawableCommand: command returning text output executed on interaction with box\n");
			fprintf(config, "# button: mouse button used for interaction with box\n");
			fprintf(config, "# menu: informationless interactionless object, residing in global object\n");
			fprintf(config, "# box: information object, residing in menu object\n");
			fprintf(config, "# innerBox: informationless interactionless object, residing in box object\n");
			fprintf(config, "# ParentWidth: size of parent object\'s width\n");
			fprintf(config, "# ParentHeight: size of parent object\'s height\n\n\n\n");
			fprintf(config, "# # # # #\n");
			fprintf(config, "# extra #\n");
			fprintf(config, "# # # # #\n\n");
			fprintf(config, "# lines: default %u\n", DefaultLinesCount);
			fprintf(config, "# hideKey: modifiers: Shift, Lock, Control, Mod1, Mod2, Mod3, Mod4, Mod5\n");
			fprintf(config, "# text: requires quotation\n");
			fprintf(config, "# command: requires quotation\n");
			fprintf(config, "# drawableCommand: requires quotation\n");
			fprintf(config, "# button: default 0 = any button, 1 = left click, 2 = middle click, 3 = right click, 4 = wheel up, 5 = wheel down\n\n\n\n");
			fprintf(config, "# /config start # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
			fprintf(config, "lines = 164\n");
			fprintf(config, "x = 0\n");
			fprintf(config, "y = ParentHeight - 19\n");
			fprintf(config, "width = ParentWidth\n");
			fprintf(config, "height = 19\n");
			fprintf(config, "border = 0\n");
			fprintf(config, "borderColor = #00000000\n");
			fprintf(config, "backgroundColor = #FF000000\n");
			fprintf(config, "globalMenuBorderColor = #00000000\n");
			fprintf(config, "globalMenuBackgroundColor = #FF000000\n");
			fprintf(config, "hideKey = Mod4 + 116\n");
			fprintf(config, "menu{\n");
			fprintf(config, "	x = 0\n");
			fprintf(config, "	y = 0\n");
			fprintf(config, "	width = 52\n");
			fprintf(config, "	height = 19\n");
			fprintf(config, "	border = 0\n");
			fprintf(config, "	box{\n");
			fprintf(config, "		x = 0\n");
			fprintf(config, "		y = 0\n");
			fprintf(config, "		width = 50\n");
			fprintf(config, "		height = 17\n");
			fprintf(config, "		border = 1\n");
			fprintf(config, "		borderColor = #FF1F1F1F\n");
			fprintf(config, "		backgroundColor = #FF000000\n");
			fprintf(config, "		text = \"restart\"\n");
			fprintf(config, "		textColor = #FF00FF00\n");
			fprintf(config, "		command = \"restart\"\n");
			fprintf(config, "		button = 1\n");
			fprintf(config, "	}\n");
			fprintf(config, "}\n");
			fprintf(config, "menu{\n");
			fprintf(config, "	x = ParentWidth - 200\n");
			fprintf(config, "	y = 0\n");
			fprintf(config, "	width = 200\n");
			fprintf(config, "	height = 19\n");
			fprintf(config, "	border = 0\n");
			fprintf(config, "	globalBoxBorderColor = #FF1F1F1F\n");
			fprintf(config, "	globalBoxBackgroundColor = #FF000000\n");
			fprintf(config, "	globalTextColor = #FFFFFFFF\n");
			fprintf(config, "	box{\n");
			fprintf(config, "		x = ParentWidth - 200\n");
			fprintf(config, "		y = 0\n");
			fprintf(config, "		width = 74\n");
			fprintf(config, "		height = 17\n");
			fprintf(config, "		border = 1\n");
			fprintf(config, "		text = \"date\"\n");
			fprintf(config, "		drawableCommand = \"date \'+D%%d/M%%m/Y%%y\'\"\n");
			fprintf(config, "		button = 1\n");
			fprintf(config, "	}\n");
			fprintf(config, "	box{\n");
			fprintf(config, "		x = ParentWidth - 123\n");
			fprintf(config, "		y = 0\n");
			fprintf(config, "		width = 86\n");
			fprintf(config, "		height = 17\n");
			fprintf(config, "		border = 1\n");
			fprintf(config, "		text = \"time\"\n");
			fprintf(config, "		drawableCommand = \"date \'+%%H:%%M:%%S %%Z\'\"\n");
			fprintf(config, "		button = 1\n");
			fprintf(config, "	}\n");
			fprintf(config, "	box{\n");
			fprintf(config, "		x = ParentWidth - 34\n");
			fprintf(config, "		y = 0\n");
			fprintf(config, "		width = 32\n");
			fprintf(config, "		height = 17\n");
			fprintf(config, "		border = 1\n");
			fprintf(config, "		text = \"exit\"\n");
			fprintf(config, "		textColor = #FFFF0000\n");
			fprintf(config, "		command = \"exit\"\n");
			fprintf(config, "		button = 1\n");
			fprintf(config, "	}\n");
			fprintf(config, "}\n");
			fprintf(config, "# /config end # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
			fclose(config);
			config = fopen(configPath, "r");
		}else{
			fprintf(stderr, "%s: could not create config file\n", ProgramName);
		}
	}
	return config;
}
static bool pushSpaces(const char *const restrict lineArray, unsigned int *const restrict element){
	unsigned int dereferencedElement = *element;
	bool value = 0;
	while(lineArray[dereferencedElement] != '\n' && (lineArray[dereferencedElement] == ' ' || lineArray[dereferencedElement] == '	')){
		++dereferencedElement;
	}
	if(dereferencedElement > *element){
		*element = dereferencedElement;
		value = 1;
	}
	return value;
}
static bool isVariable(const char *const restrict variable, const char *const restrict lineArray, unsigned int *const restrict element){
	unsigned int dereferencedElement = *element;
	bool value = 0;
	unsigned int currentCharacter = 0;
	while(lineArray[dereferencedElement] != '\n' && variable[currentCharacter] != '\0'){
		if(variable[currentCharacter] >= 'A' && variable[currentCharacter] <= 'Z'){
			if(!(lineArray[dereferencedElement] == variable[currentCharacter] || lineArray[dereferencedElement] == variable[currentCharacter] + 32)){
				currentCharacter = 0;
				break;
			}
		}else if(variable[currentCharacter] >= 'a' && variable[currentCharacter] <= 'z'){
			if(!(lineArray[dereferencedElement] == variable[currentCharacter] || lineArray[dereferencedElement] == variable[currentCharacter] - 32)){
				currentCharacter = 0;
				break;
			}
		}else{
			if(!(lineArray[dereferencedElement] == variable[currentCharacter])){
				currentCharacter = 0;
				break;
			}
		}
		++dereferencedElement;
		++currentCharacter;
	}
	if(currentCharacter != 0){
		*element = dereferencedElement;
		value = 1;
	}
	return value;
}
static unsigned int getUnsignedDecimalNumber(const Window *const restrict parentWindow, const unsigned int *const restrict currentLine, const char *const restrict lineArray, unsigned int *const restrict element){
	unsigned int number = getDecimalNumber(parentWindow, lineArray, element);
	if((int)number < 0){
		fprintf(stderr, "%s: line %u: %i is not an unsigned integer\n", ProgramName, *currentLine, (int)number);
		number = 0;
	}
	return number;
}
static int getDecimalNumber(const Window *const restrict parentWindow, const char *const restrict lineArray, unsigned int *const restrict element){
	unsigned int dereferencedElement = *element;
	int number = 0;
	int numberRead = 0;
	int numberOperatedOn = 0;
	unsigned int operation = NoOperation;
	unsigned int lastOperation = NoOperation;
	XWindowAttributes windowAttributes;
	XGetWindowAttributes(display, *parentWindow, &windowAttributes);
	if(*parentWindow == XDefaultRootWindow(display)){
		windowAttributes.width = monitorInfo[currentMonitor].width;
		windowAttributes.height = monitorInfo[currentMonitor].height;
	}
	while(lineArray[dereferencedElement] != '\n'){
		pushSpaces(lineArray, &dereferencedElement);
		if(lineArray[dereferencedElement] >= '0' && lineArray[dereferencedElement] <= '9'){
			numberRead *= 10;
			numberRead += lineArray[dereferencedElement];
			numberRead -= 48;
			++dereferencedElement;
		}else if(isVariable("ParentWidth", lineArray, &dereferencedElement)){
			numberRead = windowAttributes.width;
		}else if(isVariable("ParentHeight", lineArray, &dereferencedElement)){
			numberRead = windowAttributes.height;
		}else if(lineArray[dereferencedElement] == '+' || lineArray[dereferencedElement] == '-' || lineArray[dereferencedElement] == '*' || lineArray[dereferencedElement] == '/'){
			if(number == 0 && numberRead == 0){
				if(lineArray[dereferencedElement] == '/'){
					break;
				}
			}
			if(operation == OperationAddition){
				if(lineArray[dereferencedElement] != '*' && lineArray[dereferencedElement] != '/'){
					if(numberOperatedOn == 0){
						number += numberRead;
					}else{
						if(lastOperation == OperationAddition){
							number += numberOperatedOn;
						}else if(lastOperation == OperationSubtraction){
							number -= numberOperatedOn;
						}
					}
				}else{
					numberOperatedOn = numberRead;
					lastOperation = operation;
				}
			}else if(operation == OperationSubtraction){
				if(lineArray[dereferencedElement] != '*' && lineArray[dereferencedElement] != '/'){
					if(numberOperatedOn == 0){
						number -= numberRead;
					}else{
						if(lastOperation == OperationAddition){
							number += numberOperatedOn;
						}else if(lastOperation == OperationSubtraction){
							number -= numberOperatedOn;
						}
					}
				}else{
					numberOperatedOn = numberRead;
					lastOperation = operation;
				}
			}else if(operation == OperationMultiplication){
				if(numberOperatedOn == 0){
					number *= numberRead;
				}else{
					numberOperatedOn *= numberRead;
				}
				if(lineArray[dereferencedElement] == '+' || lineArray[dereferencedElement] == '-'){
					if(lastOperation == OperationAddition){
						number += numberOperatedOn;
					}else if(lastOperation == OperationSubtraction){
						number -= numberOperatedOn;
					}
					numberOperatedOn = 0;
				}
			}else if(operation == OperationDivision){
				if(numberOperatedOn == 0){
					number /= numberRead;
				}else{
					numberOperatedOn /= numberRead;
				}
				if(lineArray[dereferencedElement] == '+' || lineArray[dereferencedElement] == '-'){
					if(lastOperation == OperationAddition){
						number += numberOperatedOn;
					}else if(lastOperation == OperationSubtraction){
						number -= numberOperatedOn;
					}
					numberOperatedOn = 0;
				}
			}else{
				if(number == 0){
					number = numberRead;
				}
			}
			if(lineArray[dereferencedElement] == '+'){
				operation = OperationAddition;
			}else if(lineArray[dereferencedElement] == '-'){
				operation = OperationSubtraction;
			}else if(lineArray[dereferencedElement] == '*'){
				operation = OperationMultiplication;
			}else if(lineArray[dereferencedElement] == '/'){
				operation = OperationDivision;
			}
			numberRead = 0;
			++dereferencedElement;
		}else{
			break;
		}
	}
	if(operation == OperationAddition){
		if(numberOperatedOn > 0){
			if(lastOperation == OperationAddition){
				number += numberOperatedOn;
			}else if(lastOperation == OperationSubtraction){
				number -= numberOperatedOn;
			}
		}
		number += numberRead;
	}else if(operation == OperationSubtraction){
		if(numberOperatedOn > 0){
			if(lastOperation == OperationAddition){
				number += numberOperatedOn;
			}else if(lastOperation == OperationSubtraction){
				number -= numberOperatedOn;
			}
		}
		number -= numberRead;
	}else if(operation == OperationMultiplication){
		if(numberOperatedOn == 0){
			number *= numberRead;
		}else{
			numberOperatedOn *= numberRead;
			if(lastOperation == OperationAddition){
				number += numberOperatedOn;
			}else if(lastOperation == OperationSubtraction){
				number -= numberOperatedOn;
			}
		}
	}else if(operation == OperationDivision){
		if(number > 0 || numberOperatedOn > 0){
			if(numberOperatedOn == 0){
				number /= numberRead;
			}else{
				numberOperatedOn /= numberRead;
				if(lastOperation == OperationAddition){
					number += numberOperatedOn;
				}else if(lastOperation == OperationSubtraction){
					number -= numberOperatedOn;
				}
			}
		}
	}else{
		if(number == 0){
			number = numberRead;
		}
	}
	*element = dereferencedElement;
	return number;
}
static bytes4 getARGB(const char *const restrict lineArray, unsigned int *const restrict element){
	unsigned int dereferencedElement = *element;
	int color = 0x00000000;
	if(lineArray[dereferencedElement] == '#'){
		++dereferencedElement;
	}
	unsigned int currentCharacter = 0;
	while(lineArray[dereferencedElement] != '\n' && currentCharacter < 8){
		color *= 16;
		if(lineArray[dereferencedElement] >= '0' && lineArray[dereferencedElement] <= '9'){
			color += lineArray[dereferencedElement];
			color -= 48;
		}else if(lineArray[dereferencedElement] >= 'A' && lineArray[dereferencedElement] <= 'F'){
			color += lineArray[dereferencedElement];
			color -= 55;
		}else if(lineArray[dereferencedElement] >= 'a' && lineArray[dereferencedElement] <= 'f'){
			color += lineArray[dereferencedElement];
			color -= 87;
		}else{
			fprintf(stderr, "%s: \'%c\' is not recognized as a hexadecimal number\n", ProgramName, lineArray[dereferencedElement]);
			color = 0x00000000;
			break;
		}
		++dereferencedElement;
		++currentCharacter;
	}
	if(currentCharacter == 8){
		*element = dereferencedElement;
	}
	return color;
}
static void getKeys(const Window *const restrict window, const unsigned int *const restrict currentLine, const char *const restrict lineArray, unsigned int *const restrict element, unsigned int *const restrict key, int *const restrict masks){
	unsigned int dereferencedElement = *element;
	*key = 0;
	int dereferencedMasks = 0;
	unsigned int lookingForValue = 1;
	while(lineArray[dereferencedElement] != '\n'){
		pushSpaces(lineArray, &dereferencedElement);
		if(lookingForValue){
			if(lineArray[dereferencedElement] >= '0' && lineArray[dereferencedElement] <= '9'){
				*key = getUnsignedDecimalNumber(window, currentLine, lineArray, &dereferencedElement);
				if((lineArray[dereferencedElement] >= 'A' && lineArray[dereferencedElement] <= 'Z') || (lineArray[dereferencedElement] >= 'a' && lineArray[dereferencedElement] <= 'z')){
					--dereferencedElement;
				}
			}else if(isVariable("Shift", lineArray, &dereferencedElement)){
				dereferencedMasks |= ShiftMask;
			}else if(isVariable("Lock", lineArray, &dereferencedElement)){
				dereferencedMasks |= LockMask;
			}else if(isVariable("Control", lineArray, &dereferencedElement)){
				dereferencedMasks |= ControlMask;
			}else if(isVariable("Mod1", lineArray, &dereferencedElement)){
				dereferencedMasks |= Mod1Mask;
			}else if(isVariable("Mod2", lineArray, &dereferencedElement)){
				dereferencedMasks |= Mod2Mask;
			}else if(isVariable("Mod3", lineArray, &dereferencedElement)){
				dereferencedMasks |= Mod3Mask;
			}else if(isVariable("Mod4", lineArray, &dereferencedElement)){
				dereferencedMasks |= Mod4Mask;
			}else if(isVariable("Mod5", lineArray, &dereferencedElement)){
				dereferencedMasks |= Mod5Mask;
			}else{
				break;
			}
			lookingForValue = 0;
		}else{
			if(lineArray[dereferencedElement] == '+'){
				++dereferencedElement;
				lookingForValue = 1;
			}else{
				break;
			}
		}
	}
	*element = dereferencedElement;
	*masks = dereferencedMasks;
	return;
}
static char *getText(const char *const restrict lineArray, unsigned int *const restrict element, const unsigned int addAmpersand){
	unsigned int dereferencedElement = *element;
	char *restrict text = NULL;
	unsigned int length = 0;
	{
		const char quotation = lineArray[dereferencedElement];
		++dereferencedElement;
		while(lineArray[dereferencedElement + length] != '\n' && lineArray[dereferencedElement + length] != quotation){
			++length;
		}
	}
	if(length > 0){
		{
			unsigned int extraCharacters;
			if(addAmpersand){
				extraCharacters = 1;
			}else{
				extraCharacters = 0;
			}
			text = (char *)malloc((length + 1 + extraCharacters) * sizeof(char));
		}
		if(text){
			unsigned int currentCharacter = 0;
			while(lineArray[dereferencedElement] != '\n' && currentCharacter < length){
				text[currentCharacter] = lineArray[dereferencedElement];
				++dereferencedElement;
				++currentCharacter;
			}
			if(currentCharacter == length){
				++dereferencedElement;
				if(addAmpersand){
					text[length] = '&';
					++length;
				}
				text[length] = '\0';
				*element = dereferencedElement;
			}
		}else{
			fprintf(stderr, "%s: could not allocate space for text\n", ProgramName);
		}
	}
	return text;
}
static bool printLineError(const char *const restrict lineArray, const unsigned int *const restrict element, const unsigned int *const restrict currentLine){
	bool value = 0;
	if(lineArray[*element] != '\n'){
		unsigned int length = 0;
		while(lineArray[length] != '\n'){
			++length;
		}
		fprintf(stderr, "%s: line %u: \"", ProgramName, *currentLine);
		for(unsigned int currentCharacter = *element; currentCharacter < length; ++currentCharacter){
			fprintf(stderr, "%c", lineArray[currentCharacter]);
		}
		fprintf(stderr, "\" not recognized as an internal variable\n");
		value = 1;
	}
	return value;
}
