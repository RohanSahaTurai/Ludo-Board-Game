#include "Game.h"
#include "Player.h"
#include "Console.h"

uint8_t NbPlayerInGame;

HANDLE HConsole;
HANDLE HStdin;
DWORD ConsoleOldMode;

typedef enum
{
  KeyEvent, MouseEvent
}InputType;

/*******************************************************************************
 * Gets the console input
 * Waits indefinitely till a input is received
*******************************************************************************/
void GetInput(InputType inpType, COORD* cursorPos)
{
  INPUT_RECORD inputBuffer;
  MOUSE_EVENT_RECORD mer;
  KEY_EVENT_RECORD ker;
  DWORD nbRead, consoleMode;

  //Flush the input buffer
  FlushConsoleInputBuffer(HStdin);

  // Set the console mode
  consoleMode = ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT;
  SetConsoleMode(HStdin, consoleMode);

  while (1)
  {
    // Wait for the events
    ReadConsoleInput(HStdin, &inputBuffer, 1, &nbRead);

    if (inpType == MouseEvent &&
        inputBuffer.EventType == MOUSE_EVENT)
    {
      mer = inputBuffer.Event.MouseEvent;

      if (mer.dwEventFlags == DOUBLE_CLICK)
      {
        *cursorPos = mer.dwMousePosition;

        // Restore input mode
        SetConsoleMode(HStdin, ConsoleOldMode);
        return;
      }
    }

    else if (inpType == KeyEvent &&
             inputBuffer.EventType == KEY_EVENT)
    {
      ker = inputBuffer.Event.KeyEvent;

      if (!ker.bKeyDown && ker.wVirtualKeyCode == VK_SPACE)
      {
        // Restore input mode
        SetConsoleMode(HStdin, ConsoleOldMode);
        return;
      }
    }
  }
}

/*******************************************************************************
 * Gets the token number based on COORD
 * Returns -1 if no token is found
*******************************************************************************/
int8_t GetTokenNb (const player_t player, COORD coordPos)
{
  uint8_t i;

  for (i = 0; i < TOKEN_NUM; i++)
  {
    if (player.token[i].coord.X == coordPos.X &&
        player.token[i].coord.Y == coordPos.Y &&
        player.token[i].has_won == false)
      return i;
  }

  return -1;
}

/*******************************************************************************
 * Gets the token number if only token is in the game
*******************************************************************************/
int8_t GetSingleTokenNb (const player_t player)
{
  uint8_t i;

  for (i = 0; i < TOKEN_NUM; i++)
  {
    if (player.token[i].steps_moved != -1 &&
        player.token[i].has_won == false)
      return i;
  }

  return -1;
}

/*******************************************************************************
 * Function to ENTER A NEW TOKEN
*******************************************************************************/
void EnterNewTokenProc(player_t* player, uint8_t playerNb)
{
  COORD newCoord;
  uint8_t tokenNb;
  char buffer [128];
  token_t* currToken;

  tokenNb = Game_EnterToken(player[playerNb].token, playerNb);

  currToken = &(player[playerNb].token[tokenNb]);

  newCoord = Console_DisplayToken(currToken->position,
                                  currToken->coord,
                                  player[playerNb].colorAttrib);

 Game_UpdateCoord(currToken, newCoord);

 sprintf(buffer, "New Token Has Been Entered!");
 Console_DisplayInfo(buffer, false, COLOUR_WHITE);
 Sleep(1500);
}

/*******************************************************************************
 * Function to ROLL THE DICE
*******************************************************************************/
Game_MoveStatus RollDiceProc (uint8_t* roll, int8_t* rollNb)
{
  char buffer[128];
  COORD clickCoord;
  int8_t startRollNb = *rollNb;

  // if the dice is six, roll the dice again
  for (; (*rollNb) < (startRollNb + 3); (*rollNb)++)
  {
    sprintf(buffer, "Hit SPACEBAR to roll the dice.");
    Console_DisplayInfo(buffer, false, COLOUR_WHITE);

    // Waits until spacebar is pressed
    GetInput(KeyEvent, &clickCoord);

    roll[*rollNb] = Game_RollDice();

    sprintf(buffer, "Dice Score = %d", roll[*rollNb]);
    Console_DisplayInfo(buffer, false, COLOUR_WHITE);
    Sleep(1000);

    if (roll[*rollNb] != 6)
    {
      (*rollNb)++;
      return MOVE_PROCEED;
    }

    // if the score is 6 three times a row, discard the move
    if (roll[startRollNb + 2] == 6)
    {
      roll[startRollNb + 2] = 0;

      sprintf(buffer, "!!! Move Discarded !!!");
      Console_DisplayInfo(buffer, false, COLOUR_WHITE);
      Sleep(2500);
      *rollNb = startRollNb;

      return MOVE_DISCARDED;
    }
  }

  return MOVE_PROCEED;
}

