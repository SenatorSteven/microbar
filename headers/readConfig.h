#ifndef READCONFIG_H
#define READCONFIG_H

#include "defines.h"

unsigned int readConfigTopLevelWindow(Display *const display, const unsigned int *const currentMonitor, const char *const pathArray, const Window *const parentWindow, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor, bytes4 *const globalMenuBorderColor, bytes4 *const globalMenuBackgroundColor, unsigned int *const menuAmount);
unsigned int readConfigMenuWindow(Display *const display, const unsigned int *const currentMonitor, const char *const pathArray, const Window *const parentWindow, const unsigned int *const currentMenu, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor, bytes4 *const globalBoxBorderColor, bytes4 *const globalBoxBackgroundColor, unsigned int *const boxAmount);
unsigned int readConfigBoxWindow(Display *const display, const unsigned int *const currentMonitor, const char *const pathArray, const Window *const parentWindow, const unsigned int *const currentMenu, const unsigned int *const currentBox, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor, unsigned int *const innerBoxAmount);
unsigned int readConfigInnerBoxWindow(Display *const display, const unsigned int *const currentMonitor, const char *const pathArray, const Window *const parentWindow, const unsigned int *const currentMenu, const unsigned int *const currentBox, const unsigned int *const currentInnerBox, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor);
unsigned int readConfigTextCommands(Display *const display, const unsigned int *const currentMonitor, const char *const pathArray, const Window *const window, const unsigned int *const currentBox, char **const textPointerArray, bytes4 *const textColor, char **const commandPointerArray, char **const drawableCommandPointerArray);
unsigned int readConfigButton(Display *const display, const unsigned int *const currentMonitor, const char *const pathArray, const Window *const window, const unsigned int *const currentBox);

#endif
