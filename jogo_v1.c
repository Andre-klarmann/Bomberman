#include "raylib.h"
#define SCREEN_WIDTH 1190
#define SCREEN_HEIGHT 910
#define BMAN_WIDTH 45
#define BMAN_HEIGHT 65
#define VELOCIDADE 4
#define SIDES 4
#define X 15
#define Y 11
#define BLOCO 70
#define N_BLOCOS 35
#define N_MUROS 5

typedef struct
{
    Rectangle rec;
    int lives;
    int points;

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

//Funcoes de jogo:
void GameOver (int gameover);
void Pause (int *pause);
void FillBlocks (Rectangle blocos[N_BLOCOS], int gameArray[X][Y]);
void GenWalls (WALL muros[N_MUROS], int tam, int gameArray[X][Y]);
//Funcoes de inimigos:
short CheckCollisionEnemy (Rectangle bmanRect, ENEMY enemyList[], int tam);
void MoveEnemy (ENEMY enemyList[2], BOMBERMAN bomberman, Rectangle blocos[N_BLOCOS], Rectangle sides[SIDES], WALL muros[N_MUROS], float velocidade);
//Funcoes de bomba:
void DropBomb (BOMB bombList[], int tam, BOMBERMAN bomberman);
short PositionBomb (BOMB *bomb, BOMBERMAN bomberman);
//Funcoes de movimento dos personagens:
short PlayerMovement (BOMBERMAN *bomberman, ENEMY enemyList[], int tamE, Rectangle sides[], Rectangle blocos[], WALL muros[N_MUROS]);
void MoveRight (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideR, WALL muros[N_MUROS], float velocidade);
void MoveLeft (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideL, WALL muros[N_MUROS], float velocidade);
void MoveUp (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideU, WALL muros[N_MUROS], float velocidade);
void MoveDown (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideD, WALL muros[N_MUROS], float velocidade);
//Funcoes de desenho:
void DrawEnemy (ENEMY enemyList[], int tam, Texture2D tex);
void DrawRecs (Rectangle listRec[], int tam);
void DrawBomb (BOMB bombList[], int tam, Texture2D bombTex);
void DrawWalls (WALL muros[N_MUROS], int tam, Texture2D tex);
void DrawBlocoTex (Rectangle blocos[], Texture2D tex);


int main (void)
{
    short gameover=1;
    int gameArray[X][Y]={};
    int pause=1;

    Texture2D bomberman_png, bomb_active_png, bloco_png, enemy_png, wall_png;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "bomberman");
    SetTargetFPS(60);
    GetFontDefault();

    //Texturas
    bomberman_png = LoadTexture ("Bomberman.png");
    bomb_active_png = LoadTexture ("Bombactive.png");
    bloco_png = LoadTexture ("Block.png");
    enemy_png = LoadTexture ("enemy.png");
    wall_png = LoadTexture ("Wall.png");

    BOMBERMAN bomberman={{BLOCO + 1, BLOCO + 1, BMAN_WIDTH, BMAN_HEIGHT}, 3, 0};
    Vector2 bombermanpos={};

    BOMB bombList[3]={};//Lista teste de bombas

    ENEMY enemyList[2]={{{600, 350, BMAN_WIDTH+5, BMAN_HEIGHT-10}, 1},
                        {{420, 350, BMAN_WIDTH+5, BMAN_HEIGHT-10}, 1}};//Lista teste de inimigos

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
    GenWalls(muros, N_MUROS, gameArray);

    //Loop do jogo
    while (!WindowShouldClose())
    {
        if (pause==1)
        {
            //Atualiza o vetor de posicao do bomberman (para passagem de parametros)
            bombermanpos.x=bomberman.rec.x;
            bombermanpos.y=bomberman.rec.y;

            //Movimento do bomberman e controle game over
            gameover=PlayerMovement (&bomberman, enemyList, 2, sides, blocos, muros);

            //Movimento dos inimigos
            MoveEnemy(enemyList, bomberman, blocos, sides, muros, 1.0f);

            //Checa se deve posicionar uma bomba
            DropBomb(bombList, 3, bomberman);
            
        }


        //Comeco do desenho
        BeginDrawing();
        ClearBackground(DARKGREEN);
        DrawRecs (sides, SIDES);
        DrawRecs (blocos, N_BLOCOS);
        DrawBlocoTex(blocos, bloco_png);
        DrawWalls(muros, N_MUROS, wall_png);
        DrawBomb (bombList, 3, bomb_active_png);
        DrawEnemy (enemyList, 2, enemy_png);
        DrawFPS(10, 10);
        DrawRectangleRec(bomberman.rec, BLANK);
        DrawTextureEx (bomberman_png, bombermanpos, 0, 0.3, WHITE);
        
        Pause(&pause);
        GameOver(gameover);

        EndDrawing();


    }
    CloseWindow();

    UnloadTexture(bomberman_png);
    UnloadTexture(bomb_active_png);
    UnloadTexture(bloco_png);
    UnloadTexture(enemy_png);

    return 0;
}

