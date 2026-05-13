#include "render.h"
#include "game.h"
#include <cmath>

static float Clamp01(float t) { return t < 0.f ? 0.f : t > 1.f ? 1.f : t; }

static float Smooth(float t) { return t * t * (3.f - 2.f * t); }

static void DrawTile(Texture2D tex, float x, float y, float w, float h, Color tint = WHITE)
{
    DrawTexturePro(
        tex,
        Rectangle{ 0, 0, (float)tex.width, (float)tex.height },
        Rectangle{ x, y, w, h },
        Vector2{ 0, 0 }, 0.f, tint
    );
}

static unsigned Hash2(int x, int y, int seed)
{
    unsigned ux = (unsigned)x, uy = (unsigned)y, us = (unsigned)seed;
    unsigned h = ux * 374761393u + uy * 668265263u + us * 1442695041u;
    h ^= h >> 13; h *= 1274126177u; h ^= h >> 16;
    return h;
}

Vector2 GetPlayerDrawPos(const GameState& g)
{
    if (!g.moving)
        return { (float)g.playerX * g.tileSize, (float)g.playerY * g.tileSize };

    float t = Clamp01(g.moveT / MOVE_DURATION);
    float s = Smooth(t);
    float x = (g.fromX + (g.toX - g.fromX) * s) * (float)g.tileSize;
    float y = (g.fromY + (g.toY - g.fromY) * s) * (float)g.tileSize;
    y += std::sin(t * PI_F) * 5.f;  
    return { x, y };
}

static void DrawGroundLayer(const GameState& g, const Textures& tex, int ox, int oy)
{
    for (int y = 0; y < g.mapH; ++y) {
        for (int x = 0; x < g.mapW; ++x) {
            unsigned h = Hash2(x, y, g.levelIndex + 11);
            Texture2D ground = (h % 7u == 0u) ? tex.ground2 : tex.ground1;

            DrawTile(ground,
                (float)(ox + x * g.tileSize),
                (float)(oy + y * g.tileSize),
                (float)g.tileSize, (float)g.tileSize);


            if (g.terrain[y][x] == ' ' && g.targets[y][x] == ' ') {
                if (h % 23u == 0u) {
                    Texture2D fl = (h & 1u) ? tex.flower1 : tex.flower2;
                    float margin = g.tileSize * 14.f / 56.f;
                    DrawTile(fl,
                        (float)(ox + x * g.tileSize) + margin,
                        (float)(oy + y * g.tileSize) + margin,
                        (float)(g.tileSize) - margin * 2,
                        (float)(g.tileSize) - margin * 2);
                }
            }
        }
    }
}

static void DrawTargets(const GameState& g, const Textures& tex, int ox, int oy)
{
    for (int y = 0; y < g.mapH; ++y)
        for (int x = 0; x < g.mapW; ++x)
            if (HasTarget(g, x, y))
                DrawTile(tex.target,
                    (float)(ox + x * g.tileSize),
                    (float)(oy + y * g.tileSize),
                    (float)g.tileSize, (float)g.tileSize);
}

static void DrawSolids(const GameState& g, const Textures& tex, int ox, int oy)
{
    for (int y = 0; y < g.mapH; ++y) {
        for (int x = 0; x < g.mapW; ++x) {
            char c = g.terrain[y][x];
            if (c == '#') {
                DrawTile(tex.wall,
                    (float)(ox + x * g.tileSize),
                    (float)(oy + y * g.tileSize),
                    (float)g.tileSize, (float)g.tileSize);
            }
            else if (c == 'R') {
                float inset = g.tileSize * 4.f / 56.f;
                DrawTile(tex.rock,
                    (float)(ox + x * g.tileSize) + inset,
                    (float)(oy + y * g.tileSize) + inset,
                    (float)g.tileSize - inset * 2,
                    (float)g.tileSize - inset * 2);
            }
        }
    }
}

