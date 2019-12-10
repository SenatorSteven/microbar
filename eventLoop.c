#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include "headers/readConfig.h"

#define ModeContinue /*-*/ ((unsigned int)0)
#define ModeRestart /*--*/ ((unsigned int)1)
#define ModeExit /*-----*/ ((unsigned int)2)

static unsigned int getBoxAmount(Display *const display, const Window *const topLevelWindow);
static void drawCommand(Display *const display, const Window *const topLevelWindow, const char *const systemCommandArray, const char *const drawableCommandPathArray, const Window *const box, const char *const drawableCommand2DRemappedArray, const int *const textColor);
static void onExpose(Display *const display, const Window *const topLevelWindow, const Window *const boxArray, const unsigned int *const boxAmount, const char *const text2DRemappedArray, const unsigned int *const textMaxWordLength, const int *const textColorArray);
static unsigned int isCommandRestart(const char *const commandArray);
static unsigned int isCommandExit(const char *const commandArray);

void eventLoop(Display *const display, const char *const pathArray, const Window *const topLevelWindowArray, const unsigned int *const monitorAmount, unsigned int *const mode){
	const unsigned int dereferencedMonitorAmount = *monitorAmount;
	unsigned int currentMonitor;
	unsigned int currentBox;
	unsigned int boxAmount = getBoxAmount(display, &topLevelWindowArray[0]);
	Window box[dereferencedMonitorAmount][boxAmount];
	{
		Window rootWindow;
		Window parentWindow;
		Window *menu;
		unsigned int menuAmount;
		Window *boxArray;
		unsigned int menuBoxAmount;
		unsigned int boxNumber;
		unsigned int currentMenu;
		for(currentMonitor = 0; currentMonitor < dereferencedMonitorAmount; currentMonitor++){
			XQueryTree(display, topLevelWindowArray[currentMonitor], &rootWindow, &parentWindow, &menu, &menuAmount);
			boxNumber = 0;
			for(currentMenu = 0; currentMenu < menuAmount; currentMenu++){
				XQueryTree(display, menu[currentMenu], &rootWindow, &parentWindow, &boxArray, &menuBoxAmount);
				currentBox = 0;
				while(currentBox < menuBoxAmount){
					box[currentMonitor][boxNumber] = boxArray[currentBox];
					boxNumber++;
					currentBox++;
				}
				if(menuBoxAmount > 0){
					XFree(boxArray);
				}
			}
			if(menuAmount > 0){
				XFree(menu);
			}
		}
	}
	char *allocatedText[boxAmount];
	int textColor[boxAmount];
	char *allocatedCommand[boxAmount];
	char *allocatedDrawableCommand[boxAmount];
	for(currentBox = 0; currentBox < boxAmount; currentBox++){
		readConfigTextCommands(display, &currentMonitor, pathArray, &box[0][currentBox], &currentBox, &allocatedText[currentBox], &textColor[currentBox], &allocatedCommand[currentBox], &allocatedDrawableCommand[currentBox]);
	}
	for(currentMonitor = 0; currentMonitor < dereferencedMonitorAmount; currentMonitor++){
		for(currentBox = 0; currentBox < boxAmount; currentBox++){
			readConfigButton(display, &currentMonitor, pathArray, &box[currentMonitor][currentBox], &currentBox);
		}
	}
	unsigned int textMaxWordLength = 0;
	{
		unsigned int copy;
		for(currentBox = 0; currentBox < boxAmount; currentBox++){
			if(allocatedText[currentBox]){
				copy = 0;
				while(allocatedText[currentBox][copy] > '\0'){
					copy++;
				}
				if(copy > textMaxWordLength){
					textMaxWordLength = copy;
				}
			}
		}
		textMaxWordLength++;
	}
	char text2DRemappedArray[boxAmount * textMaxWordLength];
	{
		unsigned int currentCharacter;
		unsigned int wordBeginning = 0;
		for(currentBox = 0; currentBox < boxAmount; currentBox++){
			currentCharacter = 0;
			if(allocatedText[currentBox]){
				while(allocatedText[currentBox][currentCharacter] > '\0'){
					text2DRemappedArray[wordBeginning + currentCharacter] = allocatedText[currentBox][currentCharacter];
					currentCharacter++;
				}
			}
			while(currentCharacter < textMaxWordLength){
				text2DRemappedArray[wordBeginning + currentCharacter] = '\0';
				currentCharacter++;
			}
			wordBeginning += textMaxWordLength;
		}
	}
	for(currentBox = 0; currentBox < boxAmount; currentBox++){
		free(allocatedText[currentBox]);
	}
	unsigned int commandMaxWordLength = 0;
	{
		unsigned int copy;
		for(currentBox = 0; currentBox < boxAmount; currentBox++){
			if(allocatedCommand[currentBox]){
				copy = 0;
				while(allocatedCommand[currentBox][copy] > '\0'){
					copy++;
				}
				if(copy > commandMaxWordLength){
					commandMaxWordLength = copy;
				}
			}
		}
		commandMaxWordLength++;
	}
	char command2DRemappedArray[boxAmount * commandMaxWordLength];
	{
		unsigned int currentCharacter;
		unsigned int wordBeginning = 0;
		for(currentBox = 0; currentBox < boxAmount; currentBox++){
			currentCharacter = 0;
			if(allocatedCommand[currentBox]){
				while(allocatedCommand[currentBox][currentCharacter] > '\0'){
					command2DRemappedArray[wordBeginning + currentCharacter] = allocatedCommand[currentBox][currentCharacter];
					currentCharacter++;
				}
			}
			while(currentCharacter < commandMaxWordLength){
				command2DRemappedArray[wordBeginning + currentCharacter] = '\0';
				currentCharacter++;
			}
			wordBeginning += commandMaxWordLength;
		}
	}
	for(currentBox = 0; currentBox < boxAmount; currentBox++){
		free(allocatedCommand[currentBox]);
	}
	unsigned int drawableCommandMaxWordLength = 0;
	{
		unsigned int copy;
		for(currentBox = 0; currentBox < boxAmount; currentBox++){
			if(allocatedDrawableCommand[currentBox]){
				copy = 0;
				while(allocatedDrawableCommand[currentBox][copy] > '\0'){
					copy++;
				}
				if(copy > drawableCommandMaxWordLength){
					drawableCommandMaxWordLength = copy;
				}
			}
		}
		drawableCommandMaxWordLength++;
	}
	unsigned int drawableCommandPathLength = 0;
	{
		while(pathArray[drawableCommandPathLength] > '\0'){
			drawableCommandPathLength++;
		}
		while(pathArray[drawableCommandPathLength] != '/'){
			drawableCommandPathLength--;
		}
		drawableCommandPathLength += 17;
	}
	char drawableCommandPath[drawableCommandPathLength];
	{
		unsigned int currentCharacter = 0;
		{
			unsigned int lastSlash = drawableCommandPathLength - 17;
			while(currentCharacter < lastSlash){
				drawableCommandPath[currentCharacter] = pathArray[currentCharacter];
				currentCharacter++;
			}
		}
		drawableCommandPath[currentCharacter++] = '/';
		drawableCommandPath[currentCharacter++] = 'd';
		drawableCommandPath[currentCharacter++] = 'r';
		drawableCommandPath[currentCharacter++] = 'a';
		drawableCommandPath[currentCharacter++] = 'w';
		drawableCommandPath[currentCharacter++] = 'a';
		drawableCommandPath[currentCharacter++] = 'b';
		drawableCommandPath[currentCharacter++] = 'l';
		drawableCommandPath[currentCharacter++] = 'e';
		drawableCommandPath[currentCharacter++] = 'C';
		drawableCommandPath[currentCharacter++] = 'o';
		drawableCommandPath[currentCharacter++] = 'm';
		drawableCommandPath[currentCharacter++] = 'm';
		drawableCommandPath[currentCharacter++] = 'a';
		drawableCommandPath[currentCharacter++] = 'n';
		drawableCommandPath[currentCharacter++] = 'd';
		drawableCommandPath[currentCharacter] = '\0';
	}
	drawableCommandMaxWordLength += drawableCommandPathLength;
	char drawableCommand2DRemappedArray[boxAmount * drawableCommandMaxWordLength];
	{
		unsigned int currentCharacter;
		unsigned int wordBeginning = 0;
		unsigned int currentCharacterRight;
		for(currentBox = 0; currentBox < boxAmount; currentBox++){
			currentCharacter = 0;
			if(allocatedDrawableCommand[currentBox]){
				currentCharacterRight = 0;
				while(allocatedDrawableCommand[currentBox][currentCharacterRight] > '\0'){
					drawableCommand2DRemappedArray[wordBeginning + currentCharacter] = allocatedDrawableCommand[currentBox][currentCharacterRight];
					currentCharacter++;
					currentCharacterRight++;
				}
				drawableCommand2DRemappedArray[wordBeginning + currentCharacter++] = '>';
				currentCharacterRight = 0;
				while(currentCharacterRight < drawableCommandPathLength){
					drawableCommand2DRemappedArray[wordBeginning + currentCharacter] = drawableCommandPath[currentCharacterRight];
					currentCharacter++;
					currentCharacterRight++;
				}
			}
			while(currentCharacter < drawableCommandMaxWordLength){
				drawableCommand2DRemappedArray[wordBeginning + currentCharacter] = '\0';
				currentCharacter++;
			}
			wordBeginning += drawableCommandMaxWordLength;
		}
	}
	for(currentBox = 0; currentBox < boxAmount; currentBox++){
		free(allocatedDrawableCommand[currentBox]);
	}
	for(currentMonitor = 0; currentMonitor < dereferencedMonitorAmount; currentMonitor++){
		XMapWindow(display, topLevelWindowArray[currentMonitor]);
	}
	XAutoRepeatOff(display);
	XEvent event;
	unsigned int topLevelWindowArrayMapped = 1;
	unsigned int commandWordBeginning;
	for(;;){
		XNextEvent(display, &event);
		if(event.type == KeyPress){
			if(topLevelWindowArrayMapped){
				for(currentMonitor = 0; currentMonitor < dereferencedMonitorAmount; currentMonitor++){
					XUnmapWindow(display, topLevelWindowArray[currentMonitor]);
				}
				topLevelWindowArrayMapped = 0;
			}else{
				for(currentMonitor = 0; currentMonitor < dereferencedMonitorAmount; currentMonitor++){
					XMapWindow(display, topLevelWindowArray[currentMonitor]);
				}
				topLevelWindowArrayMapped = 1;
			}
		}
		if(event.type == ButtonPress){
			for(currentMonitor = 0; currentMonitor < dereferencedMonitorAmount; currentMonitor++){
				commandWordBeginning = 0;
				for(currentBox = 0; currentBox < boxAmount; currentBox++){
					if(event.xbutton.window == box[currentMonitor][currentBox]){
						if(drawableCommand2DRemappedArray[currentBox * drawableCommandMaxWordLength]){
							drawCommand(display, &topLevelWindowArray[currentMonitor], drawableCommand2DRemappedArray + currentBox * drawableCommandMaxWordLength, drawableCommandPath, &box[currentMonitor][currentBox], drawableCommand2DRemappedArray, &textColor[currentBox]);
						}
						if(command2DRemappedArray[commandWordBeginning]){
							if(isCommandRestart(&command2DRemappedArray[commandWordBeginning])){
								*mode = ModeRestart;
							}else if(isCommandExit(&command2DRemappedArray[commandWordBeginning])){
								*mode = ModeExit;
							}else{
								system(&command2DRemappedArray[commandWordBeginning]);
							}
						}
						currentMonitor = dereferencedMonitorAmount;
						break;
					}
					commandWordBeginning += commandMaxWordLength;
				}
			}
			if(*mode == ModeRestart || *mode == ModeExit){
				break;
			}
		}else if(event.type == Expose){
			for(currentMonitor = 0; currentMonitor < dereferencedMonitorAmount; currentMonitor++){
				onExpose(display, &topLevelWindowArray[currentMonitor], box[currentMonitor], &boxAmount, text2DRemappedArray, &textMaxWordLength, textColor);
			}
		}
	}
	XUngrabKeyboard(display, CurrentTime);
	return;
}
static unsigned int getBoxAmount(Display *const display, const Window *const topLevelWindow){
	unsigned int boxAmount = 0;
	Window rootWindow;
	Window parentWindow;
	Window *menu;
	unsigned int menuAmount;
	XQueryTree(display, *topLevelWindow, &rootWindow, &parentWindow, &menu, &menuAmount);
	Window *boxArray;
	unsigned int menuBoxAmount;
	for(unsigned int currentMenu = 0; currentMenu < menuAmount; currentMenu++){
		XQueryTree(display, menu[currentMenu], &rootWindow, &parentWindow, &boxArray, &menuBoxAmount);
		boxAmount += menuBoxAmount;
		if(menuBoxAmount > 0){
			XFree(boxArray);
		}
	}
	if(menuAmount > 0){
		XFree(menu);
	}
	return boxAmount;
}
static void drawCommand(Display *const display, const Window *const topLevelWindow, const char *const systemCommandArray, const char *const drawableCommandPathArray, const Window *const box, const char *const drawableCommand2DRemappedArray, const int *const textColor){
	system(systemCommandArray);
	char *line;
	{
		FILE *drawableCommand = fopen(drawableCommandPathArray, "r");
		size_t characters = 200;
		line = (char *)malloc(characters * sizeof(char));
		getline(&line, &characters, drawableCommand);
		fclose(drawableCommand);
	}
	unsigned int resultLength = 0;
	{
		while(line[resultLength] > '\0'){
			resultLength++;
		}
	}
	if(resultLength > 1){
		resultLength--;
		char result[resultLength];
		{
			for(unsigned int currentCharacter = 0; currentCharacter < resultLength; currentCharacter++){
				result[currentCharacter] = line[currentCharacter];
			}
		}
		GC gc = XCreateGC(display, *topLevelWindow, None, None);
		XSetForeground(display, gc, *textColor);
		int x;
		int y;
		{
			XCharStruct charStruct;
			{
				XFontStruct *font = XLoadQueryFont(display, "fixed");
				int direction;
				XTextExtents(font, drawableCommand2DRemappedArray, resultLength, &direction, (int *)&charStruct.ascent, (int *)&charStruct.descent, &charStruct);
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
		XDrawString(display, *box, gc, x, y, result, resultLength);
	}
	free(line);
	return;
}
static void onExpose(Display *const display, const Window *const topLevelWindow, const Window *const boxArray, const unsigned int *const boxAmount, const char *const text2DRemappedArray, const unsigned int *const textMaxWordLength, const int *const textColorArray){
	const unsigned int dereferencedBoxAmount = *boxAmount;
	if(dereferencedBoxAmount > 0){
		GC gc = XCreateGC(display, *topLevelWindow, None, None);
		const unsigned int dereferencedTextMaxWordLength = *textMaxWordLength;
		unsigned int wordBeginning = 0;
		XFontStruct *font = XLoadQueryFont(display, "fixed");
		unsigned int actualWordLength;
		int direction;
		XCharStruct charStruct;
		XWindowAttributes windowAttributes;
		int x;
		int y;
		for(unsigned int currentBox = 0; currentBox < dereferencedBoxAmount; currentBox++){
			if(text2DRemappedArray[wordBeginning]){
				XSetForeground(display, gc, textColorArray[currentBox]);
				actualWordLength = 0;
				while(text2DRemappedArray[wordBeginning + actualWordLength] > '\0'){
					actualWordLength++;
				}
				XTextExtents(font, &text2DRemappedArray[wordBeginning], actualWordLength, &direction, (int *)&charStruct.ascent, (int *)&charStruct.descent, &charStruct);
				XGetWindowAttributes(display, boxArray[currentBox], &windowAttributes);
				x = windowAttributes.width;
				x -= charStruct.width;
				x /= 2;
				y = windowAttributes.height;
				y += charStruct.ascent;
				y /= 2;
				XDrawString(display, boxArray[currentBox], gc, x, y, &text2DRemappedArray[wordBeginning], actualWordLength);
			}
			wordBeginning += dereferencedTextMaxWordLength;
		}
		XFreeFont(display, font);
	}
	return;
}
static unsigned int isCommandRestart(const char *const commandArray){
	return (
		(commandArray[0] == 'R' || commandArray[0] == 'r') &&
		(commandArray[1] == 'E' || commandArray[1] == 'e') &&
		(commandArray[2] == 'S' || commandArray[2] == 's') &&
		(commandArray[3] == 'T' || commandArray[3] == 't') &&
		(commandArray[4] == 'A' || commandArray[4] == 'a') &&
		(commandArray[5] == 'R' || commandArray[5] == 'r') &&
		(commandArray[6] == 'T' || commandArray[6] == 't') &&
		commandArray[7] == '&' &&
		commandArray[8] == '\0'
	);
}
static unsigned int isCommandExit(const char *const commandArray){
	return (
		(commandArray[0] == 'E' || commandArray[0] == 'e') &&
		(commandArray[1] == 'X' || commandArray[1] == 'x') &&
		(commandArray[2] == 'I' || commandArray[2] == 'i') &&
		(commandArray[3] == 'T' || commandArray[3] == 't') &&
		commandArray[4] == '&' &&
		commandArray[5] == '\0'
	);
}
