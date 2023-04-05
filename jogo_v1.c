#include "raylib.h"
#include <stdio.h>
#include <string.h>
#define SCREEN_WIDTH 1190
#define SCREEN_HEIGHT 910
#define STR 21
#define RANKING 10
#define FPS 60
#define VIDAS 3
#define BMAN_WIDTH 45
#define BMAN_HEIGHT 65
#define VELOCIDADE 4
#define KILL_POINTS 100
#define SIDES 4
#define BOOM 5
#define X 15
#define Y 11
#define BLOCO 70
#define N_TEXTURAS 6
#define N_BLOCOS 35
#define N_MUROS 40
#define N_INIMIGOS 20
#define N_BOMBAS 30

typedef struct
{
    Rectangle rec;
    int lives;
    float points;
    char name[STR];

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
    explodindo,
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
    porta,
    encontrada

}STATUSPORTA;

typedef struct
{
    Rectangle rec;
    STATUSPORTA status;

}DOOR;

typedef struct
{
    char name[STR];
    float points;
    double time;
    
}PLAYERFILE;

//Funcoes de jogo:
void GameLoop (BOMBERMAN *bomberman);
void LevelPassed (short *gameState, int *level);
void GetName (char name[STR], int *namePos, short *gameState);
void RankingFile (BOMBERMAN bomberman, double time, short *gameState, PLAYERFILE *player, PLAYERFILE ranking[RANKING]);
void BubbleSort (PLAYERFILE ranking[RANKING]);
void PrintFile (PLAYERFILE ranking[RANKING], BOMBERMAN *bman, short *gameState, int *level, double *time);
void GenLevel
(short *game, int level, BOMBERMAN *bman, ENEMY enemies[], int *nEnemies, BOMB bombs[], int *nBombs, int *bombsLeft, WALL muros[], int *nWalls, DOOR *door, int array[X][Y]);
void ResetWalls (WALL muros[N_MUROS]);
void ResetEnemies(ENEMY enemyList[N_INIMIGOS]);
void ResetArray (int gameArray[X][Y]);
void FillArray (int gameArray[X][Y]);
int IncrInt (int i, int incr, int limit);
void GameOver (short *gameState, int *level, double *time, BOMBERMAN *bomberman);
void ResetLevel (short *gameState);
void Restart (short *gameState, int *level, double *time, BOMBERMAN *bomberman);
void Pause (int *pause);
void GameInfo (BOMBERMAN bomberman, DOOR door, int level, int bombs, double time);
void FillBlocks (Rectangle blocos[N_BLOCOS], int gameArray[X][Y]);
void RandWalls (WALL muros[N_MUROS], int tam, int gameArray[X][Y]);
short GenWall (WALL *muro, int x, int y, int gameArray[X][Y]);
void BreakWalls (Rectangle explosions[BOOM], BOMBERMAN *bman, WALL muros[], int len);
void FindDoor (Rectangle explosions[SIDES], DOOR *door);
void GenDoor (DOOR *door, int gameArray[X][Y]);
void StatusDoor (DOOR *door, ENEMY enemyList[], int len);
//Funcoes de inimigos:
void RandEnemy (ENEMY enemyList[], int len, int gameArray[X][Y]);
short GenEnemy (ENEMY *enemy, int x, int y, int gameArray[X][Y]);
short CheckCollisionEnemy (Rectangle bmanRect, ENEMY enemyList[], int len);
void MoveEnemy (ENEMY enemies[N_INIMIGOS], int len, BOMBERMAN bman, Rectangle blocos[N_BLOCOS], Rectangle sides[SIDES], WALL muros[N_MUROS], int lenW, DOOR door, float speed);
void KillEnemies (Rectangle explosions[BOOM], BOMBERMAN *bman, ENEMY enemies[], int lenE);
//Funcoes de bomba:
void ResetBombs (BOMB bombList[], int tam);
void DropBomb (BOMB bombList[], int tam, BOMBERMAN bomberman, int *bombs);
short PositionBomb (BOMB *bomb, BOMBERMAN bomberman);
void ExplodeBombs (BOMB bombList[], int lenB, BOMBERMAN *bman, ENEMY enemies[], int lenE, WALL muros[], int lenW, DOOR *door, short *gameover, float *timer);
void DetonateBomb (BOMB *bomb, BOMBERMAN *bman, ENEMY enemies[], int lenE, WALL muros[], int lenW, DOOR *door, short *gameover, float *timer);
void ExplodeBomberman (BOMBERMAN *bomberman, Rectangle explosions[BOOM], short *gameover);
//Funcoes de movimento dos personagens:
short PlayerMovement (BOMBERMAN *bomberman, ENEMY enemyList[], int lenE, Rectangle sides[], Rectangle blocos[], WALL muros[N_MUROS], int lenW, DOOR door);
void MoveRight (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideR, WALL muros[N_MUROS], int lenW, DOOR door, short *game, float velocidade);
void MoveLeft (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideL, WALL muros[N_MUROS], int lenW, DOOR door, short *game, float velocidade);
void MoveUp (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideU, WALL muros[N_MUROS], int lenW, DOOR door, short *game, float velocidade);
void MoveDown (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideD, WALL muros[N_MUROS], int lenW, DOOR door, short *game, float velocidade);
void CheckCollisionWalls (Rectangle rec, WALL muros[N_MUROS], int len, short *collision);
void CheckCollisionDoor (Rectangle rec, DOOR door, short *collision, short *game);
//Funcoes de desenho:
void DrawEnemy (ENEMY enemyList[N_INIMIGOS], int len, Texture2D tex);
void DrawRecs (Rectangle listRec[], int tam);
void DrawBomb (BOMB bombList[], int tam, Texture2D bombTex);
void DrawExplosions (Rectangle explosions[BOOM]);
void DrawWalls (WALL muros[N_MUROS], int tam, Texture2D tex);
void DrawDoor (DOOR door, Texture2D wallTex, Texture2D doorTex);
void DrawBlocoTex (Rectangle blocos[], Texture2D tex);


