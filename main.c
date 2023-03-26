#include "raylib.h"

#include "Biblio.h"
#define SCREEN_WIDTH 1190
#define SCREEN_HEIGHT 910
#define FPS 60
#define BMAN_WIDTH 45
#define BMAN_HEIGHT 65
#define VELOCIDADE 4
#define KILL_POINTS 100
#define SIDES 4
#define BOOM 5
#define X 15
#define Y 11
#define BLOCO 70
#define N_BLOCOS 35
#define N_MUROS 5
#define N_INIMIGOS 2
#define N_BOMBAS 3

int main (void)
{
    short gameover=1;
    int gameArray[X][Y]={};
    int pause=1;
    int bombs=N_BOMBAS;
    double time=0.0d;
    float timer=0.0f;

    Texture2D bomberman_png, bomb_active_png, bloco_png, enemy_png, wall_png, door_png;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "bomberman");
    SetTargetFPS(FPS);
    GetFontDefault();

    //Texturas
    bomberman_png = LoadTexture ("Bomberman.png");
    bomb_active_png = LoadTexture ("Bombactive.png");
    bloco_png = LoadTexture ("Block.png");
    enemy_png = LoadTexture ("enemy.png");
    wall_png = LoadTexture ("Wall.png");
    door_png = LoadTexture ("Door.png");

    BOMBERMAN bomberman={{BLOCO + 1, BLOCO + 1, BMAN_WIDTH, BMAN_HEIGHT}, 3, 0};
    Vector2 bombermanpos={};

    BOMB bombList[N_BOMBAS]={};//Lista teste de bombas

    ENEMY enemyList[N_INIMIGOS]={};//Lista teste de inimigos

    DOOR door={};

    //Paredes do cenario
    Rectangle sides[SIDES]={
        {SCREEN_WIDTH - BLOCO, 0, BLOCO, SCREEN_HEIGHT},
        {0, 0, BLOCO, SCREEN_HEIGHT},
        {0, 0, SCREEN_WIDTH, BLOCO},
        {0, SCREEN_HEIGHT - BLOCO, SCREEN_WIDTH, BLOCO}};

    WALL muros[N_MUROS]={};

    //Blocos do cenario
    Rectangle blocos[N_BLOCOS];
    FillBlocks (blocos, gameArray);
    RandWalls(muros, N_MUROS, gameArray);
    GenDoor(&door, gameArray);
    RandEnemy(enemyList, N_INIMIGOS, gameArray);

    //Loop do jogo
    while (!WindowShouldClose())
    {
        DrawBomb (bombList, N_BOMBAS, bomb_active_png);
        if (pause==1 && gameover==1)
        {
            time+=GetFrameTime();
            //Atualiza o vetor de posicao do bomberman (para passagem de parametros)
            bombermanpos.x=bomberman.rec.x;
            bombermanpos.y=bomberman.rec.y;

            //Movimento do bomberman e controle game over
            gameover=PlayerMovement (&bomberman, enemyList, N_INIMIGOS, sides, blocos, muros, N_MUROS, door);

            //Movimento dos inimigos
            MoveEnemy(enemyList, N_INIMIGOS, bomberman, blocos, sides, muros, N_MUROS, door, 1.0f);

            //Checa se deve posicionar uma bomba
            DropBomb(bombList, N_BOMBAS, bomberman, &bombs);

            //Checa se deve explodir as bombas posicionadas
            ExplodeBombs (bombList, N_BOMBAS, &bomberman, enemyList, N_INIMIGOS, muros, N_MUROS, &gameover, &timer);

            //Checa se deve mudar o status da porta
            StatusDoor (&door, enemyList, N_INIMIGOS);

        }

        //Comeco do desenho
        BeginDrawing();
        ClearBackground(DARKGREEN);
        DrawRecs (sides, SIDES);
        DrawRecs (blocos, N_BLOCOS);
        DrawBlocoTex(blocos, bloco_png);
        DrawWalls(muros, N_MUROS, wall_png);
        DrawDoor (door, wall_png, door_png);
        DrawEnemy (enemyList, N_INIMIGOS, enemy_png);
        DrawRectangleRec(bomberman.rec, BLANK);
        DrawTextureEx (bomberman_png, bombermanpos, 0, 0.3, WHITE);

        Pause(&pause);
        GameOver(gameover);
        GameInfo(bomberman, bombs, time);

        EndDrawing();


    }
    CloseWindow();

    UnloadTexture(bomberman_png);
    UnloadTexture(bomb_active_png);
    UnloadTexture(bloco_png);
    UnloadTexture(enemy_png);

    return 0;
}
