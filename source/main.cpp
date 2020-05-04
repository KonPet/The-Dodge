#include <stdio.h>
#include <nds.h>
#include <nf_lib.h>
#include <simplenfl.h>
#include <vector>
#include <ctime>
#include <math.h>

int player_x = 116;
int player_width = 24;
int player_height = 20;
int player_vel = 2;

float fallingspeed = 0;
float bgmove = 0.2;
float bg_height = 0;

using namespace std;

class meteor {
public:
    int x = x;
    int y = y;
    int screen;
};

meteor createMeteor(int index, int x, int y) {
    meteor buffer;
    buffer.x = x;
    buffer.y = y;
    buffer.screen = 1;
    return buffer;
}

void move(int dir) {
    switch (dir) {
        case 0:
            player_x += player_vel;
            if (player_x > 255) {
                player_x -= 256 + player_width;
            }
            NF_HflipSprite(0, 0, false);
            break;
        case 1:
            player_x -= player_vel;
            if (player_x < 0 - player_width) {
                player_x += 256 + player_width;
            }
            NF_HflipSprite(0, 0, true);
            break;
    }
}

vector <meteor> fall(vector <meteor> meteors) {
    for (unsigned i = 0; i < meteors.size(); i++) {
        meteors[i].y += fallingspeed + 1;
        if (meteors[i].y > 204) {
            if (meteors[i].screen == 0) {
                meteors[i].screen = 1;
                meteors[i].x = rand() % 272 - 16;
                meteors[i].y = rand() % 192;
            } else {
                meteors[i].screen = 0;
                meteors[i].y = -16;
            }
        }
    }
    return meteors;
}

