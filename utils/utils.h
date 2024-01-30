#ifndef UTILS_H
#define UTILS_H
#include "../game/game.h"
#include "../agent/agent.h"
#include "string.h"
#define maxN 256

typedef struct position_e {
	int x;
	int y;
	int dist;
} Position_e;

int QUEUEempty(void);
void QUEUEput(Position_e val);
Position_e QUEUEget(void);
int distance_to_goal(volatile Player *p);
#endif

