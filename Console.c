#include "Console.h"

// Definitions of colour attributes
const DWORD COLOUR_RED    = 4;
const DWORD COLOUR_BLUE   = 1;
const DWORD COLOUR_GREEN  = 2;
const DWORD COLOUR_YELLOW = 6;
const DWORD COLOUR_WHITE  = 7;

const COORD PosToCoord [100] =
{
  {21,31}, { 7,24}, {21,27}, {21,25}, {21,23}, {21,21},
  {16,18}, {13,18}, {10,18}, {7, 18}, {4, 18}, {1, 18}, {1, 16},
  {1,14},  { 7, 4}, {7, 14}, {10,14}, {13,14}, {16,14},
  {21,11}, {21, 9}, {21, 7}, {21, 5}, {21, 3}, {21, 1}, {24, 1},
  {27, 1}, {38, 4}, {27, 5}, {27, 7}, {27, 9}, {27,11},
  {32,14}, {35,14}, {38,14}, {41,14}, {44,14}, {47,14}, {47,16},
  {47,18}, {38,24}, {41,18}, {38,18}, {35,18}, {32,18},
  {27,21}, {27,23}, {27,25}, {27,27}, {27,29}, {27,31}, {24,31},
  {00,00}, {00,00}, {00,00}, {00,00}, {00,00}, {00,00}, {00,00}, {00,00},
  {24,29}, {24,27}, {24,25}, {24,23}, {24,21},
  {00,00}, {00,00}, {00,00}, {00,00}, {00,00},
  {4, 16}, {7, 16}, {10,16}, {13,16}, {16,16},
  {00,00}, {00,00}, {00,00}, {00,00}, {00,00},
  {24, 3}, {24, 5}, {24, 7}, {24, 9}, {24,11},
  {00,00}, {00,00}, {00,00}, {00,00}, {00,00},
  {44,16}, {41,16}, {38,16}, {35,16}, {32,16},
  {00,00}, {00,00}, {00,00}, {00,00}, {00,00}
};

/********************************************************************
* Draws a rectangle
* Final Cursor position offset from origin:
*   X = columns * 2; Y = rows
*********************************************************************/
static void Console_DrawRect(COORD originPos, uint8_t rows, uint8_t columns, WORD attrib)
{
  uint8_t rowIndex, columnIndex;
  COORD currPos = originPos;

  // Set the origin position
  SetConsoleCursorPosition(HConsole, originPos);

  // Change the color
  SetConsoleTextAttribute(HConsole, attrib);

  for (columnIndex = 0; columnIndex < columns; columnIndex++)
    printf("##");

  // The first and the last row aren't part of the loop.
  for (rowIndex = 0; rowIndex < rows - 2; rowIndex++)
  {
    // Translate the cursor one unit downwards
    currPos.Y ++;
    SetConsoleCursorPosition(HConsole, currPos);

    printf("#");

    for (columnIndex = 0; columnIndex < columns - 1; columnIndex++)
      printf("  ");

    printf("#");
  }

  // Translate the cursor one unit downwards
  currPos.Y ++;
  SetConsoleCursorPosition(HConsole, currPos);

  for (columnIndex = 0; columnIndex < columns; columnIndex++)
    printf("##");

  // Reset the color
  SetConsoleTextAttribute(HConsole, COLOUR_WHITE);
}

/********************************************************************
* Draws a grid
* Final Cursor position offset from the origin:
*    X = columns * 3 + 1; Y = rows * 2 + 1
*********************************************************************/
static void Console_DrawGrid (COORD originPos, uint8_t rows, uint8_t columns)
{
  uint8_t rowIndex, columnIndex;
  COORD currPos = originPos;

  // Set the origin position
  SetConsoleCursorPosition(HConsole, originPos);

  for (rowIndex = 0; rowIndex < rows; rowIndex++)
  {
    for (columnIndex = 0; columnIndex < columns; columnIndex++)
      printf("~~~");

    // Translate the cursor one unit downwards
    currPos.Y ++;
    SetConsoleCursorPosition(HConsole, currPos);

    for (columnIndex = 0; columnIndex < columns; columnIndex++)
      printf("|  ");

    printf("|");

    // Translate the cursor one unit downwards
    currPos.Y ++;
    SetConsoleCursorPosition(HConsole, currPos);
  }

  for (columnIndex = 0; columnIndex < columns; columnIndex++)
    printf("~~~");
}

