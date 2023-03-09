#include "raylib.h"
#define SCREEN_WIDTH 1190
#define SCREEN_HEIGHT 910
#define BMAN_WIDTH 45
#define BMAN_HEIGHT 65
#define VELOCIDADE 4
#define SIDES 4
#define BLOCO 70
#define N_BLOCOS 35

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

//Funcoes de jogo:
void GameOver (int gameover);
void FillBlocks (Rectangle blocos[N_BLOCOS]);
//Funcoes de inimigos:
short CheckCollisionEnemy (Rectangle bmanRect, ENEMY enemyList[], int tam);
//Funcoes de bomba:
void DropBomb (BOMB bombList[], int tam, BOMBERMAN bomberman);
short PositionBomb (BOMB *bomb, BOMBERMAN bomberman);
//Funcoes de movimento dos personagens:
short PlayerMovement (BOMBERMAN *bomberman, ENEMY enemyList[], int tamE, Rectangle sides[], Rectangle blocos[]);
void MoveRight (BOMBERMAN *bomberman, Rectangle blocos[N_BLOCOS], Rectangle sideR);
void MoveLeft (BOMBERMAN *bomberman, Rectangle blocos[N_BLOCOS], Rectangle sideL);
void MoveUp (BOMBERMAN *bomberman, Rectangle blocos[N_BLOCOS], Rectangle sideU);
void MoveDown (BOMBERMAN *bomberman, Rectangle blocos[N_BLOCOS], Rectangle sideD);
//Funcoes de desenho:
void DrawEnemy (ENEMY enemyList[], int tam, Texture2D tex);
void DrawRecs (Rectangle listRec[], int tam);
void DrawBomb (BOMB bombList[], int tam, Texture2D bombTex);
void DrawBlocoTex (Rectangle blocos[], Texture2D tex);


int main (void)
{
    short gameover;

    Texture2D bomberman_png, bomb_active_png, bloco_png, enemy_png;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "bomberman");
    SetTargetFPS(60);
    GetFontDefault();

    //Texturas
    bomberman_png = LoadTexture ("Bomberman.png"); 
    bomb_active_png = LoadTexture ("Bombactive.png"); 
    bloco_png = LoadTexture ("Block.png");
    enemy_png = LoadTexture ("enemy.png");
         
    BOMBERMAN bomberman={{BLOCO + 1, BLOCO + 1, BMAN_WIDTH, BMAN_HEIGHT}, 3, 0};
    Vector2 bombermanpos={};
    
    BOMB bombList[3]={};//Lista teste de bombas
    
    ENEMY enemyList[2]={{{600, 350, 50, 50}, 1},
                        {{420, 350, 50, 50}, 1}};//Lista teste de inimigos
    
    //Paredes do cenario
    Rectangle sides[SIDES]={
        {SCREEN_WIDTH-BLOCO, 0, BLOCO, SCREEN_HEIGHT},
        {0, 0, BLOCO, SCREEN_HEIGHT},
        {0, 0, SCREEN_WIDTH, BLOCO},
        {0, SCREEN_HEIGHT-BLOCO, SCREEN_WIDTH, BLOCO}};
        
    //Blocos do cenario
    Rectangle blocos[N_BLOCOS];
    FillBlocks (blocos);
        
    //Loop do jogo
    while (!WindowShouldClose())
    {             
        //Atualiza o vetor de posicao do bomberman (para passagem de parametros)
        bombermanpos.x=bomberman.rec.x;
        bombermanpos.y=bomberman.rec.y;
        
        //Movimento do bomberman e controle game over
        gameover=PlayerMovement (&bomberman, enemyList, 2, sides, blocos);
        
        //Checa se deve posicionar uma bomba
        DropBomb(bombList, 3, bomberman);
        
            
        //Comeco do desenho
        BeginDrawing();
        ClearBackground(DARKGREEN);
        DrawRecs (sides, SIDES);
        DrawRecs (blocos, N_BLOCOS);
        DrawBlocoTex(blocos, bloco_png);
        DrawBomb (bombList, 3, bomb_active_png);
        DrawEnemy (enemyList, 2, enemy_png);
        DrawFPS(10, 10);
        DrawRectangleRec(bomberman.rec, BLANK);
        DrawTextureEx (bomberman_png, bombermanpos, 0, 0.3, WHITE);
        
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
        DrawText("GAME OVER", SCREEN_WIDTH/2-300, SCREEN_HEIGHT/2-50, 100, RAYWHITE);
        
    }
}