int main (void)
{
    BOMBERMAN bomberman={{BLOCO + 1, BLOCO + 1, BMAN_WIDTH, BMAN_HEIGHT}, VIDAS, 0};

    GameLoop (&bomberman);

    return 0;
}

void GameLoop (BOMBERMAN *bomberman)
{
    int level=1;
    int namePos=0;
    short gameState=1;
    int gameArray[X][Y]={};
    int pause=1;
    int bombsLeft=level+2;
    int nWalls=level+4;
    int nBombs=level+2;
    int nEnemies=level+1;
    double time=0;
    float timer=0;

    PLAYERFILE player={};
    PLAYERFILE ranking[RANKING]={};

    Texture2D bomberman_png, bomb_active_png, bloco_png, enemy_png, wall_png, door_png;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "bomberman");
    SetTargetFPS(FPS);
    GetFontDefault();

    //Texturas
    bomberman_png=LoadTexture("Bomberman.png");
    bomb_active_png = LoadTexture ("Bombactive.png");
    bloco_png = LoadTexture ("Block.png");
    enemy_png = LoadTexture ("enemy.png");
    wall_png = LoadTexture ("Wall.png");
    door_png = LoadTexture ("Door.png");

    Vector2 bombermanpos={};

    BOMB bombList[N_BOMBAS]={};//Lista teste de bombas

    ENEMY enemyList[N_INIMIGOS]={};//Lista  de inimigos

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
    RandWalls(muros, nWalls, gameArray);
    GenDoor(&door, gameArray);
    RandEnemy(enemyList, nEnemies, gameArray);

    //Loop do jogo
    while (!WindowShouldClose())
    {
        if (pause==1 && gameState==1)
        {
            time+=GetFrameTime();
            //Atualiza o vetor de posicao do bomberman (para passagem de parametros)
            bombermanpos.x=bomberman->rec.x;
            bombermanpos.y=bomberman->rec.y;

            //Movimento do bomberman e controle game over
            gameState = PlayerMovement (bomberman, enemyList, nEnemies, sides, blocos, muros, N_MUROS, door);

            //Movimento dos inimigos
            MoveEnemy(enemyList, nEnemies, *bomberman, blocos, sides, muros, nWalls, door, 1.0f);

            //Checa se deve posicionar uma bomba
            DropBomb(bombList, nBombs, *bomberman, &bombsLeft);

            //Checa se deve explodir as bombas posicionadas
            ExplodeBombs (bombList, nBombs, bomberman, enemyList, nEnemies, muros, nWalls, &door, &gameState, &timer);

            //Checa se deve mudar o status da porta
            StatusDoor (&door, enemyList, nEnemies);

        }

        GenLevel (&gameState, level, bomberman, enemyList, &nEnemies, bombList, &nBombs, &bombsLeft, muros, &nWalls, &door, gameArray);

        //Comeco do desenho
        BeginDrawing();
        ClearBackground(DARKGREEN);
        DrawRecs (sides, SIDES);
        DrawRecs (blocos, N_BLOCOS);
        DrawBlocoTex(blocos, bloco_png);
        DrawBomb (bombList, N_BOMBAS, bomb_active_png);
        DrawWalls(muros, nWalls, wall_png);
        DrawDoor (door, wall_png, door_png);
        DrawEnemy (enemyList, nEnemies, enemy_png);
        DrawRectangleRec(bomberman->rec, BLANK);
        DrawTextureEx (bomberman_png, bombermanpos, 0, 0.3, WHITE);

        Pause(&pause);
        LevelPassed(&gameState, &level);
        GameOver(&gameState, &level, &time, bomberman);
        GameInfo(*bomberman, door, level, bombsLeft, time);

        GetName(bomberman->name, &namePos, &gameState);
        RankingFile(*bomberman, time, &gameState, &player, ranking);
        PrintFile(ranking, bomberman, &gameState, &level, &time);

        EndDrawing();


    }
    CloseWindow();

    UnloadTexture(bomberman_png);
    UnloadTexture(bomb_active_png);
    UnloadTexture(bloco_png);
    UnloadTexture(enemy_png);

}

