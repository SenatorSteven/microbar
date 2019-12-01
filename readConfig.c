#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>

#define DefaultCharactersCount /*------------*/ ((unsigned int)200)
#define DefaultLinesCount /*-----------------*/ ((unsigned int)100)

#define NoPositions /*-----------------------*/ 0
#define LinesPosition /*---------------------*/ (1 << 0)
#define XPosition /*-------------------------*/ (1 << 1)
#define YPosition /*-------------------------*/ (1 << 2)
#define WidthPosition /*---------------------*/ (1 << 3)
#define HeightPosition /*--------------------*/ (1 << 4)
#define BorderPosition /*--------------------*/ (1 << 5)
#define BorderColorPosition /*---------------*/ (1 << 6)
#define BackgroundColorPosition /*-----------*/ (1 << 7)
#define GlobalMenuBorderColorPosition /*-----*/ (1 << 8)
#define GlobalMenuBackgroundColorPosition /*-*/ (1 << 9)
#define GlobalBoxBorderColorPosition /*------*/ (1 << 10)
#define GlobalBoxBackgroundColorPosition /*--*/ (1 << 11)
#define HideKeyPosition /*-------------------*/ (1 << 12)
#define TextPosition /*----------------------*/ (1 << 13)
#define TextColorPosition /*-----------------*/ (1 << 14)
#define GlobalTextColorPosition /*-----------*/ (1 << 15)
#define CommandPosition /*-------------------*/ (1 << 16)
#define DrawableCommandPosition /*-----------*/ (1 << 17)
#define ButtonPosition /*--------------------*/ (1 << 18)
#define MenuPosition /*----------------------*/ (1 << 19)
#define BoxPosition /*-----------------------*/ (1 << 20)

#define NoOperation /*-----------------------*/ ((unsigned int)0)
#define OperationAddition /*-----------------*/ ((unsigned int)1)
#define OperationSubtraction /*--------------*/ ((unsigned int)2)
#define OperationMultiplication /*-----------*/ ((unsigned int)3)
#define OperationDivision /*-----------------*/ ((unsigned int)4)

static FILE *getConfigFile(const char *const pathArray);
static unsigned int pushSpaces(const char *const lineArray, unsigned int *const element);
static unsigned int isVariable(const char *const variableArray, const char *const lineArray, unsigned int *const element);
static unsigned int getUnsignedDecimalNumber(Display *const display, const Window *const window, const char *const lineArray, unsigned int *const element);
static int getDecimalNumber(Display *const display, const Window *const window, const char *const lineArray, unsigned int *const element);
static int getARGB(const char *const lineArray, unsigned int *const element);
static void getKeys(Display *const display, const Window *const window, const char *const lineArray, unsigned int *const element, unsigned int *const key, int *const masks);
static char *getText(const char *const lineArray, unsigned int *const element);
static char *getCommand(const char *const lineArray, unsigned int *const element);
// static unsigned int printLineError(const char *const lineArray, const unsigned int *const element, const unsigned int *const currentLine);

