#include "levels.h"

// ────────────────────────────────────────────────────────────────────────────
//  Level data  (standard Sokoban chars)
//    '#' = wall   'R' = rock   ' ' = floor
//    '@' = player start        '$' = box
//    '.' = target              '*' = box on target (parsed as both)
// ────────────────────────────────────────────────────────────────────────────

// Level 0 – "Warm Up"  (10×8, 2 boxes)
// Lesson: push each box down one step.
static const char* L0[] = {
    "##########",
    "#   @    #",
    "#   $    #",
    "#   .    #",
    "#        #",
    "#     $  #",
    "#     .  #",
    "##########",
    nullptr
};

// Level 1 – "Three in a Row"  (12×7, 3 boxes)
// Lesson: navigate between boxes across the room.
static const char* L1[] = {
    "############",
    "#    @     #",
    "#  $  $  $ #",
    "#          #",
    "#  .  .  . #",
    "#          #",
    "############",
    nullptr
};

// Level 2 – "Detour"  (13×8, 3 boxes)
// Lesson: two boxes go down; the third must be pushed sideways.
static const char* L2[] = {
    "#############",
    "#     @     #",
    "#  $  R  $  #",
    "#     R     #",
    "#  .        #",
    "#        .  #",
    "#     $  .  #",
    "#############",
    nullptr
};

// Level 3 – "Stacked"  (12×11, 4 boxes)
// Lesson: two columns of stacked boxes; push the lower one first.
static const char* L3[] = {
    "############",
    "#   @      #",
    "#  $    $  #",
    "#          #",
    "#   R  R   #",
    "#          #",
    "#  $    $  #",
    "#          #",
    "#  .    .  #",
    "#  .    .  #",
    "############",
    nullptr
};

// Level 4 – "Staircase"  (14×8, 4 boxes)
// Lesson: diagonal arrangement; player must visit every box in order.
static const char* L4[] = {
    "##############",
    "#    @       #",
    "# $          #",
    "#    $       #",
    "#       $    #",
    "#          $ #",
    "# .  .  .  . #",
    "##############",
    nullptr
};

// ────────────────────────────────────────────────────────────────────────────
//  Level registry
// ────────────────────────────────────────────────────────────────────────────
static const char** LEVELS[TOTAL_LEVELS] = { L0, L1, L2, L3, L4 };

static const char* LEVEL_NAMES[TOTAL_LEVELS] = {
    "Warm Up",
    "Three in a Row",
    "Detour",
    "Stacked",
    "Staircase"
};

// ────────────────────────────────────────────────────────────────────────────
//  Parser
// ────────────────────────────────────────────────────────────────────────────
void LoadLevel(GameState& g, int levelIndex)
{
    // Wrap index so it loops back to the first level
    g.levelIndex = levelIndex % TOTAL_LEVELS;

    const char** rows = LEVELS[g.levelIndex];

    // Measure dimensions
    int height = 0;
    int width = 0;
    for (int r = 0; rows[r] != nullptr; ++r) {
        int w = (int)TextLength(rows[r]);
        if (w > width) width = w;
        ++height;
    }

    g.mapW = width;
    g.mapH = height;
    g.terrain.assign(height, std::string(width, ' '));
    g.targets.assign(height, std::string(width, ' '));
    g.boxes.assign(height, std::string(width, ' '));

    for (int y = 0; y < height; ++y) {
        const char* row = rows[y];
        for (int x = 0; row[x] != '\0'; ++x) {
            char c = row[x];
            switch (c) {
            case '#': g.terrain[y][x] = '#'; break;
            case 'R': g.terrain[y][x] = 'R'; break;
            case '$': g.boxes[y][x] = '$'; break;
            case '.': g.targets[y][x] = '.'; break;
            case '@': g.playerX = x; g.playerY = y; break;
            case '*': // box on target
                g.boxes[y][x] = '$';
                g.targets[y][x] = '.';
                break;
            case '+': // player on target
                g.targets[y][x] = '.';
                g.playerX = x; g.playerY = y;
                break;
            default: break;
            }
        }
    }

    // Reset game state
    g.moving = false;
    g.moveT = 0.0f;
    g.pushingBox = false;
    g.moves = 0;
    g.won = false;
    g.winTimer = 0.0f;
    g.facing = 0;
}