//Funcoes de jogo:
//Funcao proxima fase
void LevelPassed (short *gameState, int *level)
{
    if (*gameState==2)
    {
        BeginDrawing();
        DrawText("LEVEL PASSED!", SCREEN_WIDTH/2-MeasureText("LEVEL PASSED!", 100)/2, SCREEN_HEIGHT/2-50, 100, RAYWHITE);
        DrawText ("PRESS ENTER TO CONTINUE", SCREEN_WIDTH/2-MeasureText("PRESS ENTER TO CONTINUE", 50)/2, SCREEN_HEIGHT/2+50, 50, RAYWHITE);

        if (IsKeyPressed(KEY_ENTER))
        {
            *level+=1;
            *gameState=3;
        }
    }
}

//Funcao recebe o nome inserido do usuario
void GetName (char name[STR], int *namePos, short *gameState)
{
    int letter=0;

    if (*gameState==4)
    {
        DrawText("ENTER YOUR NAME", SCREEN_WIDTH/2-MeasureText("ENTER YOUR NAME", 100)/2, SCREEN_HEIGHT/2-50, 100, RAYWHITE);
        DrawText(TextFormat("%s", name), SCREEN_WIDTH/2-MeasureText("          ", 50)/2, SCREEN_HEIGHT/2+50, 50, RAYWHITE);
        letter=GetCharPressed();

        if (letter != 0)
        {
            name[*namePos]=(char)letter;
            name[*namePos +1]='\0';
            *namePos+=1;

            letter=0;
        }
        if (IsKeyPressed(KEY_BACKSPACE) && namePos>0)
        {
            *namePos-=1;
            name[*namePos]='\0';
        }
        if (IsKeyPressed(KEY_ENTER))
            *gameState=5;
    }
}

//Funcao abre o arquivo e escreve as informacoes
void RankingFile (BOMBERMAN bomberman, double time, short *gameState, PLAYERFILE *player, PLAYERFILE ranking[RANKING])
{
    FILE *arqBin;
    int i;
    arqBin= fopen("ranking.bin", "a+b");

    if (*gameState==5)
    {
        strcpy(player->name, bomberman.name);
        player->points=bomberman.points;
        player->time=time;

        if (!arqBin)
            DrawText("FILE COULD NOT OPEN", 10, 10, 50, RAYWHITE);
        else
        {
            fwrite(&player->name, sizeof(STR), 1, arqBin);
            fwrite(&player->points, sizeof(float), 1, arqBin);
            fwrite(&player->time, sizeof(double), 1, arqBin);
            
            rewind(arqBin);

        }
        for (i=0; i<RANKING; i++)
        {
            if (!feof(arqBin))
            {
                fread(&ranking[i].name, sizeof(STR), 1, arqBin);
                fread(&ranking[i].points, sizeof(float), 1, arqBin);
                fread(&ranking[i].time, sizeof(double), 1, arqBin);
            }
        }

        if (IsKeyPressed(KEY_ENTER))
            *gameState=6;
    }

    fclose(arqBin);
}

//Funcao se ordenacao do vetor com o ranking por bubble sort
void BubbleSort (PLAYERFILE ranking[RANKING])
{
    int i, j;
    PLAYERFILE aux={};
    
    for (i=0; i<RANKING-1; i++)
    {
        for (j=0; j<RANKING-1; j++)
        {
            if (ranking[j].points < ranking[j+1].points)
            {
                strcpy(aux.name, ranking[j].name);
                aux.points=ranking[j].points;
                aux.time=ranking[j].time;
            
                strcpy(ranking[j].name, ranking[j+1].name);
                ranking[j].points=ranking[j+1].points;
                ranking[j].time=ranking[j+1].time;
            
                strcpy(ranking[j+1].name, aux.name);
                ranking[j+1].points=aux.points;
                ranking[j+1].time=aux.time;
            }
        }
    }
}

