#include "raylib.h"
#include "types.h"
#include "textures.h"
#include "levels.h"
#include "game.h"
#include "render.h"

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "Sokoban");
    SetTargetFPS(60);

    Textures  tex  = LoadTextures();
    GameState game;
    LoadLevel(game, 0);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_R))
            ResetLevel(game);

        if (game.won) {
            game.winTimer += dt;
            bool isLastLevel = (game.levelIndex == TOTAL_LEVELS - 1);
            if (isLastLevel) {
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE))
                    break;  
                if (IsKeyPressed(KEY_R))
                    LoadLevel(game, 0);
            } else {
                if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ENTER))
                    LoadLevel(game, game.levelIndex + 1);
            }
        }
        else if (!game.moving) {
            if      (IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W)) StartMove(game,  0, -1);
            else if (IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S)) StartMove(game,  0,  1);
            else if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) StartMove(game, -1,  0);
            else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) StartMove(game,  1,  0);
        }

        UpdateMove(game, dt);

        int boardW = game.mapW * game.tileSize;
        int boardH = game.mapH * game.tileSize;
        int boardX = (SCREEN_W - boardW) / 2;
        int boardY = (SCREEN_H - boardH - HUD_H - 20) / 2;

        BeginDrawing();
        ClearBackground(Color{ 18, 21, 34, 255 });

        DrawRectangleRounded(
            Rectangle{ (float)(boardX - 6), (float)(boardY - 6),
                       (float)(boardW + 12), (float)(boardH + 12) },
            0.04f, 6,
            Fade(Color{ 6, 8, 14, 255 }, 0.9f)
        );

        DrawFrame(game, tex, boardX, boardY);
        DrawHUD(game, boardX, boardY, boardW, boardH);

        if (game.won)
            DrawWinOverlay(game, boardW, boardH);

        EndDrawing();
    }

    UnloadTextures(tex);
    CloseWindow();
    return 0;
}
