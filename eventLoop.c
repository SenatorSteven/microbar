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
extern Mode mode;
extern Display *display;
extern unsigned int monitorAmount;
extern unsigned int containerAmount;
extern char line[DefaultCharactersCount + 1];
extern Window *topLevelWindow;
extern unsigned int currentMonitor;
extern Window *const *container;

static void grabKeys(const unsigned int sectionShortcutAmount, const unsigned int containerShortcutAmount, const Shortcut interactAll, const Shortcut *const interactSection, const Shortcut *const interactContainer, const Shortcut hide, const Shortcut peek, const Shortcut restart, const Shortcut exit);
static void grabButtons(void);
static XFontSet createFontSet(void);
static GC createGC(void);
static void executeCommands(const char drawableCommand, const char *const systemCommand, const Window container, const XFontSet fontSet, const GC gc, const ARGB drawableCommandColor, const unsigned int drawableCommandOffsetX, const unsigned int drawableCommandOffsetY, const char *const command, const unsigned int *const topLevelWindowX, const unsigned int *const topLevelWindowY, bool *const topLevelWindowsMapped, bool *const topLevelWindowsShown);
static void drawCommand(const char *const systemCommand, const Window container, const XFontSet fontSet, const GC gc, const ARGB drawableCommandColor, const int drawableCommandOffsetX, const int drawableCommandOffsetY);
static bool isCommand(const char *const command, const char *const vector);
static void hideToggle(const unsigned int *const topLevelWindowX, const unsigned int *const topLevelWindowY, bool *const topLevelWindowsMapped, bool *const topLevelWindowsShown);
static void expose(const char *const *const text, const XFontSet fontSet, const int *const textOffsetX, const int *const textOffsetY, const GC gc, const ARGB *const textColor, const char *const *const systemCommand, const ARGB *const drawableCommandColor, const int *const drawableCommandOffsetX, const int *const drawableCommandOffsetY);
static void ungrabButtons(void);
static void ungrabKeys(const unsigned int sectionShortcutAmount, const unsigned int containerShortcutAmount, const Shortcut interactAll, const Shortcut *const interactSection, const Shortcut *const interactContainer, const Shortcut hide, const Shortcut peek, const Shortcut restart, const Shortcut exit);

