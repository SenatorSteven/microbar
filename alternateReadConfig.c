/*unsigned int readConfigScan(Display *const display, const unsigned int *const currentMonitor, const char *const pathArray, const Window *const parentWindow, unsigned int *const menuAmount, unsigned int *const boxAmount, unsigned int *const innerBoxAmount){
	unsigned int value = 0;
	FILE *config = getConfigFile(pathArray);
	if(config){
		*menuAmount = 0;
		*boxAmount = 0;
		*innerBoxAmount = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		size_t characters = DefaultCharactersCount;
		char fileBuffer[characters];
		char *line = fileBuffer;
		bytes4 hasReadVariable = NoPositions;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; currentLine++){
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
								maxLinesCount = getUnsignedDecimalNumber(display, currentMonitor, parentWindow, &currentLine, line, &element);
								hasReadVariable |= LinesPosition;
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
								getKeys(display, currentMonitor, parentWindow, line, &element, &key, &masks);
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
					if(!isVariable("Lines",                     line, &element) &&
					   !isVariable("X",                         line, &element) &&
					   !isVariable("Y",                         line, &element) &&
					   !isVariable("Width",                     line, &element) &&
					   !isVariable("Height",                    line, &element) &&
					   !isVariable("Border",                    line, &element) &&
					   !isVariable("BorderColor",               line, &element) &&
					   !isVariable("BackgroundColor",           line, &element) &&
					   !isVariable("GlobalMenuBorderColor",     line, &element) &&
					   !isVariable("GlobalMenuBackgroundColor", line, &element) &&
					   !isVariable("HideKey",                   line, &element) &&
					   !isVariable("Menu",                      line, &element) &&
					   !isVariable("}",                         line, &element)){
						printLineError(line, &element, &currentLine);
						continue;
					}
				}else if(!(hasReadVariable & BoxPosition)){
					if(isVariable("Box", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							(*boxAmount)++;
							hasReadVariable |= BoxPosition;
						}
						continue;
					}
					if(isVariable("}", line, &element)){
						hasReadVariable ^= MenuPosition;
						continue;
					}
					if(!isVariable("X",                        line, &element) &&
					   !isVariable("Y",                        line, &element) &&
					   !isVariable("Width",                    line, &element) &&
					   !isVariable("Height",                   line, &element) &&
					   !isVariable("Border",                   line, &element) &&
					   !isVariable("BorderColor",              line, &element) &&
					   !isVariable("BackgroundColor",          line, &element) &&
					   !isVariable("GlobalBoxBorderColor",     line, &element) &&
					   !isVariable("GlobalBoxBackgroundColor", line, &element) &&
					   !isVariable("GlobalTextColor",          line, &element) &&
					   !isVariable("Box",                      line, &element) &&
					   !isVariable("}",                        line, &element)){
						printLineError(line, &element, &currentLine);
						continue;
					}
				}else if(!(hasReadVariable & InnerBoxPosition)){
					if(isVariable("InnerBox", line, &element)){
						pushSpaces(line, &element);
						if(isVariable("{", line, &element)){
							(*innerBoxAmount)++;
							hasReadVariable |= InnerBoxPosition;
						}
						continue;
					}
					if(isVariable("}", line, &element)){
						hasReadVariable ^= BoxPosition;
						continue;
					}
					if(!isVariable("X",               line, &element) &&
					   !isVariable("Y",               line, &element) &&
					   !isVariable("Width",           line, &element) &&
					   !isVariable("Height",          line, &element) &&
					   !isVariable("Border",          line, &element) &&
					   !isVariable("BorderColor",     line, &element) &&
					   !isVariable("BackgroundColor", line, &element) &&
					   !isVariable("Text",            line, &element) &&
					   !isVariable("TextColor",       line, &element) &&
					   !isVariable("Command",         line, &element) &&
					   !isVariable("DrawableCommand", line, &element) &&
					   !isVariable("Button",          line, &element) &&
					   !isVariable("InnerBox",        line, &element) &&
					   !isVariable("}",               line, &element)){
						printLineError(line, &element, &currentLine);
						continue;
					}
				}else{
					if(isVariable("}", line, &element)){
						hasReadVariable ^= InnerBoxPosition;
						continue;
					}
					if(!isVariable("X",               line, &element) &&
					   !isVariable("Y",               line, &element) &&
					   !isVariable("Width",           line, &element) &&
					   !isVariable("Height",          line, &element) &&
					   !isVariable("Border",          line, &element) &&
					   !isVariable("BorderColor",     line, &element) &&
					   !isVariable("BackgroundColor", line, &element) &&
					   !isVariable("}",               line, &element)){
						printLineError(line, &element, &currentLine);
						continue;
					}
				}
			}
		}
		fclose(config);
		value = 1;
	}
	return value;
}*/
