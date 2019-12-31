#ifndef READCONFIG_H
#define READCONFIG_H

#include "defines.h"

bool readConfigScan(const Window *const restrict parentWindow);
bool readConfigTopLevelWindow(const Window *const restrict parentWindow, int *const restrict x, int *const restrict y, unsigned int *const restrict width, unsigned int *const restrict height, unsigned int *const restrict border, bytes4 *const restrict borderColor, bytes4 *const restrict backgroundColor, bytes4 *const restrict globalMenuBorderColor, bytes4 *const restrict globalMenuBackgroundColor, unsigned int *const restrict menuAmount);
bool readConfigMenuWindow(const Window *const restrict parentWindow, const unsigned int *const restrict currentMenu, int *const restrict x, int *const restrict y, unsigned int *const restrict width, unsigned int *const restrict height, unsigned int *const restrict border, bytes4 *const restrict borderColor, bytes4 *const restrict backgroundColor, bytes4 *const restrict globalBoxBorderColor, bytes4 *const restrict globalBoxBackgroundColor, unsigned int *const restrict boxAmount);
bool readConfigBoxWindow(const Window *const restrict parentWindow, const unsigned int *const restrict currentMenu, const unsigned int *const restrict currentBox, int *const restrict x, int *const restrict y, unsigned int *const restrict width, unsigned int *const restrict height, unsigned int *const restrict border, bytes4 *const restrict borderColor, bytes4 *const restrict backgroundColor, bytes4 *const restrict globalInnerBoxBorderColor, bytes4 *const restrict globalInnerBoxBackgroundColor, unsigned int *const restrict innerBoxAmount);
bool readConfigInnerBoxWindow(const Window *const restrict parentWindow, const unsigned int *const restrict currentMenu, const unsigned int *const restrict currentBox, const unsigned int *const restrict currentInnerBox, int *const restrict x, int *const restrict y, unsigned int *const restrict width, unsigned int *const restrict height, unsigned int *const restrict border, bytes4 *const restrict borderColor, bytes4 *const restrict backgroundColor);
bool readConfigTextCommands(const Window *const restrict window, const unsigned int *const restrict currentBox, char **const restrict textPointerArray, bytes4 *const restrict textColor, char **const restrict commandPointerArray, char **const restrict drawableCommandPointerArray);
bool readConfigButton(const Window *const restrict window, const unsigned int *const restrict currentBox);

#endif
