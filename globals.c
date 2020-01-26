#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include "headers/defines.h"

const char *programName;
const char *configPath;
const char *workplacePath;
unsigned int mode = ModeContinue;
Display *display;
unsigned int monitorAmount;
const XRRMonitorInfo *monitorInfo;
FILE *file;
unsigned int totalBoxAmount;
XFontStruct *fontStruct;
static char fileBuffer[DefaultCharactersCount];
char *line = fileBuffer;
Window *topLevelWindowArray;
unsigned int currentMonitor;