/*******************************************************************************
 * STARTS THE GAME
*******************************************************************************/
void StartGame()
{
  uint8_t i, j;
  char buffer[128];
  token_t* currToken;
  bool isSelectionValid;
  int selectedRoll, input;
  COORD newCoord, clickCoord;
  Game_MoveStatus move = MOVE_END;
  uint8_t roll[15] = {0}, playerNb = 0;
  uint8_t nbTokenEliminated, nbTokenWon, nbTokenInGame;
  int8_t tokenNb = -1, rollNb, unplayedRollNb, oldRollNb;

  srand(time(NULL));

  player_t player [NbPlayerInGame];

  Player_InitPlayers(player);

  while (move != GAME_ENDED)
  {
    nbTokenInGame = Game_GetTokenStat(player[playerNb], &nbTokenEliminated, &nbTokenWon);

    // Skip move for players who have already won
    if (player[playerNb].scorecard == 0)
    {
      sprintf(buffer, "Player %d's Move...", playerNb + 1);
      Console_DisplayInfo(buffer, true, player[playerNb].colorAttrib);

      // reset the number of rolls to zero at the start
      rollNb = 0;

      // roll the dice
      move = RollDiceProc(roll, &rollNb);

      // reset the number of unplayedRollNb
      unplayedRollNb = rollNb;

      // Select a score
      for (i = 0; i < rollNb && move != MOVE_DISCARDED; i++)
      {
        /* Change the score to 0 if that roll has been used */

        //reset the move status
        move = MOVE_PROCEED;

        // If there is no token in game, wait for a 6 to enter a token
        if (roll[0] == 6 && nbTokenInGame == 0)
        {
          EnterNewTokenProc(player, playerNb);

          roll[0] = 0;
          move = MOVE_END;
          unplayedRollNb--;  //one roll score has already been used
          nbTokenInGame++;
          nbTokenEliminated--;
        }

        // If the move has ended, skip all the following instructions
        if (move == MOVE_END)
          continue;

        // if only one roll, select the only score (1st one)
        if (rollNb == 1)
        {
           selectedRoll = roll[0];
           roll[0] = 0;
        }

        // reset the input validation flag
        isSelectionValid = false;

        // Select and validate a score
        while (rollNb > 1 && !isSelectionValid)
        {
          sprintf(buffer, "Dice Scores-> ");
          Console_DisplayInfo(buffer, false, COLOUR_WHITE);

          // Prints the available dice scores to play
          for (j = 0; j < rollNb; j++)
          {
            if (roll[j] != 0)
            {
              printf("%d ", roll[j]);

              if (unplayedRollNb == 1)
              {
                 selectedRoll = roll[j];
                 roll[j] = 0;
                 isSelectionValid = true;
                 Sleep(2000);
              }
            }
          }

          // Ask User to only select the score if more than one is available
          if (unplayedRollNb > 1)
          {
            sprintf(buffer, "Input: ");
            Console_DisplayInfo(buffer, false, COLOUR_WHITE);

            scanf("%d", &selectedRoll);

            // clears the input buffer
            while (getchar()!='\n');

            // Validate selected score
            for (j = 0; j < rollNb; j++)
            {
              if (roll[j] == selectedRoll)
              {
                roll[j] = 0;
                unplayedRollNb--;
                isSelectionValid = true;
                break;
              }
            }
          }
        }

        // reset the input validation flag
        isSelectionValid = false;

        // if the selected score is 6, then the user may enter a token
        // or move a token, given there is a token to be entered
        while (selectedRoll == 6 && nbTokenEliminated > 0 && isSelectionValid == false)
        {
          sprintf(buffer, "Choice: 1)Enter Token 2)Move Token");
          Console_DisplayInfo(buffer, false, COLOUR_WHITE);
          sprintf(buffer, "Input: ");
          Console_DisplayInfo(buffer, false, COLOUR_WHITE);

          scanf("%d", &input);

          // assume the input is valid
          isSelectionValid = true;

          switch (input)
          {
            case 1:
              EnterNewTokenProc(player, playerNb);
              nbTokenInGame++;
              nbTokenEliminated--;
              move = MOVE_END;
              break;

            case 2:
              move = MOVE_PROCEED;
              break;

            default:
              isSelectionValid = false;
              break;
          }

        }

        if (nbTokenInGame && move == MOVE_PROCEED)
        {
          // If only one token in game, select that token
          if (nbTokenInGame == 1)
          {
            tokenNb = GetSingleTokenNb(player[playerNb]);

            move = Game_PlayerMove(player, playerNb, tokenNb, selectedRoll);

            if (move == MOVE_OUTOFSTEPS)
            {
              sprintf(buffer, "Invalid Move!");
              Console_DisplayInfo(buffer, false, COLOUR_WHITE);
            }
          }

          // If  more than one token in game, then get the user's input
          else
          {
            while (1)
            {
              sprintf(buffer, "Double Click on a Token (Score = %d)", selectedRoll);
              Console_DisplayInfo(buffer, false, player[playerNb].colorAttrib);

              tokenNb = -1;

              // Get token number through mouse input
              while (tokenNb == -1)
              {
                GetInput(MouseEvent, &clickCoord);

                tokenNb = GetTokenNb(player[playerNb], clickCoord);
              }

              move = Game_PlayerMove(player, playerNb, tokenNb, selectedRoll);

              if (move != MOVE_OUTOFSTEPS)
                break;

              sprintf(buffer, "Invalid Move!");
              Console_DisplayInfo(buffer, false, COLOUR_WHITE);
            }
          }

          currToken = &(player[playerNb].token[tokenNb]);

          if (move != MOVE_OUTOFSTEPS)
          {
            newCoord = Console_DisplayToken(currToken->position,
                                            currToken->coord,
                                            player[playerNb].colorAttrib);

            Game_UpdateCoord(currToken, newCoord);
          }

          // If the player eliminates another token or wins, roll the dice another time
          if (move == TOKEN_WON || move == TOKEN_ELIMINATED)
          {
            // Increment nbTokenWon and decrement nbTokenInGame
            if (move == TOKEN_WON)
            {
              sprintf(buffer, "Token Won! Roll the dice again!");
              nbTokenWon++;
              nbTokenInGame--;
            }

            else
              sprintf(buffer, "Token Eliminated! Roll the dice!");

            Console_DisplayInfo(buffer, false, player[playerNb].colorAttrib);
            Sleep(2000);

            oldRollNb = rollNb;
            move = RollDiceProc(roll, &rollNb);
            unplayedRollNb = unplayedRollNb + rollNb  - oldRollNb - 1;
          }
        }
      }
    }

    // Wrap Player Number
    playerNb++;

    if (playerNb == NbPlayerInGame)
      playerNb = 0;
  }

  // playerNb is already incremented, wrap playerNb
  if (playerNb == 0)
    playerNb = NbPlayerInGame - 1;

  sprintf(buffer, "Congratulations Player %d!", playerNb);
  Console_DisplayInfo(buffer, true, player[playerNb].colorAttrib);

  sprintf(buffer, "Press Spacebar to exit...");
  Console_DisplayInfo(buffer, false, player[playerNb].colorAttrib);

  GetInput(KeyEvent, &clickCoord);
}

/*******************************************************************************
 * main
*******************************************************************************/
int main(void)
{
  char buffer[128];
  int input = 0;

  Console_Init();

  sprintf(buffer, "How many players want to play?");
  Console_DisplayInfo(buffer, true, COLOUR_WHITE);

  while (1)
  {
    sprintf(buffer, "Input (2 or 4): ");
    Console_DisplayInfo(buffer, false, COLOUR_WHITE);

    scanf("%d", &input);

    // clears the input buffer
    while (getchar()!='\n');

    if (input == 2 || input == 4 )
      break;

    sprintf(buffer, "Invalid Input! ");
    Console_DisplayInfo(buffer, false, COLOUR_RED);
  }

  // Set the current number of players
  NbPlayerInGame = input;

  sprintf(buffer, "Starting Game...");
  Console_DisplayInfo(buffer, false, COLOUR_GREEN);
  Sleep(3000);

  StartGame();

  return 0;
}
