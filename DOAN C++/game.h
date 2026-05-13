#pragma once
#include "types.h"

bool InBounds(const GameState& g, int x, int y);
bool HasBox(const GameState& g, int x, int y);
bool HasTarget(const GameState& g, int x, int y);
bool IsSolid(const GameState& g, int x, int y);   
bool CheckWin(const GameState& g);

bool StartMove(GameState& g, int dx, int dy); 
void UpdateMove(GameState& g, float dt);        

void ResetLevel(GameState& g);