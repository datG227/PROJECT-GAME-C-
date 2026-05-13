#include "game.h"
#include "levels.h"

bool InBounds(const GameState& g, int x, int y)
{
    return x >= 0 && x < g.mapW && y >= 0 && y < g.mapH;
}

bool HasBox(const GameState& g, int x, int y)
{
    return InBounds(g, x, y) && g.boxes[y][x] == '$';
}

bool HasTarget(const GameState& g, int x, int y)
{
    return InBounds(g, x, y) && g.targets[y][x] == '.';
}

bool IsSolid(const GameState& g, int x, int y)
{
    if (!InBounds(g, x, y))      return true;
    if (g.terrain[y][x] == '#')  return true;
    if (g.terrain[y][x] == 'R')  return true;
    return false;
}

bool CheckWin(const GameState& g)
{
    for (int y = 0; y < g.mapH; ++y)
        for (int x = 0; x < g.mapW; ++x)
            if (g.targets[y][x] == '.' && g.boxes[y][x] != '$')
                return false;
    return true;
}

bool StartMove(GameState& g, int dx, int dy)
{
    if (g.moving || g.won) return false;

    int nx = g.playerX + dx;
    int ny = g.playerY + dy;

    if (!InBounds(g, nx, ny)) return false;
    if (IsSolid(g, nx, ny))   return false;

    if (dy < 0) g.facing = 3;
    else if (dy > 0) g.facing = 0;
    else if (dx < 0) g.facing = 1;
    else             g.facing = 2;

    if (HasBox(g, nx, ny)) {
        int bx = nx + dx;
        int by = ny + dy;
        if (!InBounds(g, bx, by))            return false;
        if (IsSolid(g, bx, by))              return false;
        if (HasBox(g, bx, by))               return false;

        g.pushingBox = true;
        g.boxFromX = nx; g.boxFromY = ny;
        g.boxToX = bx; g.boxToY = by;
    }
    else {
        g.pushingBox = false;
    }

    g.fromX = g.playerX; g.fromY = g.playerY;
    g.toX = nx;        g.toY = ny;
    g.moveT = 0.0f;
    g.moving = true;
    return true;
}

static void FinishMove(GameState& g)
{
    g.playerX = g.toX;
    g.playerY = g.toY;

    if (g.pushingBox) {
        g.boxes[g.boxFromY][g.boxFromX] = ' ';
        g.boxes[g.boxToY][g.boxToX] = '$';
    }

    ++g.moves;
    g.moving = false;
    g.pushingBox = false;

    g.won = CheckWin(g);
    if (g.won) g.winTimer = 0.0f;
}

void UpdateMove(GameState& g, float dt)
{
    if (!g.moving) return;
    g.moveT += dt;
    if (g.moveT >= MOVE_DURATION) FinishMove(g);
}

void ResetLevel(GameState& g)
{
    LoadLevel(g, g.levelIndex);
}