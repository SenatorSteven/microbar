/* eventLoop.c

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
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include "headers/defines.h"
#include "headers/readConfig.h"

extern const char *programName;
extern const char *configPath;
extern const char *workplacePath;
extern FILE *file;
extern unsigned int workplacePathLength;
extern unsigned int drawableCommandPathLength;
extern const char *drawableCommandPath;
extern uint8_t mode;
extern Display *display;
extern unsigned int monitorAmount;
extern unsigned int boxAmount;
extern char line[DefaultCharactersCount + 1];
extern Window *topLevelWindowArray;
extern unsigned int currentMonitor;

static XFontSet createFontSet();
static void drawCommand(const char *const systemCommand, const Window box, const XFontSet fontSet, const unsigned int drawableCommandOffsetX, const unsigned int drawableCommandOffsetY, const GC gc, const uint32_t textColor);
static bool isCommand(const char *const command, const char *const vector);
static void onExpose(const Window *const *const box, char *const *const text, const XFontSet fontSet, const int *const fontOffsetX, const int *const fontOffsetY, const GC *gc, const uint32_t *const textColor);

void eventLoop(void){
	unsigned int currentBox;
	Window _box[monitorAmount][boxAmount];
	const Window *box[monitorAmount];
	{
		Window rootWindow;
		Window parentWindow;
		Window *section;
		unsigned int sectionAmount;
		Window *boxArray;
		unsigned int boxAmount;
		unsigned int boxNumber;
		unsigned int currentSection;
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			XQueryTree(display, topLevelWindowArray[currentMonitor], &rootWindow, &parentWindow, &section, &sectionAmount);
			boxNumber = 0;
			if(sectionAmount > 0){
				for(currentSection = 0; currentSection < sectionAmount; ++currentSection){
					XQueryTree(display, section[currentSection], &rootWindow, &parentWindow, &boxArray, &boxAmount);
					if(boxAmount > 0){
						for(currentBox = 0; currentBox < boxAmount; ++currentBox){
							_box[currentMonitor][boxNumber] = boxArray[currentBox];
							++boxNumber;
						}
						XFree(boxArray);
					}
				}
				XFree(section);
			}
			box[currentMonitor] = _box[currentMonitor];
		}
	}
	unsigned int textMaxLength;
	unsigned int commandMaxLength;
	unsigned int drawableCommandMaxLength;
	readConfigArrayLengths(&textMaxLength, &commandMaxLength, &drawableCommandMaxLength);
	char _text[boxAmount][textMaxLength + 1];
	char _command[boxAmount][commandMaxLength + 1];
	char _drawableCommand[boxAmount][drawableCommandMaxLength + 1];
	char *text[boxAmount];
	uint32_t textColor[boxAmount];
	char *command[boxAmount];
	char *drawableCommand[boxAmount];
	for(currentBox = 0; currentBox < boxAmount; ++currentBox){
		text[currentBox] = _text[currentBox];
		command[currentBox] = _command[currentBox];
		drawableCommand[currentBox] = _drawableCommand[currentBox];
		readConfigFillArrays(currentBox, text[currentBox], &textColor[currentBox], command[currentBox], drawableCommand[currentBox]);
	}
	char _systemCommand[boxAmount][drawableCommandMaxLength + drawableCommandPathLength + 2];
	const char *systemCommand[boxAmount];
	{
		unsigned int element;
		unsigned int currentCharacter;
		for(currentBox = 0; currentBox < boxAmount; ++currentBox){
			element = 0;
			currentCharacter = 0;
			if(drawableCommand[currentBox][currentCharacter]){
				while(drawableCommand[currentBox][currentCharacter]){
					_systemCommand[currentBox][element] = drawableCommand[currentBox][currentCharacter];
					++element;
					++currentCharacter;
				}
				_systemCommand[currentBox][element] = '>';
				++element;
				currentCharacter = 0;
				while(currentCharacter < drawableCommandPathLength){
					_systemCommand[currentBox][element] = drawableCommandPath[currentCharacter];
					++element;
					++currentCharacter;
				}
			}
			_systemCommand[currentBox][element] = '\0';
			systemCommand[currentBox] = _systemCommand[currentBox];
		}
	}
	for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
		for(currentBox = 0; currentBox < boxAmount; ++currentBox){
			readConfigButton(box[currentMonitor][currentBox], currentBox);
		}
	}
	XEvent event;
	bool topLevelWindowsMapped = 1;
	unsigned int topLevelWindowX[monitorAmount];
	unsigned int topLevelWindowY[monitorAmount];
	{
		XWindowAttributes windowAttributes;
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			XGetWindowAttributes(display, topLevelWindowArray[currentMonitor], &windowAttributes);
			topLevelWindowX[currentMonitor] = windowAttributes.x;
			topLevelWindowY[currentMonitor] = windowAttributes.y;
		}
	}
	const XFontSet fontSet = createFontSet();
	int textOffsetX[boxAmount];
	int textOffsetY[boxAmount];
	int drawableCommandOffsetX[boxAmount];
	int drawableCommandOffsetY[boxAmount];
	readConfigFontOffsets(textOffsetX, textOffsetY, drawableCommandOffsetX, drawableCommandOffsetY);
	GC gc[monitorAmount];
	{
		XGCValues gcValues = {
			.subwindow_mode = IncludeInferiors
		};
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			gc[currentMonitor] = XCreateGC(display, topLevelWindowArray[currentMonitor], GCSubwindowMode, &gcValues);
		}
	}
	int rrEventBase;
	{
		int rrErrorBase;
		XRRQueryExtension(display, &rrEventBase, &rrErrorBase);
	}
	for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
		XMapWindow(display, topLevelWindowArray[currentMonitor]);
	}
	for(;;){
		XNextEvent(display, &event);
		if(event.type == KeyPress){
			if(topLevelWindowsMapped){
				for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
					XUnmapWindow(display, topLevelWindowArray[currentMonitor]);
				}
				topLevelWindowsMapped = 0;
			}else{
				for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
					XMoveWindow(display, topLevelWindowArray[currentMonitor], topLevelWindowX[currentMonitor], topLevelWindowY[currentMonitor]);
					XMapWindow(display, topLevelWindowArray[currentMonitor]);
				}
				topLevelWindowsMapped = 1;
			}
		}else if(event.type == ButtonPress){
			for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
				for(currentBox = 0; currentBox < boxAmount; ++currentBox){
					if(event.xbutton.window == box[currentMonitor][currentBox]){
						if(*drawableCommand[currentBox]){
							drawCommand(systemCommand[currentBox], box[currentMonitor][currentBox], fontSet, drawableCommandOffsetX[currentBox], drawableCommandOffsetY[currentBox], gc[currentMonitor], textColor[currentBox]);
						}
						if(command[currentBox]){
							if(isCommand("Restart", command[currentBox])){
								mode = ModeRestart;
							}else if(isCommand("Exit", command[currentBox])){
								mode = ModeExit;
							}else{
								system(command[currentBox]);
							}
						}
						currentMonitor = monitorAmount;
						break;
					}
				}
			}
			if(mode == ModeRestart || mode == ModeExit){
				break;
			}
		}else if(event.type == Expose){
			onExpose(box, text, fontSet, textOffsetX, textOffsetY, gc, textColor);
		}else if(event.type == rrEventBase + RRScreenChangeNotify){
			mode = ModeRestart;
			break;
		}
	}
	for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
		XFreeGC(display, gc[currentMonitor]);
	}
	if(fontSet){
		XFreeFontSet(display, fontSet);
	}
	return;
}
static XFontSet createFontSet(){
	XFontSet fontSet = NULL;
	unsigned int fontAmount;
	readConfigFontAmount(&fontAmount);
	if(fontAmount){
		unsigned int userFontLength[fontAmount];
		readConfigFontLength(fontAmount, userFontLength);
		unsigned int setLength  = fontAmount;
		setLength -= 1;
		for(unsigned int currentFont = 0; currentFont < fontAmount; ++currentFont){
			setLength += userFontLength[currentFont];
		}
		char set[setLength + 1];
		{
			unsigned int element = 0;
			for(unsigned int currentFont = 0; currentFont < fontAmount; ++currentFont){
				readConfigFillFontArray(currentFont, &set[element]);
				element += userFontLength[currentFont];
				if(currentFont < fontAmount - 1){
					set[element] = ',';
					++element;
				}
			}
			set[element] = '\0';
		}
		char **missingFont;
		unsigned int missingAmount;
		{
			char *def_string;
			fontSet = XCreateFontSet(display, set, &missingFont, (int *)&missingAmount, &def_string);
		}
		for(unsigned int currentItem = 0; currentItem < missingAmount; ++currentItem){
			fprintf(stderr, "%s: missing font: %s\n", programName, missingFont[currentItem]);
		}
		XFreeStringList(missingFont);
		if(!fontSet){
			fprintf(stderr, "%s: could not create fontset\n", programName);
		}
	}
	return fontSet;
}
static void drawCommand(const char *const systemCommand, const Window box, const XFontSet fontSet, const unsigned int drawableCommandOffsetX, const unsigned int drawableCommandOffsetY, const GC gc, const uint32_t textColor){
	if(fontSet){
		system(systemCommand);
		if((file = fopen(drawableCommandPath, "r"))){
			unsigned int length = 0;
			while(length < DefaultCharactersCount){
				line[length] = fgetc(file);
				if(line[length] == '\n'){
					line[length] = '\0';
					break;
				}else{
					++length;
				}
			}
			fclose(file);
			if(length > 0){
				int x;
				int y;
				{
					XRectangle overallLogicalReturn;
					XmbTextExtents(fontSet, line, length, NULL, &overallLogicalReturn);
					XWindowAttributes windowAttributes;
					XGetWindowAttributes(display, box, &windowAttributes);
					x = windowAttributes.width;
					x -= overallLogicalReturn.width;
					x /= 2;
					x += drawableCommandOffsetX;
					y = windowAttributes.height;
					y += overallLogicalReturn.height;
					y /= 2;
					y += drawableCommandOffsetY;
				}
				XSetForeground(display, gc, textColor);
				XClearWindow(display, box);
				XmbDrawString(display, box, fontSet, gc, x, y, line, length);
			}
		}else{
			fprintf(stderr, "%s: could not read drawableCommand file\n", programName);
		}
	}
	return;
}
static bool isCommand(const char *const command, const char *const vector){
	bool value = 0;
	unsigned int element = 0;
	while(command[element] || vector[element]){
		if(command[element] >= 'A' && command[element] <= 'Z'){
			if(!(vector[element] == command[element] || vector[element] == command[element] + 32)){
				element = 0;
				break;
			}
		}else if(command[element] >= 'a' && command[element] <= 'z'){
			if(!(vector[element] == command[element] || vector[element] == command[element] - 32)){
				element = 0;
				break;
			}
		}else{
			if(!(vector[element] == command[element])){
				element = 0;
				break;
			}
		}
		++element;
	}
	if(element != 0){
		value = 1;
	}
	return value;
}
static void onExpose(const Window *const *const box, char *const *const text, const XFontSet fontSet, const int *const textOffsetX, const int *const textOffsetY, const GC *gc, const uint32_t *const textColor){
	if(boxAmount > 0 && fontSet){
		unsigned int currentBox;
		unsigned int length;
		XRectangle overallLogicalReturn;
		XWindowAttributes windowAttributes;
		int x;
		int y;
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			for(currentBox = 0; currentBox < boxAmount; ++currentBox){
				if(text[currentBox]){
					length = 0;
					while(text[currentBox][length]){
						++length;
					}
					XmbTextExtents(fontSet, line, length, NULL, &overallLogicalReturn);
					XGetWindowAttributes(display, box[currentMonitor][currentBox], &windowAttributes);
					x = windowAttributes.width;
					x -= overallLogicalReturn.width;
					x /= 2;
					x += textOffsetX[currentBox];
					y = windowAttributes.height;
					y += overallLogicalReturn.height;
					y /= 2;
					y += textOffsetY[currentBox];
					XSetForeground(display, gc[currentMonitor], textColor[currentBox]);
					XClearWindow(display, box[currentMonitor][currentBox]);
					XmbDrawString(display, box[currentMonitor][currentBox], fontSet, gc[currentMonitor], x, y, text[currentBox], length);
				}
			}
		}
	}
	return;
}
