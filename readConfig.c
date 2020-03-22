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

#define NoPositions /*----------------------------*/ 0
#define LinesPosition /*--------------------------*/ (1 << 0)
#define XPosition /*------------------------------*/ (1 << 1)
#define YPosition /*------------------------------*/ (1 << 2)
#define WidthPosition /*--------------------------*/ (1 << 3)
#define HeightPosition /*-------------------------*/ (1 << 4)
#define BorderPosition /*-------------------------*/ (1 << 5)
#define BorderColorPosition /*--------------------*/ (1 << 6)
#define BackgroundColorPosition /*----------------*/ (1 << 7)
#define GlobalSectionBorderColorPosition /*-------*/ (1 << 8)
#define GlobalSectionBackgroundColorPosition /*---*/ (1 << 9)
#define GlobalBoxBorderColorPosition /*-----------*/ (1 << 10)
#define GlobalBoxBackgroundColorPosition /*-------*/ (1 << 11)
#define GlobalRectangleBorderColorPosition /*-----*/ (1 << 12)
#define GlobalRectangleBackgroundColorPosition /*-*/ (1 << 13)
#define HideKeyPosition /*------------------------*/ (1 << 14)
#define TextPosition /*---------------------------*/ (1 << 15)
#define TextColorPosition /*----------------------*/ (1 << 16)
#define GlobalTextColorPosition /*----------------*/ (1 << 17)
#define CommandPosition /*------------------------*/ (1 << 18)
#define DrawableCommandPosition /*----------------*/ (1 << 19)
#define TextOffsetXPosition /*--------------------*/ (1 << 20)
#define TextOffsetYPosition /*--------------------*/ (1 << 21)
#define DrawableCommandOffsetXPosition /*---------*/ (1 << 22)
#define DrawableCommandOffsetYPosition /*---------*/ (1 << 23)
#define ButtonPosition /*-------------------------*/ (1 << 24)
#define SectionPosition /*------------------------*/ (1 << 25)
#define BoxPosition /*----------------------------*/ (1 << 26)
#define RectanglePosition /*----------------------*/ (1 << 27)

#define NoOperation /*----------------------------*/ 0
#define AdditionOperation /*----------------------*/ 1
#define SubtractionOperation /*-------------------*/ 2
#define MultiplicationOperation /*----------------*/ 3
#define DivisionOperation /*----------------------*/ 4

extern const char *programName;
extern const char *configPath;
extern FILE *file;
extern Display *display;
extern const XRRMonitorInfo *monitorInfo;
extern unsigned int boxAmount;
extern char line[DefaultCharactersCount + 1];
extern Window *topLevelWindowArray;
extern unsigned int currentMonitor;

static FILE *getConfigFile(void);
static bool getLine(void);
static void pushWhitespace(unsigned int *const element);
static bool isVariable(const char *const variable, unsigned int *const element);
static unsigned int getUnsignedDecimalNumber(const Window window, const unsigned int currentLine, unsigned int *const element);
static int getDecimalNumber(const Window window, unsigned int *const element);
static uint32_t getARGB(unsigned int *const element);
static unsigned int getQuotedStringLength(unsigned int *const element);
static bool grabKey(const Window window, unsigned int *const element);
static bool getButton(unsigned int *const element, uint8_t *const button, uint16_t *const masks);
static void printLineError(const unsigned int currentLine);

