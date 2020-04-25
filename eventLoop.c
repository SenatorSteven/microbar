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
extern unsigned int workplacePathLength;
extern unsigned int drawableCommandPathLength;
extern const char *drawableCommandPath;
extern uint8_t mode;
extern Display *display;
extern unsigned int monitorAmount;
extern unsigned int containerAmount;
extern char line[DefaultCharactersCount + 1];
extern Window *topLevelWindow;
extern unsigned int currentMonitor;

static void grabKeys(Shortcut hide, Shortcut peek, Shortcut restart, Shortcut exit);
static XFontSet createFontSet(void);
static void drawCommand(const char *const systemCommand, const Window container, const XFontSet fontSet, const unsigned int drawableCommandOffsetX, const unsigned int drawableCommandOffsetY, const GC gc, const uint32_t textColor);
static bool isCommand(const char *const command, const char *const vector);
static void hideToggle(bool *const topLevelWindowsMapped, bool *const topLevelWindowsShown, unsigned int *const topLevelWindowX, unsigned int *const topLevelWindowY);
static void onExpose(const Window *const *const container, char *const *const text, const XFontSet fontSet, const int *const fontOffsetX, const int *const fontOffsetY, const GC *gc, const uint32_t *const textColor);
static void ungrabKeys(Shortcut hide, Shortcut peek, Shortcut restart, Shortcut exit);