//Funcoes de jogo:
//Funcao Game Over
void GameOver (int gameover)
{    
    if (!gameover)
    {
        BeginDrawing();
        DrawText("GAME OVER", SCREEN_WIDTH/2-MeasureText("GAME OVER", 100)/2, SCREEN_HEIGHT/2-50, 100, RAYWHITE);

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
void GenWalls (WALL muros[N_MUROS], int tam, int gameArray[X][Y])
{
    int i, rx, ry, positioned;
    
    for (i=0; i<tam; i++)
    {
        positioned=0;
        
        while (!positioned)
        {
            rx=GetRandomValue(1, X);
            ry=GetRandomValue(1, Y);
            
            if (gameArray[rx][ry]==0)
            {
                muros[i].rec.x = rx * BLOCO;
                muros[i].rec.y = ry * BLOCO;
                muros[i].rec.width = BLOCO;
                muros[i].rec.height = BLOCO;
                muros[i].status = 1;
                
                gameArray[rx][ry]=1;
                
                positioned=1;
            }    
        }
    }
}

//Funcoes de inimigos:
//Funcao checa colisao com um vetor de inimigos
short CheckCollisionEnemy (Rectangle bmanRect, ENEMY enemyList[], int tam)
{
    int i=0;
    short collision=0;

    while (i<tam && !collision)
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
void MoveEnemy (ENEMY enemyList[2], BOMBERMAN bomberman, Rectangle blocos[N_BLOCOS], Rectangle sides[SIDES], WALL muros[N_MUROS], float velocidade)
{
    int i;

    for (i=0; i<2; i++)
    {
        if (enemyList[i].rec.x < bomberman.rec.x)
            MoveRight(&enemyList[i].rec, blocos, sides[0], muros, velocidade);
        
        if (enemyList[i].rec.x > bomberman.rec.x)
            MoveLeft(&enemyList[i].rec, blocos, sides[1], muros, velocidade);
        
        if (enemyList[i].rec.y > bomberman.rec.y)
            MoveUp(&enemyList[i].rec, blocos, sides[0], muros, velocidade);
        
        if (enemyList[i].rec.y < bomberman.rec.y)
            MoveDown(&enemyList[i].rec, blocos, sides[0], muros, velocidade);
    }
}

//Funcoes de bomba:
//Funcao larga a bomba
void DropBomb (BOMB bombList[], int tam, BOMBERMAN bomberman)
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
    }
}

//Funcao posiciona a bomba
short PositionBomb (BOMB *bomb, BOMBERMAN bomberman)
{
   short dropped=0;

   if ((*bomb).status==0)
   {
        (*bomb).rec.x =((int) (bomberman.rec.x + BMAN_WIDTH - 10)/70) * 70;
        (*bomb).rec.y =((int) (bomberman.rec.y + BMAN_HEIGHT - 15)/70) * 70;
        (*bomb).rec.width=BLOCO;
        (*bomb).rec.height=BLOCO;
        (*bomb).status=1;

        dropped=1;
    }

    return dropped;
}

//Funcoes de movimento dos personagens:
//Funcao controla o movimento do bomberman e colisoes
short PlayerMovement (BOMBERMAN *bomberman, ENEMY enemyList[], int tamE, Rectangle sides[], Rectangle blocos[], WALL muros[N_MUROS])
{
    short game=1;

    if (!CheckCollisionEnemy ((*bomberman).rec, enemyList, tamE))
    {
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            MoveRight (&(*bomberman).rec, blocos, sides[0], muros, VELOCIDADE);

        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            MoveLeft (&(*bomberman).rec, blocos, sides[1], muros, VELOCIDADE);

        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
            MoveUp (&(*bomberman).rec, blocos, sides[2], muros, VELOCIDADE);

        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
            MoveDown (&(*bomberman).rec, blocos, sides[3], muros, VELOCIDADE);
    }
        else
            game=0;

    return game;
}

//Funcao movimenta o bomberman para a direita
void MoveRight (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideR, WALL muros[N_MUROS], float velocidade)
{
    short collision=0;
    int i=0;
    Rectangle recRight={(*rec).x+velocidade, (*rec).y, (*rec).width, (*rec).height};

    while (i<N_BLOCOS && !collision)
    {
        if (CheckCollisionRecs (recRight, blocos[i]) || CheckCollisionRecs (recRight, sideR))
            collision=1;

        i++;
    }
    
    i=0;
    while (i<N_MUROS && !collision)
    {
        if (CheckCollisionRecs (recRight, muros[i].rec))
            collision=1;
        
        i++;
    }

    if (!collision)
        (*rec).x += velocidade;
}

//Funcao movimenta o bomberman para a esquerda
void MoveLeft (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideL, WALL muros[N_MUROS], float velocidade)
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
    
    i=0;
    while (i<N_MUROS && !collision)
    {
        if (CheckCollisionRecs (recLeft, muros[i].rec))
            collision=1;
        
        i++;
    }


    if (!collision)
        (*rec).x -= velocidade;

}

//Funcao movimenta o bomberman para cima
void MoveUp (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideU, WALL muros[N_MUROS], float velocidade)
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
    
    i=0;
    while (i<N_MUROS && !collision)
    {
        if (CheckCollisionRecs (recUp, muros[i].rec))
            collision=1;
        
        i++;
    }


    if (!collision)
        (*rec).y -= velocidade;
}

//Funcao movimenta o bomberman para baixo
void MoveDown (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideD, WALL muros[N_MUROS], float velocidade)
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
    
    i=0;
    while (i<N_MUROS && !collision)
    {
        if (CheckCollisionRecs (recDown, muros[i].rec))
            collision=1;
        
        i++;
    }

    if (!collision)
        (*rec).y += velocidade;
}

//Funcoes de desenho:
//Funcao desenha inimigos
void DrawEnemy (ENEMY enemyList[], int tam, Texture2D tex)
{
    int i;
    Vector2 pos={};

    BeginDrawing();

    for (i=0; i<tam; i++)
    {
        if (enemyList[i].status)
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