//Funcao escreve as informacoes do arquivo na tela
void PrintFile (PLAYERFILE ranking[RANKING], BOMBERMAN *bman, short *gameState, int *level, double *time)
{
    int i, Posx=10;
    
    BubbleSort(ranking);
    
    if (*gameState==6)
    {        
        for (i=0; i<RANKING; i++)
        {
            DrawText(TextFormat("%s", ranking[i].name), 10, Posx, 50, RED);
            DrawText(TextFormat("%.f", ranking[i].points), 300, Posx, 50, RED);
            DrawText(TextFormat("%.f", ranking[i].time), 500, Posx, 50, RED);
            
            Posx+=50;
        }
        DrawText("PRESS SPACE TO CONTINUE", 10, Posx, 50, WHITE);
        if(IsKeyPressed(KEY_SPACE))
        {
            *gameState=7;
        }
    }
    if (*gameState==7)
    {
        DrawText("PRESS R TO RESTART", SCREEN_WIDTH/2-MeasureText("PRESS R TO RESTART", 50)/2, SCREEN_HEIGHT/2+50, 50, RAYWHITE);
        Restart(gameState, level, time, bman);
    }
}

//Funcao que cria a nova fase
void GenLevel
(short *game, int level, BOMBERMAN *bman, ENEMY enemies[], int *nEnemies, BOMB bombs[], int *nBombs, int *bombsLeft, WALL muros[], int *nWalls, DOOR *door, int array[X][Y])
{
    if (*game==3)
    {
        bman->rec.x=BLOCO+1;
        bman->rec.y=BLOCO+1;

        *nEnemies=IncrInt(level, 1, N_INIMIGOS);
        *nBombs=IncrInt(level, 3, N_BOMBAS);
        *bombsLeft=*nBombs;
        *nWalls=IncrInt(level, 4, N_MUROS);

        door->status=muro;

        ResetArray(array);
        GenDoor (door, array);
        ResetWalls(muros);
        RandWalls(muros, *nWalls, array);
        ResetEnemies(enemies);
        RandEnemy(enemies, *nEnemies, array);
        ResetBombs (bombs, *nBombs);

        *game=1;
    }
}

//Funcao reinicia os muros do cenario
void ResetWalls (WALL muros[N_MUROS])
{
    int i;
    
    for (i=0; i<N_MUROS; i++)
    {
        muros[i].status=0;
    }
}

//Funcao reinicia os inimigos do jogo
void ResetEnemies(ENEMY enemyList[N_INIMIGOS])
{
    int i;
    
    for (i=0; i<N_INIMIGOS; i++)
    {
        enemyList[i].status=morto;
    }
}

//Funcao reseta a matriz de posicoes do jogo
void ResetArray (int gameArray[X][Y])
{
    int i, j;

    for (i=0; i<X; i++)
    {
        for (j=0; j<Y; j++)
        {
            gameArray[i][j]=0;
        }
    }
    FillArray (gameArray);

}

//Funcao preenche as posicoes ocupadas da matriz do jogo
void FillArray (int gameArray[X][Y])
{
    int i, j;

    for (i=2; i<X; i+=2)
    {
        for (j=2; j<Y; j+=2)
        {
            gameArray[i][j]=1;
        }
    }
    for (i=0; i<SIDES; i++)
    {
        for (j=0; j<SIDES; j++)
        {
            gameArray[i][j]=1;
        }
    }
}

//Funcao incrementa um inteiro com um limite
int IncrInt (int i, int incr, int limit)
{
    if (i+incr < limit)
        return i+incr;
    else
        return limit;
}

//Funcao Game Over
void GameOver (short *gameState, int *level, double *time, BOMBERMAN *bomberman)
{
    if (*gameState==0)
    {

        DrawText("GAME OVER", SCREEN_WIDTH/2-MeasureText("GAME OVER", 100)/2, SCREEN_HEIGHT/2-50, 100, RAYWHITE);
        DrawText ("PRESS C TO CONTINUE", SCREEN_WIDTH/2-MeasureText("PRESS C TO CONTINUE", 50)/2, SCREEN_HEIGHT/2+50, 50, RAYWHITE);

        if (bomberman->lives==0)
        {
            if (IsKeyPressed (KEY_C))
                *gameState=4;

            Restart (gameState, level, time, bomberman);
        }
        else
        {
            ResetLevel (gameState);
        }
    }
}

//Funcao recomeca a fase atual do jogo
void ResetLevel (short *gameState)
{
    if (IsKeyPressed(KEY_C))
    {
        *gameState=3;
    }
}

//Funcao de recomeco do jogo
void Restart (short *gameState, int *level, double *time, BOMBERMAN *bomberman)
{
    if (IsKeyPressed (KEY_R))
    {
        *level=1;
        *time=0;
        bomberman->lives=3;
        bomberman->points=0.0f;
        *gameState=3;
    }
}

//Funcao de pausa
void Pause (int *pause)
{
    if (IsKeyPressed(KEY_P))
        *pause*=-1;

    BeginDrawing();

    if (*pause==-1)
    {
        DrawText("PAUSED", SCREEN_WIDTH/2-MeasureText("PAUSED", 100)/2, SCREEN_HEIGHT/2-50, 100, RAYWHITE);
        DrawText("arrows or wasd : move Bomberman", SCREEN_WIDTH/2-MeasureText("arrows or wasd : move Bomberman", 50)/2, SCREEN_HEIGHT/2+100, 50, RAYWHITE);
        DrawText("space : drop bomb", SCREEN_WIDTH/2-MeasureText("space : drop bomb", 50)/2, SCREEN_HEIGHT/2+150, 50, RAYWHITE);
        DrawText("enter : explode bombs", SCREEN_WIDTH/2-MeasureText("enter : explode bombs", 50)/2, SCREEN_HEIGHT/2+200, 50, RAYWHITE);
    }
}

