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

bool readConfigScan(void){
	bool value = 0;
	whichMonitor = monitorAmount;
	containerAmount = 0;
	FILE *const file = getConfigFile();
	if(file){
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
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= SectionVariable;
						}
						continue;
					}
					if(line[element]){
						if(!isVariable("x",                            &element) &&
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
				}else if(!(hasReadVariable & ContainerVariable)){
					if(isVariable("container", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							++containerAmount;
							hasReadVariable |= ContainerVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= SectionVariable;
						continue;
					}
					if(line[element]){
						if(!isVariable("x",                              &element) &&
						   !isVariable("y",                              &element) &&
						   !isVariable("width",                          &element) &&
						   !isVariable("height",                         &element) &&
						   !isVariable("border",                         &element) &&
						   !isVariable("borderColor",                    &element) &&
						   !isVariable("backgroundColor",                &element) &&
						   !isVariable("globalContainerBackgroundColor", &element) &&
						   !isVariable("globalContainerBorderColor",     &element) &&
						   !isVariable("globalTextColor",                &element) &&
						   !isVariable("globalDrawableCommandColor",     &element)){
							printLineError(currentLine);
							continue;
						}
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
					if(line[element]){
						if(!isVariable("x",                              &element) &&
						   !isVariable("y",                              &element) &&
						   !isVariable("width",                          &element) &&
						   !isVariable("height",                         &element) &&
						   !isVariable("border",                         &element) &&
						   !isVariable("borderColor",                    &element) &&
						   !isVariable("backgroundColor",                &element) &&
						   !isVariable("globalRectangleBackgroundColor", &element) &&
						   !isVariable("globalRectangleBorderColor",     &element) &&
						   !isVariable("text",                           &element) &&
						   !isVariable("textColor",                      &element) &&
						   !isVariable("textOffsetXVariable",            &element) &&
						   !isVariable("textOffsetYVariable",            &element) &&
						   !isVariable("command",                        &element) &&
						   !isVariable("drawableCommand",                &element) &&
						   !isVariable("drawableCommandColor",           &element) &&
						   !isVariable("drawableCommandOffsetXVariable", &element) &&
						   !isVariable("drawableCommandOffsetYVariable", &element) &&
						   !isVariable("button",                         &element)){
							printLineError(currentLine);
							continue;
						}
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= RectangleVariable;
						continue;
					}
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
		fclose(file);
		value = 1;
	}
	return value;
}
bool readConfigTopLevelWindow(const Window parentWindow, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, ARGB *const borderColor, ARGB *const backgroundColor){
	bool value = 0;
	for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
		x[currentMonitor] = 0;
		y[currentMonitor] = 0;
		width[currentMonitor] = 0;
		height[currentMonitor] = 0;
		border[currentMonitor] = 0;
		borderColor[currentMonitor] = 0x00000000;
		backgroundColor[currentMonitor] = 0x00000000;
	}
	FILE *const file = getConfigFile();
	if(file){
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		VariableList hasReadVariable = NoVariables;
		unsigned int elementCopy;
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
					if(!(hasReadVariable & XVariable)){
						if(isVariable("x", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									x[currentMonitor] = getInteger(parentWindow, &element);
									element = elementCopy;
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									y[currentMonitor] = getInteger(parentWindow, &element);
									element = elementCopy;
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									width[currentMonitor] = getUnsignedInteger(currentLine, parentWindow, &element);
									element = elementCopy;
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									height[currentMonitor] = getUnsignedInteger(currentLine, parentWindow, &element);
									element = elementCopy;
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									borderColor[currentMonitor] = getARGB(&element);
									element = elementCopy;
								}
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									border[currentMonitor] = getUnsignedInteger(currentLine, parentWindow, &element);
									element = elementCopy;
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									backgroundColor[currentMonitor] = getARGB(&element);
									element = elementCopy;
								}
								hasReadVariable |= BackgroundColorVariable;
							}
							continue;
						}
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
bool readConfigSectionRectangleAmount(unsigned int *const sectionAmount, unsigned int *const rectangleAmount){
	bool value = 0;
	*sectionAmount = 0;
	*rectangleAmount = 0;
	FILE *const file = getConfigFile();
	if(file){
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
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							++(*sectionAmount);
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
							++(*rectangleAmount);
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
bool readConfigGlobalColors(const unsigned int sectionAmount, ARGB *const globalSectionBorderColor, ARGB *const globalSectionBackgroundColor, ARGB *const globalContainerBorderColor, ARGB *const globalContainerBackgroundColor, ARGB *const globalRectangleBorderColor, ARGB *const globalRectangleBackgroundColor){
	bool value = 0;
	*globalSectionBorderColor = 0x00000000;
	*globalSectionBackgroundColor = 0x00000000;
	unsigned int currentSection;
	unsigned int currentContainer;
	for(currentSection = 0; currentSection < sectionAmount; ++currentSection){
		globalContainerBorderColor[currentSection] = 0x00000000;
		globalContainerBackgroundColor[currentSection] = 0x00000000;
	}
	for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
		globalRectangleBorderColor[currentContainer] = 0x00000000;
		globalRectangleBackgroundColor[currentContainer] = 0x00000000;
	}
	FILE *const file = getConfigFile();
	if(file){
		currentSection = 0;
		currentContainer = 0;
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
					if(!(hasReadVariable & GlobalSectionBorderColorVariable)){
						if(isVariable("globalSectionBorderColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*globalSectionBorderColor = getARGB(&element);
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
								*globalSectionBackgroundColor = getARGB(&element);
								hasReadVariable |= GlobalSectionBackgroundColorVariable;
							}
							continue;
						}
					}
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= SectionVariable;
						}
						continue;
					}
				}else if(!(hasReadVariable & ContainerVariable)){
					if(!(hasReadVariable & GlobalContainerBorderColorVariable)){
						if(isVariable("globalContainerBorderColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								globalContainerBorderColor[currentSection] = getARGB(&element);
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
								globalContainerBackgroundColor[currentSection] = getARGB(&element);
								hasReadVariable |= GlobalContainerBackgroundColorVariable;
							}
							continue;
						}
					}
					if(isVariable("container", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= ContainerVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
						if(hasReadVariable & GlobalContainerBorderColorVariable){
							hasReadVariable ^= GlobalContainerBorderColorVariable;
						}
						if(hasReadVariable & GlobalContainerBackgroundColorVariable){
							hasReadVariable ^= GlobalContainerBackgroundColorVariable;
						}
						++currentSection;
						hasReadVariable ^= SectionVariable;
						continue;
					}
				}else if(!(hasReadVariable & RectangleVariable)){
					if(!(hasReadVariable & GlobalRectangleBorderColorVariable)){
						if(isVariable("globalRectangleBorderColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								globalRectangleBorderColor[currentContainer] = getARGB(&element);
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
								globalRectangleBackgroundColor[currentContainer] = getARGB(&element);
								hasReadVariable |= GlobalRectangleBackgroundColorVariable;
							}
							continue;
						}
					}
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectangleVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
						if(hasReadVariable & GlobalRectangleBorderColorVariable){
							hasReadVariable ^= GlobalRectangleBorderColorVariable;
						}
						if(hasReadVariable & GlobalRectangleBackgroundColorVariable){
							hasReadVariable ^= GlobalRectangleBackgroundColorVariable;
						}
						++currentContainer;
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
bool readConfigSectionWindows(const Window *const parentWindow, const unsigned int sectionAmount, int *const *const x, int *const *const y, unsigned int *const *const width, unsigned int *const *const height, unsigned int *const *const border, ARGB *const borderColor, ARGB *const backgroundColor){
	bool value = 0;
	unsigned int currentSection;
	for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
		for(currentSection = 0; currentSection < sectionAmount; ++currentSection){
			x[currentMonitor][currentSection] = 0;
			y[currentMonitor][currentSection] = 0;
			width[currentMonitor][currentSection] = 0;
			height[currentMonitor][currentSection] = 0;
			border[currentMonitor][currentSection] = 0;
		}
	}
	for(currentSection = 0; currentSection < sectionAmount; ++currentSection){
		borderColor[currentSection] = 0x00000000;
		backgroundColor[currentSection] = 0x00000000;
	}
	FILE *const file = getConfigFile();
	if(file){
		currentSection = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		VariableList hasReadVariable = NoVariables;
		unsigned int elementCopy;
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
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= SectionVariable;
						}
						continue;
					}
				}else if(!(hasReadVariable & ContainerVariable)){
					if(!(hasReadVariable & XVariable)){
						if(isVariable("x", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									x[currentMonitor][currentSection] = getInteger(parentWindow[currentMonitor], &element);
									element = elementCopy;
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									y[currentMonitor][currentSection] = getInteger(parentWindow[currentMonitor], &element);
									element = elementCopy;
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									width[currentMonitor][currentSection] = getUnsignedInteger(currentLine, parentWindow[currentMonitor], &element);
									element = elementCopy;
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									height[currentMonitor][currentSection] = getUnsignedInteger(currentLine, parentWindow[currentMonitor], &element);
									element = elementCopy;
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
								borderColor[currentSection] = getARGB(&element);
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									border[currentMonitor][currentSection] = getUnsignedInteger(currentLine, parentWindow[currentMonitor], &element);
									element = elementCopy;
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
								backgroundColor[currentSection] = getARGB(&element);
								hasReadVariable |= BackgroundColorVariable;
							}
							continue;
						}
					}
					if(isVariable("container", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= ContainerVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
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
						++currentSection;
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
bool readConfigSectionChildren(const unsigned int sectionAmount, unsigned int *const sectionChildrenAmount){
	bool value = 0;
	unsigned int currentSection;
	for(currentSection = 0; currentSection < sectionAmount; ++currentSection){
		sectionChildrenAmount[currentSection] = 0;
	}
	FILE *const file = getConfigFile();
	if(file){
		currentSection = 0;
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
							++sectionChildrenAmount[currentSection];
							hasReadVariable |= ContainerVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
						++currentSection;
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
bool readConfigContainerWindows(Window *const *const parentWindow, int *const *const x, int *const *const y, unsigned int *const *const width, unsigned int *const *const height, unsigned int *const *const border, ARGB *const borderColor, ARGB *const backgroundColor){
	bool value = 0;
	unsigned int currentContainer;
	for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
		for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
			x[currentMonitor][currentContainer] = 0;
			y[currentMonitor][currentContainer] = 0;
			width[currentMonitor][currentContainer] = 0;
			height[currentMonitor][currentContainer] = 0;
			border[currentMonitor][currentContainer] = 0;
		}
	}
	for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
		borderColor[currentContainer] = 0x00000000;
		backgroundColor[currentContainer] = 0x00000000;
	}
	FILE *const file = getConfigFile();
	if(file){
		currentContainer = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		VariableList hasReadVariable = NoVariables;
		unsigned int currentSection = 0;
		unsigned int elementCopy;
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
						++currentSection;
						hasReadVariable ^= SectionVariable;
						continue;
					}
				}else if(!(hasReadVariable & RectangleVariable)){
					if(!(hasReadVariable & XVariable)){
						if(isVariable("x", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									x[currentMonitor][currentContainer] = getInteger(parentWindow[currentMonitor][currentSection], &element);
									element = elementCopy;
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									y[currentMonitor][currentContainer] = getInteger(parentWindow[currentMonitor][currentSection], &element);
									element = elementCopy;
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									width[currentMonitor][currentContainer] = getUnsignedInteger(currentLine, parentWindow[currentMonitor][currentSection], &element);
									element = elementCopy;
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									height[currentMonitor][currentContainer] = getUnsignedInteger(currentLine, parentWindow[currentMonitor][currentSection], &element);
									element = elementCopy;
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
								borderColor[currentContainer] = getARGB(&element);
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									border[currentMonitor][currentContainer] = getUnsignedInteger(currentLine, parentWindow[currentMonitor][currentSection], &element);
									element = elementCopy;
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
								backgroundColor[currentContainer] = getARGB(&element);
								hasReadVariable |= BackgroundColorVariable;
							}
							continue;
						}
					}
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectangleVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
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
						++currentContainer;
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
bool readConfigContainerChildren(unsigned int *const containerChildrenAmount){
	bool value = 0;
	unsigned int currentContainer;
	for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
		containerChildrenAmount[currentContainer] = 0;
	}
	FILE *const file = getConfigFile();
	if(file){
		currentContainer = 0;
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
							++containerChildrenAmount[currentContainer];
							hasReadVariable |= RectangleVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
						++currentContainer;
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
bool readConfigRectangleWindows(Window *const *const parentWindow, const unsigned int rectangleAmount, int *const *const x, int *const *const y, unsigned int *const *const width, unsigned int *const *const height, unsigned int *const *const border, ARGB *const borderColor, ARGB *const backgroundColor){
	bool value = 0;
	unsigned int currentRectangle;
	for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
		for(currentRectangle = 0; currentRectangle < rectangleAmount; ++currentRectangle){
			x[currentMonitor][currentRectangle] = 0;
			y[currentMonitor][currentRectangle] = 0;
			width[currentMonitor][currentRectangle] = 0;
			height[currentMonitor][currentRectangle] = 0;
			border[currentMonitor][currentRectangle] = 0;
		}
	}
	for(currentRectangle = 0; currentRectangle < rectangleAmount; ++currentRectangle){
		borderColor[currentRectangle] = 0x00000000;
		backgroundColor[currentRectangle] = 0x00000000;
	}
	FILE *const file = getConfigFile();
	if(file){
		currentRectangle = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		VariableList hasReadVariable = NoVariables;
		unsigned int currentContainer = 0;
		unsigned int elementCopy;
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
						++currentContainer;
						hasReadVariable ^= ContainerVariable;
						continue;
					}
				}else{
					if(!(hasReadVariable & XVariable)){
						if(isVariable("x", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									x[currentMonitor][currentRectangle] = getInteger(parentWindow[currentMonitor][currentContainer], &element);
									element = elementCopy;
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									y[currentMonitor][currentRectangle] = getInteger(parentWindow[currentMonitor][currentContainer], &element);
									element = elementCopy;
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									width[currentMonitor][currentRectangle] = getUnsignedInteger(currentLine, parentWindow[currentMonitor][currentContainer], &element);
									element = elementCopy;
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									height[currentMonitor][currentRectangle] = getUnsignedInteger(currentLine, parentWindow[currentMonitor][currentContainer], &element);
									element = elementCopy;
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
								borderColor[currentRectangle] = getARGB(&element);
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
								elementCopy = element;
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									border[currentMonitor][currentRectangle] = getUnsignedInteger(currentLine, parentWindow[currentMonitor][currentContainer], &element);
									element = elementCopy;
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
								backgroundColor[currentRectangle] = getARGB(&element);
								hasReadVariable |= BackgroundColorVariable;
							}
							continue;
						}
					}
					if(isVariable("}", &element)){
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
						++currentRectangle;
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
bool readConfigArrayLengths(unsigned int *const textMaxWordLength, unsigned int *const commandMaxWordLength, unsigned int *const drawableCommandMaxWordLength){
	bool value = 0;
	*textMaxWordLength = 0;
	*commandMaxWordLength = 0;
	*drawableCommandMaxWordLength = 0;
	FILE *const file = getConfigFile();
	if(file){
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
					if(!(hasReadVariable & TextVariable)){
						if(isVariable("text", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								unsigned int length = getQuotedStringLength(&element);
								if(length > *textMaxWordLength){
									*textMaxWordLength = length;
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
								if(length > *commandMaxWordLength){
									*commandMaxWordLength = length;
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
								if(length > *drawableCommandMaxWordLength){
									*drawableCommandMaxWordLength = length;
								}
								hasReadVariable |= DrawableCommandVariable;
							}
							continue;
						}
					}
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectangleVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
						if(hasReadVariable & TextVariable){
							hasReadVariable ^= TextVariable;
						}
						if(hasReadVariable & CommandVariable){
							hasReadVariable ^= CommandVariable;
						}
						if(hasReadVariable & DrawableCommandVariable){
							hasReadVariable ^= DrawableCommandVariable;
						}
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
bool readConfigFillArrays(char *const *const text, ARGB *const textColor, char *const *const command, char *const *const drawableCommand, ARGB *const drawableCommandColor){
	bool value = 0;
	unsigned int currentContainer;
	for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
		*text[currentContainer] = '\0';
		textColor[currentContainer] = 0x00000000;
		*command[currentContainer] = '\0';
		*drawableCommand[currentContainer] = '\0';
		drawableCommandColor[currentContainer] = 0x00000000;
	}
	FILE *const file = getConfigFile();
	if(file){
		currentContainer = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		VariableList hasReadVariable = NoVariables;
		ARGB globalTextColor = 0x00000000;
		ARGB globalDrawableCommandColor = 0x00000000;
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
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= SectionVariable;
						}
						continue;
					}
				}else if(!(hasReadVariable & ContainerVariable)){
					if(!(hasReadVariable & GlobalTextColorVariable)){
						if(isVariable("globalTextColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								globalTextColor = getARGB(&element);
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
								globalDrawableCommandColor = getARGB(&element);
								hasReadVariable |= GlobalDrawableCommandColorVariable;
							}
							continue;
						}
					}
					if(isVariable("container", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= ContainerVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
						if(hasReadVariable & GlobalTextColorVariable){
							hasReadVariable ^= GlobalTextColorVariable;
						}
						if(hasReadVariable & GlobalDrawableCommandColorVariable){
							hasReadVariable ^= GlobalDrawableCommandColorVariable;
						}
						hasReadVariable ^= SectionVariable;
						continue;
					}
				}else if(!(hasReadVariable & RectangleVariable)){
					if(!(hasReadVariable & TextVariable)){
						if(isVariable("text", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								text[currentContainer][getQuotedString(text[currentContainer], &element)] = '\0';
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
								textColor[currentContainer] = getARGB(&element);
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
								command[currentContainer][getQuotedString(command[currentContainer], &element)] = '\0';
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
								drawableCommand[currentContainer][getQuotedString(drawableCommand[currentContainer], &element)] = '\0';
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
								drawableCommandColor[currentContainer] = getARGB(&element);
								hasReadVariable |= DrawableCommandColorVariable;
							}
							continue;
						}
					}
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectangleVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
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
						++currentContainer;
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
		for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
			if(!textColor[currentContainer]){
				textColor[currentContainer] = globalTextColor;
			}
			if(!drawableCommandColor[currentContainer]){
				drawableCommandColor[currentContainer] = globalDrawableCommandColor;
			}
		}
		value = 1;
	}
	return value;
}
bool readConfigVariableShortcuts(unsigned int *const sectionShortcutAmount, unsigned int *const containerShortcutAmount){
	bool value = 0;
	*sectionShortcutAmount = 0;
	*containerShortcutAmount = 0;
	FILE *const file = getConfigFile();
	if(file){
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
						getShortcut(&element);
						if(isVariable("interact", &element)){
							pushWhitespace(&element);
							if(isVariable("section", &element)){
								++(*sectionShortcutAmount);
							}else if(isVariable("container", &element)){
								++(*containerShortcutAmount);
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
bool readConfigButtons(void){
	bool value = 0;
	FILE *const file = getConfigFile();
	if(file){
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		VariableList hasReadVariable = NoVariables;
		unsigned int currentContainer = 0;
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
					if(!(hasReadVariable & ButtonVariable)){
						if(isVariable("button", &element)){
							pushWhitespace(&element);
							const Button button = getButton(&element);
							if(button.button != AnyButton){
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									XGrabButton(display, button.button, button.masks, container[currentMonitor][currentContainer], True, NoEventMask, GrabModeAsync, GrabModeAsync, None, None);
								}
							}
							hasReadVariable |= ButtonVariable;
							continue;
						}
					}
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectangleVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
						if(hasReadVariable & ButtonVariable){
							hasReadVariable ^= ButtonVariable;
						}
						++currentContainer;
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
bool readConfigFontAmount(unsigned int *const fontAmount){
	bool value = 0;
	*fontAmount = 0;
	FILE *const file = getConfigFile();
	if(file){
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
					if(isVariable("font", &element)){
						pushWhitespace(&element);
						if(isVariable("=", &element)){
							++(*fontAmount);
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
bool readConfigFontLength(const unsigned int fontAmount, unsigned int *const userFontLength){
	bool value = 0;
	unsigned int currentFont;
	for(currentFont = 0; currentFont < fontAmount; ++currentFont){
		userFontLength[currentFont] = 0;
	}
	FILE *const file = getConfigFile();
	if(file){
		currentFont = 0;
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
					if(isVariable("font", &element)){
						pushWhitespace(&element);
						if(isVariable("=", &element)){
							pushWhitespace(&element);
							userFontLength[currentFont] = getQuotedStringLength(&element);
							++currentFont;
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
bool readConfigFillFontArray(const unsigned int currentFont, char *const font){
	bool value = 0;
	FILE *const file = getConfigFile();
	if(file){
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		VariableList hasReadVariable = NoVariables;
		unsigned int fontsRead = 0;
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
					if(isVariable("font", &element)){
						if(fontsRead == currentFont){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								getQuotedString(font, &element);
							}
							++fontsRead;
							break;
						}else{
							++fontsRead;
							continue;
						}
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
bool readConfigFontOffsets(int *const textOffsetX, int *const textOffsetY, int *const drawableCommandOffsetX, int *const drawableCommandOffsetY){
	bool value = 0;
	unsigned int currentContainer;
	for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
		textOffsetX[currentContainer] = 0;
		textOffsetY[currentContainer] = 0;
		drawableCommandOffsetX[currentContainer] = 0;
		drawableCommandOffsetY[currentContainer] = 0;
	}
	FILE *const file = getConfigFile();
	if(file){
		currentContainer = 0;
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
					if(!(hasReadVariable & TextOffsetXVariable)){
						if(isVariable("textOffsetX", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								textOffsetX[currentContainer] = getInteger(None, &element);;
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
								textOffsetY[currentContainer] = getInteger(None, &element);;
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
								drawableCommandOffsetX[currentContainer] = getInteger(None, &element);;
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
								drawableCommandOffsetY[currentContainer] = getInteger(None, &element);;
								hasReadVariable |= DrawableCommandOffsetYVariable;
							}
							continue;
						}
					}
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectangleVariable;
						}
						continue;
					}
					if(isVariable("}", &element)){
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
						hasReadVariable ^= ContainerVariable;
						++currentContainer;
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
	unsigned int dereferencedElement = *element;
	char l = line[dereferencedElement];
	while(l && (l == ' ' || l == '\t')){
		l = line[++dereferencedElement];
	}
	*element = dereferencedElement;
	return;
}
static bool isVariable(const char *const variable, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	bool value = 0;
	unsigned int currentCharacter = 0;
	char l = line[dereferencedElement];
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
		l = line[++dereferencedElement];
		v = variable[++currentCharacter];
	}
	if(currentCharacter){
		*element = dereferencedElement;
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
	unsigned int dereferencedElement = *element;
	int number = 0;
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
	while(line[dereferencedElement]){
		pushWhitespace(&dereferencedElement);
		if(line[dereferencedElement] >= '0' && line[dereferencedElement] <= '9'){
			numberRead *= 10;
			numberRead += line[dereferencedElement];
			numberRead -= 48;
			++dereferencedElement;
		}else if(isVariable("ParentWidth", &dereferencedElement)){
			if(parentWindow){
				numberRead = windowAttributes.width;
			}
		}else if(isVariable("ParentHeight", &dereferencedElement)){
			if(parentWindow){
				numberRead = windowAttributes.height;
			}
		}else if(line[dereferencedElement] == '('){
			++dereferencedElement;
			numberRead = getInteger(parentWindow, &dereferencedElement);
		}else if(line[dereferencedElement] == ')'){
			++dereferencedElement;
			break;
		}else if(line[dereferencedElement] == '+' || line[dereferencedElement] == '-' || line[dereferencedElement] == '*' || line[dereferencedElement] == '/'){
			if(number == 0 && numberRead == 0){
				if(line[dereferencedElement] == '/'){
					break;
				}
			}
			if(operation == AdditionMathOperation){
				if(line[dereferencedElement] != '*' && line[dereferencedElement] != '/'){
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
				if(line[dereferencedElement] != '*' && line[dereferencedElement] != '/'){
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
				if(line[dereferencedElement] == '+' || line[dereferencedElement] == '-'){
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
				if(line[dereferencedElement] == '+' || line[dereferencedElement] == '-'){
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
			if(line[dereferencedElement] == '+'){
				operation = AdditionMathOperation;
			}else if(line[dereferencedElement] == '-'){
				operation = SubtractionMathOperation;
			}else if(line[dereferencedElement] == '*'){
				operation = MultiplicationMathOperation;
			}else if(line[dereferencedElement] == '/'){
				operation = DivisionMathOperation;
			}
			numberRead = 0;
			++dereferencedElement;
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
	*element = dereferencedElement;
	return number;
}
static ARGB getARGB(unsigned int *const element){
	unsigned int dereferencedElement = *element;
	ARGB color = 0x00000000;
	if(line[dereferencedElement] == '#'){
		++dereferencedElement;
	}
	uint8_t charactersRead = 0;
	char l = line[dereferencedElement];
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
		++dereferencedElement;
		++charactersRead;
		l = line[dereferencedElement];
	}
	if(charactersRead == 8){
		*element = dereferencedElement;
	}
	return color;
}
static unsigned int getQuotedStringLength(unsigned int *const element){
	unsigned int dereferencedElement = *element;
	unsigned int length = 0;
	const char quotation = line[dereferencedElement];
	++dereferencedElement;
	while(line[dereferencedElement] != quotation && line[dereferencedElement]){
		++length;
		++dereferencedElement;
	}
	*element = dereferencedElement;
	return length;
}
static unsigned int getQuotedString(char *const string, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	unsigned int currentCharacter = 0;
	const char quotation = line[dereferencedElement];
	++dereferencedElement;
	while(line[dereferencedElement] != quotation && line[dereferencedElement]){
		string[currentCharacter] = line[dereferencedElement];
		++currentCharacter;
		++dereferencedElement;
	}
	*element = dereferencedElement;
	return currentCharacter;
}
static Shortcut getShortcut(unsigned int *const element){
	unsigned int dereferencedElement = *element;
	Shortcut shortcut = {
		.keycode = AnyKey,
		.masks = None
	};
	bool lookingForValue = 1;
	while(line[dereferencedElement]){
		pushWhitespace(&dereferencedElement);
		if(lookingForValue){
			if(line[dereferencedElement] >= '0' && line[dereferencedElement] <= '9'){
				do{
					shortcut.keycode *= 10;
					shortcut.keycode += line[dereferencedElement];
					shortcut.keycode -= 48;
					++dereferencedElement;
				}while(line[dereferencedElement] >= '0' && line[dereferencedElement] <= '9');
			}else if(isVariable("AnyModifier", &dereferencedElement)){
				shortcut.masks |= AnyModifier;
			}else if(isVariable("Shift", &dereferencedElement)){
				shortcut.masks |= ShiftMask;
			}else if(isVariable("Lock", &dereferencedElement)){
				shortcut.masks |= LockMask;
			}else if(isVariable("Control", &dereferencedElement)){
				shortcut.masks |= ControlMask;
			}else if(isVariable("Mod1", &dereferencedElement)){
				shortcut.masks |= Mod1Mask;
			}else if(isVariable("Mod2", &dereferencedElement)){
				shortcut.masks |= Mod2Mask;
		 	}else if(isVariable("Mod3", &dereferencedElement)){
				shortcut.masks |= Mod3Mask;
			}else if(isVariable("Mod4", &dereferencedElement)){
				shortcut.masks |= Mod4Mask;
			}else if(isVariable("Mod5", &dereferencedElement)){
				shortcut.masks |= Mod5Mask;
			}else{
				break;
			}
			lookingForValue = 0;
		}else{
			if(line[dereferencedElement] == '+'){
				++dereferencedElement;
				lookingForValue = 1;
			}else{
				break;
			}
		}
	}
	if(shortcut.keycode != AnyKey){
		*element = dereferencedElement;
	}
	return shortcut;
}
static Button getButton(unsigned int *const element){
	unsigned int dereferencedElement = *element;
	Button button = {
		.button = AnyButton,
		.masks = None
	};
	bool lookingForValue = 1;
	while(line[dereferencedElement]){
		pushWhitespace(&dereferencedElement);
		if(lookingForValue){
			if(isVariable("Button1", &dereferencedElement)){
				button.button = Button1;
			}else if(isVariable("Button2", &dereferencedElement)){
				button.button = Button2;
			}else if(isVariable("Button3", &dereferencedElement)){
				button.button = Button3;
			}else if(isVariable("Button4", &dereferencedElement)){
				button.button = Button4;
			}else if(isVariable("Button5", &dereferencedElement)){
				button.button = Button5;
			}else if(isVariable("AnyModifier", &dereferencedElement)){
				button.masks |= AnyModifier;
			}else if(isVariable("Shift", &dereferencedElement)){
				button.masks |= ShiftMask;
			}else if(isVariable("Lock", &dereferencedElement)){
				button.masks |= LockMask;
			}else if(isVariable("Control", &dereferencedElement)){
				button.masks |= ControlMask;
			}else if(isVariable("Mod1", &dereferencedElement)){
				button.masks |= Mod1Mask;
			}else if(isVariable("Mod2", &dereferencedElement)){
				button.masks |= Mod2Mask;
			}else if(isVariable("Mod3", &dereferencedElement)){
				button.masks |= Mod3Mask;
			}else if(isVariable("Mod4", &dereferencedElement)){
				button.masks |= Mod4Mask;
			}else if(isVariable("Mod5", &dereferencedElement)){
				button.masks |= Mod5Mask;
			}else{
				break;
			}
			lookingForValue = 0;
		}else{
			if(line[dereferencedElement] == '+'){
				++dereferencedElement;
				lookingForValue = 1;
			}else{
				break;
			}
		}
	}
	if(button.button != AnyButton){
		*element = dereferencedElement;
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
