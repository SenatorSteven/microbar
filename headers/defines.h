/* defines.h

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

#ifndef DEFINES_H
#define DEFINES_H

/* # /user-defined constants start # # # */
#define Tab /*------------------------------*/ "    "
#define DefaultCharactersCount /*-----------*/ 255
#define DefaultLinesCount /*----------------*/ 150
/* # /user-defined constants end # # # # */

#define ContinueMode /*---------------------*/ 0
#define RestartMode /*----------------------*/ 1
#define ExitMode /*-------------------------*/ 2

#define ScanConfigMode /*-------------------*/ 0
#define TopLevelWindowsConfigMode /*--------*/ 1
#define SectionRectangleAmountConfigMode /*-*/ 2
#define SectionWindowsConfigMode /*---------*/ 3
#define SectionChildrenConfigMode /*--------*/ 4
#define ContainerWindowsConfigMode /*-------*/ 5
#define ContainerChildrenConfigMode /*------*/ 6
#define RectangleWindowsConfigMode /*-------*/ 7
#define ArrayLengthsConfigMode /*-----------*/ 8
#define FillArraysConfigMode /*-------------*/ 9
#define VariableShortcutsConfigMode /*------*/ 10
#define ShortcutsConfigMode /*--------------*/ 11
#define ButtonsConfigMode /*----------------*/ 12
#define FontAmountConfigMode /*-------------*/ 13
#define FontLengthConfigMode /*-------------*/ 14
#define FontSetConfigMode /*----------------*/ 15
#define FontOffsetsConfigMode /*------------*/ 16

typedef uint8_t bool;
typedef uint8_t Mode;
typedef uint32_t ARGB;
typedef uint8_t ConfigMode;

typedef struct{
	const Window **window;
	int ***integer;
	uint8_t integerDimension0;
	uint8_t integerDimension1;
	uint8_t integerDimension2;
	unsigned int ***unsignedInteger;
	uint8_t unsignedIntegerDimension0;
	uint8_t unsignedIntegerDimension1;
	uint8_t unsignedIntegerDimension2;
	ARGB ***argb;
	uint8_t argbDimension0;
	uint8_t argbDimension1;
	uint8_t argbDimension2;
	char ***character;
	uint8_t characterDimension0;
	uint8_t characterDimension1;
	uint8_t characterDimension2;
} ConfigInfo;

typedef struct{
	uint16_t keycode;
	uint16_t masks;
} Shortcut;

typedef struct{
	uint8_t button;
	uint16_t masks;
} Button;

#endif
