#pragma once
#include "raylib.h"
#include <vector>
#include <string>

// ────────────────────────────────────────────────────────────────────────────
//  Constants
// ────────────────────────────────────────────────────────────────────────────
static constexpr int   SCREEN_W = 1280;
static constexpr int   SCREEN_H = 800;
static constexpr int   TILE_SIZE = 56;
static constexpr int   HUD_H = 80;
static constexpr float MOVE_DURATION = 0.14f;
static constexpr float PI_F = 3.1415926f;
static constexpr int   TOTAL_LEVELS = 5;

// ────────────────────────────────────────────────────────────────────────────
//  Texture bundle
// ────────────────────────────────────────────────────────────────────────────
struct Textures {
    Texture2D player{};
    Texture2D box{};
    Texture2D target{};
    Texture2D rock{};
    Texture2D ground1{};
    Texture2D ground2{};
    Texture2D wall{};
    Texture2D flower1{};
    Texture2D flower2{};
};

// ────────────────────────────────────────────────────────────────────────────
//  Game state
// ────────────────────────────────────────────────────────────────────────────
struct GameState {
    int levelIndex = 0;
    int mapW = 0;
    int mapH = 0;

    // Three separate layers: terrain, targets, boxes
    std::vector<std::string> terrain;  // '#'=wall  'R'=rock  ' '=floor
    std::vector<std::string> targets;  // '.'=target
    std::vector<std::string> boxes;    // '$'=box

    int playerX = 0;
    int playerY = 0;

    // Smooth movement
    bool  moving = false;
    float moveT = 0.0f;
    int   fromX = 0, fromY = 0;
    int   toX = 0, toY = 0;

    // Animated box push
    bool pushingBox = false;
    int  boxFromX = 0, boxFromY = 0;
    int  boxToX = 0, boxToY = 0;

    int  facing = 0;   // 0=down 1=left 2=right 3=up
    int  moves = 0;
    bool won = false;
    float winTimer = 0.0f;
};