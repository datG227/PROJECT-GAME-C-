#pragma once
#include "types.h"

// Draw one complete frame (call between BeginDrawing / EndDrawing).
void DrawFrame(const GameState& g, const Textures& tex, int boardX, int boardY);

// Draw the HUD bar below the board.
void DrawHUD(const GameState& g, int boardX, int boardY, int boardW, int boardH);

// Draw the win overlay.
void DrawWinOverlay(const GameState& g, int boardW, int boardH);

// Interpolated player draw position (world-space, before board offset).
Vector2 GetPlayerDrawPos(const GameState& g);