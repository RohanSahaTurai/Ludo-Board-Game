#include "Player.h"

/*******************************************************************************
 * Function to RESET A TOKEN
*******************************************************************************/
void inline Player_ResetToken (token_t* token)
{
  // -1 indicates the token hasn't entered the game yet
  token->position = -1;
  token->steps_moved = -1;

  token->is_safe = true;
  token->has_won = false;
}

/*******************************************************************************
 * Function to INITIALIZE TOKENS
*******************************************************************************/
void inline Player_InitPlayers(player_t* player)
{
  uint8_t i, j;

  for(i = 0; i < NbPlayerInGame; i++)
  {
    player[i].scorecard = 0;

    for(j=0; j<TOKEN_NUM; j++)
      Player_ResetToken(&player[i].token[j]);

    // Select the colour attribute of the player
    switch (i)
    {
      case 0:
        player[i].colorAttrib = COLOUR_RED;
        break;

      case 1:
        if (NbPlayerInGame == 4)
          player[i].colorAttrib = COLOUR_GREEN;
        else if (NbPlayerInGame == 2)
          player[i].colorAttrib = COLOUR_YELLOW;
        break;

      case 2:
        player[i].colorAttrib = COLOUR_YELLOW;
        break;

      case 3:
        player[i].colorAttrib = COLOUR_BLUE;
        break;
    }
  }
}


/*******************************************************************************
 * Function to PRINT TOKENS
*******************************************************************************/
void Player_PrintPlayers (const player_t* player)
{
  uint8_t i, j;

  printf("PlayerNb  Scorecard  position  steps  won? safe?\n\n");
  for(i = 0; i < NbPlayerInGame; i++)
  {
    printf("PLAYER%d  %d\t", i, player[i].scorecard);

    for(j=0; j<TOKEN_NUM; j++)
    {
      printf("%d ",player[i].token[j].position);
      printf("%d ",player[i].token[j].steps_moved);

      printf("%d ",player[i].token[j].has_won);
      printf("%d ", player[i].token[j].is_safe);

      printf("\t\t");
    }
    printf("\n");
  }
}
