#pragma once
#include "types.h"

void DrawFrame(const GameState& g, const Textures& tex, int boardX, int boardY);

void DrawHUD(const GameState& g, int boardX, int boardY, int boardW, int boardH);

void DrawWinOverlay(const GameState& g, int boardW, int boardH);

Vector2 GetPlayerDrawPos(const GameState& g);