//Imprime as informacoes do jogo na tela
void GameInfo (BOMBERMAN bomberman, DOOR door, int level, int bombs, double time)
{
    int min=(int)time/60;
    int sec=(int)time%60;
    DrawText (TextFormat("LIVES %d", bomberman.lives), SCREEN_WIDTH-MeasureText("LIVES %d", 25), 10, 25, BLACK);
    DrawText (TextFormat("BOMBS %d", bombs), SCREEN_WIDTH-MeasureText("BOMBS %d", 25), 45, 25, BLACK);
    DrawText (TextFormat("POINTS %.f", bomberman.points), 10, 10, 25, BLACK);
    DrawText (TextFormat("TIME %02d:%02d", min, sec), 10, 45, 25, BLACK);
    DrawText (TextFormat("LEVEL %d", level), SCREEN_WIDTH/2 - MeasureText("LEVEL %d", 50)/2, 10, 50, BLACK);
    
    if (door.status==encontrada)
    {
        DrawText("DOOR LOCKED", SCREEN_WIDTH/2-MeasureText("PAUSED", 100)/2, SCREEN_HEIGHT-50, 50, BLACK);
    }
}

//Funcao preenche o vetor de blocos
void FillBlocks (Rectangle blocos[N_BLOCOS], int gameArray[X][Y])
{
    int i=2, j, pos=0;

    while (i<X)
    {
        j=2;
        while (j<Y)
        {
            blocos[pos].x=i*BLOCO;
            blocos[pos].y=j*BLOCO;
            blocos[pos].width=BLOCO;
            blocos[pos].height=BLOCO;

            gameArray[i][j]=1;

            j+=2;
            pos++;

        }
        i+=2;
    }

    //Delimitando a area de 4x4 em volta do bomberman como ocupada
    for (i=0; i<SIDES; i++)
    {
        for (j=0; j<SIDES; j++)
        {
            gameArray[i][j]=1;
        }
    }
}

//Funcao gera muros aleatoriamente
void RandWalls (WALL muros[N_MUROS], int tam, int gameArray[X][Y])
{
    int i, rx, ry, positioned;

    for (i=0; i<tam; i++)
    {
        positioned=0;

        while (!positioned)
        {
            rx=GetRandomValue(1, X);
            ry=GetRandomValue(1, Y);

            positioned=GenWall (&muros[i], rx, ry, gameArray);
        }
    }
}

//Funcao que inicia os dados de um muros
short GenWall (WALL *muro, int x, int y, int gameArray[X][Y])
{
    if (gameArray[x][y]==0)
    {
        muro->rec.x = x * BLOCO;
        muro->rec.y = y * BLOCO;
        muro->rec.width = BLOCO;
        muro->rec.height = BLOCO;
        muro->status = 1;

        gameArray[x][y]=1;

        return 1;
    }
    else
        return 0;
}

//Funcao a posicao da porta aleatoriamente
void GenDoor (DOOR *door, int gameArray[X][Y])
{
    int positioned=0, rx, ry;

    while (!positioned)
    {
        rx=GetRandomValue(1, X);
        ry=GetRandomValue(1, Y);

        if (gameArray[rx][ry]==0)
        {
            door->rec.x=rx*BLOCO;
            door->rec.y=ry*BLOCO;
            door->rec.width=BLOCO;
            door->rec.height=BLOCO;
            positioned=1;

            gameArray[rx][ry]=1;
        }
    }
}

//Funcao muda o status da porta
void StatusDoor (DOOR *door, ENEMY enemyList[], int len)
{
    int i, enemiesLeft=0;

    for (i=0; i<len; i++)
    {
        if (enemyList[i].status==1)
            enemiesLeft=1;
    }

    if (!enemiesLeft)
        door->status=porta;
}

//Funcoes de inimigos:
//Funcao gera inimigos aleatoriamente
void RandEnemy (ENEMY enemyList[], int len, int gameArray[X][Y])
{
    int i, rx, ry, positioned;

    for (i=0; i<len; i++)
    {
        positioned=0;

        while (!positioned)
        {
            rx=GetRandomValue(1, X);
            ry=GetRandomValue(1, Y);

            positioned=GenEnemy(&enemyList[i], rx, ry, gameArray);
        }
    }
}

