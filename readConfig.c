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

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
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

extern const char *programName;
extern const char *configPath;
extern Display *display;
extern const XRRMonitorInfo *monitorInfo;
extern FILE *file;
extern unsigned int boxAmount;
extern char line[DefaultCharactersCount];
extern Window *topLevelWindowArray;
extern unsigned int currentMonitor;

static FILE *getConfigFile(void);
static bool getLine(void);
static bool pushSpaces(unsigned int *const element);
static bool isVariable(const char *const variableArray, unsigned int *const element);
static unsigned int getUnsignedDecimalNumber(const Window *const window, const unsigned int *const currentLine, unsigned int *const element);
static int getDecimalNumber(const Window *const window, unsigned int *const element);
static bytes4 getARGB(unsigned int *const element);
static bool grabKey(const Window *const window, unsigned int *const element);
static bool printLineError(const unsigned int *const currentLine);

bool readConfigScan(const Window *const parentWindow){
	bool value = 0;
	if((file = getConfigFile())){
		boxAmount = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		bytes4 hasReadVariable = NoPositions;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushSpaces(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								maxLinesCount = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HideKeyPosition)){
						if(isVariable("hideKey", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								grabKey(parentWindow, &element);
								hasReadVariable |= HideKeyPosition;
							}
							continue;
						}
					}
					if(isVariable("menu", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= MenuPosition;
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
					   !isVariable("globalMenuBorderColor",     &element) &&
					   !isVariable("globalMenuBackgroundColor", &element) &&
					   !isVariable("hideKey",                   &element) &&
					   !isVariable("menu",                      &element) &&
					   !isVariable("}",                         &element)){
						printLineError(&currentLine);
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("box", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							++boxAmount;
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= MenuPosition;
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
						printLineError(&currentLine);
						continue;
					}
				}else if(!(hasReadVariable & InnerBoxPosition)){
					if(isVariable("innerBox", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= InnerBoxPosition;
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
					   !isVariable("button",          &element) &&
					   !isVariable("innerBox",        &element) &&
					   !isVariable("}",               &element)){
						printLineError(&currentLine);
						continue;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= InnerBoxPosition;
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
						printLineError(&currentLine);
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
bool readConfigTopLevelWindow(const Window *const parentWindow, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor, bytes4 *const globalMenuBorderColor, bytes4 *const globalMenuBackgroundColor, unsigned int *const menuAmount){
	bool value = 0;
	if((file = getConfigFile())){
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
		bytes4 hasReadVariable = NoPositions;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushSpaces(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								maxLinesCount = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & XPosition)){
						if(isVariable("x", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*x = getDecimalNumber(parentWindow, &element);
								hasReadVariable |= XPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & YPosition)){
						if(isVariable("y", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*y = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= YPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & WidthPosition)){
						if(isVariable("width", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*width = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= WidthPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HeightPosition)){
						if(isVariable("height", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*height = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= HeightPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderColorPosition)){
						if(isVariable("borderColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*borderColor = getARGB(&element);
								hasReadVariable |= BorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderPosition)){
						if(isVariable("border", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*border = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= BorderPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BackgroundColorPosition)){
						if(isVariable("backgroundColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*backgroundColor = getARGB(&element);
								hasReadVariable |= BackgroundColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalMenuBorderColorPosition)){
						if(isVariable("globalMenuBorderColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*globalMenuBorderColor = getARGB(&element);
								hasReadVariable |= GlobalMenuBorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalMenuBackgroundColorPosition)){
						if(isVariable("globalMenuBackgroundColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*globalMenuBackgroundColor = getARGB(&element);
								hasReadVariable |= GlobalMenuBackgroundColorPosition;
							}
							continue;
						}
					}
					if(isVariable("menu", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							++(*menuAmount);
							hasReadVariable |= MenuPosition;
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("box", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= MenuPosition;
						continue;
					}
				}else if(!(hasReadVariable & InnerBoxPosition)){
					if(isVariable("innerBox", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= BoxPosition;
						continue;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= InnerBoxPosition;
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
bool readConfigMenuWindow(const Window *const parentWindow, const unsigned int *const currentMenu, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor, bytes4 *const globalBoxBorderColor, bytes4 *const globalBoxBackgroundColor, unsigned int *const boxAmount){
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
		bytes4 hasReadVariable = NoPositions;
		unsigned int menuAmountRead = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushSpaces(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								maxLinesCount = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("menu", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
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
						if(isVariable("x", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*x = getDecimalNumber(parentWindow, &element);
								hasReadVariable |= XPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & YPosition)){
						if(isVariable("y", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*y = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= YPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & WidthPosition)){
						if(isVariable("width", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*width = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= WidthPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HeightPosition)){
						if(isVariable("height", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*height = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= HeightPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderColorPosition)){
						if(isVariable("borderColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*borderColor = getARGB(&element);
								hasReadVariable |= BorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderPosition)){
						if(isVariable("border", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*border = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= BorderPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BackgroundColorPosition)){
						if(isVariable("backgroundColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*backgroundColor = getARGB(&element);
								hasReadVariable |= BackgroundColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalBoxBorderColorPosition)){
						if(isVariable("globalBoxBorderColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*globalBoxBorderColor = getARGB(&element);
								hasReadVariable |= GlobalBoxBorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalBoxBackgroundColorPosition)){
						if(isVariable("globalBoxBackgroundColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*globalBoxBackgroundColor = getARGB(&element);
								hasReadVariable |= GlobalBoxBackgroundColorPosition;
							}
							continue;
						}
					}
					if(isVariable("box", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							++(*boxAmount);
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						break;
					}
				}else if(!(hasReadVariable & InnerBoxPosition)){
					if(isVariable("innerBox", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= BoxPosition;
						continue;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= InnerBoxPosition;
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
bool readConfigBoxWindow(const Window *const parentWindow, const unsigned int *const currentMenu, const unsigned int *const currentBox, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor, bytes4 *const globalInnerBoxBorderColor, bytes4 *const globalInnerBoxBackgroundColor, unsigned int *const innerBoxAmount){
	bool value = 0;
	if((file = getConfigFile())){
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
		bytes4 hasReadVariable = NoPositions;
		unsigned int menuAmountRead = 0;
		unsigned int boxAmountRead = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushSpaces(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								maxLinesCount = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("menu", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							if(menuAmountRead == *currentMenu){
								hasReadVariable |= MenuPosition;
							}else{
								++menuAmountRead;
							}
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("box", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
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
						if(isVariable("x", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*x = getDecimalNumber(parentWindow, &element);
								hasReadVariable |= XPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & YPosition)){
						if(isVariable("y", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*y = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= YPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & WidthPosition)){
						if(isVariable("width", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*width = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= WidthPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HeightPosition)){
						if(isVariable("height", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*height = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= HeightPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderColorPosition)){
						if(isVariable("borderColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*borderColor = getARGB(&element);
								hasReadVariable |= BorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderPosition)){
						if(isVariable("border", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*border = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= BorderPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BackgroundColorPosition)){
						if(isVariable("backgroundColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*backgroundColor = getARGB(&element);
								hasReadVariable |= BackgroundColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalInnerBoxBorderColorPosition)){
						if(isVariable("globalInnerBoxBorderColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*globalInnerBoxBorderColor = getARGB(&element);
								hasReadVariable |= GlobalInnerBoxBorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & GlobalInnerBoxBackgroundColorPosition)){
						if(isVariable("globalInnerBoxBackgroundColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*globalInnerBoxBackgroundColor = getARGB(&element);
								hasReadVariable |= GlobalInnerBoxBackgroundColorPosition;
							}
							continue;
						}
					}
					if(isVariable("innerBox", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							++(*innerBoxAmount);
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						break;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= InnerBoxPosition;
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
bool readConfigInnerBoxWindow(const Window *const parentWindow, const unsigned int *const currentMenu, const unsigned int *const currentBox, const unsigned int *const currentInnerBox, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor){
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
		bytes4 hasReadVariable = NoPositions;
		unsigned int menuAmountRead = 0;
		unsigned int boxAmountRead = 0;
		unsigned int innerBoxAmountRead = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushSpaces(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								maxLinesCount = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("menu", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							if(menuAmountRead == *currentMenu){
								hasReadVariable |= MenuPosition;
							}else{
								++menuAmountRead;
							}
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("box", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							if(boxAmountRead == *currentBox){
								hasReadVariable |= BoxPosition;
							}else{
								++boxAmountRead;
							}
						}
						continue;
					}
				}else if(!(hasReadVariable & InnerBoxPosition)){
					if(isVariable("innerBox", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
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
						if(isVariable("x", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*x = getDecimalNumber(parentWindow, &element);
								hasReadVariable |= XPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & YPosition)){
						if(isVariable("y", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*y = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= YPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & WidthPosition)){
						if(isVariable("width", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*width = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= WidthPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & HeightPosition)){
						if(isVariable("height", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*height = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= HeightPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderColorPosition)){
						if(isVariable("borderColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*borderColor = getARGB(&element);
								hasReadVariable |= BorderColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BorderPosition)){
						if(isVariable("border", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*border = getUnsignedDecimalNumber(parentWindow, &currentLine, &element);
								hasReadVariable |= BorderPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & BackgroundColorPosition)){
						if(isVariable("backgroundColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
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
		bytes4 hasReadVariable = NoPositions;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushSpaces(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								maxLinesCount = getUnsignedDecimalNumber(&topLevelWindowArray[0], &currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("menu", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= MenuPosition;
						}
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("box", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						hasReadVariable ^= MenuPosition;
						continue;
					}
				}else if(!(hasReadVariable & InnerBoxPosition)){
					if(!(hasReadVariable & TextPosition)){
						if(isVariable("text", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								const char quotation = line[element];
								++element;
								unsigned int length = 0;
								while(line[element] != quotation && line[element]){
									++length;
									++element;
								}
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
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								const char quotation = line[element];
								++element;
								unsigned int length = 0;
								while(line[element] != quotation && line[element]){
									++length;
									++element;
								}
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
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								const char quotation = line[element];
								++element;
								unsigned int length = 0;
								while(line[element] != quotation && line[element]){
									++length;
									++element;
								}
								if(length > *drawableCommandMaxWordLength){
									*drawableCommandMaxWordLength = length;
								}
								hasReadVariable |= DrawableCommandPosition;
							}
							continue;
						}
					}
					if(isVariable("innerBox", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= InnerBoxPosition;
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
						hasReadVariable ^= InnerBoxPosition;
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
bool readConfigFillArrays(const Window *const window, const unsigned int *const currentBox, char *const text, bytes4 *const textColor, char *const command, char *const drawableCommand){
	bool value = 0;
	if((file = getConfigFile())){
		*text = '\0';
		*textColor = 0x00000000;
		*command = '\0';
		*drawableCommand = '\0';
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		bytes4 hasReadVariable = NoPositions;
		unsigned int boxAmountRead = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushSpaces(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								maxLinesCount = getUnsignedDecimalNumber(window, &currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("menu", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= MenuPosition;
						}
						continue;
					}
				}else if(boxAmountRead <= *currentBox){
					if(!(hasReadVariable & GlobalTextColorPosition)){
						if(isVariable("globalTextColor", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*textColor = getARGB(&element);
								hasReadVariable |= GlobalTextColorPosition;
							}
							continue;
						}
					}
					if(isVariable("box", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							++boxAmountRead;
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("innerBox", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
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
						if(isVariable("text", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
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
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								*textColor = getARGB(&element);
								hasReadVariable |= TextColorPosition;
							}
							continue;
						}
					}
					if(!(hasReadVariable & CommandPosition)){
						if(isVariable("command", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
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
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
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
					if(isVariable("innerBox", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						break;
					}
				}else{
					if(isVariable("}", &element)){
						hasReadVariable ^= InnerBoxPosition;
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
bool readConfigButton(const Window *const window, const unsigned int *const currentBox){
	bool value = 0;
	if((file = getConfigFile())){
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		bytes4 hasReadVariable = NoPositions;
		unsigned int boxAmountRead = 0;
		unsigned int button = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine()){
				break;
			}
			element = 0;
			pushSpaces(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & MenuPosition)){
					if(!(hasReadVariable & LinesPosition)){
						if(isVariable("lines", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								maxLinesCount = getUnsignedDecimalNumber(window, &currentLine, &element);
								hasReadVariable |= LinesPosition;
							}
							continue;
						}
					}
					if(isVariable("menu", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= MenuPosition;
						}
						continue;
					}
				}else if(boxAmountRead <= *currentBox){
					if(isVariable("box", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							++boxAmountRead;
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("innerBox", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
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
						if(isVariable("button", &element)){
							pushSpaces(&element);
							if(isVariable("=", &element)){
								pushSpaces(&element);
								button = getUnsignedDecimalNumber(window, &currentLine, &element);
								hasReadVariable |= ButtonPosition;
							}
							continue;
						}
					}
					if(isVariable("innerBox", &element)){
						pushSpaces(&element);
						if(isVariable("{", &element)){
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", &element)){
						break;
					}
				}else{
					if(isVariable("}", &element)){
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
			fprintf(config, "# certain values can be changed through the headers/defines.h of the program's source\n");
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
			fprintf(config, "# box object: x, y, width, height, border, borderColor, backgroundColor, globalInnerBoxBorderColor, globalInnerBoxBackgroundColor, text, textColor, command, drawableCommand, button, innerBox{}\n");
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
			fprintf(config, "# globalInnerBoxBorderColor: color of all innerBoxes\' border\n");
			fprintf(config, "# globalInnerBoxBackgroundColor: color of all innerBoxes\' background\n");
			fprintf(config, "# globalTextColor: color of all boxes\' text\n");
			fprintf(config, "# hideKey: combination of keycode and modifiers to hide the bar\n");
			fprintf(config, "# text: text label of box\n");
			fprintf(config, "# textColor: color of box\'s text\n");
			fprintf(config, "# command: command executed on interaction with box\n");
			fprintf(config, "# drawableCommand: command returning text output executed on interaction with box\n");
			fprintf(config, "# button: mouse button used to interact\n");
			fprintf(config, "# menu: informationless interactionless object, residing in global object\n");
			fprintf(config, "# box: information object, residing in menu object\n");
			fprintf(config, "# innerBox: informationless interactionless object, residing in box object\n");
			fprintf(config, "# ParentWidth: size of parent object\'s width\n");
			fprintf(config, "# ParentHeight: size of parent object\'s height\n\n\n\n");
			fprintf(config, "# # # # #\n");
			fprintf(config, "# extra #\n");
			fprintf(config, "# # # # #\n\n");
			fprintf(config, "# lines: default %u\n", DefaultLinesCount);
			fprintf(config, "# hideKey: modifiers: AnyModifier, Shift, Lock, Control, Mod1, Mod2, Mod3, Mod4, Mod5\n");
			fprintf(config, "# text: requires quotation\n");
			fprintf(config, "# command: requires quotation, program commands: restart, exit\n");
			fprintf(config, "# drawableCommand: requires quotation\n");
			fprintf(config, "# button: default 0 = any button, 1 = left click, 2 = middle click, 3 = right click, 4 = wheel up, 5 = wheel down\n\n\n\n");
			fprintf(config, "# /config start # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
			fprintf(config, "lines = 167\n");
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
			fprintf(stderr, "%s: could not create config file\n", programName);
		}
	}
	return config;
}
static bool getLine(void){
	bool value = 0;
	char characterRead;
	unsigned int element = 0;
	for(;;){
		characterRead = fgetc(file);
		if(characterRead == '\n' || feof(file)){
			break;
		}else if(element < DefaultCharactersCount){
			line[element] = characterRead;
			++element;
		}
	}
	line[element] = '\0';
	++element;
	if(!feof(file)){
		value = 1;
	}
	return value;
}
static bool pushSpaces(unsigned int *const element){
	unsigned int dereferencedElement = *element;
	bool value = 0;
	while(line[dereferencedElement] == ' ' || line[dereferencedElement] == '	'){
		++dereferencedElement;
	}
	if(dereferencedElement > *element){
		*element = dereferencedElement;
		value = 1;
	}
	return value;
}
static bool isVariable(const char *const variable, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	bool value = 0;
	unsigned int currentCharacter = 0;
	while(variable[currentCharacter] && line[dereferencedElement]){
		if(variable[currentCharacter] >= 'A' && variable[currentCharacter] <= 'Z'){
			if(!(variable[currentCharacter] == line[dereferencedElement] || variable[currentCharacter] == line[dereferencedElement] - 32)){
				currentCharacter = 0;
				break;
			}
		}else if(variable[currentCharacter] >= 'a' && variable[currentCharacter] <= 'z'){
			if(!(variable[currentCharacter] == line[dereferencedElement] || variable[currentCharacter] == line[dereferencedElement] + 32)){
				currentCharacter = 0;
				break;
			}
		}else{
			if(!(variable[currentCharacter] == line[dereferencedElement])){
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
static unsigned int getUnsignedDecimalNumber(const Window *const parentWindow, const unsigned int *const currentLine, unsigned int *const element){
	unsigned int number = getDecimalNumber(parentWindow, element);
	if((int)number < 0){
		fprintf(stderr, "%s: line %u: %i is not an unsigned integer\n", programName, *currentLine, (int)number);
		number = 0;
	}
	return number;
}
static int getDecimalNumber(const Window *const parentWindow, unsigned int *const element){
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
	while(line[dereferencedElement]){
		pushSpaces(&dereferencedElement);
		if(line[dereferencedElement] >= '0' && line[dereferencedElement] <= '9'){
			numberRead *= 10;
			numberRead += line[dereferencedElement];
			numberRead -= 48;
			++dereferencedElement;
		}else if(isVariable("ParentWidth", &dereferencedElement)){
			numberRead = windowAttributes.width;
		}else if(isVariable("ParentHeight", &dereferencedElement)){
			numberRead = windowAttributes.height;
		}else if(line[dereferencedElement] == '+' || line[dereferencedElement] == '-' || line[dereferencedElement] == '*' || line[dereferencedElement] == '/'){
			if(number == 0 && numberRead == 0){
				if(line[dereferencedElement] == '/'){
					break;
				}
			}
			if(operation == OperationAddition){
				if(line[dereferencedElement] != '*' && line[dereferencedElement] != '/'){
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
				if(line[dereferencedElement] != '*' && line[dereferencedElement] != '/'){
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
				if(line[dereferencedElement] == '+' || line[dereferencedElement] == '-'){
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
				if(line[dereferencedElement] == '+' || line[dereferencedElement] == '-'){
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
			if(line[dereferencedElement] == '+'){
				operation = OperationAddition;
			}else if(line[dereferencedElement] == '-'){
				operation = OperationSubtraction;
			}else if(line[dereferencedElement] == '*'){
				operation = OperationMultiplication;
			}else if(line[dereferencedElement] == '/'){
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
static bytes4 getARGB(unsigned int *const element){
	unsigned int dereferencedElement = *element;
	bytes4 color = 0x00000000;
	if(line[dereferencedElement] == '#'){
		++dereferencedElement;
	}
	unsigned int currentCharacter = 0;
	while(line[dereferencedElement] && currentCharacter < 8){
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
		++currentCharacter;
	}
	if(currentCharacter == 8){
		*element = dereferencedElement;
	}
	return color;
}
static bool grabKey(const Window *const window, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	bool value = 0;
	unsigned int keycode = AnyKey;
	bytes4 masks = None;
	bool lookingForValue = 1;
	while(line[dereferencedElement]){
		pushSpaces(&dereferencedElement);
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
		XGrabKey(display, keycode, masks, *window, True, GrabModeAsync, GrabModeAsync);
		value = 1;
	}
	return value;
}
static bool printLineError(const unsigned int *const currentLine){
	bool value = 0;
	if(line[0]){
		unsigned int element = 0;
		fprintf(stderr, "%s: line %u: \"", programName, *currentLine);
		while(line[element]){
			fprintf(stderr, "%c", line[element]);
			++element;
		}
		fprintf(stderr, "\" not recognized as an internal variable\n");
		value = 1;
	}
	return value;
}
