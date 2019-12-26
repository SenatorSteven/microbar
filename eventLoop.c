#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include "headers/readConfig.h"
#include "headers/defines.h"

#define ModeContinue /*-*/ ((unsigned int)0)
#define ModeRestart /*--*/ ((unsigned int)1)
#define ModeExit /*-----*/ ((unsigned int)2)

extern const char *const configPath;
extern unsigned int mode;
extern Display *const display;
extern const unsigned int monitorAmount;
extern const Window *const topLevelWindow;

static unsigned int getBoxAmount();
static void drawCommand(const Window *const topLevelWindow, const char *const systemCommandArray, const char *const drawableCommandPathArray, const Window *const box, const char *const drawableCommand2DRemappedArray, const bytes4 *const textColor);
static unsigned int isCommand(const char *const command, const char *const commandArray);
static void onExpose(const Window *const topLevelWindow, const Window *const boxArray, const unsigned int *const boxAmount, const char *const text2DRemappedArray, const unsigned int *const textMaxWordLength, const bytes4 *const textColorArray);

void eventLoop(){
	const unsigned int boxAmount = getBoxAmount();
	unsigned int currentMonitor;
	unsigned int currentBox;
	Window box[monitorAmount][boxAmount];
	{
		Window rootWindow;
		Window parentWindow;
		Window *menu;
		unsigned int menuAmount;
		Window *boxArray;
		unsigned int menuBoxAmount;
		unsigned int boxNumber;
		unsigned int currentMenu;
		for(currentMonitor = 0; currentMonitor < monitorAmount; currentMonitor++){
			XQueryTree(display, topLevelWindow[currentMonitor], &rootWindow, &parentWindow, &menu, &menuAmount);
			boxNumber = 0;
			if(menuAmount > 0){
				for(currentMenu = 0; currentMenu < menuAmount; currentMenu++){
					XQueryTree(display, menu[currentMenu], &rootWindow, &parentWindow, &boxArray, &menuBoxAmount);
					if(menuBoxAmount > 0){
						for(currentBox = 0; currentBox < menuBoxAmount; currentBox++){
							box[currentMonitor][boxNumber] = boxArray[currentBox];
							boxNumber++;
						}
						XFree(boxArray);
					}
				}
				XFree(menu);
			}
		}
	}
	char *allocatedText[boxAmount];
	bytes4 textColor[boxAmount];
	char *allocatedCommand[boxAmount];
	char *allocatedDrawableCommand[boxAmount];
	for(currentBox = 0; currentBox < boxAmount; currentBox++){
		readConfigTextCommands(&currentMonitor, &box[0][currentBox], &currentBox, &allocatedText[currentBox], &textColor[currentBox], &allocatedCommand[currentBox], &allocatedDrawableCommand[currentBox]);
	}
	unsigned int textMaxWordLength = 0;
	unsigned int commandMaxWordLength = 0;
	unsigned int drawableCommandMaxWordLength = 0;
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
			if(allocatedCommand[currentBox]){
				copy = 0;
				while(allocatedCommand[currentBox][copy] > '\0'){
					copy++;
				}
				if(copy > commandMaxWordLength){
					commandMaxWordLength = copy;
				}
			}
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
		textMaxWordLength++;
		commandMaxWordLength++;
		drawableCommandMaxWordLength++;
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
	unsigned int drawableCommandPathLength = 0;
	{
		while(configPath[drawableCommandPathLength] > '\0'){
			drawableCommandPathLength++;
		}
		while(configPath[drawableCommandPathLength] != '/'){
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
				drawableCommandPath[currentCharacter] = configPath[currentCharacter];
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
		free(allocatedText[currentBox]);
		free(allocatedCommand[currentBox]);
		free(allocatedDrawableCommand[currentBox]);
	}
	for(currentMonitor = 0; currentMonitor < monitorAmount; currentMonitor++){
		for(currentBox = 0; currentBox < boxAmount; currentBox++){
			readConfigButton(&currentMonitor, &box[currentMonitor][currentBox], &currentBox);
		}
		XMapWindow(display, topLevelWindow[currentMonitor]);
	}
	XEvent event;
	unsigned int topLevelWindowMapped = 1;
	unsigned int commandWordBeginning;
	for(;;){
		XNextEvent(display, &event);
		if(event.type == KeyPress){
			if(topLevelWindowMapped){
				for(currentMonitor = 0; currentMonitor < monitorAmount; currentMonitor++){
					XUnmapWindow(display, topLevelWindow[currentMonitor]);
				}
				topLevelWindowMapped = 0;
			}else{
				for(currentMonitor = 0; currentMonitor < monitorAmount; currentMonitor++){
					XMapWindow(display, topLevelWindow[currentMonitor]);
				}
				topLevelWindowMapped = 1;
			}
		}else if(event.type == ButtonPress){
			for(currentMonitor = 0; currentMonitor < monitorAmount; currentMonitor++){
				commandWordBeginning = 0;
				for(currentBox = 0; currentBox < boxAmount; currentBox++){
					if(event.xbutton.window == box[currentMonitor][currentBox]){
						if(drawableCommand2DRemappedArray[currentBox * drawableCommandMaxWordLength]){
							drawCommand(&topLevelWindow[currentMonitor], drawableCommand2DRemappedArray + currentBox * drawableCommandMaxWordLength, drawableCommandPath, &box[currentMonitor][currentBox], drawableCommand2DRemappedArray, &textColor[currentBox]);
						}
						if(command2DRemappedArray[commandWordBeginning]){
							if(isCommand("Restart&", &command2DRemappedArray[commandWordBeginning])){
								mode = ModeRestart;
							}else if(isCommand("Exit&", &command2DRemappedArray[commandWordBeginning])){
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
			for(currentMonitor = 0; currentMonitor < monitorAmount; currentMonitor++){
				onExpose(&topLevelWindow[currentMonitor], box[currentMonitor], &boxAmount, text2DRemappedArray, &textMaxWordLength, textColor);
			}
		}
	}
	XUngrabKeyboard(display, CurrentTime);
	return;
}
static unsigned int getBoxAmount(){
	unsigned int boxAmount = 0;
	Window rootWindow;
	Window parentWindow;
	Window *menus;
	unsigned int menuAmount;
	XQueryTree(display, *topLevelWindow, &rootWindow, &parentWindow, &menus, &menuAmount);
	if(menuAmount > 0){
		Window *boxes;
		unsigned int menuBoxAmount;
		for(unsigned int currentMenu = 0; currentMenu < menuAmount; currentMenu++){
			XQueryTree(display, menus[currentMenu], &rootWindow, &parentWindow, &boxes, &menuBoxAmount);
			if(menuBoxAmount > 0){
				boxAmount += menuBoxAmount;
				XFree(boxes);
			}
		}
		XFree(menus);
	}
	return boxAmount;
}
static void drawCommand(const Window *const topLevelWindow, const char *const systemCommandArray, const char *const drawableCommandPathArray, const Window *const box, const char *const drawableCommand2DRemappedArray, const bytes4 *const textColor){
	system(systemCommandArray);
	char *result;
	{
		FILE *drawableCommand = fopen(drawableCommandPathArray, "r");
		size_t characters = DefaultLinesCount;
		result = (char *)malloc(characters * sizeof(char));
		getline(&result, &characters, drawableCommand);
		fclose(drawableCommand);
	}
	if(result){
		unsigned int resultLength = 0;
		while(result[resultLength] > '\0'){
			resultLength++;
		}
		if(resultLength > 1){
			resultLength--;
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
		free(result);
	}else{
		fprintf(stderr, "%s: could not allocate space for drawable command line\n", ProgramName);
	}
	return;
}
static unsigned int isCommand(const char *const command, const char *const commandArray){
	unsigned int value = 0;
	unsigned int length = 0;
	while(command[length] > '\0'){
		length++;
	}
	unsigned int element = 0;
	while(element < length){
		if(command[element] >= 'A' && command[element] <= 'Z'){
			if(!(commandArray[element] == command[element] || commandArray[element] == command[element] + 32)){
				break;
			}
		}else if(command[element] >= 'a' && command[element] <= 'z'){
			if(!(commandArray[element] == command[element] || commandArray[element] == command[element] - 32)){
				break;
			}
		}else{
			if(!(commandArray[element] == command[element])){
				break;
			}
		}
		element++;
	}
	if(element == length){
		value = 1;
	}
	return value;
}
static void onExpose(const Window *const topLevelWindow, const Window *const boxArray, const unsigned int *const boxAmount, const char *const text2DRemappedArray, const unsigned int *const textMaxWordLength, const bytes4 *const textColorArray){
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
