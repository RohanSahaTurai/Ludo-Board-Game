#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

/*******************************************************************************
 * Preprocessing directives
*******************************************************************************/
#define TOKEN_NUM 4

/*******************************************************************************
 * GLOBAL VARIABLE
*******************************************************************************/
// Number of players in the current game
// The number of player is set by the user
extern uint8_t NbPlayerInGame;

// Extern Colour Attributes
extern const DWORD COLOUR_RED;
extern const DWORD COLOUR_BLUE;
extern const DWORD COLOUR_GREEN;
extern const DWORD COLOUR_YELLOW;
extern const DWORD COLOUR_WHITE;

/*******************************************************************************
 * Custom Defined Structure for Tokens
*******************************************************************************/
typedef struct token
{
    int8_t position;
    int8_t steps_moved;

    bool has_won;
    bool is_safe;

    COORD coord;

}token_t;

typedef struct player
{
    token_t token [TOKEN_NUM];
    DWORD colorAttrib;
    uint8_t scorecard;

}player_t;

/*******************************************************************************
 * Function Prototype
*******************************************************************************/
void Player_ResetToken (token_t* token);

void Player_InitPlayers(player_t* player);

void Player_PrintPlayers (const player_t* player);

#endif // TOKEN_H

