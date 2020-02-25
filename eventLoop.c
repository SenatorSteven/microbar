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
extern unsigned int mode;
extern Display *display;
extern unsigned int monitorAmount;
extern unsigned int boxAmount;
extern char line[DefaultCharactersCount];
extern Window *topLevelWindowArray;
extern unsigned int currentMonitor;

static bool drawCommand(const Window *const topLevelWindow, const char *const systemCommand, const char *const drawableCommandPath, const Window *const box, const bytes4 *const textColor);
static bool isCommand(const char *const command, const char *const vector);
static bool onExpose(Window *const *const box, char *const *const text, const bytes4 *const textColor);

void eventLoop(void){
	unsigned int currentBox;
	Window _box[monitorAmount][boxAmount];
	Window *box[monitorAmount];
	{
		Window rootWindow;
		Window parentWindow;
		Window *menu;
		unsigned int menuAmount;
		Window *boxArray;
		unsigned int boxAmount;
		unsigned int boxNumber;
		unsigned int currentMenu;
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			box[currentMonitor] = _box[currentMonitor];
			XQueryTree(display, topLevelWindowArray[currentMonitor], &rootWindow, &parentWindow, &menu, &menuAmount);
			boxNumber = 0;
			if(menuAmount > 0){
				for(currentMenu = 0; currentMenu < menuAmount; ++currentMenu){
					XQueryTree(display, menu[currentMenu], &rootWindow, &parentWindow, &boxArray, &boxAmount);
					if(boxAmount > 0){
						for(currentBox = 0; currentBox < boxAmount; ++currentBox){
							box[currentMonitor][boxNumber] = boxArray[currentBox];
							++boxNumber;
						}
						XFree(boxArray);
					}
				}
				XFree(menu);
			}
		}
	}
	unsigned int textMaxWordLength;
	unsigned int commandMaxWordLength;
	unsigned int drawableCommandMaxWordLength;
	readConfigArrayLengths(&textMaxWordLength, &commandMaxWordLength, &drawableCommandMaxWordLength);
	char _text[boxAmount][textMaxWordLength + 1];
	char _command[boxAmount][commandMaxWordLength + 1];
	char _drawableCommand[boxAmount][drawableCommandMaxWordLength + 1];
	char *text[boxAmount];
	bytes4 textColor[boxAmount];
	char *command[boxAmount];
	char *drawableCommand[boxAmount];
	for(currentBox = 0; currentBox < boxAmount; ++currentBox){
		text[currentBox] = _text[currentBox];
		command[currentBox] = _command[currentBox];
		drawableCommand[currentBox] = _drawableCommand[currentBox];
		readConfigFillArrays(&currentBox, text[currentBox], &textColor[currentBox], command[currentBox], drawableCommand[currentBox]);
	}
	unsigned int drawableCommandPathLength = 0;
	{
		while(workplacePath[drawableCommandPathLength]){
			++drawableCommandPathLength;
		}
		if(workplacePath[drawableCommandPathLength - 1] == '/'){
			--drawableCommandPathLength;
		}
		drawableCommandPathLength += 16;
	}
	char drawableCommandPath[drawableCommandPathLength + 1];
	{
		unsigned int currentCharacter = 0;
		{
			const unsigned int lastCharacter = drawableCommandPathLength - 16;
			while(currentCharacter < lastCharacter){
				drawableCommandPath[currentCharacter] = workplacePath[currentCharacter];
				++currentCharacter;
			}
		}
		drawableCommandPath[currentCharacter] = '/';
		drawableCommandPath[++currentCharacter] = 'd';
		drawableCommandPath[++currentCharacter] = 'r';
		drawableCommandPath[++currentCharacter] = 'a';
		drawableCommandPath[++currentCharacter] = 'w';
		drawableCommandPath[++currentCharacter] = 'a';
		drawableCommandPath[++currentCharacter] = 'b';
		drawableCommandPath[++currentCharacter] = 'l';
		drawableCommandPath[++currentCharacter] = 'e';
		drawableCommandPath[++currentCharacter] = 'C';
		drawableCommandPath[++currentCharacter] = 'o';
		drawableCommandPath[++currentCharacter] = 'm';
		drawableCommandPath[++currentCharacter] = 'm';
		drawableCommandPath[++currentCharacter] = 'a';
		drawableCommandPath[++currentCharacter] = 'n';
		drawableCommandPath[++currentCharacter] = 'd';
		drawableCommandPath[++currentCharacter] = '\0';
	}
	char _systemCommand[boxAmount][drawableCommandMaxWordLength + 1 + drawableCommandPathLength + 1];
	char *systemCommand[boxAmount];
	{
		unsigned int element;
		unsigned int currentCharacter;
		for(currentBox = 0; currentBox < boxAmount; ++currentBox){
			systemCommand[currentBox] = _systemCommand[currentBox];
			element = 0;
			currentCharacter = 0;
			if(drawableCommand[currentBox][currentCharacter]){
				while(drawableCommand[currentBox][currentCharacter]){
					systemCommand[currentBox][element] = drawableCommand[currentBox][currentCharacter];
					++element;
					++currentCharacter;
				}
				systemCommand[currentBox][element] = '>';
				++element;
				currentCharacter = 0;
				while(currentCharacter < drawableCommandPathLength){
					systemCommand[currentBox][element] = drawableCommandPath[currentCharacter];
					++element;
					++currentCharacter;
				}
			}
			systemCommand[currentBox][element] = '\0';
		}
	}
	for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
		for(currentBox = 0; currentBox < boxAmount; ++currentBox){
			readConfigButton(&box[currentMonitor][currentBox], &currentBox);
		}
		XMapWindow(display, topLevelWindowArray[currentMonitor]);
	}
	XEvent event;
	int rrEventBase;
	{
		int rrErrorBase;
		XRRQueryExtension(display, &rrEventBase, &rrErrorBase);
	}
	bool topLevelWindowsMapped = 1;
	unsigned int x[monitorAmount];
	unsigned int y[monitorAmount];
	{
		XWindowAttributes windowAttributes;
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			XGetWindowAttributes(display, topLevelWindowArray[currentMonitor], &windowAttributes);
			x[currentMonitor] = windowAttributes.x;
			y[currentMonitor] = windowAttributes.y;
		}
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
					XMoveWindow(display, topLevelWindowArray[currentMonitor], x[currentMonitor], y[currentMonitor]);
					XMapWindow(display, topLevelWindowArray[currentMonitor]);
				}
				topLevelWindowsMapped = 1;
			}
		}else if(event.type == ButtonPress){
			for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
				for(currentBox = 0; currentBox < boxAmount; ++currentBox){
					if(event.xbutton.window == box[currentMonitor][currentBox]){
						if(*drawableCommand[currentBox]){
							drawCommand(&topLevelWindowArray[currentMonitor], systemCommand[currentBox], drawableCommandPath, &box[currentMonitor][currentBox], &textColor[currentBox]);
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
			onExpose(box, text, textColor);
		}else if(event.type == rrEventBase + RRScreenChangeNotify){
			mode = ModeRestart;
			break;
		}
	}
	return;
}
static bool drawCommand(const Window *const topLevelWindow, const char *const systemCommand, const char *const drawableCommandPath, const Window *const box, const bytes4 *const textColor){
	bool value = 0;
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
			GC gc;
			{
				XGCValues gcValues = {
					.foreground = *textColor,
					.subwindow_mode = IncludeInferiors
				};
				gc = XCreateGC(display, *topLevelWindow, GCForeground | GCSubwindowMode, &gcValues);
			}
			int x;
			int y;
			XCharStruct charStruct;
			{
				XFontStruct *fontStruct = XLoadQueryFont(display, "fixed");
				int direction;
				XTextExtents(fontStruct, line, length, &direction, (int *)&charStruct.ascent, (int *)&charStruct.descent, &charStruct);
				XFreeFont(display, fontStruct);
			}
			XWindowAttributes windowAttributes;
			XGetWindowAttributes(display, *box, &windowAttributes);
			x = windowAttributes.width;
			x -= charStruct.width;
			x /= 2;
			y = windowAttributes.height;
			y += charStruct.ascent;
			y /= 2;
			XClearWindow(display, *box);
			XDrawString(display, *box, gc, x, y, line, length);
			XFreeGC(display, gc);
			value = 1;
		}
	}else{
		fprintf(stderr, "%s: could not read temporary file (drawableCommand)\n", programName);
	}
	return value;
}
static bool isCommand(const char *const command, const char *const vector){
	bool value = 0;
	unsigned int element = 0;
	while(command[element] || vector[element]){
		if(command[element] >= 'A' && command[element] <= 'Z'){
			if(!(command[element] == vector[element] || command[element] == vector[element] - 32)){
				element = 0;
				break;
			}
		}else if(command[element] >= 'a' && command[element] <= 'z'){
			if(!(command[element] == vector[element] || command[element] == vector[element] + 32)){
				element = 0;
				break;
			}
		}else{
			if(!(command[element] == vector[element])){
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
static bool onExpose(Window *const *const box, char *const *const text, const bytes4 *const textColor){
	bool value = 0;
	if(boxAmount > 0){
		GC gc;
		XGCValues GCValues = {
			.subwindow_mode = IncludeInferiors
		};
		unsigned int currentBox;
		unsigned int length;
		XFontStruct *fontStruct = XLoadQueryFont(display, "fixed");
		int direction;
		XCharStruct charStruct;
		XWindowAttributes windowAttributes;
		int x;
		int y;
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			gc = XCreateGC(display, topLevelWindowArray[currentMonitor], GCSubwindowMode, &GCValues);
			for(currentBox = 0; currentBox < boxAmount; ++currentBox){
				if(text[currentBox]){
					XSetForeground(display, gc, textColor[currentBox]);
					length = 0;
					while(text[currentBox][length]){
						++length;
					}
					XTextExtents(fontStruct, text[currentBox], length, &direction, (int *)&charStruct.ascent, (int *)&charStruct.descent, &charStruct);
					XGetWindowAttributes(display, box[currentMonitor][currentBox], &windowAttributes);
					x = windowAttributes.width;
					x -= charStruct.width;
					x /= 2;
					y = windowAttributes.height;
					y += charStruct.ascent;
					y /= 2;
					XClearWindow(display, box[currentMonitor][currentBox]);
					XDrawString(display, box[currentMonitor][currentBox], gc, x, y, text[currentBox], length);
				}
			}
			XFreeGC(display, gc);
		}
		XFreeFont(display, fontStruct);
		value = 1;
	}
	return value;
}