//Funcao que inicia os dados de um inimigos
short GenEnemy (ENEMY *enemy, int x, int y, int gameArray[X][Y])
{
    if (gameArray[x][y]==0)
    {
        (*enemy).rec.x=x * BLOCO;
        (*enemy).rec.y=y * BLOCO;
        (*enemy).rec.width = BMAN_WIDTH;
        (*enemy).rec.height = BMAN_HEIGHT;
        (*enemy).status = 1;

        gameArray[x][y]=1;

        return 1;
    }
    else
        return 0;
}

//Funcao destroi muros
void BreakWalls (Rectangle explosions[BOOM], BOMBERMAN *bman, WALL muros[], int len)
{
    int i, j, collision;

    for (i=0; i<len; i++)
    {
        j=0;
        collision=0;
        while (j<BOOM && collision==0)
        {
            if (CheckCollisionRecs(explosions[j], muros[i].rec) && muros[i].status==1)
            {
                muros[i].status=0;
                collision=1;
                bman->points+=KILL_POINTS/2;
            }

            j++;
        }
    }
}

//Funcao encontra a porta com as bombas
void FindDoor (Rectangle explosions[SIDES], DOOR *door)
{
    int i=0, collision=0;
    
    while (i<BOOM && !collision)
    {
        if (CheckCollisionRecs(explosions[i], door->rec))
        {
            door->status=encontrada;
        }
        
        i++;
    }
}

//Funcao checa colisao com um vetor de inimigos
short CheckCollisionEnemy (Rectangle bmanRect, ENEMY enemyList[], int len)
{
    int i=0;
    short collision=0;

    while (i<len && !collision)
    {
        if (CheckCollisionRecs (bmanRect, enemyList[i].rec) && enemyList[i].status)
        {
            collision=1;
        }

        i++;
    }

    return collision;
}

//Funcao movimenta os inimigos:
void MoveEnemy (ENEMY enemies[N_INIMIGOS], int len, BOMBERMAN bman, Rectangle blocos[N_BLOCOS], Rectangle sides[SIDES], WALL muros[N_MUROS], int lenW, DOOR door, float speed)
{
    short game;
    int i;

    for (i=0; i<len; i++)
    {
        if (enemies[i].rec.x < bman.rec.x)
            MoveRight(&enemies[i].rec, blocos, sides[0], muros, lenW, door, &game, speed);

        if (enemies[i].rec.x > bman.rec.x)
            MoveLeft(&enemies[i].rec, blocos, sides[1], muros, lenW, door, &game, speed);

        if (enemies[i].rec.y > bman.rec.y)
            MoveUp(&enemies[i].rec, blocos, sides[0], muros, lenW, door, &game, speed);

        if (enemies[i].rec.y < bman.rec.y)
            MoveDown(&enemies[i].rec, blocos, sides[0], muros, lenW, door, &game, speed);
    }
}

//Funcao mata inimigos
void KillEnemies (Rectangle explosions[BOOM], BOMBERMAN *bman, ENEMY enemies[], int lenE)
{
    int i, j, collision;

    for (i=0; i<lenE; i++)
    {
        j=0;
        collision=0;
        while (j<BOOM && collision==0)
        {
            if (CheckCollisionRecs(explosions[j], enemies[i].rec) && enemies[i].status==1)
            {
                enemies[i].status=0;
                collision=1;
                bman->points+=KILL_POINTS;
            }

            j++;
        }
    }
}

//Funcoes de bomba:
//Funcao reseta o status da bomba para inata
void ResetBombs (BOMB bombList[], int tam)
{
    int i;

    for (i=0; i<tam; i++)
    {
        bombList[i].status=inata;
    }
}

//Funcao larga a bomba
void DropBomb (BOMB bombList[], int tam, BOMBERMAN bomberman, int *bombs)
{
    int i=0;
    short dropped=0;

    if (IsKeyPressed (KEY_SPACE))
    {        
        while (i<tam && !dropped)
        {

            dropped = PositionBomb (&bombList[i], bomberman);

            i++;
        }

        if (dropped)
            *bombs-=1;
    }
}

//Funcao posiciona a bomba
short PositionBomb (BOMB *bomb, BOMBERMAN bomberman)
{
   short dropped=0;

   if (bomb->status==0)
   {
        bomb->rec.x =((int) (bomberman.rec.x + BMAN_WIDTH/2)/BLOCO) * BLOCO;
        bomb->rec.y =((int) (bomberman.rec.y + BMAN_HEIGHT/2)/BLOCO) * BLOCO;
        bomb->rec.width=BLOCO;
        bomb->rec.height=BLOCO;
        bomb->status=1;

        dropped=1;
    }

    return dropped;
}