int main(int argc, char **argv) {
    srand(time(NULL));

    consoleDemoInit();
    NF_Set2D(0, 0);

    NF_SetRootFolder("NITROFS");

    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(0);

    loadbg("background", "background", 256, 256, 0, 3);
    NF_LoadTiledBg("GAME_OVER", "GAME OVER", 256, 256);
    NF_LoadTiledBg("NewHighscore", "NewHighscore", 256, 256);

    NF_InitSpriteBuffers();
    NF_InitSpriteSys(0);

    loadsprite(0, 0, 0, 32, 32, "player", "player", false);
    loadsprite(0, 1, 1, 16, 16, "meteor", "meteor", false);

    int maxScore = 0;

    scanKeys();
    int keys = keysHeld();

    bool tryagain = true;
    bool breaking = false;

    int j;

    iprintf("\x1b[1;0HUse the D-Pad to dodge the\n");
    iprintf("\x1b[2;0Hmeteors. You can wrap around on\n");
    iprintf("\x1b[3;0Hthe sides\n");
    iprintf("\x1b[5;0HThe game was mainly made by me\n");
    iprintf("\x1b[6;0H(SkilLP), but I also got alot of\n");
    iprintf("\x1b[7;0Hhelp from PolyMars and PipeWarp.\n");
    iprintf("\x1b[9;0HWhat will your highscore be?\n");
    iprintf("\x1b[10;0HGood luck,\n");
    iprintf("\x1b[12;0HSkilLP.\n");
    iprintf("\x1b[23;0HPress any button to start\n");

    while (keysHeld() == 0) { scanKeys(); }

    while (1) {
        vector <meteor> meteors(20);

        j = 0;
        for (unsigned i = 0; i < meteors.size(); i++) {
            meteors[i].x = rand() % 272 - 16;
            meteors[i].y = rand() % 384 - 192;
            meteors[i].screen = 1;
            NF_CreateSprite(0, j + 1, 1, 1, -30, 10);
            NF_EnableSpriteRotScale(0, j + 1, j, false);
            j++;
        }

        int start = time(NULL);
        int currentTime;
        bool div = false;

        consoleClear();

        while (1) {
            meteors = fall(meteors);
            scanKeys();
            keys = keysHeld();

            if (keys == KEY_LEFT) {
                move(1);
            } else if (keys == KEY_RIGHT) {
                move(0);
            }

            j = 0;
            consoleClear();
            for (unsigned int i = 0; i < meteors.size(); i++) {
                if (meteors[j].screen == 0) {
                    NF_CreateSprite(0, j + 1, 1, 1, meteors[j].x, meteors[j].y);
                    NF_SpriteRotScale(0, j, meteors[j].y, 256, 256);
                }
//                iprintf("\x1b[j;0H%i %i %i\n", meteors[j].screen, meteors[j].x, meteors[j].y);
                j++;
            }

            currentTime = time(NULL) - start;

            iprintf("\x1b[0;0HTime       : %i", currentTime);
            iprintf("\x1b[1;0HDifficulty : %i", (int) round(fallingspeed + fallingspeed));
            iprintf("\x1b[2;0HHighscore  : %i", maxScore);

            if (currentTime % 10 == 0) {
                if (!div) {
                    div = true;
                    fallingspeed += 0.5;
                    if (currentTime % 15 == 0) {
                        player_vel++;
                    }
                }
            } else {
                div = false;
            }

            NF_CreateSprite(0, 0, 0, 0, player_x - 4, 172);

            NF_SpriteOamSet(0);
            NF_ResetSpriteBuffers();
            swiWaitForVBlank();
            oamUpdate(&oamMain);

            j = 0;
            for (unsigned int i = 0; i < meteors.size(); i++) {
                if (meteors[j].screen == 0) {
                    if (meteors[j].y <= 188 && meteors[j].y >= 160) {
                        if ((player_x + 4 >= meteors[j].x && player_x + 4 <= meteors[j].x + 16) ||
                            (player_x + 20 >= meteors[j].x && player_x + 20 <= meteors[j].x + 16)) {
                            consoleClear();
//                            iprintf("\x1b[21;0H%i %i %i", meteors[j].screen, meteors[j].x, meteors[j].y);
//                            iprintf("\x1b[22;0H%i", player_x);
                            breaking = true;
                        }
                    }
                }
                j++;
            }
            if (breaking) {
                break;
            }
            bg_height += bgmove;
            NF_ScrollBg(0, 3, 0, bg_height);
//            while (1) {
//                scanKeys();
//                if (keysHeld() == KEY_B) {
//                    break;
//                }
//            }
        }

        j = 0;
        for (unsigned int i = 0; i < meteors.size(); i++) {
            if (meteors[j].screen == 0) {
                NF_DeleteSprite(0, j + 1);
            }
            j++;
        }
        meteors.clear();

        NF_SpriteOamSet(0);
        NF_ResetSpriteBuffers();
        swiWaitForVBlank();
        oamUpdate(&oamMain);\

        if (currentTime > maxScore) {
            maxScore = currentTime;
            NF_DeleteTiledBg(0, 3);
            NF_CreateTiledBg(0, 3, "NewHighscore");
            iprintf("\x1b[0;0HNEW HIGHSCORE!");
            iprintf("\x1b[3;0HScore      : %i", currentTime);
            iprintf("\x1b[4;0HHighscore  : %i", maxScore);
            iprintf("\x1b[22;0HPress A to play again");
            iprintf("\x1b[23;0HPress START to quit");
        } else {
            NF_DeleteTiledBg(0, 3);
            NF_CreateTiledBg(0, 3, "GAME OVER");
            iprintf("\x1b[0;0HWell done!");
            iprintf("\x1b[3;0HScore      : %i", currentTime);
            iprintf("\x1b[4;0HHighscore  : %i", maxScore);
            iprintf("\x1b[22;0HPress A to play again");
            iprintf("\x1b[23;0HPress START to quit");
        }

        while (1) {
            scanKeys();
            if (keysDown() == KEY_A) {
                NF_DeleteTiledBg(0, 3);
                NF_CreateTiledBg(0, 3, "background");
                fallingspeed = 0;
                player_vel = 2;
                breaking = false;
                break;
            } else if (keysDown() == KEY_START) {
                tryagain = false;
                break;
            }
        }
        if (!tryagain) {
            break;
        }
    }
}