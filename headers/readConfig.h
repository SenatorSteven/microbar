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

bool readConfigScan(const Window parentWindow);
bool readConfigTopLevelWindow(const Window parentWindow, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, uint32_t *const borderColor, uint32_t *const backgroundColor, uint32_t *const globalMenuBorderColor, uint32_t *const globalMenuBackgroundColor, unsigned int *const menuAmount);
bool readConfigMenuWindow(const Window parentWindow, const unsigned int currentMenu, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, uint32_t *const borderColor, uint32_t *const backgroundColor, uint32_t *const globalBoxBorderColor, uint32_t *const globalBoxBackgroundColor, unsigned int *const boxAmount);
bool readConfigBoxWindow(const Window parentWindow, const unsigned int currentMenu, const unsigned int currentBox, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, uint32_t *const borderColor, uint32_t *const backgroundColor, uint32_t *const globalInnerBoxBorderColor, uint32_t *const globalInnerBoxBackgroundColor, unsigned int *const innerBoxAmount);
bool readConfigInnerBoxWindow(const Window parentWindow, const unsigned int currentMenu, const unsigned int currentBox, const unsigned int currentInnerBox, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, uint32_t *const borderColor, uint32_t *const backgroundColor);
bool readConfigArrayLengths(unsigned int *const textMaxWordLength, unsigned int *const commandMaxWordLength, unsigned int *const drawableCommandMaxWordLength);
bool readConfigFillArrays(const unsigned int currentBox, char *const text, uint32_t *const textColor, char *const command, char *const drawableCommand);
bool readConfigButton(const Window window, const unsigned int currentBox);

#endif
