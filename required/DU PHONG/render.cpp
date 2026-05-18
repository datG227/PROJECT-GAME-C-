#include "render.h"
#include "game.h"
#include <cmath>

// ────────────────────────────────────────────────────────────────────────────
//  Internal helpers
// ────────────────────────────────────────────────────────────────────────────

static float Clamp01(float t) { return t < 0.f ? 0.f : t > 1.f ? 1.f : t; }

// Smoothstep easing
static float Smooth(float t) { return t * t * (3.f - 2.f * t); }

// Draw a texture scaled to an arbitrary rect
static void DrawTile(Texture2D tex, float x, float y, float w, float h, Color tint = WHITE)
{
    DrawTexturePro(
        tex,
        Rectangle{ 0, 0, (float)tex.width, (float)tex.height },
        Rectangle{ x, y, w, h },
        Vector2{ 0, 0 }, 0.f, tint
    );
}

// Cheap hash for deterministic per-cell decoration
static unsigned Hash2(int x, int y, int seed)
{
    unsigned ux = (unsigned)x, uy = (unsigned)y, us = (unsigned)seed;
    unsigned h = ux * 374761393u + uy * 668265263u + us * 1442695041u;
    h ^= h >> 13; h *= 1274126177u; h ^= h >> 16;
    return h;
}

// ────────────────────────────────────────────────────────────────────────────
//  Public: interpolated player position
// ────────────────────────────────────────────────────────────────────────────

Vector2 GetPlayerDrawPos(const GameState& g)
{
    float t = g.moving ? Clamp01(g.moveT / MOVE_DURATION) : 1.f;
    float s = Smooth(t);
    float x = (g.fromX + (g.toX - g.fromX) * s) * (float)TILE_SIZE;
    float y = (g.fromY + (g.toY - g.fromY) * s) * (float)TILE_SIZE;
    if (g.moving) y += std::sin(t * PI_F) * 5.f;   // tiny hop arc
    return { x, y };
}

// ────────────────────────────────────────────────────────────────────────────
//  Ground layer (floor + flowers)
// ────────────────────────────────────────────────────────────────────────────

static void DrawGroundLayer(const GameState& g, const Textures& tex, int ox, int oy)
{
    for (int y = 0; y < g.mapH; ++y) {
        for (int x = 0; x < g.mapW; ++x) {
            unsigned h = Hash2(x, y, g.levelIndex + 11);
            Texture2D ground = (h % 7u == 0u) ? tex.ground2 : tex.ground1;

            DrawTile(ground,
                (float)(ox + x * TILE_SIZE),
                (float)(oy + y * TILE_SIZE),
                (float)TILE_SIZE, (float)TILE_SIZE);

            // Scatter small flowers on empty floor tiles
            if (g.terrain[y][x] == ' ' && g.targets[y][x] == ' ') {
                if (h % 23u == 0u) {
                    Texture2D fl = (h & 1u) ? tex.flower1 : tex.flower2;
                    float margin = 14.f;
                    DrawTile(fl,
                        (float)(ox + x * TILE_SIZE) + margin,
                        (float)(oy + y * TILE_SIZE) + margin,
                        (float)(TILE_SIZE)-margin * 2,
                        (float)(TILE_SIZE)-margin * 2);
                }
            }
        }
    }
}

// ────────────────────────────────────────────────────────────────────────────
//  Targets
// ────────────────────────────────────────────────────────────────────────────

static void DrawTargets(const GameState& g, const Textures& tex, int ox, int oy)
{
    for (int y = 0; y < g.mapH; ++y)
        for (int x = 0; x < g.mapW; ++x)
            if (HasTarget(g, x, y))
                DrawTile(tex.target,
                    (float)(ox + x * TILE_SIZE),
                    (float)(oy + y * TILE_SIZE),
                    (float)TILE_SIZE, (float)TILE_SIZE);
}

// ────────────────────────────────────────────────────────────────────────────
//  Walls & rocks
// ────────────────────────────────────────────────────────────────────────────

static void DrawSolids(const GameState& g, const Textures& tex, int ox, int oy)
{
    for (int y = 0; y < g.mapH; ++y) {
        for (int x = 0; x < g.mapW; ++x) {
            char c = g.terrain[y][x];
            if (c == '#') {
                DrawTile(tex.wall,
                    (float)(ox + x * TILE_SIZE),
                    (float)(oy + y * TILE_SIZE),
                    (float)TILE_SIZE, (float)TILE_SIZE);
            }
            else if (c == 'R') {
                // Rock drawn slightly smaller for a 3-D feel
                float inset = 4.f;
                DrawTile(tex.rock,
                    (float)(ox + x * TILE_SIZE) + inset,
                    (float)(oy + y * TILE_SIZE) + inset,
                    (float)TILE_SIZE - inset * 2,
                    (float)TILE_SIZE - inset * 2);
            }
        }
    }
}

