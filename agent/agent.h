#ifndef AGENT_H
#define AGENT_H
#include <stdint.h>
#include "../game/game.h"
#include "../utils/utils.h"
#define INFINITY 999999
#define MAXMOVES 15


typedef int (*EvaluationFunction)(int game_matrix[15][15]);

typedef enum
{
    PLACE,
    MOVE
} action_id;


typedef struct
{
    uint32_t new_x;
    uint32_t new_y;
    uint32_t old_x;
    uint32_t old_y;
} action_move;

typedef struct
{
    uint32_t x;
    uint32_t y;
    uint8_t horizontal;
    uint8_t dist_from_enemy;
} action_wall;

typedef union
{
    action_wall wall;
    action_move move;
} action_info;

typedef struct action
{
    int agent;
    action_id action;
    
    action_info info;

} Action ;

typedef struct actionPair
{
    Action action;
    int evaluation;
} ActionPair;


int is_terminal_state(void);
ActionPair alpha_beta(int game_matrix[15][15], uint32_t depth, uint8_t agent, int alpha, int beta, EvaluationFunction eval);
ActionPair perform_action(int depth);
int play_action(Action a);
void sort_walls(Action arr[], int n);
#endif
