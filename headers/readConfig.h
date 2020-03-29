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
bool readConfigTopLevelWindow(const Window parentWindow, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, uint32_t *const borderColor, uint32_t *const backgroundColor, uint32_t *const globalSectionBorderColor, uint32_t *const globalSectionBackgroundColor, unsigned int *const sectionAmount);
bool readConfigSectionWindow(const Window parentWindow, const unsigned int currentSection, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, uint32_t *const borderColor, uint32_t *const backgroundColor, uint32_t *const globalContainerBorderColor, uint32_t *const globalContainerBackgroundColor, unsigned int *const boxAmount);
bool readConfigContainerWindow(const Window parentWindow, const unsigned int currentSection, const unsigned int currentContainer, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, uint32_t *const borderColor, uint32_t *const backgroundColor, uint32_t *const globalRectangleBorderColor, uint32_t *const globalRectangleBackgroundColor, unsigned int *const rectangleAmount);
bool readConfigRectangleWindow(const Window parentWindow, const unsigned int currentSection, const unsigned int currentContainer, const unsigned int currentRectangle, int *const x, int *const y, unsigned int *const width, unsigned int *const height, unsigned int *const border, uint32_t *const borderColor, uint32_t *const backgroundColor);
bool readConfigArrayLengths(unsigned int *const textMaxWordLength, unsigned int *const commandMaxWordLength, unsigned int *const drawableCommandMaxWordLength);
bool readConfigFillArrays(const unsigned int currentContainer, char *const text, uint32_t *const textColor, char *const command, char *const drawableCommand);
bool readConfigButton(const Window window, const unsigned int currentContainer);
bool readConfigFontAmount(unsigned int *const fontAmount);
bool readConfigFontLength(const unsigned int fontAmount, unsigned int *const userFontLength);
bool readConfigFillFontArray(const unsigned int currentFont, char *font);
bool readConfigFontOffsets(int *const textOffsetX, int *const textOffsetY, int *const drawableCommandOffsetX, int *const drawableCommandOffsetY);

#endif