/********************************************************************
* Draws the ludo board
*********************************************************************/
void Console_DrawBoard(void)
{
  DWORD nbWritten;
  COORD coord;

  COORD pos = {0,0};
  Console_DrawRect(pos, 13, 10, COLOUR_GREEN);

  pos.X = 20;
  Console_DrawGrid(pos, 6, 3);

  // Colour the finish track
  for (coord.X = 23, coord.Y = 2; coord.Y <= 12; coord.Y++)
    FillConsoleOutputAttribute(HConsole, COLOUR_YELLOW, 4, coord, &nbWritten);

  // Colour the start cell
  for (coord.X = 27, coord.Y = 2; coord.Y <= 4; coord.Y++)
    FillConsoleOutputAttribute(HConsole, COLOUR_YELLOW, 3, coord, &nbWritten);

  pos.X = 31;
  Console_DrawRect(pos, 13, 10, COLOUR_YELLOW);

  /********************************************/
  pos.X = 0;
  pos.Y = 13;
  Console_DrawGrid(pos, 3, 6);

  // Colour the finish track
  for (coord.X = 3, coord.Y = 15; coord.Y <= 17 ; coord.Y++)
    FillConsoleOutputAttribute(HConsole, COLOUR_GREEN, 16, coord,&nbWritten);

  // Colour the start cell
  for (coord.X = 3, coord.Y = 13; coord.Y <= 14; coord.Y++)
    FillConsoleOutputAttribute(HConsole, COLOUR_GREEN, 4, coord, &nbWritten);

  pos.X = 20;
  Console_DrawRect(pos, 7, 5, COLOUR_WHITE);

  pos.X = 31;
  Console_DrawGrid(pos, 3, 6);

  // Colour the finish track
  for (coord.X = 31, coord.Y = 15; coord.Y <= 17 ; coord.Y++)
    FillConsoleOutputAttribute(HConsole, COLOUR_BLUE, 16, coord,&nbWritten);

  // Colour the start cell
  for (coord.X = 43, coord.Y = 18; coord.Y <= 19; coord.Y++)
    FillConsoleOutputAttribute(HConsole, COLOUR_BLUE, 4, coord, &nbWritten);


  /******************************************/
  pos.X = 0;
  pos.Y = 20;
  Console_DrawRect(pos, 13, 10, COLOUR_RED);

  pos.X = 20;
  Console_DrawGrid(pos, 6, 3);

  // Colour the finish track
  for (coord.X = 23, coord.Y = 20; coord.Y <= 30 ; coord.Y++)
    FillConsoleOutputAttribute(HConsole, COLOUR_RED, 4, coord,&nbWritten);

  // Colour the start cell
  for (coord.X = 20, coord.Y = 28; coord.Y <= 30; coord.Y++)
    FillConsoleOutputAttribute(HConsole, COLOUR_RED, 3, coord, &nbWritten);

  pos.X = 31;
  Console_DrawRect(pos, 13, 10, COLOUR_BLUE);

  /*****************************************/
  printf("\n");
}