void eventLoop(void){
	unsigned int currentContainer;
	Window _container[monitorAmount][containerAmount];
	const Window *container[monitorAmount];
	{
		Window rootWindow;
		Window parentWindow;
		Window *section;
		unsigned int sectionAmount;
		Window *containerArray;
		unsigned int containerAmount;
		unsigned int containerNumber;
		unsigned int currentSection;
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			XQueryTree(display, topLevelWindow[currentMonitor], &rootWindow, &parentWindow, &section, &sectionAmount);
			containerNumber = 0;
			if(sectionAmount > 0){
				for(currentSection = 0; currentSection < sectionAmount; ++currentSection){
					XQueryTree(display, section[currentSection], &rootWindow, &parentWindow, &containerArray, &containerAmount);
					if(containerAmount > 0){
						for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
							_container[currentMonitor][containerNumber] = containerArray[currentContainer];
							++containerNumber;
						}
						XFree(containerArray);
					}
				}
				XFree(section);
			}
			container[currentMonitor] = _container[currentMonitor];
		}
	}
	unsigned int textMaxLength;
	unsigned int commandMaxLength;
	unsigned int drawableCommandMaxLength;
	readConfigArrayLengths(&textMaxLength, &commandMaxLength, &drawableCommandMaxLength);
	char _text[containerAmount][textMaxLength + 1];
	char _command[containerAmount][commandMaxLength + 1];
	char _drawableCommand[containerAmount][drawableCommandMaxLength + 1];
	char *text[containerAmount];
	uint32_t textColor[containerAmount];
	char *command[containerAmount];
	char *drawableCommand[containerAmount];
	for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
		text[currentContainer] = _text[currentContainer];
		command[currentContainer] = _command[currentContainer];
		drawableCommand[currentContainer] = _drawableCommand[currentContainer];
		readConfigFillArrays(currentContainer, text[currentContainer], &textColor[currentContainer], command[currentContainer], drawableCommand[currentContainer]);
	}
	char _systemCommand[containerAmount][drawableCommandMaxLength + drawableCommandPathLength + 2];
	const char *systemCommand[containerAmount];
	{
		unsigned int element;
		unsigned int currentCharacter;
		for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
			element = 0;
			currentCharacter = 0;
			if(drawableCommand[currentContainer][currentCharacter]){
				while(drawableCommand[currentContainer][currentCharacter]){
					_systemCommand[currentContainer][element] = drawableCommand[currentContainer][currentCharacter];
					++element;
					++currentCharacter;
				}
				_systemCommand[currentContainer][element] = '>';
				++element;
				currentCharacter = 0;
				while(currentCharacter < drawableCommandPathLength){
					_systemCommand[currentContainer][element] = drawableCommandPath[currentCharacter];
					++element;
					++currentCharacter;
				}
			}
			_systemCommand[currentContainer][element] = '\0';
			systemCommand[currentContainer] = _systemCommand[currentContainer];
		}
	}
	Shortcut hide;
	Shortcut peek;
	Shortcut restart;
	Shortcut exit;
	readConfigShortcuts(&hide, &peek, &restart, &exit);
	grabKeys(hide, peek, restart, exit);
	for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
		for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
			readConfigButton(container[currentMonitor][currentContainer], currentContainer);
		}
	}
	bool hasBeenExposed = 0;
	XEvent event;
	bool topLevelWindowsMapped = 1;
	bool topLevelWindowsShown = 0;
	unsigned int topLevelWindowX[monitorAmount];
	unsigned int topLevelWindowY[monitorAmount];
	{
		XWindowAttributes windowAttributes;
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			XGetWindowAttributes(display, topLevelWindow[currentMonitor], &windowAttributes);
			topLevelWindowX[currentMonitor] = windowAttributes.x;
			topLevelWindowY[currentMonitor] = windowAttributes.y;
		}
	}
	const XFontSet fontSet = createFontSet();
	int textOffsetX[containerAmount];
	int textOffsetY[containerAmount];
	int drawableCommandOffsetX[containerAmount];
	int drawableCommandOffsetY[containerAmount];
	readConfigFontOffsets(textOffsetX, textOffsetY, drawableCommandOffsetX, drawableCommandOffsetY);
	GC gc[monitorAmount];
	{
		XGCValues gcValues = {
			.subwindow_mode = IncludeInferiors
		};
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			gc[currentMonitor] = XCreateGC(display, topLevelWindow[currentMonitor], GCSubwindowMode, &gcValues);
			if(!gc[currentMonitor]){
				fprintf(stderr, "%s: could not create graphics context for monitor %u\n", programName, currentMonitor);
			}
		}
	}
	int rrEventBase;
	{
		int rrErrorBase;
		XRRQueryExtension(display, &rrEventBase, &rrErrorBase);
	}
	for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
		XMapWindow(display, topLevelWindow[currentMonitor]);
	}
	for(;;){
		XNextEvent(display, &event);
		if(event.type == KeyPress){
			if(event.xkey.keycode == hide.keycode && event.xkey.state == hide.masks){
				hideToggle(&topLevelWindowsMapped, &topLevelWindowsShown, topLevelWindowX, topLevelWindowY);
			}else if(event.xkey.keycode == peek.keycode && event.xkey.state == peek.masks){
				if(!topLevelWindowsMapped){
					for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
						XMoveWindow(display, topLevelWindow[currentMonitor], topLevelWindowX[currentMonitor], topLevelWindowY[currentMonitor]);
						XMapWindow(display, topLevelWindow[currentMonitor]);
					}
					topLevelWindowsShown = 1;
				}
			}else if(event.xkey.keycode == restart.keycode && event.xkey.state == restart.masks){
				mode = RestartMode;
				break;
			}else if(event.xkey.keycode == exit.keycode && event.xkey.state == exit.masks){
				mode = ExitMode;
				break;
			}
		}else if(event.type == KeyRelease){
			if(topLevelWindowsShown){
				for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
					XUnmapWindow(display, topLevelWindow[currentMonitor]);
				}
				topLevelWindowsShown = 0;
			}
		}else if(event.type == ButtonPress){
			for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
				for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
					if(event.xbutton.window == container[currentMonitor][currentContainer]){
						if(*drawableCommand[currentContainer]){
							drawCommand(systemCommand[currentContainer], container[currentMonitor][currentContainer], fontSet, drawableCommandOffsetX[currentContainer], drawableCommandOffsetY[currentContainer], gc[currentMonitor], textColor[currentContainer]);
						}
						if(command[currentContainer]){
							if(isCommand("hide", command[currentContainer])){
								hideToggle(&topLevelWindowsMapped, &topLevelWindowsShown, topLevelWindowX, topLevelWindowY);
							}else if(isCommand("restart", command[currentContainer])){
								mode = RestartMode;
							}else if(isCommand("exit", command[currentContainer])){
								mode = ExitMode;
							}else{
								system(command[currentContainer]);
							}
						}
						currentMonitor = monitorAmount;
						break;
					}
				}
			}
			if(mode != ContinueMode){
				break;
			}
		}else if(event.type == Expose){
			if(!hasBeenExposed){
				onExpose(container, text, fontSet, textOffsetX, textOffsetY, gc, textColor);
				hasBeenExposed = 1;
			}else{
				if(!XPending(display)){
					hasBeenExposed = 0;
				}
			}
		}else if(event.type == rrEventBase + RRScreenChangeNotify){
			mode = RestartMode;
			break;
		}
	}
	for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
		if(gc[currentMonitor]){
			XFreeGC(display, gc[currentMonitor]);
		}
	}
	if(fontSet){
		XFreeFontSet(display, fontSet);
	}
	ungrabKeys(hide, peek, restart, exit);
	return;
}
static void grabKeys(Shortcut hide, Shortcut peek, Shortcut restart, Shortcut exit){
	if(hide.keycode != AnyKey){
		XGrabKey(display, hide.keycode, hide.masks, XDefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
	}
	if(peek.keycode != AnyKey){
		XGrabKey(display, peek.keycode, peek.masks, XDefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
	}
	if(restart.keycode != AnyKey){
		XGrabKey(display, restart.keycode, restart.masks, XDefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
	}
	if(exit.keycode != AnyKey){
		XGrabKey(display, exit.keycode, exit.masks, XDefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
	}
	return;
}
static XFontSet createFontSet(void){
	XFontSet fontSet = NULL;
	unsigned int fontAmount;
	readConfigFontAmount(&fontAmount);
	if(fontAmount){
		unsigned int userFontLength[fontAmount];
		readConfigFontLength(fontAmount, userFontLength);
		unsigned int setLength = fontAmount;
		setLength -= 1;
		unsigned int currentFont;
		for(currentFont = 0; currentFont < fontAmount; ++currentFont){
			setLength += userFontLength[currentFont];
		}
		char set[setLength + 1];
		{
			unsigned int element = 0;
			for(currentFont = 0; currentFont < fontAmount; ++currentFont){
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
			if(!fontSet){
				fprintf(stderr, "%s: could not create fontset\n", programName);
			}
		}
		for(currentFont = 0; currentFont < missingAmount; ++currentFont){
			fprintf(stderr, "%s: missing font: %s\n", programName, missingFont[currentFont]);
		}
		if(missingFont){
			XFreeStringList(missingFont);
		}
	}
	return fontSet;
}
static void drawCommand(const char *const systemCommand, const Window container, const XFontSet fontSet, const unsigned int drawableCommandOffsetX, const unsigned int drawableCommandOffsetY, const GC gc, const uint32_t textColor){
	if(fontSet && gc){
		system(systemCommand);
		FILE *const file = fopen(drawableCommandPath, "r");
		if(file){
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
					XRectangle overallSize;
					XmbTextExtents(fontSet, line, length, NULL, &overallSize);
					XWindowAttributes windowAttributes;
					XGetWindowAttributes(display, container, &windowAttributes);
					x = windowAttributes.width;
					x -= overallSize.width;
					x /= 2;
					x += drawableCommandOffsetX;
					y = windowAttributes.height;
					y += overallSize.height;
					y /= 2;
					y += drawableCommandOffsetY;
				}
				XSetForeground(display, gc, textColor);
				XClearWindow(display, container);
				XmbDrawString(display, container, fontSet, gc, x, y, line, length);
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
		if(vector[element] >= 'A' && vector[element] <= 'Z'){
			if(!(vector[element] == command[element] || vector[element] == command[element] + 32)){
				element = 0;
				break;
			}
		}else if(vector[element] >= 'a' && vector[element] <= 'z'){
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
static void hideToggle(bool *const topLevelWindowsMapped, bool *const topLevelWindowsShown, unsigned int *const topLevelWindowX, unsigned int *const topLevelWindowY){
	if(*topLevelWindowsMapped){
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			XUnmapWindow(display, topLevelWindow[currentMonitor]);
		}
		*topLevelWindowsMapped = 0;
	}else{
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			XMoveWindow(display, topLevelWindow[currentMonitor], topLevelWindowX[currentMonitor], topLevelWindowY[currentMonitor]);
			XMapWindow(display, topLevelWindow[currentMonitor]);
		}
		*topLevelWindowsMapped = 1;
		*topLevelWindowsShown = 0;
	}
	return;
}
static void onExpose(const Window *const *const container, char *const *const text, const XFontSet fontSet, const int *const textOffsetX, const int *const textOffsetY, const GC *gc, const uint32_t *const textColor){
	if(containerAmount > 0 && fontSet && gc){
		unsigned int currentContainer;
		unsigned int length;
		XRectangle overallSize;
		XWindowAttributes windowAttributes;
		int x;
		int y;
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
				if(text[currentContainer]){
					length = 0;
					while(text[currentContainer][length]){
						++length;
					}
					XmbTextExtents(fontSet, line, length, NULL, &overallSize);
					XGetWindowAttributes(display, container[currentMonitor][currentContainer], &windowAttributes);
					x = windowAttributes.width;
					x -= overallSize.width;
					x /= 2;
					x += textOffsetX[currentContainer];
					y = windowAttributes.height;
					y += overallSize.height;
					y /= 2;
					y += textOffsetY[currentContainer];
					XSetForeground(display, gc[currentMonitor], textColor[currentContainer]);
					XClearWindow(display, container[currentMonitor][currentContainer]);
					XmbDrawString(display, container[currentMonitor][currentContainer], fontSet, gc[currentMonitor], x, y, text[currentContainer], length);
				}
			}
		}
	}
	return;
}
static void ungrabKeys(Shortcut hide, Shortcut peek, Shortcut restart, Shortcut exit){
	if(hide.keycode != AnyKey){
		XUngrabKey(display, hide.keycode, hide.masks, XDefaultRootWindow(display));
	}
	if(peek.keycode != AnyKey){
		XUngrabKey(display, peek.keycode, peek.masks, XDefaultRootWindow(display));
	}
	if(restart.keycode != AnyKey){
		XUngrabKey(display, restart.keycode, restart.masks, XDefaultRootWindow(display));
	}
	if(exit.keycode != AnyKey){
		XUngrabKey(display, exit.keycode, exit.masks, XDefaultRootWindow(display));
	}
	return;
}
