#ifndef CONSOLE_H
#define CONSOLE_H

#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Extern Colour Attributes
extern const DWORD COLOUR_RED;
extern const DWORD COLOUR_BLUE;
extern const DWORD COLOUR_GREEN;
extern const DWORD COLOUR_YELLOW;
extern const DWORD COLOUR_WHITE;

// Look up table to convert position to coordinates
extern const COORD PosToCoord [100];
extern HANDLE HConsole;
extern HANDLE HStdin;
extern DWORD ConsoleOldMode;

/*******************************************************************************
 * Function Prototype
*******************************************************************************/
bool Console_Init();

void Console_DrawBoard(void);

void Console_DisplayInfo(char* buffer, bool clearBox, DWORD attrib);

COORD Console_DisplayToken (int8_t position, COORD oldCoord, WORD attrib);

#endif // CONSOLE_H