/********************************************************************
* Draws the info screen
* If clear box is false, prints the string in a new line
*********************************************************************/
void Console_DisplayInfo(char* buffer, bool clearBox, DWORD attrib)
{
  static COORD pos;

  if (clearBox)
  {
    pos.X = 60;
    pos.Y = 0;
    SetConsoleCursorPosition(HConsole, pos);
    Console_DrawRect(pos, 33, 20, COLOUR_WHITE);

    pos.X = 62;
    pos.Y = 1;
    SetConsoleCursorPosition(HConsole, pos);

    // Set the color
    SetConsoleTextAttribute(HConsole, attrib);

    if (buffer)
      printf("%s", buffer);
  }

  else
  {
    // New line
    pos.Y++;

    //wrap and clear the box if the end of box is reached
    if (pos.Y == 32)
    {
      //pause for a while
      Sleep(1500);

      uint8_t i;

      // Clear the box (except the first line.
      for (pos.Y = 2; pos.Y <= 31; pos.Y++)
      {
        SetConsoleCursorPosition(HConsole, pos);

        for (i = 0; i < 37; i++)
          printf(" ");
      }

      pos.Y = 2;
    }

    SetConsoleCursorPosition(HConsole, pos);

    // Set the color
    SetConsoleTextAttribute(HConsole, attrib);

    if (buffer)
      printf("%s", buffer);
  }

  // Reset the color
  SetConsoleTextAttribute(HConsole, COLOUR_WHITE);
}

/********************************************************************
* Displays the token on the ludo board
* Needs to be called after PlayerMove() has been called
* Returns the new coordinate of the token
*********************************************************************/
COORD Console_DisplayToken (int8_t position, COORD oldCoord, WORD attrib)
{
  TCHAR readChar;
  WORD readAttrib;
  COORD currCoord;
  DWORD nbChar;
  uint8_t countColumn = 0;

  if (position == -1)
    printf("ERROR! NO COORDINATES FOUND\n");

  if (position == 100)
  {
    // Write inside the win box
    currCoord.X = 21;
    currCoord.Y = 14;
  }

  else
    currCoord = PosToCoord[position];

  // Set the color
  SetConsoleTextAttribute(HConsole, attrib);

  // Get the first character
  ReadConsoleOutputCharacter(HConsole, &readChar, 1, currCoord, &nbChar);
  countColumn++;

  if (position == 1  || position == 14 || position == 27 ||
      position == 40 || position == 100)
  {
    while (readChar != ' ')
    {
      // First character has been read
      currCoord.X += 2;
      if (countColumn == 4)
      {
        currCoord.X -= 8;
        currCoord.Y++;
      }

      ReadConsoleOutputCharacter(HConsole, &readChar, 1, currCoord, &nbChar);
      countColumn++;
    }

    SetConsoleCursorPosition(HConsole, currCoord);
    printf("O");
  }

  else
  {
    SetConsoleCursorPosition(HConsole, currCoord);

    // If the current cell is empty, then print the token
    if (readChar == ' ')
      printf("O");

    // if the current cell is not empty, then check if that is opponent's token
    else if (readChar == 'O')
    {
      // Get the color attribute of the current position
      ReadConsoleOutputAttribute(HConsole, &readAttrib, 1, currCoord, &nbChar);

      // if the colour doesn't match, then this is rival token
      if (readAttrib != attrib)
        printf("O");

      // if it is current player's token, print the number of token
      else
        printf("2");
    }

    else
      printf("%c", readChar + 1);
  }

  /* Clear the previous cell */

  //Check if the token has just entered the game
  if (oldCoord.X != PosToCoord[99].X && oldCoord.Y != PosToCoord[99].Y)
  {
    ReadConsoleOutputCharacter(HConsole, &readChar, 1, oldCoord, &nbChar);

    SetConsoleCursorPosition(HConsole, oldCoord);

    if (readChar == 'O')
        printf(" ");

    else if (readChar == '2')
      printf("O");

    else
      printf("%c", readChar - 1);
  }

  // Reset the color
  SetConsoleTextAttribute(HConsole, COLOUR_WHITE);

  return currCoord;
}

/********************************************************************
* Initializes the console module
*********************************************************************/
bool Console_Init()
{
  HWND hWnd = GetConsoleWindow();

  ShowWindow(hWnd, SW_MAXIMIZE);

  HConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  // Get the standard input handle
  HStdin = GetStdHandle(STD_INPUT_HANDLE);

  // Get the current input mode, to be restored on exit
  GetConsoleMode(HStdin, &ConsoleOldMode);

  if (HConsole == INVALID_HANDLE_VALUE)
  {
    printf("Error: STD_OUTPUT_HANDLE\n");
    return false;
  }

  Console_DrawBoard();

  return true;
}

