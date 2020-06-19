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

#define ScanConfigMode /*-------------------*/ 1
#define TopLevelWindowConfigMode /*---------*/ 2
#define SectionRectangleAmountConfigMode /*-*/ 3
#define GlobalColorsConfigMode /*-----------*/ 4
#define SectionWindowsConfigMode /*---------*/ 5
#define SectionChildrenConfigMode /*--------*/ 6
#define ContainerWindowsConfigMode /*-------*/ 7
#define ContainerChildrenConfigMode /*------*/ 8
#define RectangleWindowsConfigMode /*-------*/ 9
#define ArrayLengthsConfigMode /*-----------*/ 10
#define FillArraysConfigMode /*-------------*/ 11
#define VariableShortcutsConfigMode /*------*/ 12
#define ShortcutsConfigMode /*--------------*/ 13
#define ButtonsConfigMode /*----------------*/ 14
#define FontAmountConfigMode /*-------------*/ 15
#define FontLengthConfigMode /*-------------*/ 16
#define FontSetConfigMode /*----------------*/ 17
#define FontOffsetsConfigMode /*------------*/ 18

typedef uint8_t bool;
typedef uint8_t Mode;
typedef uint32_t ARGB;

typedef struct{
	uint16_t keycode;
	uint16_t masks;
} Shortcut;

typedef struct{
	uint8_t button;
	uint16_t masks;
} Button;

#endif
