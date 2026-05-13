#include "textures.h"

static Texture2D SafeLoad(const char* path, Color fallback)
{
    Texture2D tex = LoadTexture(path);
    if (tex.id == 0) {
        Image img = GenImageColor(TILE_SIZE, TILE_SIZE, fallback);
        tex = LoadTextureFromImage(img);
        UnloadImage(img);
        TraceLog(LOG_WARNING, "Could not load %s – using fallback colour.", path);
    }
    SetTextureFilter(tex, TEXTURE_FILTER_POINT);
    return tex;
}

Textures LoadTextures()
{
    Textures t;
    t.player = SafeLoad("tiles/player.png", SKYBLUE);
    t.box = SafeLoad("tiles/box.png", BROWN);
    t.target = SafeLoad("tiles/target.png", GOLD);
    t.rock = SafeLoad("tiles/rock.png", GRAY);
    t.ground1 = SafeLoad("tiles/ground1.png", Color{ 62, 192, 95, 255 });
    t.ground2 = SafeLoad("tiles/ground2.png", Color{ 86, 220, 120, 255 });
    t.wall = SafeLoad("tiles/wall.png", Color{ 78, 132, 66, 255 });
    t.flower1 = SafeLoad("tiles/flower1.png", BLUE);
    t.flower2 = SafeLoad("tiles/flower2.png", RED);
    return t;
}

void UnloadTextures(Textures& t)
{
    UnloadTexture(t.player);
    UnloadTexture(t.box);
    UnloadTexture(t.target);
    UnloadTexture(t.rock);
    UnloadTexture(t.ground1);
    UnloadTexture(t.ground2);
    UnloadTexture(t.wall);
    UnloadTexture(t.flower1);
    UnloadTexture(t.flower2);
}