bool readConfigScan(const Window parentWindow){
	bool value = 0;
	if((file = getConfigFile())){
		boxAmount = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		uint32_t hasReadVariable = NoPositions;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & SectionPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								maxLinesCount = getUnsignedDecimalNumber(None, currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HideKeyPosition)){
						if(isVariable("hideKey", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								grabKey(parentWindow, &element);
								hasReadVariable |= HideKeyPosition;
							}
							continue;
						}
					}
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= SectionPosition;
						}
						continue;
					}
					if(!isVariable("lines",                     &element) &&
					   !isVariable("x",                         &element) &&
					   !isVariable("y",                         &element) &&
					   !isVariable("width",                     &element) &&
					   !isVariable("height",                    &element) &&
					   !isVariable("border",                    &element) &&
					   !isVariable("borderColor",               &element) &&
					   !isVariable("backgroundColor",           &element) &&
					   !isVariable("globalSectionBorderColor",     &element) &&
					   !isVariable("globalSectionBackgroundColor", &element) &&
					   !isVariable("font",                      &element) &&
					   !isVariable("hideKey",                   &element) &&
					   !isVariable("section",                      &element) &&
					   !isVariable("}",                         &element)){
						printLineError(currentLine);
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("box", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							++boxAmount;
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= SectionPosition;
						continue;
					}
					if(!isVariable("x",                        &element) &&
					   !isVariable("y",                        &element) &&
					   !isVariable("width",                    &element) &&
					   !isVariable("height",                   &element) &&
					   !isVariable("border",                   &element) &&
					   !isVariable("borderColor",              &element) &&
					   !isVariable("backgroundColor",          &element) &&
					   !isVariable("globalBoxBorderColor",     &element) &&
					   !isVariable("globalBoxBackgroundColor", &element) &&
					   !isVariable("globalTextColor",          &element) &&
					   !isVariable("box",                      &element) &&
					   !isVariable("}",                        &element)){
						printLineError(currentLine);
						continue;
					}
				}else if(!(hasReadVariable & RectanglePosition)){
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectanglePosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= BoxPosition;
						continue;
					}
					if(!isVariable("x",               &element) &&
					   !isVariable("y",               &element) &&
					   !isVariable("width",           &element) &&
					   !isVariable("height",          &element) &&
					   !isVariable("border",          &element) &&
					   !isVariable("borderColor",     &element) &&
					   !isVariable("backgroundColor", &element) &&
					   !isVariable("text",            &element) &&
					   !isVariable("textColor",       &element) &&
					   !isVariable("command",         &element) &&
					   !isVariable("drawableCommand", &element) &&
					   !isVariable("textOffsetX",     &element) &&
					   !isVariable("textOffsetY",     &element) &&
					   !isVariable("button",          &element) &&
					   !isVariable("rectangle",        &element) &&
					   !isVariable("}",               &element)){
						printLineError(currentLine);
						continue;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= RectanglePosition;
						continue;
					}
					if(!isVariable("x",               &element) &&
					   !isVariable("y",               &element) &&
					   !isVariable("width",           &element) &&
					   !isVariable("height",          &element) &&
					   !isVariable("border",          &element) &&
					   !isVariable("borderColor",     &element) &&
					   !isVariable("backgroundColor", &element) &&
					   !isVariable("}",               &element)){
						printLineError(currentLine);
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
bool readConfigTopLevelWindow(const Window parentWindow, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, uint32_t *const borderColor, uint32_t *const backgroundColor, uint32_t *const globalSectionBorderColor, uint32_t *const globalSectionBackgroundColor, unsigned int *const sectionAmount){
	bool value = 0;
	if((file = getConfigFile())){
		*x = 0;
		*y = 0;
		*width = 0;
		*height = 0;
		*border = 0;
		*borderColor = 0x00000000;
		*backgroundColor = 0x00000000;
		*globalSectionBorderColor = 0x00000000;
		*globalSectionBackgroundColor = 0x00000000;
		*sectionAmount = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		uint32_t hasReadVariable = NoPositions;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & SectionPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								maxLinesCount = getUnsignedDecimalNumber(None, currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & XPosition)){
						if(isVariable("x", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*x = getDecimalNumber(parentWindow, &element);
								hasReadVariable |= XPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & YPosition)){
						if(isVariable("y", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*y = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= YPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & WidthPosition)){
						if(isVariable("width", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*width = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= WidthPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HeightPosition)){
						if(isVariable("height", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*height = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= HeightPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderColorPosition)){
						if(isVariable("borderColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*borderColor = getARGB(&element);
								hasReadVariable |= BorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderPosition)){
						if(isVariable("border", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*border = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= BorderPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BackgroundColorPosition)){
						if(isVariable("backgroundColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*backgroundColor = getARGB(&element);
								hasReadVariable |= BackgroundColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalSectionBorderColorPosition)){
						if(isVariable("globalSectionBorderColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*globalSectionBorderColor = getARGB(&element);
								hasReadVariable |= GlobalSectionBorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalSectionBackgroundColorPosition)){
						if(isVariable("globalSectionBackgroundColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*globalSectionBackgroundColor = getARGB(&element);
								hasReadVariable |= GlobalSectionBackgroundColorPosition;
							}
							continue;
						}
					}
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							++(*sectionAmount);
							hasReadVariable |= SectionPosition;
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("box", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= SectionPosition;
						continue;
					}
				}else if(!(hasReadVariable & RectanglePosition)){
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectanglePosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= BoxPosition;
						continue;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= RectanglePosition;
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
bool readConfigSectionWindow(const Window parentWindow, const unsigned int currentSection, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, uint32_t *const borderColor, uint32_t *const backgroundColor, uint32_t *const globalBoxBorderColor, uint32_t *const globalBoxBackgroundColor, unsigned int *const boxAmount){
	bool value = 0;
	if((file = getConfigFile())){
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
		uint32_t hasReadVariable = NoPositions;
		unsigned int sectionAmountRead = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & SectionPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								maxLinesCount = getUnsignedDecimalNumber(None, currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							if(sectionAmountRead == currentSection){
								hasReadVariable |= SectionPosition;
							}else{
								++sectionAmountRead;
							}
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(!(hasReadVariable & XPosition)){
						if(isVariable("x", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*x = getDecimalNumber(parentWindow, &element);
								hasReadVariable |= XPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & YPosition)){
						if(isVariable("y", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*y = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= YPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & WidthPosition)){
						if(isVariable("width", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*width = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= WidthPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HeightPosition)){
						if(isVariable("height", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*height = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= HeightPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderColorPosition)){
						if(isVariable("borderColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*borderColor = getARGB(&element);
								hasReadVariable |= BorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderPosition)){
						if(isVariable("border", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*border = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= BorderPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BackgroundColorPosition)){
						if(isVariable("backgroundColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*backgroundColor = getARGB(&element);
								hasReadVariable |= BackgroundColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalBoxBorderColorPosition)){
						if(isVariable("globalBoxBorderColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*globalBoxBorderColor = getARGB(&element);
								hasReadVariable |= GlobalBoxBorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalBoxBackgroundColorPosition)){
						if(isVariable("globalBoxBackgroundColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*globalBoxBackgroundColor = getARGB(&element);
								hasReadVariable |= GlobalBoxBackgroundColorPosition;
							}
							continue;
						}
					}
					if(isVariable("box", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							++(*boxAmount);
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						break;
					}
				}else if(!(hasReadVariable & RectanglePosition)){
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectanglePosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= BoxPosition;
						continue;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= RectanglePosition;
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
bool readConfigBoxWindow(const Window parentWindow, const unsigned int currentSection, const unsigned int currentBox, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, uint32_t *const borderColor, uint32_t *const backgroundColor, uint32_t *const globalRectangleBorderColor, uint32_t *const globalRectangleBackgroundColor, unsigned int *const rectangleAmount){
	bool value = 0;
	if((file = getConfigFile())){
		*x = 0;
		*y = 0;
		*width = 0;
		*height = 0;
		*border = 0;
		*borderColor = 0x00000000;
		*backgroundColor = 0x00000000;
		*globalRectangleBorderColor = 0x00000000;
		*globalRectangleBackgroundColor = 0x00000000;
		*rectangleAmount = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		uint32_t hasReadVariable = NoPositions;
		unsigned int sectionAmountRead = 0;
		unsigned int boxAmountRead = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & SectionPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								maxLinesCount = getUnsignedDecimalNumber(None, currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							if(sectionAmountRead == currentSection){
								hasReadVariable |= SectionPosition;
							}else{
								++sectionAmountRead;
							}
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("box", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							if(boxAmountRead == currentBox){
								hasReadVariable |= BoxPosition;
							}else{
								++boxAmountRead;
							}
						}
						continue;
					}
				}else if(!(hasReadVariable & RectanglePosition)){
					if(!(hasReadVariable & XPosition)){
						if(isVariable("x", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*x = getDecimalNumber(parentWindow, &element);
								hasReadVariable |= XPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & YPosition)){
						if(isVariable("y", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*y = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= YPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & WidthPosition)){
						if(isVariable("width", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*width = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= WidthPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HeightPosition)){
						if(isVariable("height", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*height = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= HeightPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderColorPosition)){
						if(isVariable("borderColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*borderColor = getARGB(&element);
								hasReadVariable |= BorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderPosition)){
						if(isVariable("border", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*border = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= BorderPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BackgroundColorPosition)){
						if(isVariable("backgroundColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*backgroundColor = getARGB(&element);
								hasReadVariable |= BackgroundColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalRectangleBorderColorPosition)){
						if(isVariable("globalRectangleBorderColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*globalRectangleBorderColor = getARGB(&element);
								hasReadVariable |= GlobalRectangleBorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalRectangleBackgroundColorPosition)){
						if(isVariable("globalRectangleBackgroundColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*globalRectangleBackgroundColor = getARGB(&element);
								hasReadVariable |= GlobalRectangleBackgroundColorPosition;
							}
							continue;
						}
					}
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							++(*rectangleAmount);
							hasReadVariable |= RectanglePosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						break;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= RectanglePosition;
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
bool readConfigRectangleWindow(const Window parentWindow, const unsigned int currentSection, const unsigned int currentBox, const unsigned int currentRectangle, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, uint32_t *const borderColor, uint32_t *const backgroundColor){
	bool value = 0;
	if((file = getConfigFile())){
		*x = 0;
		*y = 0;
		*width = 0;
		*height = 0;
		*border = 0;
		*borderColor = 0x00000000;
		*backgroundColor = 0x00000000;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		uint32_t hasReadVariable = NoPositions;
		unsigned int sectionAmountRead = 0;
		unsigned int boxAmountRead = 0;
		unsigned int rectangleAmountRead = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & SectionPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								maxLinesCount = getUnsignedDecimalNumber(None, currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							if(sectionAmountRead == currentSection){
								hasReadVariable |= SectionPosition;
							}else{
								++sectionAmountRead;
							}
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("box", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							if(boxAmountRead == currentBox){
								hasReadVariable |= BoxPosition;
							}else{
								++boxAmountRead;
							}
						}
						continue;
					}
				}else if(!(hasReadVariable & RectanglePosition)){
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							if(rectangleAmountRead == currentRectangle){
								hasReadVariable |= RectanglePosition;
							}else{
								++rectangleAmountRead;
							}
						}
						continue;
					}
				}else{
					if(!(hasReadVariable & XPosition)){
						if(isVariable("x", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*x = getDecimalNumber(parentWindow, &element);
								hasReadVariable |= XPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & YPosition)){
						if(isVariable("y", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*y = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= YPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & WidthPosition)){
						if(isVariable("width", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*width = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= WidthPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HeightPosition)){
						if(isVariable("height", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*height = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= HeightPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderColorPosition)){
						if(isVariable("borderColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*borderColor = getARGB(&element);
								hasReadVariable |= BorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderPosition)){
						if(isVariable("border", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*border = getUnsignedDecimalNumber(parentWindow, currentLine, &element);
								hasReadVariable |= BorderPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BackgroundColorPosition)){
						if(isVariable("backgroundColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*backgroundColor = getARGB(&element);
								hasReadVariable |= BackgroundColorPosition;
							}
							continue;
						}
					}
					if(isVariable("}", &element)){
						break;
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
	if((file = getConfigFile())){
		*textMaxWordLength = 0;
		*commandMaxWordLength = 0;
		*drawableCommandMaxWordLength = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		uint32_t hasReadVariable = NoPositions;
		unsigned int length;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & SectionPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								maxLinesCount = getUnsignedDecimalNumber(None, currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= SectionPosition;
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("box", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= SectionPosition;
						continue;
					}
				}else if(!(hasReadVariable & RectanglePosition)){
					if(!(hasReadVariable & TextPosition)){
						if(isVariable("text", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								length = getQuotedStringLength(&element);
								if(length > *textMaxWordLength){
									*textMaxWordLength = length;
								}
								hasReadVariable |= TextPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & CommandPosition)){
						if(isVariable("command", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								length = getQuotedStringLength(&element);
								if(length > *commandMaxWordLength){
									*commandMaxWordLength = length;
								}
								hasReadVariable |= CommandPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & DrawableCommandPosition)){
						if(isVariable("drawableCommand", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								length = getQuotedStringLength(&element);
								if(length > *drawableCommandMaxWordLength){
									*drawableCommandMaxWordLength = length;
								}
								hasReadVariable |= DrawableCommandPosition;
							}
							continue;
						}
					}
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectanglePosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						if(hasReadVariable & TextPosition){
							hasReadVariable ^= TextPosition;
						}
						if(hasReadVariable & CommandPosition){
							hasReadVariable ^= CommandPosition;
						}
						if(hasReadVariable & DrawableCommandPosition){
							hasReadVariable ^= DrawableCommandPosition;
						}
						hasReadVariable ^= BoxPosition;
						continue;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= RectanglePosition;
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
bool readConfigFillArrays(const unsigned int currentBox, char *const text, uint32_t *const textColor, char *const command, char *const drawableCommand){
	bool value = 0;
	if((file = getConfigFile())){
		*text = '\0';
		*textColor = 0x00000000;
		*command = '\0';
		*drawableCommand = '\0';
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		uint32_t hasReadVariable = NoPositions;
		unsigned int boxAmountRead = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & SectionPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								maxLinesCount = getUnsignedDecimalNumber(None, currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= SectionPosition;
						}
						continue;
					}
				}else if(boxAmountRead <= currentBox){
					if(!(hasReadVariable & GlobalTextColorPosition)){
						if(isVariable("globalTextColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*textColor = getARGB(&element);
								hasReadVariable |= GlobalTextColorPosition;
							}
							continue;
						}
					}
					if(isVariable("box", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							++boxAmountRead;
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectanglePosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						if(hasReadVariable & RectanglePosition){
							hasReadVariable ^= RectanglePosition;
						}else if(hasReadVariable & BoxPosition){
							hasReadVariable ^= BoxPosition;
						}else{
							if(hasReadVariable & GlobalTextColorPosition){
								*textColor = 0x00000000;
								hasReadVariable ^= GlobalTextColorPosition;
							}
							hasReadVariable ^= SectionPosition;
						}
						continue;
					}
				}else if(!(hasReadVariable & RectanglePosition)){
					if(!(hasReadVariable & TextPosition)){
						if(isVariable("text", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								const char quotation = line[element];
								++element;
								unsigned int currentCharacter = 0;
								while(line[element] != quotation && line[element]){
									text[currentCharacter] = line[element];
									++currentCharacter;
									++element;
								}
								text[currentCharacter] = '\0';
								hasReadVariable |= TextPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & TextColorPosition)){
						if(isVariable("textColor", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								*textColor = getARGB(&element);
								hasReadVariable |= TextColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & CommandPosition)){
						if(isVariable("command", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								const char quotation = line[element];
								++element;
								unsigned int currentCharacter = 0;
								while(line[element] != quotation && line[element]){
									command[currentCharacter] = line[element];
									++currentCharacter;
									++element;
								}
								command[currentCharacter] = '\0';
								hasReadVariable |= CommandPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & DrawableCommandPosition)){
						if(isVariable("drawableCommand", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								const char quotation = line[element];
								++element;
								unsigned int currentCharacter = 0;
								while(line[element] != quotation && line[element]){
									drawableCommand[currentCharacter] = line[element];
									++currentCharacter;
									++element;
								}
								drawableCommand[currentCharacter] = '\0';
								hasReadVariable |= DrawableCommandPosition;
							}
							continue;
						}
					}
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectanglePosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						break;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= RectanglePosition;
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
bool readConfigButton(const Window window, const unsigned int currentBox){
	bool value = 0;
	if((file = getConfigFile())){
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		uint32_t hasReadVariable = NoPositions;
		unsigned int boxAmountRead = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & SectionPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								maxLinesCount = getUnsignedDecimalNumber(None, currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= SectionPosition;
						}
						continue;
					}
				}else if(boxAmountRead <= currentBox){
					if(isVariable("box", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							++boxAmountRead;
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectanglePosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						if(hasReadVariable & RectanglePosition){
							hasReadVariable ^= RectanglePosition;
						}else if(hasReadVariable & BoxPosition){
							hasReadVariable ^= BoxPosition;
						}else{
							hasReadVariable ^= SectionPosition;
						}
						continue;
					}
				}else if(!(hasReadVariable & RectanglePosition)){
					if(!(hasReadVariable & ButtonPosition)){
						if(isVariable("button", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								uint8_t button = AnyButton;
								uint16_t masks = None;
								getButton(&element, &button, &masks);
								if(button != AnyButton){
									XGrabButton(display, button, AnyModifier, window, True, NoEventMask, GrabModeAsync, GrabModeAsync, None, None);
								}
								hasReadVariable |= ButtonPosition;
							}
							continue;
						}
					}
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectanglePosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						break;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= RectanglePosition;
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
	if((file = getConfigFile())){
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		uint32_t hasReadVariable = NoPositions;
		*fontAmount = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & SectionPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								maxLinesCount = getUnsignedDecimalNumber(None, currentLine, &element);
								hasReadVariable |= LinesPosition;
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
							hasReadVariable |= SectionPosition;
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("box", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= SectionPosition;
						continue;
					}
				}else if(!(hasReadVariable & RectanglePosition)){
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectanglePosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= BoxPosition;
						continue;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= RectanglePosition;
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
	if((file = getConfigFile())){
		unsigned int currentFont;
		for(currentFont = 0; currentFont < fontAmount; ++currentFont){
			userFontLength[currentFont] = 0;
		}
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		uint32_t hasReadVariable = NoPositions;
		currentFont = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & SectionPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								maxLinesCount = getUnsignedDecimalNumber(None, currentLine, &element);
								hasReadVariable |= LinesPosition;
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
							hasReadVariable |= SectionPosition;
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("box", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= SectionPosition;
						continue;
					}
				}else if(!(hasReadVariable & RectanglePosition)){
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectanglePosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= BoxPosition;
						continue;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= RectanglePosition;
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
bool readConfigFillFontArray(const unsigned int currentFont, char *font){
	bool value = 0;
	if((file = getConfigFile())){
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		uint32_t hasReadVariable = NoPositions;
		unsigned int fontsRead = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & SectionPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								maxLinesCount = getUnsignedDecimalNumber(None, currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("font", &element)){
						if(fontsRead == currentFont){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								const char quotation = line[element];
								++element;
								unsigned int currentCharacter = 0;
								while(line[element] != quotation && line[element]){
									font[currentCharacter] = line[element];
									++currentCharacter;
									++element;
								}
							}
							++fontsRead;
							break;
						}else{
							++fontsRead;
						}
					}
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= SectionPosition;
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("box", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= SectionPosition;
						continue;
					}
				}else if(!(hasReadVariable & RectanglePosition)){
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectanglePosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= BoxPosition;
						continue;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= RectanglePosition;
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
	if((file = getConfigFile())){
		unsigned int currentBox;
		for(currentBox = 0; currentBox < boxAmount; ++currentBox){
			textOffsetX[currentBox] = 0;
			textOffsetY[currentBox] = 0;
			drawableCommandOffsetX[currentBox] = 0;
			drawableCommandOffsetY[currentBox] = 0;
		}
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		uint32_t hasReadVariable = NoPositions;
		currentBox = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & SectionPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								maxLinesCount = getUnsignedDecimalNumber(None, currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("section", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= SectionPosition;
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("box", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= SectionPosition;
						continue;
					}
				}else if(!(hasReadVariable & RectanglePosition)){
					if(!(hasReadVariable & TextOffsetXPosition)){
						if(isVariable("textOffsetX", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								textOffsetX[currentBox] = getDecimalNumber(None, &element);;
								hasReadVariable |= TextOffsetXPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & TextOffsetYPosition)){
						if(isVariable("textOffsetY", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								textOffsetY[currentBox] = getDecimalNumber(None, &element);;
								hasReadVariable |= TextOffsetYPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & DrawableCommandOffsetXPosition)){
						if(isVariable("drawableCommandOffsetX", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								drawableCommandOffsetX[currentBox] = getDecimalNumber(None, &element);;
								hasReadVariable |= DrawableCommandOffsetXPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & DrawableCommandOffsetYPosition)){
						if(isVariable("drawableCommandOffsetY", &element)){
							pushWhitespace(&element);
							if(isVariable("=", &element)){
								pushWhitespace(&element);
								drawableCommandOffsetY[currentBox] = getDecimalNumber(None, &element);;
								hasReadVariable |= DrawableCommandOffsetYPosition;
							}
							continue;
						}
					}
					if(isVariable("rectangle", &element)){
						pushWhitespace(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= RectanglePosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						if(hasReadVariable & TextOffsetXPosition){
							hasReadVariable ^= TextOffsetXPosition;
						}
						if(hasReadVariable & TextOffsetYPosition){
							hasReadVariable ^= TextOffsetYPosition;
						}
						if(hasReadVariable & DrawableCommandOffsetXPosition){
							hasReadVariable ^= DrawableCommandOffsetXPosition;
						}
						if(hasReadVariable & DrawableCommandOffsetYPosition){
							hasReadVariable ^= DrawableCommandOffsetYPosition;
						}
						hasReadVariable ^= BoxPosition;
						++currentBox;
						continue;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= RectanglePosition;
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
			fprintf(config, "# certain values can be changed through the headers/defines.h of the program\'s source\n");
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
			fprintf(config, "# global object: lines, x, y, width, height, border, borderColor, backgroundColor, globalSectionBorderColor, globalSectionBackgroundColor, font, hideKey, section{}\n");
			fprintf(config, "# section object: x, y, width, height, border, borderColor, backgroundColor, globalBoxBorderColor, globalBoxBackgroundColor, globalTextColor, box{}\n");
			fprintf(config, "# box object: x, y, width, height, border, borderColor, backgroundColor, globalRectangleBorderColor, globalRectangleBackgroundColor, text, textColor, textOffsetX, textOffsetY, command, drawableCommand, drawableCommandOffsetX, drawableCommandOffsetY, button, rectangle{}\n");
			fprintf(config, "# rectangle object: x, y, width, height, border, borderColor, backgroundColor\n");
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
			fprintf(config, "# globalSectionBorderColor: color of all sections\' border\n");
			fprintf(config, "# globalSectionBackgroundColor: color of all sections\' background\n");
			fprintf(config, "# globalBoxBorderColor: color of all boxes\' border\n");
			fprintf(config, "# globalBoxBackgroundColor: color of all boxes\' background\n");
			fprintf(config, "# globalRectangleBorderColor: color of all rectangles\' border\n");
			fprintf(config, "# globalRectangleBackgroundColor: color of all rectangles\' background\n");
			fprintf(config, "# globalTextColor: color of all boxes\' text\n");
			fprintf(config, "# font: font to form part of a font set, multiple fonts allowed and encouraged\n");
			fprintf(config, "# hideKey: combination of keycode and modifiers used to hide the bar\n");
			fprintf(config, "# text: text label of box\n");
			fprintf(config, "# textColor: color of box\'s text\n");
			fprintf(config, "# textOffsetX: x axis offset of box\'s text\n");
			fprintf(config, "# textOffsetY: y axis offset of box\'s text\n");
			fprintf(config, "# command: command executed on interaction with box\n");
			fprintf(config, "# drawableCommand: command returning text output executed on interaction with box\n");
			fprintf(config, "# drawableCommandOffsetX: x axis offset of box\'s drawableCommand\n");
			fprintf(config, "# drawableCommandOffsetY: y axis offset of box\'s drawableCommand\n");
			fprintf(config, "# button: combination of mouse button and modifiers used to interact\n");
			fprintf(config, "# section: informationless interactionless object, residing in global object\n");
			fprintf(config, "# box: information object, residing in section object\n");
			fprintf(config, "# rectangle: informationless interactionless object, residing in box object\n");
			fprintf(config, "# ParentWidth: size of parent object\'s width, if applicable\n");
			fprintf(config, "# ParentHeight: size of parent object\'s height, if applicable\n\n\n\n");
			fprintf(config, "# # # # #\n");
			fprintf(config, "# extra #\n");
			fprintf(config, "# # # # #\n\n");
			fprintf(config, "# lines: default %u\n", DefaultLinesCount);
			fprintf(config, "# font: the xfontsel application is recommended for looking at different available fonts\n");
			fprintf(config, "# font: requires quotation, there is a font hierarchy from first to last specified\n");
			fprintf(config, "# hideKey: modifiers: AnyModifier, Shift, Lock, Control, Mod1, Mod2, Mod3, Mod4, Mod5\n");
			fprintf(config, "# text: requires quotation\n");
			fprintf(config, "# command: requires quotation, program commands: restart, exit\n");
			fprintf(config, "# drawableCommand: requires quotation\n");
			fprintf(config, "# button: modifiers: AnyModifier, Shift, Lock, Control, Mod1, Mod2, Mod3, Mod4, Mod5\n");
			fprintf(config, "# button: buttons: Button1 = left click, Button2 = middle click, Button3 = right click, Button4 = wheel up, Button5 = wheel down\n\n\n\n");
			fprintf(config, "# /config start # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
			fprintf(config, "lines = 187\n");
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
			fprintf(config, "hideKey = 116 + Mod4\n");
			fprintf(config, "section{\n");
			fprintf(config, "	x = 0\n");
			fprintf(config, "	y = 0\n");
			fprintf(config, "	width = 58\n");
			fprintf(config, "	height = ParentHeight\n");
			fprintf(config, "	border = 0\n");
			fprintf(config, "	box{\n");
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
			fprintf(config, "		textOffsetY = -2\n");
			fprintf(config, "		command = \"restart\"\n");
			fprintf(config, "		button = Button1\n");
			fprintf(config, "	}\n");
			fprintf(config, "}\n");
			fprintf(config, "section{\n");
			fprintf(config, "	x = ParentWidth - 224\n");
			fprintf(config, "	y = 0\n");
			fprintf(config, "	width = 224\n");
			fprintf(config, "	height = ParentHeight\n");
			fprintf(config, "	border = 0\n");
			fprintf(config, "	globalBoxBorderColor = #FF1F1F1F\n");
			fprintf(config, "	globalBoxBackgroundColor = #FF000000\n");
			fprintf(config, "	globalTextColor = #FFFFFFFF\n");
			fprintf(config, "	box{\n");
			fprintf(config, "		x = ParentWidth - 224\n");
			fprintf(config, "		y = 0\n");
			fprintf(config, "		width = 84\n");
			fprintf(config, "		height = ParentHeight - 2\n");
			fprintf(config, "		border = 1\n");
			fprintf(config, "		text = \"date\"\n");
			fprintf(config, "		textOffsetY = -2\n");
			fprintf(config, "		drawableCommand = \"date \'+D%%d/M%%m/Y%%y\'\"\n");
			fprintf(config, "		drawableCommandOffsetX = 1\n");
			fprintf(config, "		drawableCommandOffsetY = -2\n");
			fprintf(config, "		button = Button1\n");
			fprintf(config, "	}\n");
			fprintf(config, "	box{\n");
			fprintf(config, "		x = ParentWidth - 137\n");
			fprintf(config, "		y = 0\n");
			fprintf(config, "		width = 97\n");
			fprintf(config, "		height = ParentHeight - 2\n");
			fprintf(config, "		border = 1\n");
			fprintf(config, "		text = \"time\"\n");
			fprintf(config, "		textOffsetX = 1\n");
			fprintf(config, "		textOffsetY = -2\n");
			fprintf(config, "		drawableCommand = \"date \'+%%H:%%M:%%S %%Z\'\"\n");
			fprintf(config, "		drawableCommandOffsetY = -2\n");
			fprintf(config, "		button = Button1\n");
			fprintf(config, "	}\n");
			fprintf(config, "	box{\n");
			fprintf(config, "		x = ParentWidth - 37\n");
			fprintf(config, "		y = 0\n");
			fprintf(config, "		width = 35\n");
			fprintf(config, "		height = ParentHeight - 2\n");
			fprintf(config, "		border = 1\n");
			fprintf(config, "		text = \"exit\"\n");
			fprintf(config, "		textColor = #FFFF0000\n");
			fprintf(config, "		textOffsetX = 1\n");
			fprintf(config, "		textOffsetY = -2\n");
			fprintf(config, "		command = \"exit\"\n");
			fprintf(config, "		button = Button1\n");
			fprintf(config, "	}\n");
			fprintf(config, "}\n");
			fprintf(config, "# /config end # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
			fclose(config);
			config = fopen(configPath, "r");
		}else{
			fprintf(stderr, "%s: could not create config file\n", programName);
		}
	}
	return config;
}
static bool getLine(void){
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
	while(line[dereferencedElement] == ' ' || line[dereferencedElement] == '	'){
		++dereferencedElement;
	}
	if(dereferencedElement > *element){
		*element = dereferencedElement;
	}
	return;
}
static bool isVariable(const char *const variable, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	bool value = 0;
	unsigned int currentCharacter = 0;
	while(variable[currentCharacter] && line[dereferencedElement]){
		if(variable[currentCharacter] >= 'A' && variable[currentCharacter] <= 'Z'){
			if(!(line[dereferencedElement] == variable[currentCharacter] || line[dereferencedElement] == variable[currentCharacter] + 32)){
				currentCharacter = 0;
				break;
			}
		}else if(variable[currentCharacter] >= 'a' && variable[currentCharacter] <= 'z'){
			if(!(line[dereferencedElement] == variable[currentCharacter] || line[dereferencedElement] == variable[currentCharacter] - 32)){
				currentCharacter = 0;
				break;
			}
		}else{
			if(!(line[dereferencedElement] == variable[currentCharacter])){
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
static unsigned int getUnsignedDecimalNumber(const Window parentWindow, const unsigned int currentLine, unsigned int *const element){
	unsigned int number = getDecimalNumber(parentWindow, element);
	if((int)number < 0){
		fprintf(stderr, "%s: line %u: %i is not an unsigned integer\n", programName, currentLine, (int)number);
		number = 0;
	}
	return number;
}
static int getDecimalNumber(const Window parentWindow, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	int number = 0;
	int numberRead = 0;
	int numberOperatedOn = 0;
	uint8_t operation = NoOperation;
	uint8_t lastOperation = NoOperation;
	XWindowAttributes windowAttributes;
	if(parentWindow){
		XGetWindowAttributes(display, parentWindow, &windowAttributes);
		if(parentWindow == XDefaultRootWindow(display)){
			windowAttributes.width = monitorInfo[currentMonitor].width;
			windowAttributes.height = monitorInfo[currentMonitor].height;
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
		}else if(line[dereferencedElement] == '+' || line[dereferencedElement] == '-' || line[dereferencedElement] == '*' || line[dereferencedElement] == '/'){
			if(number == 0 && numberRead == 0){
				if(line[dereferencedElement] == '/'){
					break;
				}
			}
			if(operation == AdditionOperation){
				if(line[dereferencedElement] != '*' && line[dereferencedElement] != '/'){
					if(numberOperatedOn == 0){
						number += numberRead;
					}else{
						if(lastOperation == AdditionOperation){
							number += numberOperatedOn;
						}else if(lastOperation == SubtractionOperation){
							number -= numberOperatedOn;
						}
					}
				}else{
					numberOperatedOn = numberRead;
					lastOperation = operation;
				}
			}else if(operation == SubtractionOperation){
				if(line[dereferencedElement] != '*' && line[dereferencedElement] != '/'){
					if(numberOperatedOn == 0){
						number -= numberRead;
					}else{
						if(lastOperation == AdditionOperation){
							number += numberOperatedOn;
						}else if(lastOperation == SubtractionOperation){
							number -= numberOperatedOn;
						}
					}
				}else{
					numberOperatedOn = numberRead;
					lastOperation = operation;
				}
			}else if(operation == MultiplicationOperation){
				if(numberOperatedOn == 0){
					number *= numberRead;
				}else{
					numberOperatedOn *= numberRead;
				}
				if(line[dereferencedElement] == '+' || line[dereferencedElement] == '-'){
					if(lastOperation == AdditionOperation){
						number += numberOperatedOn;
					}else if(lastOperation == SubtractionOperation){
						number -= numberOperatedOn;
					}
					numberOperatedOn = 0;
				}
			}else if(operation == DivisionOperation){
				if(numberOperatedOn == 0){
					number /= numberRead;
				}else{
					numberOperatedOn /= numberRead;
				}
				if(line[dereferencedElement] == '+' || line[dereferencedElement] == '-'){
					if(lastOperation == AdditionOperation){
						number += numberOperatedOn;
					}else if(lastOperation == SubtractionOperation){
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
				operation = AdditionOperation;
			}else if(line[dereferencedElement] == '-'){
				operation = SubtractionOperation;
			}else if(line[dereferencedElement] == '*'){
				operation = MultiplicationOperation;
			}else if(line[dereferencedElement] == '/'){
				operation = DivisionOperation;
			}
			numberRead = 0;
			++dereferencedElement;
		}else{
			break;
		}
	}
	if(operation == AdditionOperation){
		if(numberOperatedOn > 0){
			if(lastOperation == AdditionOperation){
				number += numberOperatedOn;
			}else if(lastOperation == SubtractionOperation){
				number -= numberOperatedOn;
			}
		}
		number += numberRead;
	}else if(operation == SubtractionOperation){
		if(numberOperatedOn > 0){
			if(lastOperation == AdditionOperation){
				number += numberOperatedOn;
			}else if(lastOperation == SubtractionOperation){
				number -= numberOperatedOn;
			}
		}
		number -= numberRead;
	}else if(operation == MultiplicationOperation){
		if(numberOperatedOn == 0){
			number *= numberRead;
		}else{
			numberOperatedOn *= numberRead;
			if(lastOperation == AdditionOperation){
				number += numberOperatedOn;
			}else if(lastOperation == SubtractionOperation){
				number -= numberOperatedOn;
			}
		}
	}else if(operation == DivisionOperation){
		if(number > 0 || numberOperatedOn > 0){
			if(numberOperatedOn == 0){
				number /= numberRead;
			}else{
				numberOperatedOn /= numberRead;
				if(lastOperation == AdditionOperation){
					number += numberOperatedOn;
				}else if(lastOperation == SubtractionOperation){
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
static uint32_t getARGB(unsigned int *const element){
	unsigned int dereferencedElement = *element;
	uint32_t color = 0x00000000;
	if(line[dereferencedElement] == '#'){
		++dereferencedElement;
	}
	uint8_t charactersRead = 0;
	while(line[dereferencedElement] && charactersRead < 8){
		color *= 16;
		if(line[dereferencedElement] >= '0' && line[dereferencedElement] <= '9'){
			color += line[dereferencedElement];
			color -= 48;
		}else if(line[dereferencedElement] >= 'A' && line[dereferencedElement] <= 'F'){
			color += line[dereferencedElement];
			color -= 55;
		}else if(line[dereferencedElement] >= 'a' && line[dereferencedElement] <= 'f'){
			color += line[dereferencedElement];
			color -= 87;
		}else{
			if(line[dereferencedElement] != ' '){
				fprintf(stderr, "%s: \'%c\' is not recognized as a hexadecimal number\n", programName, line[dereferencedElement]);
				color = 0x00000000;
			}
			break;
		}
		++dereferencedElement;
		++charactersRead;
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
static bool grabKey(const Window window, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	bool value = 0;
	unsigned int keycode = AnyKey;
	uint16_t masks = None;
	bool lookingForValue = 1;
	while(line[dereferencedElement]){
		pushWhitespace(&dereferencedElement);
		if(lookingForValue){
			if(line[dereferencedElement] >= '0' && line[dereferencedElement] <= '9'){
				do{
					keycode *= 10;
					keycode += line[dereferencedElement];
					keycode -= 48;
					++dereferencedElement;
				}while(line[dereferencedElement] >= '0' && line[dereferencedElement] <= '9');
			}else if(isVariable("AnyModifier", &dereferencedElement)){
				masks |= AnyModifier;
			}else if(isVariable("Shift", &dereferencedElement)){
				masks |= ShiftMask;
			}else if(isVariable("Lock", &dereferencedElement)){
				masks |= LockMask;
			}else if(isVariable("Control", &dereferencedElement)){
				masks |= ControlMask;
			}else if(isVariable("Mod1", &dereferencedElement)){
				masks |= Mod1Mask;
			}else if(isVariable("Mod2", &dereferencedElement)){
				masks |= Mod2Mask;
			}else if(isVariable("Mod3", &dereferencedElement)){
				masks |= Mod3Mask;
			}else if(isVariable("Mod4", &dereferencedElement)){
				masks |= Mod4Mask;
			}else if(isVariable("Mod5", &dereferencedElement)){
				masks |= Mod5Mask;
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
	if(keycode != AnyKey){
		*element = dereferencedElement;
		XGrabKey(display, keycode, masks, window, True, GrabModeAsync, GrabModeAsync);
		value = 1;
	}
	return value;
}
static bool getButton(unsigned int *const element, uint8_t *const button, uint16_t *const masks){
	unsigned int dereferencedElement = *element;
	uint8_t dereferencedButton = AnyButton;
	uint16_t dereferencedMasks = None;
	bool value = 0;
	bool lookingForValue = 1;
	while(line[dereferencedElement]){
		pushWhitespace(&dereferencedElement);
		if(lookingForValue){
			if(isVariable("Button1", &dereferencedElement)){
				dereferencedButton = Button1;
			}else if(isVariable("Button2", &dereferencedElement)){
				dereferencedButton = Button2;
			}else if(isVariable("Button3", &dereferencedElement)){
				dereferencedButton = Button3;
			}else if(isVariable("Button4", &dereferencedElement)){
				dereferencedButton = Button4;
			}else if(isVariable("Button5", &dereferencedElement)){
				dereferencedButton = Button5;
			}else if(isVariable("AnyModifier", &dereferencedElement)){
				dereferencedMasks |= AnyModifier;
			}else if(isVariable("Shift", &dereferencedElement)){
				dereferencedMasks |= ShiftMask;
			}else if(isVariable("Lock", &dereferencedElement)){
				dereferencedMasks |= LockMask;
			}else if(isVariable("Control", &dereferencedElement)){
				dereferencedMasks |= ControlMask;
			}else if(isVariable("Mod1", &dereferencedElement)){
				dereferencedMasks |= Mod1Mask;
			}else if(isVariable("Mod2", &dereferencedElement)){
				dereferencedMasks |= Mod2Mask;
			}else if(isVariable("Mod3", &dereferencedElement)){
				dereferencedMasks |= Mod3Mask;
			}else if(isVariable("Mod4", &dereferencedElement)){
				dereferencedMasks |= Mod4Mask;
			}else if(isVariable("Mod5", &dereferencedElement)){
				dereferencedMasks |= Mod5Mask;
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
	if(dereferencedButton != AnyButton){
		*element = dereferencedElement;
		*button = dereferencedButton;
		*masks = dereferencedMasks;
		value = 1;
	}
	return value;
}
static void printLineError(const unsigned int currentLine){
	if(line[0]){
		unsigned int element = 0;
		fprintf(stderr, "%s: line %u: \"", programName, currentLine);
		while(line[element]){
			fprintf(stderr, "%c", line[element]);
			++element;
		}
		fprintf(stderr, "\" not recognized as an internal variable\n");
	}
	return;
}
