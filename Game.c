#include "Game.h"

/*******************************************************************************
 * Function to ROLL THE DICE
*******************************************************************************/
uint8_t Game_RollDice()
{
  uint8_t diceNum, randomNum;

  // Find a random number between 1 to 6
  randomNum = rand()% 6 + 1;

  if (randomNum <= ONE_PROBAB)
      diceNum = 1;

  else if (randomNum <= ONE_PROBAB + TWO_PROBAB)
      diceNum = 2;

  else if (randomNum <= ONE_PROBAB + TWO_PROBAB + THREE_PROBAB)
      diceNum = 3;

  else if (randomNum <= ONE_PROBAB + TWO_PROBAB + THREE_PROBAB + FOUR_PROBAB)
      diceNum = 4;

  else if (randomNum <= ONE_PROBAB + TWO_PROBAB + THREE_PROBAB +
                        FOUR_PROBAB + FIVE_PROBAB)
      diceNum = 5;

  else if (randomNum <= ONE_PROBAB + TWO_PROBAB + THREE_PROBAB +
                        FOUR_PROBAB + FIVE_PROBAB + SIX_PROBAB)
      diceNum = 6;

  return diceNum;
}

/*******************************************************************************
 * Function to ENTER A TOKEN IN THE GAME
 * Returns the token number that has entered the game
*******************************************************************************/
int8_t Game_EnterToken (token_t* token, uint8_t playerNb)
{
  uint8_t i;

  // Check if any token is available to be entered into the game
  for (i = 0; i < TOKEN_NUM; i++)
    // If a token has been eliminated, it can be re-entered
    if (token[i].steps_moved == -1)
    {
      if (NbPlayerInGame == 4)
        // The starting position of the token is 13 * playerNb + 1
        token[i].position = 13 * playerNb + 1;

      else if (NbPlayerInGame == 2)
        // The starting positions of player 1 and 2 are 1 and 27 respectively
        token[i].position = 26 * playerNb + 1;

      token[i].steps_moved = 0;

      token[i].is_safe = true;
      token[i].has_won = false;

      token[i].coord = PosToCoord[99];

      // return tokenNb if a token could be re-entered
      return i;
    }

  // return -1 if no token is available to be re-entered
  return -1;
}

/*******************************************************************************
 * Function to UPDATE THE TRACK POSITION
 * Assumes the UPDATE STEPS MOVED has already been called before
*******************************************************************************/
static void Game_UpdatePosition (token_t* token, const uint8_t playerNb, const uint8_t diceNb)
{
    // currPosition stores the current position of the token
    int8_t currPosition = token->position;

    // currSteps stores the number of steps the token has moved
    int8_t currSteps = token->steps_moved;

    // check if token enters the finish track
    // currSteps has already been updated for this move
    if (currSteps >= FINISH_STEPS_MOVED)
    {
      // In 4 player mode, the condition is still satisfied for player 2
      if (NbPlayerInGame == 4 || playerNb != 1)
        token->position = (currSteps - FINISH_STEPS_MOVED) + playerNb * 10 + 60;

      // if 2 players are playing, finish track of player 2 starts at 80
      else if (NbPlayerInGame == 2 && playerNb == 1)
        token->position = (currSteps - FINISH_STEPS_MOVED) + 80;
    }

    // check if track position needs to be reset
    else if (currPosition + diceNb > TRACK_RESET_POSITION)
      token->position = diceNb - (TRACK_RESET_POSITION - currPosition) - 1;

    // else, just add to the track position
    else
      token->position = currPosition + diceNb;
}

/*******************************************************************************
 * Function to UPDATE STEPS MOVED
*******************************************************************************/
static int8_t Game_UpdateSteps (token_t* token, const uint8_t diceNb)
{
  int8_t currSteps = token->steps_moved;

  // If the dice score is less than the number of steps available,
  // this is a valid move and the token can be moved forward
  if((WIN_STEPS_MOVED - currSteps) >= diceNb)
  {
    token->steps_moved = currSteps + diceNb;

    return (token->steps_moved);
  }

  // The dice score is greater than steps available
  // Therefore, the current move is discarded
  else
    return -1;
}

/*******************************************************************************
 * Function to UPDATE IS_SAFE FLAG
*******************************************************************************/
static bool Game_UpdateSafeFlag (token_t* token)
{
  uint8_t currSteps = token->steps_moved;

  // if the token is not in any of the four safe homes
  // and the token isn't inside the finish track, the token is not safe
  if((currSteps % 13)!=0 && currSteps < FINISH_STEPS_MOVED)
    token->is_safe = false;

  // else, the token is safe
  else
    token->is_safe = true;

  return (token->is_safe);
}