//Funcao preenche o vetor de blocos
void FillBlocks (Rectangle blocos[N_BLOCOS])
{
    int i=2, j, pos=0;
    
    while (i<=14)
    {
        j=2;
        while (j<=10)
        {
            blocos[pos].x=i*BLOCO;
            blocos[pos].y=j*BLOCO; 
            blocos[pos].width=BLOCO;
            blocos[pos].height=BLOCO;
            
            j+=2;
            pos++;
        }
        i+=2;
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
        (*bomb).status=1;
                
        dropped=1;
    }

    return dropped;
}

//Funcoes de movimento dos personagens:
//Funcao controla o movimento do bomberman e colisoes
short PlayerMovement (BOMBERMAN *bomberman, ENEMY enemyList[], int tamE, Rectangle sides[], Rectangle blocos[])
{
    short game=1;
    
    if (!CheckCollisionEnemy ((*bomberman).rec, enemyList, tamE))
    {
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            MoveRight (bomberman, blocos, sides[0]);
                        
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            MoveLeft (bomberman, blocos, sides[1]);
        
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
            MoveUp (bomberman, blocos, sides[2]);
            
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
            MoveDown (bomberman, blocos, sides[3]);
    }
        else 
            game=0;
        
    return game;   
}

//Funcao movimenta o bomberman para a direita
void MoveRight (BOMBERMAN *bomberman, Rectangle blocos[N_BLOCOS], Rectangle sideR)
{
    short collision=0;
    int i=0;
    Rectangle bmanRight={(*bomberman).rec.x+VELOCIDADE, (*bomberman).rec.y, (*bomberman).rec.width, (*bomberman).rec.height};
    
    while (i<N_BLOCOS && !collision)
    {   
        if (CheckCollisionRecs (bmanRight, blocos[i]) || CheckCollisionRecs (bmanRight, sideR))
            collision=1;
        
        i++;
    }
    
    if (!collision)
        (*bomberman).rec.x += VELOCIDADE;
}

//Funcao movimenta o bomberman para a esquerda
void MoveLeft (BOMBERMAN *bomberman, Rectangle blocos[N_BLOCOS], Rectangle sideL)
{
    short collision=0;
    int i=0;
    Rectangle bmanRight={(*bomberman).rec.x-VELOCIDADE, (*bomberman).rec.y, (*bomberman).rec.width, (*bomberman).rec.height};
    
    while (i<N_BLOCOS && !collision)
    {   
        if (CheckCollisionRecs (bmanRight, blocos[i]) || CheckCollisionRecs (bmanRight, sideL))
            collision=1;
        
        i++;
    }
    
    if (!collision)
        (*bomberman).rec.x -= VELOCIDADE;

}

//Funcao movimenta o bomberman para cima 
void MoveUp (BOMBERMAN *bomberman, Rectangle blocos[N_BLOCOS], Rectangle sideU)
{
    short collision=0;
    int i=0;
    Rectangle bmanRight={(*bomberman).rec.x, (*bomberman).rec.y-VELOCIDADE, (*bomberman).rec.width, (*bomberman).rec.height};
    
    while (i<N_BLOCOS && !collision)
    {   
        if (CheckCollisionRecs (bmanRight, blocos[i]) || CheckCollisionRecs (bmanRight, sideU))
            collision=1;
        
        i++;
    }
    
    if (!collision)
        (*bomberman).rec.y -= VELOCIDADE;
}

//Funcao movimenta o bomberman para baixo
void MoveDown (BOMBERMAN *bomberman, Rectangle blocos[N_BLOCOS], Rectangle sideD)
{
    short collision=0;
    int i=0;
    Rectangle bmanRight={(*bomberman).rec.x, (*bomberman).rec.y+VELOCIDADE, (*bomberman).rec.width, (*bomberman).rec.height};
    
    while (i<N_BLOCOS && !collision)
    {   
        if (CheckCollisionRecs (bmanRight, blocos[i]) || CheckCollisionRecs (bmanRight, sideD))
            collision=1;
        
        i++;
    }
    
    if (!collision)
        (*bomberman).rec.y += VELOCIDADE;
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