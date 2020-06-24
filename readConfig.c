/* readConfig.c

MIT License

Copyright (C) 2019 Stefanos "Steven" Tsakiris

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include <stdint.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include "headers/defines.h"

#define NoVariables /*----------------------------*/ (VariableList)0
#define LinesVariable /*--------------------------*/ ((VariableList)1 << 0)
#define MonitorVariable /*------------------------*/ ((VariableList)1 << 1)
#define XVariable /*------------------------------*/ ((VariableList)1 << 2)
#define YVariable /*------------------------------*/ ((VariableList)1 << 3)
#define WidthVariable /*--------------------------*/ ((VariableList)1 << 4)
#define HeightVariable /*-------------------------*/ ((VariableList)1 << 5)
#define BorderVariable /*-------------------------*/ ((VariableList)1 << 6)
#define BorderColorVariable /*--------------------*/ ((VariableList)1 << 7)
#define BackgroundColorVariable /*----------------*/ ((VariableList)1 << 8)
#define GlobalSectionBorderColorVariable /*-------*/ ((VariableList)1 << 9)
#define GlobalSectionBackgroundColorVariable /*---*/ ((VariableList)1 << 10)
#define GlobalContainerBorderColorVariable /*-----*/ ((VariableList)1 << 11)
#define GlobalContainerBackgroundColorVariable /*-*/ ((VariableList)1 << 12)
#define GlobalRectangleBorderColorVariable /*-----*/ ((VariableList)1 << 13)
#define GlobalRectangleBackgroundColorVariable /*-*/ ((VariableList)1 << 14)
#define GlobalTextColorVariable /*----------------*/ ((VariableList)1 << 15)
#define GlobalDrawableCommandColorVariable /*-----*/ ((VariableList)1 << 16)
#define TextVariable /*---------------------------*/ ((VariableList)1 << 17)
#define TextColorVariable /*----------------------*/ ((VariableList)1 << 18)
#define TextOffsetXVariable /*--------------------*/ ((VariableList)1 << 19)
#define TextOffsetYVariable /*--------------------*/ ((VariableList)1 << 20)
#define CommandVariable /*------------------------*/ ((VariableList)1 << 21)
#define DrawableCommandVariable /*----------------*/ ((VariableList)1 << 22)
#define DrawableCommandColorVariable /*-----------*/ ((VariableList)1 << 23)
#define DrawableCommandOffsetXVariable /*---------*/ ((VariableList)1 << 24)
#define DrawableCommandOffsetYVariable /*---------*/ ((VariableList)1 << 25)
#define InteractAllVariable /*--------------------*/ ((VariableList)1 << 26)
#define HideVariable /*---------------------------*/ ((VariableList)1 << 27)
#define PeekVariable /*---------------------------*/ ((VariableList)1 << 28)
#define RestartVariable /*------------------------*/ ((VariableList)1 << 29)
#define ExitVariable /*---------------------------*/ ((VariableList)1 << 30)
#define ButtonVariable /*-------------------------*/ ((VariableList)1 << 31)
#define SectionVariable /*------------------------*/ ((VariableList)1 << 32)
#define ContainerVariable /*----------------------*/ ((VariableList)1 << 33)
#define RectangleVariable /*----------------------*/ ((VariableList)1 << 34)

#define NoMathOperation /*------------------------*/ 0
#define AdditionMathOperation /*------------------*/ 1
#define SubtractionMathOperation /*---------------*/ 2
#define MultiplicationMathOperation /*------------*/ 3
#define DivisionMathOperation /*------------------*/ 4

extern const char *programName;
extern const char *configPath;
extern Display *display;
extern unsigned int monitorAmount;
extern unsigned int whichMonitor;
extern unsigned int containerAmount;
extern char line[DefaultCharactersCount + 1];
extern unsigned int currentMonitor;
extern Window *const *container;

typedef uint64_t VariableList;
typedef uint8_t MathOperation;

static FILE *getConfigFile(void);
static bool getLine(FILE *const file);
static void pushWhitespace(unsigned int *const element);
static bool isVariable(const char *const variable, unsigned int *const element);
static unsigned int getUnsignedInteger(const unsigned int currentLine, const Window parentWindow, unsigned int *const element);
static int getInteger(Window parentWindow, unsigned int *const element);
static ARGB getARGB(unsigned int *const element);
static unsigned int getQuotedStringLength(unsigned int *const element);
static unsigned int getQuotedString(char *const string, unsigned int *const element);
static Shortcut getShortcut(unsigned int *const element);
static Button getButton(unsigned int *const element);
static void printLineError(const unsigned int currentLine);

