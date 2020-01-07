#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include "headers/readConfig.h"
#include "headers/defines.h"

extern const char *restrict programName;
extern const char *restrict configPath;
extern const char *restrict workplacePath;
extern unsigned int mode;
extern Display *restrict display;
extern unsigned int monitorAmount;
extern FILE *restrict file;
extern size_t characters;
extern unsigned int totalBoxAmount;
extern char *line;
extern Window *restrict topLevelWindowArray;
extern unsigned int currentMonitor;

static void drawCommand(const Window *const restrict topLevelWindow, const char *const restrict systemCommandArray, const char *const restrict drawableCommandPathArray, const Window *const restrict box, const char *const restrict drawableCommand2DRemappedArray, const bytes4 *const restrict textColor);
static bool isCommand(const char *const restrict command, const char *const restrict commandArray);
static void onExpose(const Window *const restrict topLevelWindow, const Window *const restrict boxArray, const char *const restrict text2DRemappedArray, const unsigned int *const restrict textMaxWordLength, const bytes4 *const restrict textColorArray);

void eventLoop(void){
	unsigned int currentBox;
	Window box[monitorAmount][totalBoxAmount];
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
	char *allocatedText[totalBoxAmount];
	bytes4 textColor[totalBoxAmount];
	char *allocatedCommand[totalBoxAmount];
	char *allocatedDrawableCommand[totalBoxAmount];
	for(currentBox = 0; currentBox < totalBoxAmount; ++currentBox){
		readConfigTextCommands(&box[0][currentBox], &currentBox, &allocatedText[currentBox], &textColor[currentBox], &allocatedCommand[currentBox], &allocatedDrawableCommand[currentBox]);
	}
	unsigned int textMaxWordLength = 0;
	unsigned int commandMaxWordLength = 0;
	unsigned int drawableCommandMaxWordLength = 0;
	{
		unsigned int copy;
		for(currentBox = 0; currentBox < totalBoxAmount; ++currentBox){
			if(allocatedText[currentBox]){
				copy = 0;
				while(allocatedText[currentBox][copy] != '\0'){
					++copy;
				}
				if(copy > textMaxWordLength){
					textMaxWordLength = copy;
				}
			}
			if(allocatedCommand[currentBox]){
				copy = 0;
				while(allocatedCommand[currentBox][copy] != '\0'){
					++copy;
				}
				if(copy > commandMaxWordLength){
					commandMaxWordLength = copy;
				}
			}
			if(allocatedDrawableCommand[currentBox]){
				copy = 0;
				while(allocatedDrawableCommand[currentBox][copy] != '\0'){
					++copy;
				}
				if(copy > drawableCommandMaxWordLength){
					drawableCommandMaxWordLength = copy;
				}
			}
		}
		++textMaxWordLength;
		++commandMaxWordLength;
		++drawableCommandMaxWordLength;
	}
	unsigned int drawableCommandPathLength = 0;
	{
		if(workplacePath){
			while(workplacePath[drawableCommandPathLength] != '\0'){
				++drawableCommandPathLength;
			}
		}else{
			while(configPath[drawableCommandPathLength] != '\0'){
				++drawableCommandPathLength;
			}
			while(configPath[drawableCommandPathLength] != '/'){
				--drawableCommandPathLength;
			}
		}
		drawableCommandPathLength += 17;
	}
	char drawableCommandPath[drawableCommandPathLength];
	{
		unsigned int currentCharacter = 0;
		{
			unsigned int lastSlash = drawableCommandPathLength - 17;
			while(currentCharacter < lastSlash){
				drawableCommandPath[currentCharacter] = configPath[currentCharacter];
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
		drawableCommandMaxWordLength += drawableCommandPathLength;
	}
	char text2DRemappedArray[totalBoxAmount * textMaxWordLength];
	char command2DRemappedArray[totalBoxAmount * commandMaxWordLength];
	char drawableCommand2DRemappedArray[totalBoxAmount * drawableCommandMaxWordLength];
	{
		unsigned int currentCharacter;
		unsigned int wordBeginning = 0;
		for(currentBox = 0; currentBox < totalBoxAmount; ++currentBox){
			currentCharacter = 0;
			if(allocatedText[currentBox]){
				while(allocatedText[currentBox][currentCharacter] != '\0'){
					text2DRemappedArray[wordBeginning + currentCharacter] = allocatedText[currentBox][currentCharacter];
					++currentCharacter;
				}
			}
			text2DRemappedArray[wordBeginning + currentCharacter] = '\0';
			wordBeginning += textMaxWordLength;
		}
		wordBeginning = 0;
		for(currentBox = 0; currentBox < totalBoxAmount; ++currentBox){
			currentCharacter = 0;
			if(allocatedCommand[currentBox]){
				while(allocatedCommand[currentBox][currentCharacter] != '\0'){
					command2DRemappedArray[wordBeginning + currentCharacter] = allocatedCommand[currentBox][currentCharacter];
					++currentCharacter;
				}
			}
			command2DRemappedArray[wordBeginning + currentCharacter] = '\0';
			wordBeginning += commandMaxWordLength;
		}
		wordBeginning = 0;
		unsigned int currentCharacterRight;
		for(currentBox = 0; currentBox < totalBoxAmount; ++currentBox){
			currentCharacter = 0;
			if(allocatedDrawableCommand[currentBox]){
				currentCharacterRight = 0;
				while(allocatedDrawableCommand[currentBox][currentCharacterRight] != '\0'){
					drawableCommand2DRemappedArray[wordBeginning + currentCharacter] = allocatedDrawableCommand[currentBox][currentCharacterRight];
					++currentCharacter;
					++currentCharacterRight;
				}
				drawableCommand2DRemappedArray[wordBeginning + currentCharacter] = '>';
				++currentCharacter;
				currentCharacterRight = 0;
				while(currentCharacterRight < drawableCommandPathLength){
					drawableCommand2DRemappedArray[wordBeginning + currentCharacter] = drawableCommandPath[currentCharacterRight];
					++currentCharacter;
					++currentCharacterRight;
				}
			}
			drawableCommand2DRemappedArray[wordBeginning + currentCharacter] = '\0';
			wordBeginning += drawableCommandMaxWordLength;
		}
	}
	for(currentBox = 0; currentBox < totalBoxAmount; ++currentBox){
		free(allocatedText[currentBox]);
		free(allocatedCommand[currentBox]);
		free(allocatedDrawableCommand[currentBox]);
	}
	for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
		for(currentBox = 0; currentBox < totalBoxAmount; ++currentBox){
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
	unsigned int topLevelWindowArrayMapped = 1;
	unsigned int commandWordBeginning;
	for(;;){
		XNextEvent(display, &event);
		if(event.type == KeyPress){
			if(topLevelWindowArrayMapped){
				for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
					XUnmapWindow(display, topLevelWindowArray[currentMonitor]);
				}
				topLevelWindowArrayMapped = 0;
			}else{
				for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
					XMapWindow(display, topLevelWindowArray[currentMonitor]);
				}
				topLevelWindowArrayMapped = 1;
			}
		}else if(event.type == ButtonPress){
			for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
				commandWordBeginning = 0;
				for(currentBox = 0; currentBox < totalBoxAmount; ++currentBox){
					if(event.xbutton.window == box[currentMonitor][currentBox]){
						if(drawableCommand2DRemappedArray[currentBox * drawableCommandMaxWordLength]){
							drawCommand(&topLevelWindowArray[currentMonitor], drawableCommand2DRemappedArray + currentBox * drawableCommandMaxWordLength, drawableCommandPath, &box[currentMonitor][currentBox], drawableCommand2DRemappedArray, &textColor[currentBox]);
						}
						if(command2DRemappedArray[commandWordBeginning]){
							if(isCommand("Restart", &command2DRemappedArray[commandWordBeginning])){
								mode = ModeRestart;
							}else if(isCommand("Exit", &command2DRemappedArray[commandWordBeginning])){
								mode = ModeExit;
							}else{
								system(&command2DRemappedArray[commandWordBeginning]);
							}
						}
						currentMonitor = monitorAmount;
						break;
					}
					commandWordBeginning += commandMaxWordLength;
				}
			}
			if(mode == ModeRestart || mode == ModeExit){
				break;
			}
		}else if(event.type == Expose){
			for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
				onExpose(&topLevelWindowArray[currentMonitor], box[currentMonitor], text2DRemappedArray, &textMaxWordLength, textColor);
			}
		}else if(event.type == rrEventBase + RRScreenChangeNotify){
			mode = ModeRestart;
			break;
		}
	}
	XUngrabKeyboard(display, CurrentTime);
	return;
}
static void drawCommand(const Window *const restrict topLevelWindow, const char *const restrict systemCommandArray, const char *const restrict drawableCommandPathArray, const Window *const restrict box, const char *const restrict drawableCommand2DRemappedArray, const bytes4 *const restrict textColor){
	system(systemCommandArray);
	if((file = fopen(drawableCommandPathArray, "r"))){
		getline(&line, &characters, file);
		fclose(file);
		unsigned int lineLength = 0;
		while(line[lineLength] != '\0'){
			++lineLength;
		}
		if(lineLength > 1){
			--lineLength;
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
			{
				XCharStruct charStruct;
				{
					XFontStruct *restrict font = XLoadQueryFont(display, "fixed");
					int direction;
					XTextExtents(font, drawableCommand2DRemappedArray, lineLength, &direction, (int *)&charStruct.ascent, (int *)&charStruct.descent, &charStruct);
					XFreeFont(display, font);
				}
				XWindowAttributes windowAttributes;
				XGetWindowAttributes(display, *box, &windowAttributes);
				x = windowAttributes.width;
				x -= charStruct.width;
				x /= 2;
				y = windowAttributes.height;
				y += charStruct.ascent;
				y /= 2;
			}
			XClearWindow(display, *box);
			XDrawString(display, *box, gc, x, y, line, lineLength);
			XFreeGC(display, gc);
		}
	}else{
		fprintf(stdout, "%s: could not read temporary file (drawableCommand)\n", programName);
	}
	return;
}
static bool isCommand(const char *const restrict command, const char *const restrict commandArray){
	bool value = 0;
	unsigned int element = 0;
	while(command[element] != '\0'){
		if(command[element] >= 'A' && command[element] <= 'Z'){
			if(!(commandArray[element] == command[element] || commandArray[element] == command[element] + 32)){
				element = 0;
				break;
			}
		}else if(command[element] >= 'a' && command[element] <= 'z'){
			if(!(commandArray[element] == command[element] || commandArray[element] == command[element] - 32)){
				element = 0;
				break;
			}
		}else{
			if(!(commandArray[element] == command[element])){
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
static void onExpose(const Window *const restrict topLevelWindow, const Window *const restrict boxArray, const char *const restrict text2DRemappedArray, const unsigned int *const restrict textMaxWordLength, const bytes4 *const restrict textColorArray){
	if(totalBoxAmount > 0){
		GC gc;
		{
			XGCValues gcValues = {
				.subwindow_mode = IncludeInferiors
			};
			gc = XCreateGC(display, *topLevelWindow, GCSubwindowMode, &gcValues);
		}
		const unsigned int dereferencedTextMaxWordLength = *textMaxWordLength;
		unsigned int wordBeginning = 0;
		XFontStruct *restrict font = XLoadQueryFont(display, "fixed");
		unsigned int actualWordLength;
		int direction;
		XCharStruct charStruct;
		XWindowAttributes windowAttributes;
		int x;
		int y;
		for(unsigned int currentBox = 0; currentBox < totalBoxAmount; ++currentBox){
			if(text2DRemappedArray[wordBeginning]){
				XSetForeground(display, gc, textColorArray[currentBox]);
				actualWordLength = 0;
				while(text2DRemappedArray[wordBeginning + actualWordLength] != '\0'){
					++actualWordLength;
				}
				XTextExtents(font, &text2DRemappedArray[wordBeginning], actualWordLength, &direction, (int *)&charStruct.ascent, (int *)&charStruct.descent, &charStruct);
				XGetWindowAttributes(display, boxArray[currentBox], &windowAttributes);
				x = windowAttributes.width;
				x -= charStruct.width;
				x /= 2;
				y = windowAttributes.height;
				y += charStruct.ascent;
				y /= 2;
				XClearWindow(display, boxArray[currentBox]);
				XDrawString(display, boxArray[currentBox], gc, x, y, &text2DRemappedArray[wordBeginning], actualWordLength);
			}
			wordBeginning += dereferencedTextMaxWordLength;
		}
		XFreeFont(display, font);
		XFreeGC(display, gc);
	}
	return;
}