/*******************************************************************************
 * Function to ELIMINATE TOKEN
*******************************************************************************/
static bool Game_EliminateToken (player_t* player, uint8_t playerNb, uint8_t tokenNb)
{
  token_t currToken = player[playerNb].token[tokenNb];

  uint8_t i, j;
  bool token_eliminated = false;

  for (i = 0; i < NbPlayerInGame; i++)
  {
    // Skip checking for the current player (the player can't eliminate it's token)
    if (i != playerNb)
      for (j = 0; j < TOKEN_NUM; j ++)
      {
        if (player[i].token[j].is_safe == false &&
            currToken.position == player[i].token[j].position)
        {
          Player_ResetToken(&player[i].token[j]);

          token_eliminated = true;
        }
      }
  }

  return token_eliminated;
}

/*******************************************************************************
 * Finds the number of tokens not in game and won
 * Returns the number of tokens currently in game
*******************************************************************************/
uint8_t Game_GetTokenStat (const player_t player, uint8_t* NbEliminated, uint8_t* NbWon)
{
  *NbEliminated = 0;
  *NbWon = 0;

  for (uint8_t  i = 0; i < TOKEN_NUM; i++)
  {
    if (player.token[i].has_won == true)
      (*NbWon)++;

    else if (player.token[i].steps_moved == -1)
      (*NbEliminated)++;
  }

  // return the number of token in game
  return (TOKEN_NUM - *NbEliminated - *NbWon);
}

/*******************************************************************************
 * Function to MAKE A TOKEN WIN
*******************************************************************************/
static inline void Game_TokenWin (token_t* token)
{
  token->position = WIN_POSITION;
  token->has_won = true;
  token->is_safe = true;
}

/*******************************************************************************
 * Function to CHECK AND MAKE A PLAYER WIN
 * Updates SCOREDCARD
*******************************************************************************/
static bool Game_PlayerWin (player_t* player, uint8_t playerNb)
{
  uint8_t i;
  static uint8_t currentScorecard = 0;

  // Checks if all the tokens of the current player has won
  for (i = 0; i < TOKEN_NUM; i++)
    // if any token hasn't won yet, return false
    if (player[playerNb].token[i].has_won == false)
      return false;

  // if all the tokens have won, the player has won
  // therefore, update scorecard
  player[playerNb].scorecard = ++currentScorecard;

  return true;
}

/*******************************************************************************
 * Function to UPDATE THE COORDINATE POSITION
*******************************************************************************/
void inline Game_UpdateCoord (token_t* token, COORD newCoord)
{
  token->coord = newCoord;
}

/*******************************************************************************
 * Function to check if there is any valid move
 * First checks if any token can be moved and returns
 * Else checks if the score is 6 and if new token can be entered
 * Otherwise, no valid move
*******************************************************************************/
VALIDMOVETYPE inline Game_CheckValidMoveAvailable (token_t* token, uint8_t selectedRoll)
{
  uint8_t i;

  // Iterate through all the tokens and check if valid move is available
  for (i = 0; i < TOKEN_NUM; i++)
    // if at least one token has a valid move
    if (token[i].steps_moved != -1 &&
        (WIN_STEPS_MOVED - token[i].steps_moved) >= selectedRoll)
    {
      return MOVE_TOKEN;
    }

  // if there is no valid step, check if tokens can be entered if the score is 6
  if (selectedRoll == 6)
  {
    for (i = 0; i < TOKEN_NUM; i++)
      if (token[i].steps_moved == -1)
        return ENTER_TOKEN_ONLY;
  }

  return NO_VALID_MOVE;
}

/*******************************************************************************
 * Function to RUN ONE MOVE OF PLAYER
*******************************************************************************/
MOVESTATUS Game_PlayerMove (player_t* player, uint8_t playerNb, uint8_t tokenNb, uint8_t diceNb)
{
    int8_t stepsMoved, i;
    bool isSafe;
    static uint8_t nbPlayerWon = 0;

    player_t* currPlayer = &player[playerNb];

    token_t* currToken = &currPlayer->token[tokenNb];

    stepsMoved = Game_UpdateSteps(currToken, diceNb);

    // discard the current move
    if (stepsMoved == -1)
      return MOVE_OUTOFSTEPS;

    //check if the token is safe
    isSafe = Game_UpdateSafeFlag (currToken);

    // update position
    Game_UpdatePosition(currToken, playerNb, diceNb);

    // Check if the token has won
    if (stepsMoved == WIN_STEPS_MOVED)
    {
      Game_TokenWin(currToken);

      // Check if the player has won the game after this move
      if (Game_PlayerWin(player, playerNb))
      {
        nbPlayerWon++;

        // Check if the game has ended
        if (nbPlayerWon == (NbPlayerInGame - 1))
        {
          // Update the scorecard of the last position player
          for (i = 0; i < NbPlayerInGame; i++)
            // the scorecard of the last position is still zero
            if (player[i].scorecard == 0)
             {
              player[i].scorecard = NbPlayerInGame;
              return GAME_ENDED;
             }
        }

        else
          return PLAYER_WON;
      }

      return TOKEN_WON;
    }

    // if the current token is safe, it can't eliminate other tokens
    if (isSafe == false && Game_EliminateToken(player, playerNb, tokenNb))
      return TOKEN_ELIMINATED;

    return MOVE_END;
}
