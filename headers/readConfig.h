/* readConfig.h

MIT License

Copyright (C) 2019 Stefanos "Steven" Tsakiris

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#ifndef READCONFIG_H
#define READCONFIG_H

#include "defines.h"

bool readConfigScan(const Window *const parentWindow);
bool readConfigTopLevelWindow(const Window *const parentWindow, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor, bytes4 *const globalMenuBorderColor, bytes4 *const globalMenuBackgroundColor, unsigned int *const menuAmount);
bool readConfigMenuWindow(const Window *const parentWindow, const unsigned int *const currentMenu, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor, bytes4 *const globalBoxBorderColor, bytes4 *const globalBoxBackgroundColor, unsigned int *const boxAmount);
bool readConfigBoxWindow(const Window *const parentWindow, const unsigned int *const currentMenu, const unsigned int *const currentBox, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor, bytes4 *const globalInnerBoxBorderColor, bytes4 *const globalInnerBoxBackgroundColor, unsigned int *const innerBoxAmount);
bool readConfigInnerBoxWindow(const Window *const parentWindow, const unsigned int *const currentMenu, const unsigned int *const currentBox, const unsigned int *const currentInnerBox, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, bytes4 *const borderColor, bytes4 *const backgroundColor);
bool readConfigTextCommands(const Window *const window, const unsigned int *const currentBox, char **const textPointerArray, bytes4 *const textColor, char **const commandPointerArray, char **const drawableCommandPointerArray);
bool readConfigButton(const Window *const window, const unsigned int *const currentBox);

#endif
