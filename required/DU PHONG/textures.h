#pragma once
#include "types.h"

// Load all textures from the tiles/ folder.
// Missing files fall back to a coloured rectangle.
Textures LoadTextures();

// Unload all textures.
void UnloadTextures(Textures& tex);