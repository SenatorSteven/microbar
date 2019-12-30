#ifndef READCONFIG_H
#define READCONFIG_H

#include "defines.h"

bool readConfigScan(const Window *const parentWindow);
bool readConfigTopLevelWindow(const Window *const parentWindow, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor, bytes4 *const globalMenuBorderColor, bytes4 *const globalMenuBackgroundColor, unsigned int *const menuAmount);
bool readConfigMenuWindow(const Window *const parentWindow, const unsigned int *const currentMenu, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor, bytes4 *const globalBoxBorderColor, bytes4 *const globalBoxBackgroundColor, unsigned int *const boxAmount);
bool readConfigBoxWindow(const Window *const parentWindow, const unsigned int *const currentMenu, const unsigned int *const currentBox, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor, unsigned int *const innerBoxAmount);
bool readConfigInnerBoxWindow(const Window *const parentWindow, const unsigned int *const currentMenu, const unsigned int *const currentBox, const unsigned int *const currentInnerBox, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor);
bool readConfigTextCommands(const Window *const window, const unsigned int *const currentBox, char **const textPointerArray, bytes4 *const textColor, char **const commandPointerArray, char **const drawableCommandPointerArray);
bool readConfigButton(const Window *const window, const unsigned int *const currentBox);

#endif
