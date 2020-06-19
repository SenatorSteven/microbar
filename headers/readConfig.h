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

bool readConfigScan(void);
bool readConfigTopLevelWindow(const Window parentWindow, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, ARGB *const borderColor, ARGB *const backgroundColor);
bool readConfigSectionRectangleAmount(unsigned int *const sectionAmount, unsigned int *const rectangleAmount);
bool readConfigGlobalColors(const unsigned int sectionAmount, ARGB *const globalSectionBorderColor, ARGB *const globalSectionBackgroundColor, ARGB *const globalContainerBorderColor, ARGB *const globalContainerBackgroundColor, ARGB *const globalRectangleBorderColor, ARGB *const globalRectangleBackgroundColor);
bool readConfigSectionWindows(const Window *const parentWindow, const unsigned int sectionAmount, int *const *const x, int *const *const y, unsigned int *const *const width, unsigned int *const *const height, unsigned int *const *const border, ARGB *const borderColor, ARGB *const backgroundColor);
bool readConfigSectionChildren(const unsigned int sectionAmount, unsigned int *const sectionChildrenAmount);
bool readConfigContainerWindows(Window *const *const parentWindow, int *const *const x, int *const *const y, unsigned int *const *const width, unsigned int *const *const height, unsigned int *const *const border, ARGB *const borderColor, ARGB *const backgroundColor);
bool readConfigContainerChildren(unsigned int *const containerChildrenAmount);
bool readConfigRectangleWindows(Window *const *const parentWindow, const unsigned int rectangleAmount, int *const *const x, int *const *const y, unsigned int *const *const width, unsigned int *const *const height, unsigned int *const *const border, ARGB *const borderColor, ARGB *const backgroundColor);
bool readConfigArrayLengths(unsigned int *const textMaxWordLength, unsigned int *const commandMaxWordLength, unsigned int *const drawableCommandMaxWordLength);
bool readConfigFillArrays(char *const *const text, ARGB *const textColor, char *const *const command, char *const *const drawableCommand, ARGB *const drawableCommandColor);
bool readConfigVariableShortcuts(unsigned int *const sectionShortcutAmount, unsigned int *const containerShortcutAmount);
bool readConfigShortcuts(const unsigned int sectionShortcutAmount, const unsigned int containerShortcutAmount, Shortcut *const interactAll, Shortcut *const interactSection, unsigned int *const sectionNumber, Shortcut *const interactContainer, unsigned int *const containerNumber, Shortcut *const hide, Shortcut *const peek, Shortcut *const restart, Shortcut *const exit);
bool readConfigButtons(void);
bool readConfigFontAmount(unsigned int *const fontAmount);
bool readConfigFontLength(const unsigned int fontAmount, unsigned int *const userFontLength);
bool readConfigFillFontArray(const unsigned int currentFont, char *const font);
bool readConfigFontOffsets(int *const textOffsetX, int *const textOffsetY, int *const drawableCommandOffsetX, int *const drawableCommandOffsetY);

#endif