//Funcao explode as bombas ja posicionadas
void ExplodeBombs (BOMB bombList[], int lenB, BOMBERMAN *bman, ENEMY enemies[], int lenE, WALL muros[], int lenW, DOOR *door, short *gameover, float *timer)
{
    int i;

    if (IsKeyPressed(KEY_ENTER))
        *timer=0.5f;

    if (*timer>0.0f)
    {
        *timer-=GetFrameTime();

        for (i=0; i<lenB; i++)
        {
            if (bombList[i].status==ativa || bombList[i].status==explodindo)
            {
                DetonateBomb(&bombList[i], bman, enemies, lenE, muros, lenW, door, gameover, timer);
            }
        }
    }
}

//Funcao explode uma bomba individualmente
void DetonateBomb (BOMB *bomb, BOMBERMAN *bman, ENEMY enemies[], int lenE, WALL muros[], int lenW, DOOR *door, short *gameover, float *timer)
{
    Rectangle explosions[BOOM]=
    {{bomb->rec.x, bomb->rec.y, BLOCO, BLOCO},
     {bomb->rec.x+BLOCO, bomb->rec.y, BLOCO, BLOCO},
     {bomb->rec.x-BLOCO, bomb->rec.y, BLOCO, BLOCO},
     {bomb->rec.x, bomb->rec.y-BLOCO, BLOCO, BLOCO},
     {bomb->rec.x, bomb->rec.y+BLOCO, BLOCO, BLOCO}};

    if (bomb->status==ativa)
    {
        KillEnemies (explosions, bman, enemies, lenE);
        BreakWalls (explosions, bman, muros, lenW);
        FindDoor(explosions, door);
        ExplodeBomberman (bman, explosions, gameover);
        bomb->status=explodindo;
    }


    if (*timer<=0)
        bomb->status=detonada;

    DrawExplosions (explosions);
}

//Funcao explode o bomberman
void ExplodeBomberman (BOMBERMAN *bomberman, Rectangle explosions[BOOM], short *gameover)
{
    int i=0;

    while (i<BOOM && *gameover==1)
    {
        if (CheckCollisionRecs(bomberman->rec, explosions[i]))
        {
            bomberman->lives-=1;
            *gameover=0;
        }

        i++;
    }
}

//Funcoes de movimento dos personagens:
//Funcao controla o movimento do bomberman e colisoes
short PlayerMovement (BOMBERMAN *bomberman, ENEMY enemyList[], int lenE, Rectangle sides[], Rectangle blocos[], WALL muros[N_MUROS], int lenW, DOOR door)
{
    short game=1;

    if (!CheckCollisionEnemy ((*bomberman).rec, enemyList, lenE))
    {
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            MoveRight (&bomberman->rec, blocos, sides[0], muros, lenW, door, &game, VELOCIDADE);

        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            MoveLeft (&bomberman->rec, blocos, sides[1], muros, lenW, door, &game, VELOCIDADE);

        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
            MoveUp (&bomberman->rec, blocos, sides[2], muros, lenW, door, &game, VELOCIDADE);

        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
            MoveDown (&bomberman->rec, blocos, sides[3], muros, lenW, door, &game, VELOCIDADE);
    }
    else
    {
        bomberman->lives-=1;
        game=0;
    }

    return game;
}

//Funcao movimenta o bomberman para a direita
void MoveRight (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideR, WALL muros[N_MUROS], int lenW, DOOR door, short *game, float velocidade)
{
    short collision=0;
    int i=0;
    Rectangle recRight={rec->x+velocidade, rec->y, rec->width, rec->height};

    while (i<N_BLOCOS && !collision)
    {
        if (CheckCollisionRecs (recRight, blocos[i]) || CheckCollisionRecs (recRight, sideR))
            collision=1;

        i++;
    }

    CheckCollisionWalls (recRight, muros, lenW, &collision);
    CheckCollisionDoor (recRight, door, &collision, game);

    if (!collision)
        (*rec).x += velocidade;
}

//Funcao movimenta o bomberman para a esquerda
void MoveLeft (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideL, WALL muros[N_MUROS], int lenW, DOOR door, short *game, float velocidade)
{
    short collision=0;
    int i=0;
    Rectangle recLeft={(*rec).x-velocidade, (*rec).y, (*rec).width, (*rec).height};

    while (i<N_BLOCOS && !collision)
    {
        if (CheckCollisionRecs (recLeft, blocos[i]) || CheckCollisionRecs (recLeft, sideL))
            collision=1;

        i++;
    }

    CheckCollisionWalls (recLeft, muros, lenW, &collision);
    CheckCollisionDoor (recLeft, door, &collision, game);

    if (!collision)
        (*rec).x -= velocidade;

}

//Funcao movimenta o bomberman para cima
void MoveUp (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideU, WALL muros[N_MUROS], int lenW, DOOR door, short *game, float velocidade)
{
    short collision=0;
    int i=0;
    Rectangle recUp={(*rec).x, (*rec).y-velocidade, (*rec).width, (*rec).height};

    while (i<N_BLOCOS && !collision)
    {
        if (CheckCollisionRecs (recUp, blocos[i]) || CheckCollisionRecs (recUp, sideU))
            collision=1;

        i++;
    }

    CheckCollisionWalls (recUp, muros, lenW, &collision);
    CheckCollisionDoor (recUp, door, &collision, game);

    if (!collision)
        (*rec).y -= velocidade;
}

