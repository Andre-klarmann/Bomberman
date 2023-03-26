
#include "raylib.h"

#define SCREEN_WIDTH 1190
#define SCREEN_HEIGHT 910
#define BMAN_WIDTH 45
#define BMAN_HEIGHT 65
#define SIDES 4
#define BOOM 5
#define X 15
#define Y 11
#define BLOCO 70
#define N_BLOCOS 35
#define N_MUROS 5
#define N_INIMIGOS 2
#define N_BOMBAS 3


typedef struct
{
    Rectangle rec;
    int lives;
    float points;

}BOMBERMAN;

typedef enum
{
    morto,
    vivo

}STATUSINIMIGO;

typedef struct
{
    Rectangle rec;
    STATUSINIMIGO status;

}ENEMY;

typedef enum
{
    inata,
    ativa,
    detonada

}STATUSBOMBA;

typedef struct
{
    Rectangle rec;
    STATUSBOMBA status;

}BOMB;

typedef struct
{
    Rectangle rec;
    int status;

}WALL;

typedef enum
{
    muro,
    porta

}STATUSPORTA;

typedef struct
{
    Rectangle rec;
    STATUSPORTA status;

}DOOR;


//Funcoes de jogo:
void GameOver (int gameover);
void Pause (int *pause);
void GameInfo (BOMBERMAN bomberman, int bombs, double time);
void FillBlocks (Rectangle blocos[N_BLOCOS], int gameArray[X][Y]);
void RandWalls (WALL muros[N_MUROS], int tam, int gameArray[X][Y]);
short GenWall (WALL *muro, int x, int y, int gameArray[X][Y]);
short GenEnemy (ENEMY *enemy, int x, int y, int gameArray[X][Y]);
void BreakWalls (Rectangle explosions[BOOM], BOMBERMAN *bman, WALL muros[], int len);
void GenDoor (DOOR *door, int gameArray[X][Y]);
void StatusDoor (DOOR *door, ENEMY enemyList[], int len);
//Funcoes de inimigos:
void RandEnemy (ENEMY enemyList[], int len, int gameArray[X][Y]);

short CheckCollisionEnemy (Rectangle bmanRect, ENEMY enemyList[], int len);
void MoveEnemy (ENEMY enemies[N_INIMIGOS], int len, BOMBERMAN bman, Rectangle blocos[N_BLOCOS], Rectangle sides[SIDES], WALL muros[N_MUROS], int lenW, DOOR door, float speed);
void KillEnemies (Rectangle explosions[BOOM], BOMBERMAN *bman, ENEMY enemies[], int lenE);
//Funcoes de bomba:
void DropBomb (BOMB bombList[], int tam, BOMBERMAN bomberman, int *bombs);
short PositionBomb (BOMB *bomb, BOMBERMAN bomberman);
void ExplodeBombs (BOMB bombList[], int lenB, BOMBERMAN *bman, ENEMY enemies[], int lenE, WALL muros[], int lenW, short *gameover, float *timer);
void DetonateBomb (BOMB *bomb, BOMBERMAN *bman, ENEMY enemies[], int lenE, WALL muros[], int lenW, short *gameover, float *timer);
void ExplodeBomberman (BOMBERMAN *bomberman, Rectangle explosions[BOOM], short *gameover);
//Funcoes de movimento dos personagens:
short PlayerMovement (BOMBERMAN *bomberman, ENEMY enemyList[], int lenE, Rectangle sides[], Rectangle blocos[], WALL muros[N_MUROS], int lenW, DOOR door);
void MoveRight (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideR, WALL muros[N_MUROS], int lenW, DOOR door, float velocidade);
void MoveLeft (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideL, WALL muros[N_MUROS], int lenW, DOOR door, float velocidade);
void MoveUp (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideU, WALL muros[N_MUROS], int lenW, DOOR door, float velocidade);
void MoveDown (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideD, WALL muros[N_MUROS], int lenW, DOOR door, float velocidade);
void CheckCollisionWalls (Rectangle rec, WALL muros[N_MUROS], int len, short *collision);
void CheckCollisionDoor (Rectangle rec, DOOR door, short *collision);
//Funcoes de desenho:
void DrawEnemy (ENEMY enemyList[N_INIMIGOS], int len, Texture2D tex);
void DrawRecs (Rectangle listRec[], int tam);
void DrawBomb (BOMB bombList[], int tam, Texture2D bombTex);
void DrawExplosions (Rectangle explosions[BOOM]);
void DrawWalls (WALL muros[N_MUROS], int tam, Texture2D tex);
void DrawDoor (DOOR door, Texture2D wallTex, Texture2D doorTex);
void DrawBlocoTex (Rectangle blocos[], Texture2D tex);