bool readConfig(const ConfigMode configMode, ConfigInfo ci){
	bool value = 0;
	if(configMode == ScanConfigMode){
		whichMonitor = monitorAmount;
		containerAmount = 0;
	}else if(configMode != ButtonsConfigMode){
		unsigned int currentDimension0;
		unsigned int currentDimension1;
		unsigned int currentDimension2;
		if(ci.integer){
			int **dimension0;
			int *dimension1;
			for(currentDimension0 = 0; currentDimension0 < ci.integerDimension0; ++currentDimension0){
				dimension0 = ci.integer[currentDimension0];
				for(currentDimension1 = 0; currentDimension1 < ci.integerDimension1; ++currentDimension1){
					dimension1 = dimension0[currentDimension1];
					for(currentDimension2 = 0; currentDimension2 < ci.integerDimension2; ++currentDimension2){
						dimension1[currentDimension2] = 0;
					}
				}
			}
		}
		if(ci.unsignedInteger){
			unsigned int **dimension0;
			unsigned int *dimension1;
			for(currentDimension0 = 0; currentDimension0 < ci.unsignedIntegerDimension0; ++currentDimension0){
				dimension0 = ci.unsignedInteger[currentDimension0];
				for(currentDimension1 = 0; currentDimension1 < ci.unsignedIntegerDimension1; ++currentDimension1){
					dimension1 = dimension0[currentDimension1];
					for(currentDimension2 = 0; currentDimension2 < ci.unsignedIntegerDimension2; ++currentDimension2){
						dimension1[currentDimension2] = 0;
					}
				}
			}
		}
		if(ci.argb){
			ARGB **dimension0;
			ARGB *dimension1;
			for(currentDimension0 = 0; currentDimension0 < ci.argbDimension0; ++currentDimension0){
				dimension0 = ci.argb[currentDimension0];
				for(currentDimension1 = 0; currentDimension1 < ci.argbDimension1; ++currentDimension1){
					dimension1 = dimension0[currentDimension1];
					for(currentDimension2 = 0; currentDimension2 < ci.argbDimension2; ++currentDimension2){
						dimension1[currentDimension2] = 0x00000000;
					}
				}
			}
		}
		if(ci.character){
			char **dimension0;
			char *dimension1;
			for(currentDimension0 = 0; currentDimension0 < ci.characterDimension0; ++currentDimension0){
				dimension0 = ci.character[currentDimension0];
				for(currentDimension1 = 0; currentDimension1 < ci.characterDimension1; ++currentDimension1){
					dimension1 = dimension0[currentDimension1];
					for(currentDimension2 = 0; currentDimension2 < ci.characterDimension2; ++currentDimension2){
						dimension1[currentDimension2] = '\0';
					}
				}
			}
		}
	}
	FILE *const file = getConfigFile();
	if(file){
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		VariableList hasReadVariable = NoVariables;
		unsigned int counter0 = 0;
		unsigned int counter1 = 0;
		unsigned int e;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine(file)){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & SectionVariable)){
					if(!(hasReadVariable & LinesVariable)){
						if(isVariable("lines", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								maxLinesCount = getUnsignedInteger(currentLine, None, &element);
								hasReadVariable |= LinesVariable;
							}
							continue;
						}
					}
					if(configMode == ScanConfigMode){
						if(!(hasReadVariable & MonitorVariable)){
							if(isVariable("monitor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									if(!isVariable("all", &element)){
										whichMonitor = getUnsignedInteger(currentLine, None, &element);
										if(whichMonitor < monitorAmount){
											monitorAmount = 1;
										}else{
											whichMonitor = monitorAmount;
										}
									}
									hasReadVariable |= MonitorVariable;
								}
								continue;
							}
						}
					}else if(configMode == TopLevelWindowsConfigMode){
						if(!(hasReadVariable & XVariable)){
							if(isVariable("x", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.integer[0][0][currentMonitor] = getInteger(ci.window[0][0], &element);
										element = e;
									}
									hasReadVariable |= XVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & YVariable)){
							if(isVariable("y", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.integer[0][1][currentMonitor] = getInteger(ci.window[0][0], &element);
										element = e;
									}
									hasReadVariable |= YVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & WidthVariable)){
							if(isVariable("width", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.unsignedInteger[0][0][currentMonitor] = getInteger(ci.window[0][0], &element);
										element = e;
									}
									hasReadVariable |= WidthVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & HeightVariable)){
							if(isVariable("height", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.unsignedInteger[0][1][currentMonitor] = getInteger(ci.window[0][0], &element);
										element = e;
									}
									hasReadVariable |= HeightVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BorderColorVariable)){
							if(isVariable("borderColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.argb[0][0][0] = getARGB(&element);
									hasReadVariable |= BorderColorVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BorderVariable)){
							if(isVariable("border", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.unsignedInteger[0][2][currentMonitor] = getInteger(ci.window[0][0], &element);
										element = e;
									}
									hasReadVariable |= BorderVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BackgroundColorVariable)){
							if(isVariable("backgroundColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.argb[0][1][0] = getARGB(&element);
									hasReadVariable |= BackgroundColorVariable;
								}
								continue;
							}
						}
					}else if(configMode == SectionWindowsConfigMode){
						if(!(hasReadVariable & GlobalSectionBorderColorVariable)){
							if(isVariable("globalSectionBorderColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									const ARGB globalSectionBorderColor = getARGB(&element);
									for(unsigned int currentSection = counter0; currentSection < ci.argbDimension2; ++currentSection){
										ci.argb[0][0][currentSection] = globalSectionBorderColor;
									}
									hasReadVariable |= GlobalSectionBorderColorVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & GlobalSectionBackgroundColorVariable)){
							if(isVariable("globalSectionBackgroundColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									const ARGB globalSectionBackgroundColor = getARGB(&element);
									for(unsigned int currentSection = counter0; currentSection < ci.argbDimension2; ++currentSection){
										ci.argb[0][1][currentSection] = globalSectionBackgroundColor;
									}
									hasReadVariable |= GlobalSectionBackgroundColorVariable;
								}
								continue;
							}
						}
					}else if(configMode == VariableShortcutsConfigMode){
						if(isVariable("keycode", &element)){
							pushWhitespace(&element);
							getShortcut(&element);
							if(isVariable("interact", &element)){
								pushWhitespace(&element);
								if(isVariable("section", &element)){
									++ci.unsignedInteger[0][0][0];
								}else if(isVariable("container", &element)){
									++ci.unsignedInteger[0][1][0];
								}
							}
							continue;
						}
					}else if(configMode == FontAmountConfigMode){
						if(isVariable("font", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								++ci.unsignedInteger[0][0][0];
							}
							continue;
						}
					}else if(configMode == FontLengthConfigMode){
						if(isVariable("font", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								ci.unsignedInteger[0][0][counter0] = getQuotedStringLength(&element);
								++counter0;
							}
							continue;
						}
					}else if(configMode == FontSetConfigMode){
						if(isVariable("font", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								if(counter1){
									ci.character[0][0][counter0] = ',';
									++counter0;
								}
								counter0 += getQuotedString(&ci.character[0][0][counter0], &element);
								ci.character[0][0][counter0] = '\0';
								++counter1;
							}
							continue;
						}
					}
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							if(configMode == SectionRectangleAmountConfigMode){
								++ci.unsignedInteger[0][0][0];
							}
							hasReadVariable |= SectionVariable;
						}
						continue;
					}
					if(configMode == ScanConfigMode){
						if(line[element]){
							if(!isVariable("monitor",                      &element) &&
							   !isVariable("x",                            &element) &&
							   !isVariable("y",                            &element) &&
							   !isVariable("width",                        &element) &&
							   !isVariable("height",                       &element) &&
							   !isVariable("border",                       &element) &&
							   !isVariable("borderColor",                  &element) &&
							   !isVariable("backgroundColor",              &element) &&
							   !isVariable("globalSectionBorderColor",     &element) &&
							   !isVariable("globalSectionBackgroundColor", &element) &&
							   !isVariable("font",                         &element) &&
							   !isVariable("keycode",                      &element)){
								printLineError(currentLine);
								continue;
							}
						}
					}
				}else if(!(hasReadVariable & ContainerVariable)){
					if(configMode == SectionWindowsConfigMode){
						if(!(hasReadVariable & XVariable)){
							if(isVariable("x", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.integer[0][currentMonitor][counter0] = getInteger(ci.window[0][currentMonitor], &element);
										element = e;
									}
									hasReadVariable |= XVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & YVariable)){
							if(isVariable("y", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.integer[1][currentMonitor][counter0] = getInteger(ci.window[0][currentMonitor], &element);
										element = e;
									}
									hasReadVariable |= YVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & WidthVariable)){
							if(isVariable("width", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.unsignedInteger[0][currentMonitor][counter0] = getUnsignedInteger(currentLine, ci.window[0][currentMonitor], &element);
										element = e;
									}
									hasReadVariable |= WidthVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & HeightVariable)){
							if(isVariable("height", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.unsignedInteger[1][currentMonitor][counter0] = getUnsignedInteger(currentLine, ci.window[0][currentMonitor], &element);
										element = e;
									}
									hasReadVariable |= HeightVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BorderColorVariable)){
							if(isVariable("borderColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.argb[0][0][counter0] = getARGB(&element);
									hasReadVariable |= BorderColorVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BorderVariable)){
							if(isVariable("border", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.unsignedInteger[2][currentMonitor][counter0] = getUnsignedInteger(currentLine, ci.window[0][currentMonitor], &element);
										element = e;
									}
									hasReadVariable |= BorderVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BackgroundColorVariable)){
							if(isVariable("backgroundColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.argb[0][1][counter0] = getARGB(&element);
									hasReadVariable |= BackgroundColorVariable;
								}
								continue;
							}
						}
					}else if(configMode == ContainerWindowsConfigMode){
						if(!(hasReadVariable & GlobalContainerBorderColorVariable)){
							if(isVariable("globalContainerBorderColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									const ARGB globalContainerBorderColor = getARGB(&element);
									for(unsigned int currentContainer = counter1; currentContainer < containerAmount; ++currentContainer){
										ci.argb[0][0][currentContainer] = globalContainerBorderColor;
									}
									hasReadVariable |= GlobalContainerBorderColorVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & GlobalContainerBackgroundColorVariable)){
							if(isVariable("globalContainerBackgroundColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									const ARGB globalContainerBackgroundColor = getARGB(&element);
									for(unsigned int currentContainer = counter1; currentContainer < containerAmount; ++currentContainer){
										ci.argb[0][1][currentContainer] = globalContainerBackgroundColor;
									}
									hasReadVariable |= GlobalContainerBackgroundColorVariable;
								}
								continue;
							}
						}
					}else if(configMode == FillArraysConfigMode){
						if(!(hasReadVariable & GlobalTextColorVariable)){
							if(isVariable("globalTextColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									const ARGB globalTextColor = getARGB(&element);
									for(unsigned int currentContainer = counter0; currentContainer < containerAmount; ++currentContainer){
										ci.argb[0][0][currentContainer] = globalTextColor;
									}
									hasReadVariable |= GlobalTextColorVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & GlobalDrawableCommandColorVariable)){
							if(isVariable("globalDrawableCommandColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									const ARGB globalDrawableCommandColor = getARGB(&element);
									for(unsigned int currentContainer = counter0; currentContainer < containerAmount; ++currentContainer){
										ci.argb[0][1][currentContainer] = globalDrawableCommandColor;
									}
									hasReadVariable |= GlobalDrawableCommandColorVariable;
								}
								continue;
							}
						}
					}
					if(isVariable("container", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							if(configMode == ScanConfigMode){
								++containerAmount;
							}else if(configMode == SectionChildrenConfigMode){
								++ci.unsignedInteger[0][0][counter0];
							}
							hasReadVariable |= ContainerVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
						if(configMode == SectionWindowsConfigMode){
							if(hasReadVariable & XVariable){
								hasReadVariable ^= XVariable;
							}
							if(hasReadVariable & YVariable){
								hasReadVariable ^= YVariable;
							}
							if(hasReadVariable & WidthVariable){
								hasReadVariable ^= WidthVariable;
							}
							if(hasReadVariable & HeightVariable){
								hasReadVariable ^= HeightVariable;
							}
							if(hasReadVariable & BorderVariable){
								hasReadVariable ^= BorderVariable;
							}
							if(hasReadVariable & BorderColorVariable){
								hasReadVariable ^= BorderColorVariable;
							}
							if(hasReadVariable & BackgroundColorVariable){
								hasReadVariable ^= BackgroundColorVariable;
							}
							++counter0;
						}else if(configMode == SectionChildrenConfigMode){
							++counter0;
						}else if(configMode == ContainerWindowsConfigMode){
							++counter0;
							if(hasReadVariable & GlobalContainerBorderColorVariable){
								hasReadVariable ^= GlobalContainerBorderColorVariable;
							}
							if(hasReadVariable & GlobalContainerBackgroundColorVariable){
								hasReadVariable ^= GlobalContainerBackgroundColorVariable;
							}
							for(unsigned int currentContainer = counter1; currentContainer < containerAmount; ++currentContainer){
								ci.argb[0][0][currentContainer] = 0x00000000;
								ci.argb[0][1][currentContainer] = 0x00000000;
							}
						}else if(configMode == FillArraysConfigMode){
							if(hasReadVariable & GlobalTextColorVariable){
								hasReadVariable ^= GlobalTextColorVariable;
							}
							if(hasReadVariable & GlobalDrawableCommandColorVariable){
								hasReadVariable ^= GlobalDrawableCommandColorVariable;
							}
							for(unsigned int currentContainer = counter0; currentContainer < containerAmount; ++currentContainer){
								ci.argb[0][0][currentContainer] = 0x00000000;
								ci.argb[0][1][currentContainer] = 0x00000000;
							}
						}
						hasReadVariable ^= SectionVariable;
						continue;
					}
					if(configMode == ScanConfigMode){
						if(line[element]){
							if(!isVariable("x",                              &element) &&
							   !isVariable("y",                              &element) &&
							   !isVariable("width",                          &element) &&
							   !isVariable("height",                         &element) &&
							   !isVariable("border",                         &element) &&
							   !isVariable("borderColor",                    &element) &&
							   !isVariable("backgroundColor",                &element) &&
							   !isVariable("globalContainerBorderColor",     &element) &&
							   !isVariable("globalContainerBackgroundColor", &element) &&
							   !isVariable("globalTextColor",                &element) &&
							   !isVariable("globalDrawableCommandColor",     &element)){
								printLineError(currentLine);
								continue;
							}
						}
					}
				}else if(!(hasReadVariable & RectangleVariable)){
					if(configMode == ContainerWindowsConfigMode){
						if(!(hasReadVariable & XVariable)){
							if(isVariable("x", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.integer[0][currentMonitor][counter1] = getUnsignedInteger(currentLine, ci.window[currentMonitor][counter0], &element);
										element = e;
									}
									hasReadVariable |= XVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & YVariable)){
							if(isVariable("y", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.integer[1][currentMonitor][counter1] = getUnsignedInteger(currentLine, ci.window[currentMonitor][counter0], &element);
										element = e;
									}
									hasReadVariable |= YVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & WidthVariable)){
							if(isVariable("width", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.unsignedInteger[0][currentMonitor][counter1] = getUnsignedInteger(currentLine, ci.window[currentMonitor][counter0], &element);
										element = e;
									}
									hasReadVariable |= WidthVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & HeightVariable)){
							if(isVariable("height", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.unsignedInteger[1][currentMonitor][counter1] = getUnsignedInteger(currentLine, ci.window[currentMonitor][counter0], &element);
										element = e;
									}
									hasReadVariable |= HeightVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BorderColorVariable)){
							if(isVariable("borderColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.argb[0][0][counter1] = getARGB(&element);
									hasReadVariable |= BorderColorVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BorderVariable)){
							if(isVariable("border", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.unsignedInteger[2][currentMonitor][counter1] = getUnsignedInteger(currentLine, ci.window[currentMonitor][counter0], &element);
										element = e;
									}
									hasReadVariable |= BorderVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BackgroundColorVariable)){
							if(isVariable("backgroundColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.argb[0][1][counter1] = getARGB(&element);
									hasReadVariable |= BackgroundColorVariable;
								}
								continue;
							}
						}
					}else if(configMode == RectangleWindowsConfigMode){
						if(!(hasReadVariable & GlobalRectangleBorderColorVariable)){
							if(isVariable("globalRectangleBorderColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									const ARGB globalRectangleBorderColor = getARGB(&element);
									for(unsigned int currentRectangle = counter1; currentRectangle < ci.argbDimension2; ++currentRectangle){
										ci.argb[0][0][currentRectangle] = globalRectangleBorderColor;
									}
									hasReadVariable |= GlobalRectangleBorderColorVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & GlobalRectangleBackgroundColorVariable)){
							if(isVariable("globalRectangleBackgroundColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									const ARGB globalRectangleBackgroundColor = getARGB(&element);
									for(unsigned int currentRectangle = counter1; currentRectangle < ci.argbDimension2; ++currentRectangle){
										ci.argb[0][1][currentRectangle] = globalRectangleBackgroundColor;
									}
									hasReadVariable |= GlobalRectangleBackgroundColorVariable;
								}
								continue;
							}
						}
					}else if(configMode == ArrayLengthsConfigMode){
						if(!(hasReadVariable & TextVariable)){
							if(isVariable("text", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									unsigned int length = getQuotedStringLength(&element);
									if(length > ci.unsignedInteger[0][0][0]){
										ci.unsignedInteger[0][0][0] = length;
									}
									hasReadVariable |= TextVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & CommandVariable)){
							if(isVariable("command", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									unsigned int length = getQuotedStringLength(&element);
									if(length > ci.unsignedInteger[0][1][0]){
										ci.unsignedInteger[0][1][0] = length;
									}
									hasReadVariable |= CommandVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & DrawableCommandVariable)){
							if(isVariable("drawableCommand", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									unsigned int length = getQuotedStringLength(&element);
									if(length > ci.unsignedInteger[0][2][0]){
										ci.unsignedInteger[0][2][0] = length;
									}
									hasReadVariable |= DrawableCommandVariable;
								}
								continue;
							}
						}
					}else if(configMode == FillArraysConfigMode){
						if(!(hasReadVariable & TextVariable)){
							if(isVariable("text", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.character[0][counter0][getQuotedString(ci.character[0][counter0], &element)] = '\0';
									hasReadVariable |= TextVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & TextColorVariable)){
							if(isVariable("textColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.argb[0][0][counter0] = getARGB(&element);
									hasReadVariable |= TextColorVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & CommandVariable)){
							if(isVariable("command", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.character[1][counter0][getQuotedString(ci.character[1][counter0], &element)] = '\0';
									hasReadVariable |= CommandVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & DrawableCommandVariable)){
							if(isVariable("drawableCommand", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.character[2][counter0][getQuotedString(ci.character[2][counter0], &element)] = '\0';
									hasReadVariable |= DrawableCommandVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & DrawableCommandColorVariable)){
							if(isVariable("drawableCommandColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.argb[0][1][counter0] = getARGB(&element);
								}
								continue;
							}
						}
					}else if(configMode == ButtonsConfigMode){
						if(!(hasReadVariable & ButtonVariable)){
							if(isVariable("button", &element)){
								pushWhitespace(&element);
								const Button button = getButton(&element);
								if(button.button != AnyButton){
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										XGrabButton(display, button.button, button.masks, container[currentMonitor][counter0], True, NoEventMask, GrabModeAsync, GrabModeAsync, None, None);
									}
								}
								hasReadVariable |= ButtonVariable;
								continue;
							}
						}
					}else if(configMode == FontOffsetsConfigMode){
						if(!(hasReadVariable & TextOffsetXVariable)){
							if(isVariable("textOffsetX", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.integer[0][0][counter0] = getInteger(None, &element);
									hasReadVariable |= TextOffsetXVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & TextOffsetYVariable)){
							if(isVariable("textOffsetY", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.integer[0][1][counter0] = getInteger(None, &element);
									hasReadVariable |= TextOffsetYVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & DrawableCommandOffsetXVariable)){
							if(isVariable("drawableCommandOffsetX", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.integer[0][2][counter0] = getInteger(None, &element);
									hasReadVariable |= DrawableCommandOffsetXVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & DrawableCommandOffsetYVariable)){
							if(isVariable("drawableCommandOffsetY", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.integer[0][3][counter0] = getInteger(None, &element);
									hasReadVariable |= DrawableCommandOffsetYVariable;
								}
								continue;
							}
						}
					}
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							if(configMode == SectionRectangleAmountConfigMode){
								++ci.unsignedInteger[0][1][0];
							}else if(configMode == ContainerChildrenConfigMode){
								++ci.unsignedInteger[0][0][counter0];
							}
							hasReadVariable |= RectangleVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
						if(configMode == ContainerWindowsConfigMode){
							if(hasReadVariable & XVariable){
								hasReadVariable ^= XVariable;
							}
							if(hasReadVariable & YVariable){
								hasReadVariable ^= YVariable;
							}
							if(hasReadVariable & WidthVariable){
								hasReadVariable ^= WidthVariable;
							}
							if(hasReadVariable & HeightVariable){
								hasReadVariable ^= HeightVariable;
							}
							if(hasReadVariable & BorderVariable){
								hasReadVariable ^= BorderVariable;
							}
							if(hasReadVariable & BorderColorVariable){
								hasReadVariable ^= BorderColorVariable;
							}
							if(hasReadVariable & BackgroundColorVariable){
								hasReadVariable ^= BackgroundColorVariable;
							}
							++counter1;
						}else if(configMode == ContainerChildrenConfigMode){
							++counter0;
						}else if(configMode == RectangleWindowsConfigMode){
							++counter0;
							if(hasReadVariable & GlobalRectangleBorderColorVariable){
								hasReadVariable ^= GlobalRectangleBorderColorVariable;
							}
							if(hasReadVariable & GlobalRectangleBackgroundColorVariable){
								hasReadVariable ^= GlobalRectangleBackgroundColorVariable;
							}
							for(unsigned int currentRectangle = counter1; currentRectangle < ci.argbDimension2; ++currentRectangle){
								ci.argb[0][0][currentRectangle] = 0x00000000;
								ci.argb[0][1][currentRectangle] = 0x00000000;
							}
						}else if(configMode == ArrayLengthsConfigMode){
							if(hasReadVariable & TextVariable){
								hasReadVariable ^= TextVariable;
							}
							if(hasReadVariable & CommandVariable){
								hasReadVariable ^= CommandVariable;
							}
							if(hasReadVariable & DrawableCommandVariable){
								hasReadVariable ^= DrawableCommandVariable;
							}
						}else if(configMode == FillArraysConfigMode){
							if(hasReadVariable & TextVariable){
								hasReadVariable ^= TextVariable;
							}
							if(hasReadVariable & TextColorVariable){
								hasReadVariable ^= TextColorVariable;
							}
							if(hasReadVariable & CommandVariable){
								hasReadVariable ^= CommandVariable;
							}
							if(hasReadVariable & DrawableCommandVariable){
								hasReadVariable ^= DrawableCommandVariable;
							}
							if(hasReadVariable & DrawableCommandColorVariable){
								hasReadVariable ^= DrawableCommandColorVariable;
							}
							++counter0;
						}else if(configMode == ButtonsConfigMode){
							if(hasReadVariable & ButtonVariable){
								hasReadVariable ^= ButtonVariable;
							}
							++counter0;
						}else if(configMode == FontOffsetsConfigMode){
							if(hasReadVariable & TextOffsetXVariable){
								hasReadVariable ^= TextOffsetXVariable;
							}
							if(hasReadVariable & TextOffsetYVariable){
								hasReadVariable ^= TextOffsetYVariable;
							}
							if(hasReadVariable & DrawableCommandOffsetXVariable){
								hasReadVariable ^= DrawableCommandOffsetXVariable;
							}
							if(hasReadVariable & DrawableCommandOffsetYVariable){
								hasReadVariable ^= DrawableCommandOffsetYVariable;
							}
							++counter0;
						}
						hasReadVariable ^= ContainerVariable;
						continue;
					}
					if(configMode == ScanConfigMode){
						if(line[element]){
							if(!isVariable("x",                              &element) &&
							   !isVariable("y",                              &element) &&
							   !isVariable("width",                          &element) &&
							   !isVariable("height",                         &element) &&
							   !isVariable("border",                         &element) &&
							   !isVariable("borderColor",                    &element) &&
							   !isVariable("backgroundColor",                &element) &&
							   !isVariable("globalRectangleBorderColor",     &element) &&
							   !isVariable("globalRectangleBackgroundColor", &element) &&
							   !isVariable("text",                           &element) &&
							   !isVariable("textColor",                      &element) &&
							   !isVariable("textOffsetX",                    &element) &&
							   !isVariable("textOffsetY",                    &element) &&
							   !isVariable("command",                        &element) &&
							   !isVariable("drawableCommand",                &element) &&
							   !isVariable("drawableCommandColor",           &element) &&
							   !isVariable("drawableCommandOffsetX",         &element) &&
							   !isVariable("drawableCommandOffsetY",         &element) &&
							   !isVariable("button",                         &element)){
								printLineError(currentLine);
								continue;
							}
						}
					}
				}else{
					if(configMode == RectangleWindowsConfigMode){
						if(!(hasReadVariable & XVariable)){
							if(isVariable("x", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.integer[0][currentMonitor][counter1] = getUnsignedInteger(currentLine, ci.window[currentMonitor][counter0], &element);
										element = e;
									}
									hasReadVariable |= XVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & YVariable)){
							if(isVariable("y", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.integer[1][currentMonitor][counter1] = getUnsignedInteger(currentLine, ci.window[currentMonitor][counter0], &element);
										element = e;
									}
									hasReadVariable |= YVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & WidthVariable)){
							if(isVariable("width", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.unsignedInteger[0][currentMonitor][counter1] = getUnsignedInteger(currentLine, ci.window[currentMonitor][counter0], &element);
										element = e;
									}
									hasReadVariable |= WidthVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & HeightVariable)){
							if(isVariable("height", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.unsignedInteger[1][currentMonitor][counter1] = getUnsignedInteger(currentLine, ci.window[currentMonitor][counter0], &element);
										element = e;
									}
									hasReadVariable |= HeightVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BorderColorVariable)){
							if(isVariable("borderColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.argb[0][0][counter1] = getARGB(&element);
									hasReadVariable |= BorderColorVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BorderVariable)){
							if(isVariable("border", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									e = element;
									for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
										ci.unsignedInteger[2][currentMonitor][counter1] = getUnsignedInteger(currentLine, ci.window[currentMonitor][counter0], &element);
										element = e;
									}
									hasReadVariable |= BorderVariable;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BackgroundColorVariable)){
							if(isVariable("backgroundColor", &element)){
								pushWhitespace(&element);
								if(isVariable("=", &element)){
									pushWhitespace(&element);
									ci.argb[0][1][counter1] = getARGB(&element);
									hasReadVariable |= BackgroundColorVariable;
								}
								continue;
							}
						}
					}
					if(isVariable("}", &element)){
						if(configMode == RectangleWindowsConfigMode){
							if(hasReadVariable & XVariable){
								hasReadVariable ^= XVariable;
							}
							if(hasReadVariable & YVariable){
								hasReadVariable ^= YVariable;
							}
							if(hasReadVariable & WidthVariable){
								hasReadVariable ^= WidthVariable;
							}
							if(hasReadVariable & HeightVariable){
								hasReadVariable ^= HeightVariable;
							}
							if(hasReadVariable & BorderVariable){
								hasReadVariable ^= BorderVariable;
							}
							if(hasReadVariable & BorderColorVariable){
								hasReadVariable ^= BorderColorVariable;
							}
							if(hasReadVariable & BackgroundColorVariable){
								hasReadVariable ^= BackgroundColorVariable;
							}
							++counter1;
						}
						hasReadVariable ^= RectangleVariable;
						continue;
					}
					if(configMode == ScanConfigMode){
						if(line[element]){
							if(!isVariable("x",               &element) &&
							   !isVariable("y",               &element) &&
							   !isVariable("width",           &element) &&
							   !isVariable("height",          &element) &&
							   !isVariable("border",          &element) &&
							   !isVariable("borderColor",     &element) &&
							   !isVariable("backgroundColor", &element)){
								printLineError(currentLine);
								continue;
							}
						}
					}
				}
			}
		}
		fclose(file);
		value = 1;
	}
	return value;
}
bool readConfigShortcuts(const unsigned int sectionShortcutAmount, const unsigned int containerShortcutAmount, Shortcut *const interactAll, Shortcut *const interactSection, unsigned int *const sectionNumber, Shortcut *const interactContainer, unsigned int *const containerNumber, Shortcut *const hide, Shortcut *const peek, Shortcut *const restart, Shortcut *const exit){
	bool value = 0;
	unsigned int currentSectionShortcut;
	unsigned int currentContainerShortcut;
	{
		const Shortcut shortcut = {
			.keycode = AnyKey,
			.masks = None
		};
		(*interactAll) = shortcut;
		(*hide) = shortcut;
		(*peek) = shortcut;
		(*restart) = shortcut;
		(*exit) = shortcut;
		for(currentSectionShortcut = 0; currentSectionShortcut < sectionShortcutAmount; ++currentSectionShortcut){
			interactSection[currentSectionShortcut] = shortcut;
			sectionNumber[currentSectionShortcut] = 0;
		}
		for(currentContainerShortcut = 0; currentContainerShortcut < containerShortcutAmount; ++currentContainerShortcut){
			interactContainer[currentContainerShortcut] = shortcut;
			containerNumber[currentContainerShortcut] = 0;
		}
	}
	FILE *const file = getConfigFile();
	if(file){
		currentSectionShortcut = 0;
		currentContainerShortcut = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		VariableList hasReadVariable = NoVariables;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine(file)){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & SectionVariable)){
					if(!(hasReadVariable & LinesVariable)){
						if(isVariable("lines", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								maxLinesCount = getUnsignedInteger(currentLine, None, &element);
								hasReadVariable |= LinesVariable;
							}
							continue;
						}
					}
					if(isVariable("keycode", &element)){
						pushWhitespace(&element);
						const Shortcut shortcut = getShortcut(&element);
						if(isVariable("interact", &element)){
							pushWhitespace(&element);
							if(isVariable("all", &element)){
								if(!(hasReadVariable & InteractAllVariable)){
									(*interactAll) = shortcut;
								}
							}else if(isVariable("section", &element)){
								pushWhitespace(&element);
								interactSection[currentSectionShortcut] = shortcut;
								sectionNumber[currentSectionShortcut] = getUnsignedInteger(currentLine, None, &element);
								++currentSectionShortcut;
							}else if(isVariable("container", &element)){
								pushWhitespace(&element);
								interactContainer[currentContainerShortcut] = shortcut;
								containerNumber[currentContainerShortcut] = getUnsignedInteger(currentLine, None, &element);
								++currentContainerShortcut;
							}
						}else if(isVariable("hide", &element)){
							if(!(hasReadVariable & HideVariable)){
								(*hide) = shortcut;
							}
						}else if(isVariable("peek", &element)){
							if(!(hasReadVariable & PeekVariable)){
								(*peek) = shortcut;
							}
						}else if(isVariable("restart", &element)){
							if(!(hasReadVariable & RestartVariable)){
								(*restart) = shortcut;
							}
						}else if(isVariable("exit", &element)){
							if(!(hasReadVariable & ExitVariable)){
								(*exit) = shortcut;
							}
						}
						continue;
					}
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= SectionVariable;
						}
						continue;
					}
				}else if(!(hasReadVariable & ContainerVariable)){
					if(isVariable("container", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= ContainerVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= SectionVariable;
						continue;
					}
				}else if(!(hasReadVariable & RectangleVariable)){
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectangleVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= ContainerVariable;
						continue;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= RectangleVariable;
						continue;
					}
				}
			}
		}
		fclose(file);
		value = 1;
	}
	return value;
}
static FILE *getConfigFile(void){
	FILE *config = fopen(configPath, "r");
	if(!config){
		if((config = fopen(configPath, "w"))){
			fprintf(config, "# configuration file for\n\n");
			fprintf(config, "# ╔═══════════════════════════════════════════════════════════════════════════════════════════════╗\n");
			fprintf(config, "# ║                                                                                               ║\n");
			fprintf(config, "# ║   #           #   #     # # #     # # # #       # # #     # # # #       # # #     # # # #     ║\n");
			fprintf(config, "# ║   # #       # #   #   #       #   #       #   #       #   #       #   #       #   #       #   ║\n");
			fprintf(config, "# ║   #   #   #   #   #   #           #       #   #       #   #       #   #       #   #       #   ║\n");
			fprintf(config, "# ║   #     #     #   #   #           # # # #     #       #   # # # #     # # # # #   # # # #     ║\n");
			fprintf(config, "# ║   #     #     #   #   #           #   #       #       #   #       #   #       #   #   #       ║\n");
			fprintf(config, "# ║   #     #     #   #   #       #   #     #     #       #   #       #   #       #   #     #     ║\n");
			fprintf(config, "# ║   #     #     #   #     # # #     #       #     # # #     # # # #     #       #   #       #   ║\n");
			fprintf(config, "# ║                                                                                               ║\n");
			fprintf(config, "# ╚═══════════════════════════════════════════════════════════════════════════════════════════════╝\n\n\n\n");
			fprintf(config, "# # # # #\n");
			fprintf(config, "# rules #\n");
			fprintf(config, "# # # # #\n\n");
			fprintf(config, "# certain values can be changed through the headers/defines.h of the program\'s source\n");
			fprintf(config, "# this file needs to be user-specified when launched\n");
			fprintf(config, "# max line character length is %u\n", DefaultCharactersCount);
			fprintf(config, "# comments are signified by a \'#\' at the beginning of the line\n");
			fprintf(config, "# one variable per line, followed by \'=\' and its value\n");
			fprintf(config, "# all spaces and tabs are ignored\n");
			fprintf(config, "# all variables are valued 0 or undefined by default unless stated otherwise\n");
			fprintf(config, "# colors are in argb format\n");
			fprintf(config, "# all variables, argb values and macros are case insensitive\n");
			fprintf(config, "# argb do not require a \'#\' before the value\n");
			fprintf(config, "# text require the same quote character before and after it\n");
			fprintf(config, "# text quotation is variable, the first character is the quote character\n\n\n\n");
			fprintf(config, "# # # # # # #\n");
			fprintf(config, "# variables #\n");
			fprintf(config, "# # # # # # #\n\n");
			fprintf(config, "# global object: lines, monitor, x, y, width, height, border, borderColor, backgroundColor, globalSectionBorderColor, globalSectionBackgroundColor, font, keycode, section{}\n");
			fprintf(config, "# section object: x, y, width, height, border, borderColor, backgroundColor, globalContainerBorderColor, globalContainerBackgroundColor, globalTextColor, globalDrawableCommandColor, container{}\n");
			fprintf(config, "# container object: x, y, width, height, border, borderColor, backgroundColor, globalRectangleBorderColor, globalRectangleBackgroundColor, text, textColor, textOffsetX, textOffsetY, command, drawableCommand, drawableCommandColor, drawableCommandOffsetX, drawableCommandOffsetY, button, rectangle{}\n");
			fprintf(config, "# rectangle object: x, y, width, height, border, borderColor, backgroundColor\n");
			fprintf(config, "# math macros: ParentWidth, ParentHeight\n\n\n\n");
			fprintf(config, "# # # # # # # # # # # #\n");
			fprintf(config, "# variable definition #\n");
			fprintf(config, "# # # # # # # # # # # #\n\n");
			fprintf(config, "# lines: config lines to be read\n");
			fprintf(config, "# monitor: monitor to be used\n");
			fprintf(config, "# x: x axis Variable of object\n");
			fprintf(config, "# y: y axis Variable of object\n");
			fprintf(config, "# width: size of object\'s width, excluding border\n");
			fprintf(config, "# height: size of object\'s height, excluding border\n");
			fprintf(config, "# border: size of object\'s border\n");
			fprintf(config, "# borderColor: color of object\'s border\n");
			fprintf(config, "# backgroundColor: color of object\'s background\n");
			fprintf(config, "# globalSectionBorderColor: color of all sections\' border\n");
			fprintf(config, "# globalSectionBackgroundColor: color of all sections\' background\n");
			fprintf(config, "# globalContainerBorderColor: color of all containers\' border\n");
			fprintf(config, "# globalContainerBackgroundColor: color of all containers\' background\n");
			fprintf(config, "# globalRectangleBorderColor: color of all rectangles\' border\n");
			fprintf(config, "# globalRectangleBackgroundColor: color of all rectangles\' background\n");
			fprintf(config, "# globalTextColor: color of all containers\' text\n");
			fprintf(config, "# globalDrawableCommandColor: color of all containers\' drawableCommand\n");
			fprintf(config, "# font: font to form part of a font set, multiple fonts allowed and encouraged\n");
			fprintf(config, "# text: text label of container\n");
			fprintf(config, "# textColor: color of container\'s text\n");
			fprintf(config, "# textOffsetX: x axis offset of container\'s text\n");
			fprintf(config, "# textOffsetY: y axis offset of container\'s text\n");
			fprintf(config, "# command: command executed on interaction with container\n");
			fprintf(config, "# drawableCommand: command returning text output executed on interaction with container\n");
			fprintf(config, "# drawableCommandColor: color of container\'s drawableCommand\n");
			fprintf(config, "# drawableCommandOffsetX: x axis offset of container\'s drawableCommand\n");
			fprintf(config, "# drawableCommandOffsetY: y axis offset of container\'s drawableCommand\n");
			fprintf(config, "# keycode: combination of keycode + modifiers used to execute command\n");
			fprintf(config, "# button: combination of mouse button + modifiers used to interact\n");
			fprintf(config, "# section: informationless interactionless object, residing in global object\n");
			fprintf(config, "# container: information object, residing in section object\n");
			fprintf(config, "# rectangle: informationless interactionless object, residing in container object\n");
			fprintf(config, "# ParentWidth: size of parent object\'s width, if applicable\n");
			fprintf(config, "# ParentHeight: size of parent object\'s height, if applicable\n\n\n\n");
			fprintf(config, "# # # # #\n");
			fprintf(config, "# extra #\n");
			fprintf(config, "# # # # #\n\n");
			fprintf(config, "# lines: default %u\n", DefaultLinesCount);
			fprintf(config, "# monitor: all, n\n");
			fprintf(config, "# font: the xfontsel application is recommended for looking at different available fonts\n");
			fprintf(config, "# font: requires quotation, there is a font hierarchy from first to last specified\n");
			fprintf(config, "# keycode: does not take \'=\'\n");
			fprintf(config, "# keycode: modifiers: AnyModifier, Shift, Lock, Control, Mod1, Mod2, Mod3, Mod4, Mod5\n");
			fprintf(config, "# keycode: program commands: interact, hide, peek, restart, exit\n");
			fprintf(config, "# keycode: interact: all, section n, container n\n");
			fprintf(config, "# text: requires quotation\n");
			fprintf(config, "# command: requires quotation, program commands: hide, peek, restart, exit\n");
			fprintf(config, "# drawableCommand: requires quotation\n");
			fprintf(config, "# button: does not take \'=\'\n");
			fprintf(config, "# button: buttons: Button1 = left click, Button2 = middle click, Button3 = right click, Button4 = wheel up, Button5 = wheel down\n");
			fprintf(config, "# button: modifiers: AnyModifier, Shift, Lock, Control, Mod1, Mod2, Mod3, Mod4, Mod5\n");
			fprintf(config, "# math operands: +, -, *, /, (, )\n\n\n\n");
			fprintf(config, "# /config start # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
			fprintf(config, "lines = 191\n");
			fprintf(config, "monitor = all\n");
			fprintf(config, "x = 0\n");
			fprintf(config, "y = ParentHeight - 19\n");
			fprintf(config, "width = ParentWidth\n");
			fprintf(config, "height = 19\n");
			fprintf(config, "border = 0\n");
			fprintf(config, "borderColor = #00000000\n");
			fprintf(config, "backgroundColor = #FF000000\n");
			fprintf(config, "globalSectionBorderColor = #00000000\n");
			fprintf(config, "globalSectionBackgroundColor = #FF000000\n");
			fprintf(config, "font = \"-misc-fixed-medium-r-normal-*-13-120-75-75-C-70-iso10646-1\"\n");
			fprintf(config, "font = \"-*-*-*-*-*-*-*-*-*-*-*-*-gb2312.1980-*\"\n");
			fprintf(config, "keycode 43 + Mod4 hide\n");
			fprintf(config, "section{\n");
			fprintf(config, "	x = 0\n");
			fprintf(config, "	y = 0\n");
			fprintf(config, "	width = 58\n");
			fprintf(config, "	height = ParentHeight\n");
			fprintf(config, "	border = 0\n");
			fprintf(config, "	container{\n");
			fprintf(config, "		x = 0\n");
			fprintf(config, "		y = 0\n");
			fprintf(config, "		width = 56\n");
			fprintf(config, "		height = ParentHeight - 2\n");
			fprintf(config, "		border = 1\n");
			fprintf(config, "		borderColor = #FF1F1F1F\n");
			fprintf(config, "		backgroundColor = #FF000000\n");
			fprintf(config, "		text = \"restart\"\n");
			fprintf(config, "		textColor = #FF00FF00\n");
			fprintf(config, "		textOffsetX = 1\n");
			fprintf(config, "		command = \"restart\"\n");
			fprintf(config, "		button Button1\n");
			fprintf(config, "	}\n");
			fprintf(config, "}\n");
			fprintf(config, "section{\n");
			fprintf(config, "	x = ParentWidth - 224\n");
			fprintf(config, "	y = 0\n");
			fprintf(config, "	width = 224\n");
			fprintf(config, "	height = ParentHeight\n");
			fprintf(config, "	border = 0\n");
			fprintf(config, "	globalContainerBorderColor = #FF1F1F1F\n");
			fprintf(config, "	globalContainerBackgroundColor = #FF000000\n");
			fprintf(config, "	globalTextColor = #FFFFFFFF\n");
			fprintf(config, "	globalDrawableCommandColor = #FFFFFFFF\n");
			fprintf(config, "	container{\n");
			fprintf(config, "		x = ParentWidth - 224\n");
			fprintf(config, "		y = 0\n");
			fprintf(config, "		width = 84\n");
			fprintf(config, "		height = ParentHeight - 2\n");
			fprintf(config, "		border = 1\n");
			fprintf(config, "		text = \"date\"\n");
			fprintf(config, "		drawableCommand = \"date \'+D%%d/M%%m/Y%%y\'\"\n");
			fprintf(config, "		drawableCommandOffsetX = 1\n");
			fprintf(config, "		button Button1\n");
			fprintf(config, "	}\n");
			fprintf(config, "	container{\n");
			fprintf(config, "		x = ParentWidth - 137\n");
			fprintf(config, "		y = 0\n");
			fprintf(config, "		width = 97\n");
			fprintf(config, "		height = ParentHeight - 2\n");
			fprintf(config, "		border = 1\n");
			fprintf(config, "		text = \"time\"\n");
			fprintf(config, "		textOffsetX = 1\n");
			fprintf(config, "		drawableCommand = \"date \'+%%H:%%M:%%S %%Z\'\"\n");
			fprintf(config, "		button Button1\n");
			fprintf(config, "	}\n");
			fprintf(config, "	container{\n");
			fprintf(config, "		x = ParentWidth - 37\n");
			fprintf(config, "		y = 0\n");
			fprintf(config, "		width = 35\n");
			fprintf(config, "		height = ParentHeight - 2\n");
			fprintf(config, "		border = 1\n");
			fprintf(config, "		text = \"exit\"\n");
			fprintf(config, "		textColor = #FFFF0000\n");
			fprintf(config, "		textOffsetX = 1\n");
			fprintf(config, "		command = \"exit\"\n");
			fprintf(config, "		button Button1\n");
			fprintf(config, "	}\n");
			fprintf(config, "}\n");
			fprintf(config, "# /config end # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
			fclose(config);
			config = fopen(configPath, "r");
		}else{
			fprintf(stderr, "%s: could not create config file\n", programName);
		}
	}
	return config;
}
static bool getLine(FILE *const file){
	bool value = 0;
	unsigned int element = 0;
	for(;;){
		line[element] = fgetc(file);
		if(line[element] == '\n' || feof(file)){
			line[element] = '\0';
			break;
		}else if(element < DefaultCharactersCount){
			++element;
		}
	}
	if(!feof(file)){
		value = 1;
	}
	return value;
}
static void pushWhitespace(unsigned int *const element){
	unsigned int e = *element;
	char l = line[e];
	while(l && (l == ' ' || l == '\t')){
		l = line[++e];
	}
	*element = e;
	return;
}
static bool isVariable(const char *const variable, unsigned int *const element){
	bool value = 0;
	unsigned int e = *element;
	unsigned int currentCharacter = 0;
	char l = line[e];
	char v = *variable;
	while(l && v){
		if(l >= 'A' && l <= 'Z'){
			if(l != v && l != v - 32){
				currentCharacter = 0;
				break;
			}
		}else if(l >= 'a' && l <= 'z'){
			if(l != v && l != v + 32){
				currentCharacter = 0;
				break;
			}
		}else if(l != v){
			currentCharacter = 0;
			break;
		}
		l = line[++e];
		v = variable[++currentCharacter];
	}
	if(currentCharacter){
		*element = e;
		value = 1;
	}
	return value;
}
static unsigned int getUnsignedInteger(const unsigned int currentLine, const Window parentWindow, unsigned int *const element){
	unsigned int number = getInteger(parentWindow, element);
	if((int)number < 0){
		fprintf(stderr, "%s: line %u: %i is not an unsigned integer\n", programName, currentLine, (int)number);
		number = 0;
	}
	return number;
}
static int getInteger(Window parentWindow, unsigned int *const element){
	int number = 0;
	unsigned int e = *element;
	int numberRead = 0;
	int numberOperatedOn = 0;
	MathOperation operation = NoMathOperation;
	MathOperation lastOperation = NoMathOperation;
	XWindowAttributes windowAttributes;
	if(parentWindow){
		if(parentWindow == XDefaultRootWindow(display)){
			unsigned int trueMonitorAmount;
			XRRMonitorInfo *const monitorInfo = XRRGetMonitors(display, XDefaultRootWindow(display), True, (int *)&trueMonitorAmount);
			if(monitorInfo){
				if(whichMonitor == trueMonitorAmount){
					windowAttributes.width = monitorInfo[currentMonitor].width;
					windowAttributes.height = monitorInfo[currentMonitor].height;
				}else{
					windowAttributes.width = monitorInfo[whichMonitor].width;
					windowAttributes.height = monitorInfo[whichMonitor].height;
				}
				XRRFreeMonitors(monitorInfo);
			}else{
				parentWindow = None;
			}
		}else{
			XGetWindowAttributes(display, parentWindow, &windowAttributes);
		}
	}
	while(line[e]){
		pushWhitespace(&e);
		if(line[e] >= '0' && line[e] <= '9'){
			numberRead *= 10;
			numberRead += line[e];
			numberRead -= 48;
			++e;
		}else if(isVariable("ParentWidth", &e)){
			if(parentWindow){
				numberRead = windowAttributes.width;
			}
		}else if(isVariable("ParentHeight", &e)){
			if(parentWindow){
				numberRead = windowAttributes.height;
			}
		}else if(line[e] == '('){
			++e;
			numberRead = getInteger(parentWindow, &e);
		}else if(line[e] == ')'){
			++e;
			break;
		}else if(line[e] == '+' || line[e] == '-' || line[e] == '*' || line[e] == '/'){
			if(number == 0 && numberRead == 0){
				if(line[e] == '/'){
					break;
				}
			}
			if(operation == AdditionMathOperation){
				if(line[e] != '*' && line[e] != '/'){
					if(numberOperatedOn == 0){
						number += numberRead;
					}else{
						if(lastOperation == AdditionMathOperation){
							number += numberOperatedOn;
						}else if(lastOperation == SubtractionMathOperation){
							number -= numberOperatedOn;
						}
					}
				}else{
					numberOperatedOn = numberRead;
					lastOperation = operation;
				}
			}else if(operation == SubtractionMathOperation){
				if(line[e] != '*' && line[e] != '/'){
					if(numberOperatedOn == 0){
						number -= numberRead;
					}else{
						if(lastOperation == AdditionMathOperation){
							number += numberOperatedOn;
						}else if(lastOperation == SubtractionMathOperation){
							number -= numberOperatedOn;
						}
					}
				}else{
					numberOperatedOn = numberRead;
					lastOperation = operation;
				}
			}else if(operation == MultiplicationMathOperation){
				if(numberOperatedOn == 0){
					number *= numberRead;
				}else{
					numberOperatedOn *= numberRead;
				}
				if(line[e] == '+' || line[e] == '-'){
					if(lastOperation == AdditionMathOperation){
						number += numberOperatedOn;
					}else if(lastOperation == SubtractionMathOperation){
						number -= numberOperatedOn;
					}
					numberOperatedOn = 0;
				}
			}else if(operation == DivisionMathOperation){
				if(numberOperatedOn == 0){
					number /= numberRead;
				}else{
					numberOperatedOn /= numberRead;
				}
				if(line[e] == '+' || line[e] == '-'){
					if(lastOperation == AdditionMathOperation){
						number += numberOperatedOn;
					}else if(lastOperation == SubtractionMathOperation){
						number -= numberOperatedOn;
					}
					numberOperatedOn = 0;
				}
			}else{
				if(number == 0){
					number = numberRead;
				}
			}
			if(line[e] == '+'){
				operation = AdditionMathOperation;
			}else if(line[e] == '-'){
				operation = SubtractionMathOperation;
			}else if(line[e] == '*'){
				operation = MultiplicationMathOperation;
			}else if(line[e] == '/'){
				operation = DivisionMathOperation;
			}
			numberRead = 0;
			++e;
		}else{
			break;
		}
	}
	if(operation == AdditionMathOperation){
		if(numberOperatedOn > 0){
			if(lastOperation == AdditionMathOperation){
				number += numberOperatedOn;
			}else if(lastOperation == SubtractionMathOperation){
				number -= numberOperatedOn;
			}
		}
		number += numberRead;
	}else if(operation == SubtractionMathOperation){
		if(numberOperatedOn > 0){
			if(lastOperation == AdditionMathOperation){
				number += numberOperatedOn;
			}else if(lastOperation == SubtractionMathOperation){
				number -= numberOperatedOn;
			}
		}
		number -= numberRead;
	}else if(operation == MultiplicationMathOperation){
		if(numberOperatedOn == 0){
			number *= numberRead;
		}else{
			numberOperatedOn *= numberRead;
			if(lastOperation == AdditionMathOperation){
				number += numberOperatedOn;
			}else if(lastOperation == SubtractionMathOperation){
				number -= numberOperatedOn;
			}
		}
	}else if(operation == DivisionMathOperation){
		if(number > 0 || numberOperatedOn > 0){
			if(numberOperatedOn == 0){
				number /= numberRead;
			}else{
				numberOperatedOn /= numberRead;
				if(lastOperation == AdditionMathOperation){
					number += numberOperatedOn;
				}else if(lastOperation == SubtractionMathOperation){
					number -= numberOperatedOn;
				}
			}
		}
	}else{
		if(number == 0){
			number = numberRead;
		}
	}
	*element = e;
	return number;
}
static ARGB getARGB(unsigned int *const element){
	ARGB color = 0x00000000;
	unsigned int e = *element;
	if(line[e] == '#'){
		++e;
	}
	uint8_t charactersRead = 0;
	char l = line[e];
	while(l && charactersRead < 8){
		color *= 16;
		color += l;
		if(l >= '0' && l <= '9'){
			color -= 48;
		}else if(l >= 'A' && l <= 'F'){
			color -= 55;
		}else if(l >= 'a' && l <= 'f'){
			color -= 87;
		}else{
			color -= l;
			if(l != ' ' && l != '\t'){
				fprintf(stderr, "%s: \'%c\' is not recognized as a hexadecimal number\n", programName, l);
				color = 0x00000000;
			}
			break;
		}
		++e;
		++charactersRead;
		l = line[e];
	}
	if(charactersRead == 8){
		*element = e;
	}
	return color;
}
static unsigned int getQuotedStringLength(unsigned int *const element){
	unsigned int length = 0;
	unsigned int e = *element;
	const char quotation = line[e];
	++e;
	while(line[e] != quotation && line[e]){
		++length;
		++e;
	}
	*element = e;
	return length;
}
static unsigned int getQuotedString(char *const string, unsigned int *const element){
	unsigned int currentCharacter = 0;
	unsigned int e = *element;
	const char quotation = line[e];
	++e;
	while(line[e] != quotation && line[e]){
		string[currentCharacter] = line[e];
		++currentCharacter;
		++e;
	}
	*element = e;
	return currentCharacter;
}
static Shortcut getShortcut(unsigned int *const element){
	Shortcut shortcut = {
		.keycode = AnyKey,
		.masks = None
	};
	unsigned int e = *element;
	bool lookingForValue = 1;
	while(line[e]){
		pushWhitespace(&e);
		if(lookingForValue){
			if(line[e] >= '0' && line[e] <= '9'){
				do{
					shortcut.keycode *= 10;
					shortcut.keycode += line[e];
					shortcut.keycode -= 48;
					++e;
				}while(line[e] >= '0' && line[e] <= '9');
			}else if(isVariable("AnyModifier", &e)){
				shortcut.masks |= AnyModifier;
			}else if(isVariable("Shift", &e)){
				shortcut.masks |= ShiftMask;
			}else if(isVariable("Lock", &e)){
				shortcut.masks |= LockMask;
			}else if(isVariable("Control", &e)){
				shortcut.masks |= ControlMask;
			}else if(isVariable("Mod1", &e)){
				shortcut.masks |= Mod1Mask;
			}else if(isVariable("Mod2", &e)){
				shortcut.masks |= Mod2Mask;
		 	}else if(isVariable("Mod3", &e)){
				shortcut.masks |= Mod3Mask;
			}else if(isVariable("Mod4", &e)){
				shortcut.masks |= Mod4Mask;
			}else if(isVariable("Mod5", &e)){
				shortcut.masks |= Mod5Mask;
			}else{
				break;
			}
			lookingForValue = 0;
		}else{
			if(line[e] == '+'){
				++e;
				lookingForValue = 1;
			}else{
				break;
			}
		}
	}
	if(shortcut.keycode != AnyKey){
		*element = e;
	}
	return shortcut;
}
static Button getButton(unsigned int *const element){
	Button button = {
		.button = AnyButton,
		.masks = None
	};
	unsigned int e = *element;
	bool lookingForValue = 1;
	while(line[e]){
		pushWhitespace(&e);
		if(lookingForValue){
			if(isVariable("Button1", &e)){
				button.button = Button1;
			}else if(isVariable("Button2", &e)){
				button.button = Button2;
			}else if(isVariable("Button3", &e)){
				button.button = Button3;
			}else if(isVariable("Button4", &e)){
				button.button = Button4;
			}else if(isVariable("Button5", &e)){
				button.button = Button5;
			}else if(isVariable("AnyModifier", &e)){
				button.masks |= AnyModifier;
			}else if(isVariable("Shift", &e)){
				button.masks |= ShiftMask;
			}else if(isVariable("Lock", &e)){
				button.masks |= LockMask;
			}else if(isVariable("Control", &e)){
				button.masks |= ControlMask;
			}else if(isVariable("Mod1", &e)){
				button.masks |= Mod1Mask;
			}else if(isVariable("Mod2", &e)){
				button.masks |= Mod2Mask;
			}else if(isVariable("Mod3", &e)){
				button.masks |= Mod3Mask;
			}else if(isVariable("Mod4", &e)){
				button.masks |= Mod4Mask;
			}else if(isVariable("Mod5", &e)){
				button.masks |= Mod5Mask;
			}else{
				break;
			}
			lookingForValue = 0;
		}else{
			if(line[e] == '+'){
				++e;
				lookingForValue = 1;
			}else{
				break;
			}
		}
	}
	if(button.button != AnyButton){
		*element = e;
	}
	return button;
}
static void printLineError(const unsigned int currentLine){
	unsigned int element = 0;
	fprintf(stderr, "%s: line %u: \"", programName, currentLine);
	while(line[element]){
		fprintf(stderr, "%c", line[element]);
		++element;
	}
	fprintf(stderr, "\" not recognized as an internal variable\n");
	return;
}
