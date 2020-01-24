#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include "headers/defines.h"

const char *restrict programName;
const char *restrict configPath;
const char *restrict workplacePath;
unsigned int mode = ModeContinue;
Display *restrict display;
unsigned int monitorAmount;
const XRRMonitorInfo *restrict monitorInfo;
FILE *restrict file;
unsigned int totalBoxAmount;
XFontStruct *restrict fontStruct;
static char fileBuffer[DefaultCharactersCount];
char *restrict line = fileBuffer;
Window *restrict topLevelWindowArray;
unsigned int currentMonitor;
