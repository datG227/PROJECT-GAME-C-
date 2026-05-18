#pragma once
#include "types.h"

// Query helpers
bool InBounds(const GameState& g, int x, int y);
bool HasBox(const GameState& g, int x, int y);
bool HasTarget(const GameState& g, int x, int y);
bool IsSolid(const GameState& g, int x, int y);   // wall or rock or OOB
bool CheckWin(const GameState& g);

// Movement
bool StartMove(GameState& g, int dx, int dy);  // returns false if blocked
void UpdateMove(GameState& g, float dt);         // call every frame

// Undo (single level restart)
void ResetLevel(GameState& g);