unsigned int readConfigTopLevelWindow(Display *const display, const char *const pathArray, const Window *const parentWindow, unsigned int *const x, unsigned int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, int *const borderColor, int *const backgroundColor, int *const globalMenuBorderColor, int *const globalMenuBackgroundColor, unsigned int *const menuAmount){
	unsigned int value = 0;
	FILE *config = getConfigFile(pathArray);
	if(config){
		size_t characters = DefaultCharactersCount;
		char *line = (char *)malloc(characters * sizeof(char));
		if(line){
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
			int hasReadVariable = NoPositions;
			for(unsigned int currentLine = 0; currentLine < maxLinesCount; currentLine++){
				element = 0;
				getline(&line, &characters, config);
				pushSpaces(line, &element);
				if(!isVariable("#", line, &element)){
					if(!(hasReadVariable & MenuPosition)){
						if(!(hasReadVariable & LinesPosition)){
							if(isVariable("Lines", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									maxLinesCount = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= LinesPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & XPosition)){
							if(isVariable("X", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*x = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= XPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & YPosition)){
							if(isVariable("Y", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*y = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= YPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & WidthPosition)){
							if(isVariable("Width", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*width = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= WidthPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & HeightPosition)){
							if(isVariable("Height", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*height = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= HeightPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BorderPosition)){
							if(isVariable("Border", line, &element) && !isVariable("BorderColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*border = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= BorderPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BorderColorPosition)){
							if(isVariable("BorderColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*borderColor = getARGB(line, &element);
									hasReadVariable |= BorderColorPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BackgroundColorPosition)){
							if(isVariable("BackgroundColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*backgroundColor = getARGB(line, &element);
									hasReadVariable |= BackgroundColorPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & GlobalMenuBorderColorPosition)){
							if(isVariable("GlobalMenuBorderColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*globalMenuBorderColor = getARGB(line, &element);
									hasReadVariable |= GlobalMenuBorderColorPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & GlobalMenuBackgroundColorPosition)){
							if(isVariable("GlobalMenuBackgroundColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*globalMenuBackgroundColor = getARGB(line, &element);
									hasReadVariable |= GlobalMenuBackgroundColorPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & HideKeyPosition)){
							if(isVariable("HideKey", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									unsigned int key;
									int masks;
									getKeys(display, parentWindow, line, &element, &key, &masks);
									XGrabKey(display, key, masks, XDefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
								}
								hasReadVariable |= HideKeyPosition;
								continue;
							}
						}
						if(isVariable("Menu", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("{", line, &element)){
								(*menuAmount)++;
								hasReadVariable |= MenuPosition;
							}
							continue;
						}
					}else if(!(hasReadVariable & BoxPosition)){
						if(isVariable("Box", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("{", line, &element)){
								hasReadVariable |= BoxPosition;
							}
							continue;
						}
						if(isVariable("}", line, &element)){
							hasReadVariable ^= MenuPosition;
							continue;
						}
					}else{
						if(isVariable("}", line, &element)){
							hasReadVariable ^= BoxPosition;
							continue;
						}
					}
				}
			}
			free(line);
			value = 1;
		}else{
			fprintf(stderr, "microbar: could not allocate space for config line\n");
		}
		fclose(config);
	}
	return value;
}
unsigned int readConfigMenuWindow(Display *const display, const char *const pathArray, const Window *const parentWindow, const unsigned int *const currentMenu, unsigned int *const x, unsigned int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, int *const borderColor, int *const backgroundColor, int *const globalBoxBorderColor, int *const globalBoxBackgroundColor, unsigned int *const boxAmount){
	unsigned int value = 0;
	FILE *config = getConfigFile(pathArray);
	if(config){
		size_t characters = DefaultCharactersCount;
		char *line = (char *)malloc(characters * sizeof(char));
		if(line){
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
			int hasReadVariable = NoPositions;
			unsigned int menuAmountRead = 0;
			for(unsigned int currentLine = 0; currentLine < maxLinesCount; currentLine++){
				element = 0;
				getline(&line, &characters, config);
				pushSpaces(line, &element);
				if(!isVariable("#", line, &element)){
					if(!(hasReadVariable & MenuPosition)){
						if(!(hasReadVariable & LinesPosition)){
							if(isVariable("Lines", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									maxLinesCount = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= LinesPosition;
								}
								continue;
							}
						}
						if(isVariable("Menu", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("{", line, &element)){
								if(menuAmountRead == *currentMenu){
									hasReadVariable |= MenuPosition;
								}else{
									menuAmountRead++;
								}
							}
							continue;
						}
					}else if(!(hasReadVariable & BoxPosition)){
						if(!(hasReadVariable & XPosition)){
							if(isVariable("X", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*x = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= XPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & YPosition)){
							if(isVariable("Y", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*y = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= YPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & WidthPosition)){
							if(isVariable("Width", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*width = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= WidthPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & HeightPosition)){
							if(isVariable("Height", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*height = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= HeightPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BorderPosition)){
							if(isVariable("Border", line, &element) && !isVariable("BorderColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*border = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= BorderPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BorderColorPosition)){
							if(isVariable("BorderColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*borderColor = getARGB(line, &element);
									hasReadVariable |= BorderColorPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BackgroundColorPosition)){
							if(isVariable("BackgroundColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*backgroundColor = getARGB(line, &element);
									hasReadVariable |= BackgroundColorPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & GlobalBoxBorderColorPosition)){
							if(isVariable("GlobalBoxBorderColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*globalBoxBorderColor = getARGB(line, &element);
									hasReadVariable |= GlobalBoxBorderColorPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & GlobalBoxBackgroundColorPosition)){
							if(isVariable("GlobalBoxBackgroundColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*globalBoxBackgroundColor = getARGB(line, &element);
									hasReadVariable |= GlobalBoxBackgroundColorPosition;
								}
								continue;
							}
						}
						if(isVariable("Box", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("{", line, &element)){
								(*boxAmount)++;
								hasReadVariable |= BoxPosition;
							}
							continue;
						}
						if(isVariable("}", line, &element)){
							break;
						}
					}else if(isVariable("}", line, &element)){
						hasReadVariable ^= BoxPosition;
						continue;
					}
				}
			}
			free(line);
			value = 1;
		}else{
			fprintf(stderr, "microbar: could not allocate space for config line\n");
		}
		fclose(config);
	}
	return value;
}
unsigned int readConfigBoxWindow(Display *const display, const char *const pathArray, const Window *const parentWindow, const unsigned int *const currentMenu, const unsigned int *const currentBox, unsigned int *const x, unsigned int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, int *const borderColor, int *const backgroundColor){
	unsigned int value = 0;
	FILE *config = getConfigFile(pathArray);
	if(config){
		size_t characters = DefaultCharactersCount;
		char *line = (char *)malloc(characters * sizeof(char));
		if(line){
			*x = 0;
			*y = 0;
			*width = 0;
			*height = 0;
			*border = 0;
			*borderColor = 0x00000000;
			*backgroundColor = 0x00000000;
			unsigned int maxLinesCount = DefaultLinesCount;
			unsigned int element;
			int hasReadVariable = NoPositions;
			unsigned int menuAmountRead = 0;
			unsigned int boxAmountRead = 0;
			for(unsigned int currentLine = 0; currentLine < maxLinesCount; currentLine++){
				element = 0;
				getline(&line, &characters, config);
				pushSpaces(line, &element);
				if(!isVariable("#", line, &element)){
					if(!(hasReadVariable & MenuPosition)){
						if(!(hasReadVariable & LinesPosition)){
							if(isVariable("Lines", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									maxLinesCount = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= LinesPosition;
								}
								continue;
							}
						}
						if(isVariable("Menu", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("{", line, &element)){
								if(menuAmountRead == *currentMenu){
									hasReadVariable |= MenuPosition;
								}else{
									menuAmountRead++;
								}
							}
							continue;
						}
					}else if(!(hasReadVariable & BoxPosition)){
						if(isVariable("Box", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("{", line, &element)){
								if(boxAmountRead == *currentBox){
									hasReadVariable |= BoxPosition;
								}else{
									boxAmountRead++;
								}
							}
							continue;
						}
					}else{
						if(!(hasReadVariable & XPosition)){
							if(isVariable("X", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*x = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= XPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & YPosition)){
							if(isVariable("Y", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*y = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= YPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & WidthPosition)){
							if(isVariable("Width", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*width = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= WidthPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & HeightPosition)){
							if(isVariable("Height", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*height = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= HeightPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BorderPosition)){
							if(isVariable("Border", line, &element) && !isVariable("BorderColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*border = getUnsignedDecimalNumber(display, parentWindow, line, &element);
									hasReadVariable |= BorderPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BorderColorPosition)){
							if(isVariable("BorderColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*borderColor = getARGB(line, &element);
									hasReadVariable |= BorderColorPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BackgroundColorPosition)){
							if(isVariable("BackgroundColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*backgroundColor = getARGB(line, &element);
									hasReadVariable |= BackgroundColorPosition;
								}
								continue;
							}
						}
						if(isVariable("}", line, &element)){
							break;
						}
					}
				}
			}
			free(line);
			value = 1;
		}else{
			fprintf(stderr, "microbar: could not allocate space for config line\n");
		}
		fclose(config);
	}
	return value;
}
unsigned int readConfigTextCommands(Display *const display, const char *const pathArray, const Window *const window, const unsigned int *const currentBox, char **const textPointerArray, int *const textColor, char **const commandPointerArray, char **const drawableCommandPointerArray){
	unsigned int value = 0;
	FILE *config = getConfigFile(pathArray);
	if(config){
		size_t characters = DefaultCharactersCount;
		char *line = (char *)malloc(characters * sizeof(char));
		if(line){
			*textPointerArray = NULL;
			*textColor = 0x00000000;
			*commandPointerArray = NULL;
			*drawableCommandPointerArray = NULL;
			unsigned int maxLinesCount = DefaultLinesCount;
			unsigned int element;
			int hasReadVariable = NoPositions;
			unsigned int boxAmountRead = 0;
			unsigned int button = 0;
			for(unsigned int currentLine = 0; currentLine < maxLinesCount; currentLine++){
				element = 0;
				getline(&line, &characters, config);
				pushSpaces(line, &element);
				if(!isVariable("#", line, &element)){
					if(!(hasReadVariable & MenuPosition)){
						if(!(hasReadVariable & LinesPosition)){
							if(isVariable("Lines", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									maxLinesCount = getUnsignedDecimalNumber(display, window, line, &element);
									hasReadVariable |= LinesPosition;
								}
								continue;
							}
						}
						if(isVariable("Menu", line, &element)){
							pushSpaces(line, &element);
							if(isVariable("{", line, &element)){
								hasReadVariable |= MenuPosition;
							}
							continue;
						}
					}else if(boxAmountRead <= *currentBox){
						if(!(hasReadVariable & GlobalTextColorPosition)){
							if(isVariable("GlobalTextColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									if(*textColor == 0x00000000){
										*textColor = getARGB(line, &element);
									}
									hasReadVariable |= GlobalTextColorPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & BoxPosition)){
							if(isVariable("Box", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("{", line, &element)){
									boxAmountRead++;
									hasReadVariable |= BoxPosition;
								}
								continue;
							}
						}
						if(isVariable("}", line, &element)){
							if(hasReadVariable & BoxPosition){
								hasReadVariable ^= BoxPosition;
							}else{
								*textColor = 0x00000000;
								if(hasReadVariable & GlobalTextColorPosition){
									hasReadVariable ^= GlobalTextColorPosition;
								}
								hasReadVariable ^= MenuPosition;
							}
							continue;
						}
					}else{
						if(!(hasReadVariable & TextPosition)){
							if(isVariable("Text", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*textPointerArray = getText(line, &element);
									hasReadVariable |= TextPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & TextColorPosition)){
							if(isVariable("TextColor", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*textColor = getARGB(line, &element);
									hasReadVariable |= TextColorPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & CommandPosition)){
							if(isVariable("Command", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*commandPointerArray = getCommand(line, &element);
									hasReadVariable |= CommandPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & DrawableCommandPosition)){
							if(isVariable("DrawableCommand", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									*drawableCommandPointerArray = getText(line, &element);
									hasReadVariable |= DrawableCommandPosition;
								}
								continue;
							}
						}
						if(!(hasReadVariable & ButtonPosition)){
							if(isVariable("Button", line, &element)){
								pushSpaces(line, &element);
								if(isVariable("=", line, &element)){
									pushSpaces(line, &element);
									button = getUnsignedDecimalNumber(display, window, line, &element);
									if(button > 5){
										button = 0;
									}
									hasReadVariable |= ButtonPosition;
								}
								continue;
							}
						}
						if(isVariable("}", line, &element)){
							break;
						}
					}
				}
			}
			if((hasReadVariable & CommandPosition) || (hasReadVariable & DrawableCommandPosition)){
				if(button < 6){
					XGrabButton(display, button, None, *window, True, NoEventMask, GrabModeAsync, GrabModeAsync, None, None);
				}
			}
			free(line);
			value = 1;
		}else{
			fprintf(stderr, "microbar: could not allocate space for config line\n");
		}
		fclose(config);
	}
	return value;
}
static FILE *getConfigFile(const char *const pathArray){
	FILE *config = fopen(pathArray, "r");
	if(!config){
		if((config = fopen(pathArray, "w"))){
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
			fprintf(config, "# this file needs to be user-specified when launched\n");
			fprintf(config, "# max line character length is 200\n");
			fprintf(config, "# comments are signified by a \'#\' at the beginning of the line\n");
			fprintf(config, "# one variable per line, followed by \'=\' and it's value\n");
			fprintf(config, "# all spaces and tabs are ignored\n");
			fprintf(config, "# all variables are valued 0 or undefined by default unless stated otherwise\n");
			fprintf(config, "# colors are in argb format\n");
			fprintf(config, "# all variables, argb values and macros can be written with random capitalization\n");
			fprintf(config, "# argb do not require a \'#\' before the value\n");
			fprintf(config, "# text requires the same quote character before and after it\n");
			fprintf(config, "# text quotation is variable, the first character after \'=\' is the quote character\n");
			fprintf(config, "# if (y < height of screen / 2) the bar is placed on top otherwise on bottom\n");
			fprintf(config, "# # # # # # #\n");
			fprintf(config, "# variables #\n");
			fprintf(config, "# # # # # # #\n\n");
			fprintf(config, "# global object: lines, x, y, width, height, border, borderColor, backgroundColor, globalMenuBorderColor, globalMenuBackgroundColor, hideKey, menu{}\n");
			fprintf(config, "# menu object: x, y, width, height, border, borderColor, backgroundColor, globalBoxBorderColor, globalBoxBackgroundColor, globalTextColor, box{}\n");
			fprintf(config, "# box object: x, y, width, height, border, borderColor, backgroundColor, text, textColor, command, drawableCommand, button\n");
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
			fprintf(config, "# globalTextColor: color of all boxes\' text\n");
			fprintf(config, "# hideKey: combination of keycode and modifiers to hide the bar\n");
			fprintf(config, "# text: text label of box\n");
			fprintf(config, "# textColor: color of box\'s text\n");
			fprintf(config, "# command: command executed on interaction with box (followed by control operator \'&\')\n");
			fprintf(config, "# drawableCommand: command returning text output executed on interaction with box\n");
			fprintf(config, "# button: mouse button used for interaction with box\n");
			fprintf(config, "# menu: informationless interactionless object, residing in global object\n");
			fprintf(config, "# box: information object, residing in menu object\n");
			fprintf(config, "# ParentWidth: size of parent object\'s width\n");
			fprintf(config, "# ParentHeight: size of parent object\'s height\n\n\n\n");
			fprintf(config, "# # # # #\n");
			fprintf(config, "# extra #\n");
			fprintf(config, "# # # # #\n\n");
			fprintf(config, "# lines: default 100\n");
			fprintf(config, "# hideKey: modifiers: Shift, Lock, Control, Mod1, Mod2, Mod3, Mod4, Mod5\n");
			fprintf(config, "# text: requires quotation\n");
			fprintf(config, "# command: requires quotation\n");
			fprintf(config, "# drawableCommand: requires quotation\n");
			fprintf(config, "# button: default 0 = any button, 1 = left click, 2 = middle click, 3 = right click, 4 = wheel up, 5 = wheel down\n\n\n\n");
			fprintf(config, "# /config start # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
			fprintf(config, "lines = 163\n");
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
			config = fopen(pathArray, "r");
		}
	}
	return config;
}
static unsigned int pushSpaces(const char *const lineArray, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	unsigned int value = 0;
	while(dereferencedElement < DefaultCharactersCount && (lineArray[dereferencedElement] == ' ' || lineArray[dereferencedElement] == 9)){
		dereferencedElement++;
	}
	if(dereferencedElement > *element){
		value = 1;
		*element = dereferencedElement;
	}
	return value;
}
static unsigned int isVariable(const char *const variable, const char *const lineArray, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	unsigned int value = 0;
	unsigned int length = 0;
	while(variable[length] > '\0'){
		length++;
	}
	unsigned int currentCharacter = 0;
	while(currentCharacter < length){
		if(variable[currentCharacter] >= 'A' && variable[currentCharacter] <= 'Z'){
			if(!(lineArray[dereferencedElement] == variable[currentCharacter] || lineArray[dereferencedElement] == variable[currentCharacter] + 32)){
				break;
			}
		}else if(variable[currentCharacter] >= 'a' && variable[currentCharacter] <= 'z'){
			if(!(lineArray[dereferencedElement] == variable[currentCharacter] || lineArray[dereferencedElement] == variable[currentCharacter] - 32)){
				break;
			}
		}else{
			if(!(lineArray[dereferencedElement] == variable[currentCharacter])){
				break;
			}
		}
		dereferencedElement++;
		currentCharacter++;
	}
	if(currentCharacter == length){
		value = 1;
		*element = dereferencedElement;
	}
	return value;
}
static unsigned int getUnsignedDecimalNumber(Display *const display, const Window *const parentWindow, const char *const lineArray, unsigned int *const element){
	unsigned int number = getDecimalNumber(display, parentWindow, lineArray, element);
	if((int)number < 0){
		number = 0;
	}
	return number;
}
static int getDecimalNumber(Display *const display, const Window *const parentWindow, const char *const lineArray, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	XWindowAttributes windowAttributes;
	XGetWindowAttributes(display, *parentWindow, &windowAttributes);
	int number = 0;
	int numberRead = 0;
	int numberOperatedOn = 0;
	unsigned int operation = NoOperation;
	unsigned int lastOperation = NoOperation;
	while(dereferencedElement < DefaultCharactersCount){
		pushSpaces(lineArray, &dereferencedElement);
		if(lineArray[dereferencedElement] >= '0' && lineArray[dereferencedElement] <= '9'){
			numberRead *= 10;
			numberRead += lineArray[dereferencedElement];
			numberRead -= 48;
			dereferencedElement++;
		}else if(isVariable("ParentWidth", lineArray, &dereferencedElement)){
			numberRead = windowAttributes.width;
		}else if(isVariable("ParentHeight", lineArray, &dereferencedElement)){
			numberRead = windowAttributes.height;
		}else if(lineArray[dereferencedElement] == '+' || lineArray[dereferencedElement] == '-' || lineArray[dereferencedElement] == '*' || lineArray[dereferencedElement] == '/'){
			if(number == 0 && numberRead == 0){
				if(lineArray[dereferencedElement] == '/'){
					break;
				}
			}
			if(operation == OperationAddition){
				if(lineArray[dereferencedElement] != '*' && lineArray[dereferencedElement] != '/'){
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
				if(lineArray[dereferencedElement] != '*' && lineArray[dereferencedElement] != '/'){
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
				if(lineArray[dereferencedElement] == '+' || lineArray[dereferencedElement] == '-'){
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
				if(lineArray[dereferencedElement] == '+' || lineArray[dereferencedElement] == '-'){
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
			if(lineArray[dereferencedElement] == '+'){
				operation = OperationAddition;
			}else if(lineArray[dereferencedElement] == '-'){
				operation = OperationSubtraction;
			}else if(lineArray[dereferencedElement] == '*'){
				operation = OperationMultiplication;
			}else if(lineArray[dereferencedElement] == '/'){
				operation = OperationDivision;
			}
			numberRead = 0;
			dereferencedElement++;
		}else{
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
			break;
		}
	}
	*element = dereferencedElement;
	return number;
}
static int getARGB(const char *const lineArray, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	int color = 0x00000000;
	if(lineArray[dereferencedElement] == '#'){
		dereferencedElement++;
	}
	for(unsigned int currentCharacter = 0; currentCharacter < 8; currentCharacter++){
		color *= 16;
		if(lineArray[dereferencedElement] >= '0' && lineArray[dereferencedElement] <= '9'){
			color += lineArray[dereferencedElement];
			color -= 48;
		}else if(lineArray[dereferencedElement] >= 'A' && lineArray[dereferencedElement] <= 'F'){
			color += lineArray[dereferencedElement];
			color -= 55;
		}else if(lineArray[dereferencedElement] >= 'a' && lineArray[dereferencedElement] <= 'f'){
			color += lineArray[dereferencedElement];
			color -= 87;
		}else{
			fprintf(stderr, "microbar: \'%c\' is not recognized as a hexadecimal number\n", lineArray[dereferencedElement]);
			break;
		}
		dereferencedElement++;
	}
	*element = dereferencedElement;
	return color;
}
static void getKeys(Display *const display, const Window *const window, const char *const lineArray, unsigned int *const element, unsigned int *const key, int *const masks){
	unsigned int dereferencedElement = *element;
	*key = 0;
	int dereferencedMasks = 0;
	unsigned int lookingForValue = 1;
	while(dereferencedElement < DefaultCharactersCount){
		pushSpaces(lineArray, &dereferencedElement);
		if(lookingForValue){
			if(lineArray[dereferencedElement] >= '0' && lineArray[dereferencedElement] <= '9'){
				*key = getUnsignedDecimalNumber(display, window, lineArray, &dereferencedElement);
				if((lineArray[dereferencedElement] >= 'A' && lineArray[dereferencedElement] <= 'Z') || (lineArray[dereferencedElement] >= 'a' && lineArray[dereferencedElement] <= 'z')){
					dereferencedElement--;
				}
			}else if(isVariable("Shift", lineArray, &dereferencedElement)){
				dereferencedMasks |= ShiftMask;
			}else if(isVariable("Lock", lineArray, &dereferencedElement)){
				dereferencedMasks |= LockMask;
			}else if(isVariable("Control", lineArray, &dereferencedElement)){
				dereferencedMasks |= ControlMask;
			}else if(isVariable("Mod1", lineArray, &dereferencedElement)){
				dereferencedMasks |= Mod1Mask;
			}else if(isVariable("Mod2", lineArray, &dereferencedElement)){
				dereferencedMasks |= Mod2Mask;
			}else if(isVariable("Mod3", lineArray, &dereferencedElement)){
				dereferencedMasks |= Mod3Mask;
			}else if(isVariable("Mod4", lineArray, &dereferencedElement)){
				dereferencedMasks |= Mod4Mask;
			}else if(isVariable("Mod5", lineArray, &dereferencedElement)){
				dereferencedMasks |= Mod5Mask;
			}else{
				break;
			}
			lookingForValue = 0;
		}else{
			if(isVariable("+", lineArray, &dereferencedElement)){
				lookingForValue = 1;
			}else{
				break;
			}
		}
	}
	*element = dereferencedElement;
	*masks = dereferencedMasks;
	return;
}
static char *getText(const char *const lineArray, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	char *text = NULL;
	unsigned int length = 0;
	{
		const char quotation = lineArray[dereferencedElement++];
		while(dereferencedElement < DefaultCharactersCount && lineArray[dereferencedElement + length] != quotation){
			length++;
		}
	}
	if(length > 0){
		if((text = (char *)malloc((length + 1) * sizeof(char)))){
			for(unsigned int currentCharacter = 0; currentCharacter < length; currentCharacter++){
				text[currentCharacter] = lineArray[dereferencedElement];
				dereferencedElement++;
			}
			dereferencedElement++;
			text[length] = '\0';
		}else{
			fprintf(stderr, "could not allocate space for text\n");
		}
		*element = dereferencedElement;
	}
	return text;
}
static char *getCommand(const char *const lineArray, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	char *command = NULL;
	unsigned int length = 0;
	{
		const char quotation = lineArray[dereferencedElement++];
		while(dereferencedElement < DefaultCharactersCount && lineArray[dereferencedElement + length] != quotation){
			length++;
		}
	}
	if(length > 0){
		if((command = (char *)malloc((length + 2) * sizeof(char)))){
			for(unsigned int currentCharacter = 0; currentCharacter < length; currentCharacter++){
				command[currentCharacter] = lineArray[dereferencedElement];
				dereferencedElement++;
			}
			dereferencedElement++;
			command[length++] = '&';
			command[length] = '\0';
		}else{
			fprintf(stderr, "could not allocate space for command\n");
		}
		*element = dereferencedElement;
	}
	return command;
}
/*static unsigned int printLineError(const char *const lineArray, const unsigned int *const element, const unsigned int *const currentLine){
	unsigned int value = 0;
	if(lineArray[*element] != 10){
		unsigned int length = 0;
		while(lineArray[length] != 10){
			length++;
		}
		fprintf(stderr, "microbar: line %u: \"", *currentLine + 1);
		for(unsigned int currentCharacter = 0; currentCharacter < length; currentCharacter++){
			fprintf(stderr, "%c", lineArray[currentCharacter]);
		}
		fprintf(stderr, "\" not recognized as an internal variable\n");
		value = 1;
	}
	return value;
}*/