void eventLoop(void){
	unsigned int textMaxLength;
	unsigned int commandMaxLength;
	unsigned int drawableCommandMaxLength;
	{
		unsigned int *_uintArray[1][3] = {{&textMaxLength, &commandMaxLength, &drawableCommandMaxLength}};
		unsigned int **uintArray[1] = {*_uintArray};
		ConfigInfo configInfo = {
			.integer = NULL,
			.unsignedInteger = uintArray,
			.unsignedIntegerDimension0 = 1,
			.unsignedIntegerDimension1 = 3,
			.unsignedIntegerDimension2 = 1,
			.argb = NULL,
			.character = NULL
		};
		if(!readConfig(ArrayLengthsConfigMode, configInfo)){
			fprintf(stderr, "%s: could not read array lengths\n", programName);
		}
	}
	char _text[containerAmount][textMaxLength + 1];
	char _command[containerAmount][commandMaxLength + 1];
	char _drawableCommand[containerAmount][drawableCommandMaxLength + 1];
	char *text[containerAmount];
	ARGB textColor[containerAmount];
	char *command[containerAmount];
	char *drawableCommand[containerAmount];
	ARGB drawableCommandColor[containerAmount];
	for(unsigned int currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
		text[currentContainer] = _text[currentContainer];
		command[currentContainer] = _command[currentContainer];
		drawableCommand[currentContainer] = _drawableCommand[currentContainer];
	}
	{
		ARGB *_argbArray[1][2] = {{textColor, drawableCommandColor}};
		ARGB **argbArray[1] = {*_argbArray};
		char **charArray[3] = {text, command, drawableCommand};
		ConfigInfo configInfo = {
			.integer = NULL,
			.unsignedInteger = NULL,
			.argb = argbArray,
			.argbDimension0 = 1,
			.argbDimension1 = 2,
			.argbDimension2 = containerAmount,
			.character = charArray,
			.characterDimension0 = 3,
			.characterDimension1 = containerAmount,
			.characterDimension2 = 1
		};
		if(!readConfig(FillArraysConfigMode, configInfo)){
			fprintf(stderr, "%s: could not fill arrays\n", programName);
		}
	}
	char _systemCommand[containerAmount][drawableCommandMaxLength + drawableCommandPathLength + 2];
	const char *systemCommand[containerAmount];
	{
		unsigned int element;
		unsigned int currentCharacter;
		for(unsigned int currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
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
	unsigned int sectionShortcutAmount;
	unsigned int containerShortcutAmount;
	{
		unsigned int *_uintArray[1][2] = {{&sectionShortcutAmount, &containerShortcutAmount}};
		unsigned int **uintArray[1] = {*_uintArray};
		ConfigInfo configInfo = {
			.integer = NULL,
			.unsignedInteger = uintArray,
			.unsignedIntegerDimension0 = 1,
			.unsignedIntegerDimension1 = 2,
			.unsignedIntegerDimension2 = 1,
			.argb = NULL,
			.character = NULL
		};
		if(!readConfig(VariableShortcutsConfigMode, configInfo)){
			fprintf(stderr, "%s: could not read variable shortcuts\n", programName);
		}
	}
	Shortcut interactAll;
	Shortcut interactSection[sectionShortcutAmount];
	unsigned int sectionNumber[sectionShortcutAmount];
	Shortcut interactContainer[containerShortcutAmount];
	unsigned int containerNumber[containerShortcutAmount];
	Shortcut hide;
	Shortcut peek;
	Shortcut restart;
	Shortcut exit;
	if(readConfigShortcuts(sectionShortcutAmount, containerShortcutAmount, &interactAll, interactSection, sectionNumber, interactContainer, containerNumber, &hide, &peek, &restart, &exit)){
		grabKeys(sectionShortcutAmount, containerShortcutAmount, interactAll, interactSection, interactContainer, hide, peek, restart, exit);
	}else{
		fprintf(stderr, "%s: could not read shortcuts\n", programName);
	}
	grabButtons();
	{
		XEvent event;
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
		bool topLevelWindowsMapped = 1;
		bool topLevelWindowsShown = 0;
		unsigned int currentContainer;
		const XFontSet fontSet = createFontSet();
		int textOffsetX[containerAmount];
		int textOffsetY[containerAmount];
		int drawableCommandOffsetX[containerAmount];
		int drawableCommandOffsetY[containerAmount];
		{
			int *_intArray[1][4] = {{textOffsetX, textOffsetY, drawableCommandOffsetX, drawableCommandOffsetY}};
			int **intArray[1] = {*_intArray};
			ConfigInfo configInfo = {
				.integer = intArray,
				.integerDimension0 = 1,
				.integerDimension1 = 4,
				.integerDimension2 = containerAmount,
				.unsignedInteger = NULL,
				.argb = NULL,
				.character = NULL
			};
			if(!readConfig(FontOffsetsConfigMode, configInfo)){
				fprintf(stderr, "%s: could not read font offsets\n", programName);
			}
		}
		const GC gc = createGC();
		unsigned int currentShortcut;
		unsigned int currentSection;
		unsigned int startingPoint;
		unsigned int sectionAmount;
		unsigned int endingPoint;
		{
			unsigned int rectangleAmount;
			unsigned int *_uintArray[1][2] = {{&sectionAmount, &rectangleAmount}};
			unsigned int **uintArray[1] = {*_uintArray};
			ConfigInfo configInfo = {
				.integer = NULL,
				.unsignedInteger = uintArray,
				.unsignedIntegerDimension0 = 1,
				.unsignedIntegerDimension1 = 2,
				.unsignedIntegerDimension2 = 1,
				.argb = NULL,
				.character = NULL
			};
			if(!readConfig(SectionRectangleAmountConfigMode, configInfo)){
				fprintf(stderr, "%s: could not read section rectangle amount\n", programName);
			}
		}
		unsigned int sectionChildrenAmount[sectionAmount];
		{
			unsigned int *_uintArray[1][1] = {{sectionChildrenAmount}};
			unsigned int **uintArray[1] = {*_uintArray};
			ConfigInfo configInfo = {
				.integer = NULL,
				.unsignedInteger = uintArray,
				.unsignedIntegerDimension0 = 1,
				.unsignedIntegerDimension1 = 1,
				.unsignedIntegerDimension2 = sectionAmount,
				.argb = NULL,
				.character = NULL
			};
			if(!readConfig(SectionChildrenConfigMode, configInfo)){
				fprintf(stderr, "%s: could not read section children amount\n", programName);
			}
		}
		int rrEventBase;
		{
			int rrErrorBase;
			if(!XRRQueryExtension(display, &rrEventBase, &rrErrorBase)){
				fprintf(stderr, "%s: could not query the xrandr extension\n", programName);
				rrEventBase = 0;
			}
		}
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			XMapWindow(display, topLevelWindow[currentMonitor]);
		}
		goto expose;
		for(;;){
			XNextEvent(display, &event);
			if(event.type == KeyPress){
				if(event.xkey.keycode == interactAll.keycode && event.xkey.state == interactAll.masks){
					for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
						for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
							executeCommands(*drawableCommand[currentContainer], systemCommand[currentContainer], container[currentMonitor][currentContainer], fontSet, gc, drawableCommandColor[currentContainer], drawableCommandOffsetX[currentContainer], drawableCommandOffsetY[currentContainer], command[currentContainer], topLevelWindowX, topLevelWindowY, &topLevelWindowsMapped, &topLevelWindowsShown);
							if(mode != ContinueMode){
								currentMonitor = monitorAmount;
								break;
							}
						}
					}
					if(mode != ContinueMode){
						break;
					}
				}else if(event.xkey.keycode == hide.keycode && event.xkey.state == hide.masks){
					hideToggle(topLevelWindowX, topLevelWindowY, &topLevelWindowsMapped, &topLevelWindowsShown);
					if(topLevelWindowsMapped){
						goto expose;
					}
				}else if(event.xkey.keycode == peek.keycode && event.xkey.state == peek.masks){
					if(!topLevelWindowsMapped){
						for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
							XMoveWindow(display, topLevelWindow[currentMonitor], topLevelWindowX[currentMonitor], topLevelWindowY[currentMonitor]);
							XMapWindow(display, topLevelWindow[currentMonitor]);
						}
						topLevelWindowsShown = 1;
					}
					goto expose;
				}else if(event.xkey.keycode == restart.keycode && event.xkey.state == restart.masks){
					mode = RestartMode;
					break;
				}else if(event.xkey.keycode == exit.keycode && event.xkey.state == exit.masks){
					mode = ExitMode;
					break;
				}else{
					for(currentShortcut = 0; currentShortcut < sectionShortcutAmount; ++currentShortcut){
						if(event.xkey.keycode == interactSection[currentShortcut].keycode && event.xkey.state == interactSection[currentShortcut].masks){
							startingPoint = 0;
							for(currentSection = 0; currentSection < sectionNumber[currentShortcut]; ++currentSection){
								startingPoint += sectionChildrenAmount[currentSection];
							}
							endingPoint = startingPoint;
							endingPoint += sectionChildrenAmount[sectionNumber[currentShortcut]];
							for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
								for(currentContainer = startingPoint; currentContainer < endingPoint; ++currentContainer){
									executeCommands(*drawableCommand[currentContainer], systemCommand[currentContainer], container[currentMonitor][currentContainer], fontSet, gc, drawableCommandColor[currentContainer], drawableCommandOffsetX[currentContainer], drawableCommandOffsetY[currentContainer], command[currentContainer], topLevelWindowX, topLevelWindowY, &topLevelWindowsMapped, &topLevelWindowsShown);
									if(mode != ContinueMode){
										currentMonitor = monitorAmount;
										break;
									}
								}
							}
							currentShortcut = sectionShortcutAmount;
							currentShortcut += 1;
							break;
						}
					}
					if(currentShortcut <= sectionShortcutAmount){
						for(currentShortcut = 0; currentShortcut < containerShortcutAmount; ++currentShortcut){
							if(event.xkey.keycode == interactContainer[currentShortcut].keycode && event.xkey.state == interactContainer[currentShortcut].masks){
								currentContainer = containerNumber[currentShortcut];
								for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
									executeCommands(*drawableCommand[currentContainer], systemCommand[currentContainer], container[currentMonitor][currentContainer], fontSet, gc, drawableCommandColor[currentContainer], drawableCommandOffsetX[currentContainer], drawableCommandOffsetY[currentContainer], command[currentContainer], topLevelWindowX, topLevelWindowY, &topLevelWindowsMapped, &topLevelWindowsShown);
									if(mode != ContinueMode){
										break;
									}
								}
								break;
							}
						}
					}
					if(mode != ContinueMode){
						break;
					}
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
							executeCommands(*drawableCommand[currentContainer], systemCommand[currentContainer], container[currentMonitor][currentContainer], fontSet, gc, drawableCommandColor[currentContainer], drawableCommandOffsetX[currentContainer], drawableCommandOffsetY[currentContainer], command[currentContainer], topLevelWindowX, topLevelWindowY, &topLevelWindowsMapped, &topLevelWindowsShown);
							currentMonitor = monitorAmount;
							break;
						}
					}
				}
				if(mode != ContinueMode){
					break;
				}
			}else if(event.type == Expose){
				if(!event.xexpose.count){
					expose:{
						XSync(display, False);
						expose((const char *const *const)text, fontSet, textOffsetX, textOffsetY, gc, textColor, systemCommand, drawableCommandColor, drawableCommandOffsetX, drawableCommandOffsetY);
					}
				}
			}else if(event.type == ClientMessage){
				if(event.xclient.message_type == XInternAtom(display, "ALL", False)){
					for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
						for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
							executeCommands(*drawableCommand[currentContainer], systemCommand[currentContainer], container[currentMonitor][currentContainer], fontSet, gc, drawableCommandColor[currentContainer], drawableCommandOffsetX[currentContainer], drawableCommandOffsetY[currentContainer], command[currentContainer], topLevelWindowX, topLevelWindowY, &topLevelWindowsMapped, &topLevelWindowsShown);
							if(mode != ContinueMode){
								currentMonitor = monitorAmount;
								break;
							}
						}
					}
					if(mode != ContinueMode){
						break;
					}
				}else if(event.xclient.message_type == XInternAtom(display, "SECTION", False)){
					char *const name = XGetAtomName(display, event.xclient.data.l[0]);
					if(name){
						currentSection = 0;
						{
							unsigned int element = 0;
							while(name[element]){
								currentSection *= 10;
								currentSection += name[element];
								currentSection -= 48;
								++element;
							}
						}
						XFree(name);
						if(currentSection < sectionAmount){
							startingPoint = 0;
							{
								unsigned int currentChildrenAmount;
								for(currentChildrenAmount = 0; currentChildrenAmount < currentSection; ++currentChildrenAmount){
									startingPoint += sectionChildrenAmount[currentChildrenAmount];
								}
								endingPoint = startingPoint;
								endingPoint += sectionChildrenAmount[currentChildrenAmount];
							}
							for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
								for(currentContainer = startingPoint; currentContainer < endingPoint; ++currentContainer){
									executeCommands(*drawableCommand[currentContainer], systemCommand[currentContainer], container[currentMonitor][currentContainer], fontSet, gc, drawableCommandColor[currentContainer], drawableCommandOffsetX[currentContainer], drawableCommandOffsetY[currentContainer], command[currentContainer], topLevelWindowX, topLevelWindowY, &topLevelWindowsMapped, &topLevelWindowsShown);
									if(mode != ContinueMode){
										currentMonitor = monitorAmount;
										break;
									}
								}
							}
						}
					}
				}else if(event.xclient.message_type == XInternAtom(display, "CONTAINER", False)){
					char *const name = XGetAtomName(display, event.xclient.data.l[0]);
					if(name){
						currentContainer = 0;
						{
							unsigned int element = 0;
							while(name[element]){
								currentContainer *= 10;
								currentContainer += name[element];
								currentContainer -= 48;
								++element;
							}
						}
						XFree(name);
						if(currentContainer < containerAmount){
							for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
								executeCommands(*drawableCommand[currentContainer], systemCommand[currentContainer], container[currentMonitor][currentContainer], fontSet, gc, drawableCommandColor[currentContainer], drawableCommandOffsetX[currentContainer], drawableCommandOffsetY[currentContainer], command[currentContainer], topLevelWindowX, topLevelWindowY, &topLevelWindowsMapped, &topLevelWindowsShown);
								if(mode != ContinueMode){
									currentMonitor = monitorAmount;
									break;
								}
							}
						}
					}
				}
			}else if(event.type == RRScreenChangeNotify + rrEventBase){
				mode = RestartMode;
				break;
			}
		}
		if(gc){
			XFreeGC(display, gc);
		}
		if(fontSet){
			XFreeFontSet(display, fontSet);
		}
	}
	ungrabButtons();
	ungrabKeys(sectionShortcutAmount, containerShortcutAmount, interactAll, interactSection, interactContainer, hide, peek, restart, exit);
	return;
}
static void grabKeys(const unsigned int sectionShortcutAmount, const unsigned int containerShortcutAmount, const Shortcut interactAll, const Shortcut *const interactSection, const Shortcut *const interactContainer, const Shortcut hide, const Shortcut peek, const Shortcut restart, const Shortcut exit){
	if(interactAll.keycode != AnyKey){
		XGrabKey(display, interactAll.keycode, interactAll.masks, XDefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
	}
	{
		unsigned int currentShortcut;
		for(currentShortcut = 0; currentShortcut < sectionShortcutAmount; ++currentShortcut){
			if(interactSection[currentShortcut].keycode != AnyKey){
				XGrabKey(display, interactSection[currentShortcut].keycode, interactSection[currentShortcut].masks, XDefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
			}
		}
		for(currentShortcut = 0; currentShortcut < containerShortcutAmount; ++currentShortcut){
			if(interactContainer[currentShortcut].keycode != AnyKey){
				XGrabKey(display, interactContainer[currentShortcut].keycode, interactContainer[currentShortcut].masks, XDefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
			}
		}
	}
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
	XSync(display, False);
	return;
}
static void grabButtons(void){
	ConfigInfo ci;
	if(readConfig(ButtonsConfigMode, ci)){
		XSync(display, False);
	}else{
		fprintf(stderr, "%s: could not read buttons\n", programName);
	}
	return;
}
static XFontSet createFontSet(void){
	XFontSet fontSet = NULL;
	unsigned int fontAmount;
	ConfigInfo configInfo = {
		.integer = NULL,
		.argb = NULL,
		.character = NULL
	};
	{
		unsigned int *_uintArray[1][1] = {{&fontAmount}};
		unsigned int **uintArray[1] = {*_uintArray};
		configInfo.unsignedInteger = uintArray;
		configInfo.unsignedIntegerDimension0 = 1;
		configInfo.unsignedIntegerDimension1 = 1;
		configInfo.unsignedIntegerDimension2 = 1;
		if(!readConfig(FontAmountConfigMode, configInfo)){
			fontAmount = 0;
		}
	}
	if(fontAmount){
		unsigned int userFontLength[fontAmount];
		{
			unsigned int *_uintArray[1][1] = {{userFontLength}};
			unsigned int **uintArray[1] = {*_uintArray};
			configInfo.unsignedInteger = uintArray;
			configInfo.unsignedIntegerDimension2 = fontAmount;
			if(!readConfig(FontLengthConfigMode, configInfo)){
				fontAmount = 0;
			}
		}
		if(fontAmount){
			unsigned int setLength = fontAmount;
			--setLength;
			unsigned int currentFont;
			for(currentFont = 0; currentFont < fontAmount; ++currentFont){
				setLength += userFontLength[currentFont];
			}
			char set[setLength + 1];
			{
				char *_charArray[1][1] = {{set}};
				char **charArray[1] = {*_charArray};
				configInfo.unsignedInteger = NULL;
				configInfo.character = charArray;
				configInfo.characterDimension0 = 1;
				configInfo.characterDimension1 = 1;
				configInfo.characterDimension2 = 1;
				if(!readConfig(FontSetConfigMode, configInfo)){
					fontAmount = 0;
				}
			}
			if(fontAmount){
				char **missingFont;
				unsigned int missingAmount;
				if((fontSet = XCreateFontSet(display, set, &missingFont, (int *)&missingAmount, NULL))){
					for(currentFont = 0; currentFont < missingAmount; ++currentFont){
						fprintf(stderr, "%s: missing font: %s\n", programName, missingFont[currentFont]);
					}
					if(missingFont){
						XFreeStringList(missingFont);
					}
				}
			}
		}
	}
	if(!fontSet){
		fprintf(stderr, "%s: could not create fontset\n", programName);
	}
	return fontSet;
}
static GC createGC(void){
	GC gc = NULL;
	{
		XGCValues gcValues = {
			.subwindow_mode = IncludeInferiors
		};
		gc = XCreateGC(display, *topLevelWindow, GCSubwindowMode, &gcValues);
	}
	if(!gc){
		fprintf(stderr, "%s: could not create graphics context\n", programName);
	}
	return gc;
}
static void executeCommands(const char drawableCommand, const char *const systemCommand, const Window container, const XFontSet fontSet, const GC gc, const ARGB drawableCommandColor, const unsigned int drawableCommandOffsetX, const unsigned int drawableCommandOffsetY, const char *const command, const unsigned int *const topLevelWindowX, const unsigned int *const topLevelWindowY, bool *const topLevelWindowsMapped, bool *const topLevelWindowsShown){
	if(drawableCommand){
		drawCommand(systemCommand, container, fontSet, gc, drawableCommandColor, drawableCommandOffsetX, drawableCommandOffsetY);
	}
	if(command){
		if(isCommand("hide", command)){
			hideToggle(topLevelWindowX, topLevelWindowY, topLevelWindowsMapped, topLevelWindowsShown);
		}else if(isCommand("restart", command)){
			mode = RestartMode;
		}else if(isCommand("exit", command)){
			mode = ExitMode;
		}else{
			system(command);
		}
	}
	return;
}
static void drawCommand(const char *const systemCommand, const Window container, const XFontSet fontSet, const GC gc, const ARGB drawableCommandColor, const int drawableCommandOffsetX, const int drawableCommandOffsetY){
	if(fontSet && gc){
		system(systemCommand);
		FILE *const file = fopen(drawableCommandPath, "r");
		if(file){
			unsigned int length = 0;
			for(;;){
				line[length] = fgetc(file);
				if(line[length] == '\n' || feof(file) || length == DefaultCharactersCount){
					line[length] = '\0';
					break;
				}else{
					++length;
				}
			}
			fclose(file);
			if(length){
				int x;
				int y;
				{
					XWindowAttributes windowAttributes;
					XGetWindowAttributes(display, container, &windowAttributes);
					x = windowAttributes.width;
					y = windowAttributes.height;
				}
				{
					XRectangle overallSize;
					XmbTextExtents(fontSet, line, length, NULL, &overallSize);
					x -= overallSize.width;
					x /= 2;
					x -= overallSize.x;
					x += drawableCommandOffsetX;
					y -= overallSize.height;
					y /= 2;
					y -= overallSize.y;
					y += drawableCommandOffsetY;
				}
				XSetForeground(display, gc, drawableCommandColor);
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
	char v = *vector;
	char c = *command;
	while(v || c){
		if(v >= 'A' && v <= 'Z'){
			if(v != c && v != c - 32){
				element = 0;
				break;
			}
		}else if(v >= 'a' && v <= 'z'){
			if(v != c && v != c + 32){
				element = 0;
				break;
			}
		}else if(v != c){
			element = 0;
			break;
		}
		++element;
		v = vector[element];
		c = command[element];
	}
	if(element){
		value = 1;
	}
	return value;
}
static void hideToggle(const unsigned int *const topLevelWindowX, const unsigned int *const topLevelWindowY, bool *const topLevelWindowsMapped, bool *const topLevelWindowsShown){
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
static void expose(const char *const *const text, const XFontSet fontSet, const int *const textOffsetX, const int *const textOffsetY, const GC gc, const ARGB *const textColor, const char *const *const systemCommand, const ARGB *const drawableCommandColor, const int *const drawableCommandOffsetX, const int *const drawableCommandOffsetY){
	if(containerAmount && fontSet && gc){
		const Window *c;
		unsigned int currentContainer;
		const char *currentText;
		Window co;
		unsigned int length;
		XRectangle overallSize;
		XWindowAttributes windowAttributes;
		int x;
		int y;
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			c = container[currentMonitor];
			for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
				currentText = text[currentContainer];
				if(currentText){
					co = c[currentContainer];
					if(isCommand("drawableCommand", currentText)){
						drawCommand(systemCommand[currentContainer], co, fontSet, gc, drawableCommandColor[currentContainer], drawableCommandOffsetX[currentContainer], drawableCommandOffsetY[currentContainer]);
					}else{
						length = 0;
						while(currentText[length]){
							++length;
						}
						XmbTextExtents(fontSet, currentText, length, NULL, &overallSize);
						XGetWindowAttributes(display, co, &windowAttributes);
						x = windowAttributes.width;
						x -= overallSize.width;
						x /= 2;
						x -= overallSize.x;
						x += textOffsetX[currentContainer];
						y = windowAttributes.height;
						y -= overallSize.height;
						y /= 2;
						y -= overallSize.y;
						y += textOffsetY[currentContainer];
						XSetForeground(display, gc, textColor[currentContainer]);
						XClearWindow(display, co);
						XmbDrawString(display, co, fontSet, gc, x, y, currentText, length);
					}
				}
			}
		}
	}
	return;
}
static void ungrabButtons(void){
	unsigned int currentContainer;
	for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
		for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
			XUngrabButton(display, AnyButton, AnyModifier, container[currentMonitor][currentContainer]);
		}
	}
	XSync(display, False);
	return;
}
static void ungrabKeys(const unsigned int sectionShortcutAmount, const unsigned int containerShortcutAmount, const Shortcut interactAll, const Shortcut *const interactSection, const Shortcut *const interactContainer, const Shortcut hide, const Shortcut peek, const Shortcut restart, const Shortcut exit){
	if(interactAll.keycode != AnyKey){
		XUngrabKey(display, interactAll.keycode, interactAll.masks, XDefaultRootWindow(display));
	}
	{
		unsigned int currentShortcut;
		for(currentShortcut = 0; currentShortcut < sectionShortcutAmount; ++currentShortcut){
			if(interactSection[currentShortcut].keycode != AnyKey){
				XUngrabKey(display, interactSection[currentShortcut].keycode, interactSection[currentShortcut].masks, XDefaultRootWindow(display));
			}
		}
		for(currentShortcut = 0; currentShortcut < containerShortcutAmount; ++currentShortcut){
			if(interactContainer[currentShortcut].keycode != AnyKey){
				XUngrabKey(display, interactContainer[currentShortcut].keycode, interactContainer[currentShortcut].masks, XDefaultRootWindow(display));
			}
		}
	}
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
	XSync(display, False);
	return;
}
