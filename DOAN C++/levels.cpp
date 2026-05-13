#include "levels.h"


//  Cau Truc  ('#'=wall  'R'=rock  '@'=player  '$'=box  '.'=target)

// Level 0 – "First Steps"  (20×9, 2 box)
static const char* L0[] = {
    "####################",
    "#  @               #",
    "#          .       #",
    "#  $   RRRRRR      #",
    "#      R           #",
    "#      R   .       #",
    "#  $              #",
    "#                  #",
    "####################",
    nullptr
};

// Level 1 – "Side Step"  (22×10, 3 box)
static const char* L1[] = {
    "######################",
    "#  @                 #",
    "#             .     #",
    "#  $    R            #",
    "#       RRRR   .     #",
    "#  $         R       #",
    "#            R .     #",
    "#  $                 #",
    "#                    #",
    "######################",
    nullptr
};

// Level 2 – "The Island"  (24×10, 3 box)
static const char* L2[] = {
    "########################",
    "#  @                   #",
    "#             .        #",
    "#  $     R             #",
    "#        RRR     .     #",
    "#  $         R         #",
    "#               .      #",
    "#  $         R         #",
    "#                      #",
    "########################",
    nullptr
};

// Level 3 – "Zigzag"  (28×11, 4 box)
static const char* L3[] = {
    "############################",
    "#                          #",
    "#  @    R        .        #",
    "#  $    RR                 #",
    "#  $         RR   .        #",
    "#            RR            #",
    "#  $    RR        .        #",
    "#       RR                 #",
    "#  $              .        #",
    "#                          #",
    "############################",
    nullptr
};

// Level 4 – "Fortress"  (30×12, 4 box)
static const char* L4[] = {
    "##############################",
    "#                            #",
    "#  @       R                 #",
    "#      RR         .          #",
    "#  $   RR  RRRR              #",
    "#          R       .         #",
    "#  $   R                     #",
    "#      RR  RRR    .          #",
    "#  $                         #",
    "#  $       RR R    .         #",
    "#                            #",
    "##############################",
    nullptr
};

// Level 5 – "The Labyrinth"  (36×14, 5 box)
static const char* L5[] = {
    "####################################",
    "#                                  #",
    "#  @    RR      R          .       #",
    "#  $    RR    RRRR                 #",
    "#       RR      R          .       #",
    "#  $  RRRR              RR         #",
    "#         RR     R      RR   .     #",
    "#  $       R   RRRR    RRR         #",
    "#          R     R                 #",
    "#  $    RR R     R          .      #",
    "#       RRRR   RRRR                #",
    "#  $           R            .      #",
    "#              R                   #",
    "####################################",
    nullptr
};

static const char** LEVELS[TOTAL_LEVELS] = { L0, L1, L2, L3, L4, L5 };

static const char* LEVEL_NAMES[TOTAL_LEVELS] = {
    "First Steps",
    "Side Step",
    "The Island",
    "Zigzag",
    "Fortress",
    "The Labyrinth"
};

void LoadLevel(GameState& g, int levelIndex)
{
    g.levelIndex = levelIndex % TOTAL_LEVELS;

    const char** rows = LEVELS[g.levelIndex];

    int height = 0, width = 0;
    for (int r = 0; rows[r] != nullptr; ++r) {
        int w = (int)TextLength(rows[r]);
        if (w > width) width = w;
        ++height;
    }

    g.mapW = width;
    g.mapH = height;

    const int MARGIN = 40;
    int availW = SCREEN_W - MARGIN * 2;
    int availH = SCREEN_H - HUD_H - MARGIN * 2;
    int ts = (availW / width < availH / height) ? availW / width : availH / height;
    g.tileSize = (ts < TILE_SIZE) ? ts : TILE_SIZE;
    g.terrain.assign(height, std::string(width, ' '));
    g.targets.assign(height, std::string(width, ' '));
    g.boxes.assign(height,   std::string(width, ' '));

    for (int y = 0; y < height; ++y) {
        const char* row = rows[y];
        for (int x = 0; row[x] != '\0'; ++x) {
            char c = row[x];
            switch (c) {
            case '#': g.terrain[y][x] = '#'; break;
            case 'R': g.terrain[y][x] = 'R'; break;
            case '$': g.boxes[y][x]   = '$'; break;
            case '.': g.targets[y][x] = '.'; break;
            case '@': g.playerX = x; g.playerY = y; break;
            case '*':
                g.boxes[y][x]   = '$';
                g.targets[y][x] = '.';
                break;
            case '+':
                g.targets[y][x] = '.';
                g.playerX = x; g.playerY = y;
                break;
            default: break;
            }
        }
    }

    g.moving     = false;
    g.moveT      = 0.0f;
    g.pushingBox = false;
    g.moves      = 0;
    g.won        = false;
    g.winTimer   = 0.0f;
    g.facing     = 0;
}