//Funcao movimenta o bomberman para baixo
void MoveDown (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideD, WALL muros[N_MUROS], int lenW, DOOR door, short *game, float velocidade)
{
    short collision=0;
    int i=0;
    Rectangle recDown={(*rec).x, (*rec).y+velocidade, (*rec).width, (*rec).height};

    while (i<N_BLOCOS && !collision)
    {
        if (CheckCollisionRecs (recDown, blocos[i]) || CheckCollisionRecs (recDown, sideD))
            collision=1;

        i++;
    }

    CheckCollisionWalls (recDown, muros, lenW, &collision);
    CheckCollisionDoor (recDown, door, &collision, game);

    if (!collision)
        (*rec).y += velocidade;
}

//Funcao checa colisoes com os muros
void CheckCollisionWalls (Rectangle rec, WALL muros[N_MUROS], int len, short *collision)
{
    int i=0;

    while (i<len && !*collision)
    {
        if (muros[i].status==1)
        {
            if (CheckCollisionRecs(rec, muros[i].rec))
                *collision=1;
        }

        i++;
    }
}

//Funcao checa colisao com a porta
void CheckCollisionDoor (Rectangle rec, DOOR door, short *collision, short *game)
{
    if (!*collision)
    {
        if ((door.status==muro || door.status==encontrada) && CheckCollisionRecs(rec, door.rec))
            *collision=1;
        else if (door.status==porta && CheckCollisionRecs(rec, door.rec))
            *game=2;
    }
}

//Funcoes de desenho:
//Funcao desenha inimigos
void DrawEnemy (ENEMY enemyList[N_INIMIGOS], int len, Texture2D tex)
{
    int i;
    Vector2 pos={};

    BeginDrawing();

    for (i=0; i<len; i++)
    {
        if (enemyList[i].status==1)
        {
            pos.x=enemyList[i].rec.x;
            pos.y=enemyList[i].rec.y;

            DrawRectangleRec(enemyList[i].rec, BLANK);
            DrawTextureEx(tex, pos, 0, 0.35, WHITE);
        }
    }
}

//Funcao desenha retangulos de uma lista
void DrawRecs (Rectangle listRec[], int tam)
{
    int i;

    BeginDrawing();

    for (i=0; i<tam; i++)
    {
        DrawRectangleRec(listRec[i], LIGHTGRAY);
    }
}


//Funcao desenha bomba
void DrawBomb (BOMB bombList[], int tam, Texture2D bombTex)
{
    int i;
    Vector2 bombPos={0, 0};

    BeginDrawing();

    for (i=0; i<tam; i++)
    {
        if (bombList[i].status==1)
        {
            bombPos.x=bombList[i].rec.x + 15;
            bombPos.y=bombList[i].rec.y;
            DrawRectangleRec (bombList[i].rec, BLANK);
            DrawTextureEx (bombTex, bombPos, 0, 0.25, WHITE);
        }
    }
}

//Funcao desenha as explosoes
void DrawExplosions (Rectangle explosions[BOOM])
{
    int i;


    BeginDrawing();

    for (i=0; i<BOOM; i++)
    {
        DrawRectangleRec (explosions[i], ORANGE);
    }
}

//Funcao desenha muros
void DrawWalls (WALL muros[N_MUROS], int tam, Texture2D tex)
{
    int i;
    Vector2 pos={};

    BeginDrawing();

    for (i=0; i<tam; i++)
    {
        if (muros[i].status==1)
        {
            DrawRectangleRec(muros[i].rec, GRAY);
            pos.x=muros[i].rec.x;
            pos.y=muros[i].rec.y;

            DrawTextureEx (tex, pos, 0, 0.44, WHITE);
        }
    }
}

//Funcao desenha a porta
void DrawDoor (DOOR door, Texture2D wallTex, Texture2D doorTex)
{
    BeginDrawing();
    Vector2 pos={door.rec.x, door.rec.y};

    if (door.status==muro)
    {
        DrawTextureEx (wallTex, pos, 0, 0.44, WHITE);
    }
    else if (door.status==porta || door.status==encontrada)
    {
        DrawTextureEx (doorTex, pos, 0, 0.44, WHITE);
    }
}

//Funcao desenha textura dos blocos
void DrawBlocoTex (Rectangle blocos[], Texture2D tex)
{
    int i;
    Vector2 pos={};

    BeginDrawing();

    for (i=0; i<N_BLOCOS; i++)
    {
        pos.x=blocos[i].x;
        pos.y=blocos[i].y;
        DrawTextureEx (tex, pos, 0, 0.44, WHITE);
    }
}