static void DrawBoxes(const GameState& g, const Textures& tex, int ox, int oy)
{
    for (int y = 0; y < g.mapH; ++y) {
        for (int x = 0; x < g.mapW; ++x) {
            if (!HasBox(g, x, y)) continue;
            if (g.moving && g.pushingBox && x == g.boxFromX && y == g.boxFromY) continue;

            Color tint = HasTarget(g, x, y) ? Color{ 120, 220, 100, 255 } : WHITE;
            DrawTile(tex.box,
                (float)(ox + x * g.tileSize),
                (float)(oy + y * g.tileSize),
                (float)g.tileSize, (float)g.tileSize, tint);
        }
    }

    if (g.moving && g.pushingBox) {
        float t = Clamp01(g.moveT / MOVE_DURATION);
        float s = Smooth(t);
        float bx = (g.boxFromX + (g.boxToX - g.boxFromX) * s) * (float)g.tileSize;
        float by = (g.boxFromY + (g.boxToY - g.boxFromY) * s) * (float)g.tileSize;
        DrawTile(tex.box,
            (float)ox + bx, (float)oy + by,
            (float)g.tileSize, (float)g.tileSize);
    }
}

static void DrawPlayer(const GameState& g, const Textures& tex, int ox, int oy)
{
    Vector2 p = GetPlayerDrawPos(g);
    float t = g.moving ? Clamp01(g.moveT / MOVE_DURATION) : 1.f;
    float scale = 1.f + (g.moving ? std::sin(t * PI_F) * 0.07f : 0.f);
    float drawSize = g.tileSize * scale;
    float offset = (g.tileSize - drawSize) * 0.5f;

    DrawTile(tex.player,
        (float)ox + p.x + offset,
        (float)oy + p.y + offset,
        drawSize, drawSize);
}

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

    // Phong Nen
    DrawRectangleRounded(
        Rectangle{ (float)hudX, (float)hudY, (float)boardW, (float)HUD_H },
        0.2f, 8,
        Fade(Color{ 12, 16, 25, 255 }, 0.85f)
    );

    // Level 
    const char* names[] = { "First Steps", "Side Step", "The Island", "Zigzag", "Fortress", "The Labyrinth" };
    const char* name = names[g.levelIndex % TOTAL_LEVELS];

    DrawText(TextFormat("Level %d / %d  –  \"%s\"", g.levelIndex + 1, TOTAL_LEVELS, name),
        hudX + 16, hudY + 10, 22, RAYWHITE);

    DrawText(TextFormat("Moves: %d", g.moves),
        hudX + 16, hudY + 36, 20, LIGHTGRAY);

    DrawText("WASD / Arrows: move   R: restart   ENTER / N: next level",
        hudX + 16, hudY + 58, 16, Color{ 160, 160, 160, 255 });
}

void DrawWinOverlay(const GameState& g, int boardW, int boardH)
{
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.50f));

    if (g.levelIndex == TOTAL_LEVELS - 1) {
        const char* line1 = "CHUC MUNG! BAN DA HOAN THANH TAT CA CAC TANG!";
        const char* line2 = "ENTER = Thoat game     R = Choi lai tu dau";

        int fs1 = 28, fs2 = 20;
        int tw1 = MeasureText(line1, fs1);
        int tw2 = MeasureText(line2, fs2);

        DrawRectangle((SCREEN_W - tw1 - 60) / 2, SCREEN_H / 2 - 55,
                      tw1 + 60, 110,
                      Fade(Color{ 30, 20, 5, 255 }, 0.92f));
        DrawRectangleLinesEx(
            Rectangle{ (float)(SCREEN_W - tw1 - 60) / 2, (float)(SCREEN_H / 2 - 55),
                       (float)(tw1 + 60), 110.f },
            2, Color{ 255, 200, 50, 200 });

        DrawText(line1, (SCREEN_W - tw1) / 2, SCREEN_H / 2 - 38, fs1, Color{ 255, 210, 50, 255 });
        DrawText(line2, (SCREEN_W - tw2) / 2, SCREEN_H / 2 + 14, fs2, Color{ 200, 200, 200, 255 });
    }
    else {
        const char* msg = "Level Clear!  Nhan ENTER hoac N de tiep tuc.";
        int fs = 30;
        int tw = MeasureText(msg, fs);
        DrawText(msg, (SCREEN_W - tw) / 2, SCREEN_H / 2 - fs / 2, fs, YELLOW);
    }
}