// ────────────────────────────────────────────────────────────────────────────
//  Boxes  (static + animating pushed box)
// ────────────────────────────────────────────────────────────────────────────

static void DrawBoxes(const GameState& g, const Textures& tex, int ox, int oy)
{
    // Static boxes (skip the one currently being animated)
    for (int y = 0; y < g.mapH; ++y) {
        for (int x = 0; x < g.mapW; ++x) {
            if (!HasBox(g, x, y)) continue;
            if (g.moving && g.pushingBox && x == g.boxFromX && y == g.boxFromY) continue;

            // Tint box green if it's sitting on a target
            Color tint = HasTarget(g, x, y) ? Color{ 120, 220, 100, 255 } : WHITE;
            DrawTile(tex.box,
                (float)(ox + x * TILE_SIZE),
                (float)(oy + y * TILE_SIZE),
                (float)TILE_SIZE, (float)TILE_SIZE, tint);
        }
    }

    // Animated (sliding) box
    if (g.moving && g.pushingBox) {
        float t = Clamp01(g.moveT / MOVE_DURATION);
        float s = Smooth(t);
        float bx = (g.boxFromX + (g.boxToX - g.boxFromX) * s) * (float)TILE_SIZE;
        float by = (g.boxFromY + (g.boxToY - g.boxFromY) * s) * (float)TILE_SIZE;
        DrawTile(tex.box,
            (float)ox + bx, (float)oy + by,
            (float)TILE_SIZE, (float)TILE_SIZE);
    }
}

// ────────────────────────────────────────────────────────────────────────────
//  Player  (with subtle scale-bounce)
// ────────────────────────────────────────────────────────────────────────────

static void DrawPlayer(const GameState& g, const Textures& tex, int ox, int oy)
{
    Vector2 p = GetPlayerDrawPos(g);
    float t = g.moving ? Clamp01(g.moveT / MOVE_DURATION) : 1.f;
    float scale = 1.f + (g.moving ? std::sin(t * PI_F) * 0.07f : 0.f);
    float drawSize = TILE_SIZE * scale;
    float offset = (TILE_SIZE - drawSize) * 0.5f;

    DrawTile(tex.player,
        (float)ox + p.x + offset,
        (float)oy + p.y + offset,
        drawSize, drawSize);
}

// ────────────────────────────────────────────────────────────────────────────
//  Public draw functions
// ────────────────────────────────────────────────────────────────────────────

void DrawFrame(const GameState& g, const Textures& tex, int boardX, int boardY)
{
    DrawGroundLayer(g, tex, boardX, boardY);
    DrawTargets(g, tex, boardX, boardY);
    DrawSolids(g, tex, boardX, boardY);
    DrawBoxes(g, tex, boardX, boardY);
    DrawPlayer(g, tex, boardX, boardY);
}

void DrawHUD(const GameState& g, int boardX, int boardY, int boardW, int boardH)
{
    int hudY = boardY + boardH + 10;
    int hudX = boardX;

    // Background panel
    DrawRectangleRounded(
        Rectangle{ (float)hudX, (float)hudY, (float)boardW, (float)HUD_H },
        0.2f, 8,
        Fade(Color{ 12, 16, 25, 255 }, 0.85f)
    );

    // Level name
    const char* names[] = { "Warm Up", "Three in a Row", "Detour", "Stacked", "Staircase" };
    const char* name = names[g.levelIndex % TOTAL_LEVELS];

    DrawText(TextFormat("Level %d / %d  –  \"%s\"", g.levelIndex + 1, TOTAL_LEVELS, name),
        hudX + 16, hudY + 10, 22, RAYWHITE);

    DrawText(TextFormat("Moves: %d", g.moves),
        hudX + 16, hudY + 36, 20, LIGHTGRAY);

    DrawText("WASD / Arrows: move   R: restart   N / Enter: next level",
        hudX + 16, hudY + 58, 16, Color{ 160, 160, 160, 255 });
}

void DrawWinOverlay(const GameState& g, int boardW, int boardH)
{
    // Semi-transparent dark veil
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.38f));

    const char* msg = (g.levelIndex == TOTAL_LEVELS - 1)
        ? "YOU WIN!  Press N to loop back."
        : "Level Clear!  Press N or wait...";

    int fs = 30;
    int tw = MeasureText(msg, fs);
    DrawText(msg, (SCREEN_W - tw) / 2, SCREEN_H / 2 - fs / 2, fs, YELLOW);
}