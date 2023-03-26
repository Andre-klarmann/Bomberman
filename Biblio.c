
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

//Imprime as informacoes do jogo na tela
void GameInfo (BOMBERMAN bomberman, int bombs, double time)
{
    int min=(int)time/60;
    int sec=(int)time%60;
    DrawText (TextFormat("LIVES %d", bomberman.lives), SCREEN_WIDTH-MeasureText("LIVES %d", 25), 10, 25, BLACK);
    DrawText (TextFormat("BOMBS %d", bombs), SCREEN_WIDTH-MeasureText("BOMBS %d", 25), 45, 25, BLACK);
    DrawText (TextFormat("POINTS %.f", bomberman.points), 10, 10, 25, BLACK);
    DrawText (TextFormat("TIME %02d:%02d", min, sec), 10, 45, 25, BLACK);
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
    int i;

    for (i=0; i<len; i++)
    {
        if (enemies[i].rec.x < bman.rec.x)
            MoveRight(&enemies[i].rec, blocos, sides[0], muros, lenW, door, speed);

        if (enemies[i].rec.x > bman.rec.x)
            MoveLeft(&enemies[i].rec, blocos, sides[1], muros, lenW, door, speed);

        if (enemies[i].rec.y > bman.rec.y)
            MoveUp(&enemies[i].rec, blocos, sides[0], muros, lenW, door, speed);

        if (enemies[i].rec.y < bman.rec.y)
            MoveDown(&enemies[i].rec, blocos, sides[0], muros, lenW, door, speed);
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
void ExplodeBombs (BOMB bombList[], int lenB, BOMBERMAN *bman, ENEMY enemies[], int lenE, WALL muros[], int lenW, short *gameover, float *timer)
{
    int i;

    if (IsKeyPressed(KEY_ENTER))
        *timer=0.5f;

    if (*timer>0.0f)
    {
        *timer-=GetFrameTime();

        for (i=0; i<lenB; i++)
        {
            if (bombList[i].status==1)
            {
                DetonateBomb(&bombList[i], bman, enemies, lenE, muros, lenW, gameover, timer);
            }
        }
    }
}

//Funcao explode uma bomba individualmente
void DetonateBomb (BOMB *bomb, BOMBERMAN *bman, ENEMY enemies[], int lenE, WALL muros[], int lenW, short *gameover, float *timer)
{
    Rectangle explosions[BOOM]=
    {{bomb->rec.x, bomb->rec.y, BLOCO, BLOCO},
     {bomb->rec.x+BLOCO, bomb->rec.y, BLOCO, BLOCO},
     {bomb->rec.x-BLOCO, bomb->rec.y, BLOCO, BLOCO},
     {bomb->rec.x, bomb->rec.y-BLOCO, BLOCO, BLOCO},
     {bomb->rec.x, bomb->rec.y+BLOCO, BLOCO, BLOCO}};


    KillEnemies (explosions, bman, enemies, lenE);
    BreakWalls (explosions, bman, muros, lenW);
    ExplodeBomberman (bman, explosions, gameover);


    if (*timer<0.0f)
        bomb->status=2;

    DrawExplosions (explosions);
}

//Funcao explode o bomberman
void ExplodeBomberman (BOMBERMAN *bomberman, Rectangle explosions[BOOM], short *gameover)
{
    int i=0;

    while (i<BOOM && *gameover==1)
    {
        if (CheckCollisionRecs(bomberman->rec, explosions[i]))
            *gameover=0;

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
            MoveRight (&(*bomberman).rec, blocos, sides[0], muros, lenW, door, VELOCIDADE);

        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            MoveLeft (&(*bomberman).rec, blocos, sides[1], muros, lenW, door, VELOCIDADE);

        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
            MoveUp (&(*bomberman).rec, blocos, sides[2], muros, lenW, door, VELOCIDADE);

        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
            MoveDown (&(*bomberman).rec, blocos, sides[3], muros, lenW, door, VELOCIDADE);
    }
        else
            game=0;

    return game;
}

//Funcao movimenta o bomberman para a direita
void MoveRight (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideR, WALL muros[N_MUROS], int lenW, DOOR door, float velocidade)
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

    CheckCollisionWalls (recRight, muros, lenW, &collision);
    CheckCollisionDoor (recRight, door, &collision);

    if (!collision)
        (*rec).x += velocidade;
}

//Funcao movimenta o bomberman para a esquerda
void MoveLeft (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideL, WALL muros[N_MUROS], int lenW, DOOR door, float velocidade)
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
    CheckCollisionDoor (recLeft, door, &collision);

    if (!collision)
        (*rec).x -= velocidade;

}

//Funcao movimenta o bomberman para cima
void MoveUp (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideU, WALL muros[N_MUROS], int lenW, DOOR door, float velocidade)
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
    CheckCollisionDoor (recUp, door, &collision);

    if (!collision)
        (*rec).y -= velocidade;
}

//Funcao movimenta o bomberman para baixo
void MoveDown (Rectangle *rec, Rectangle blocos[N_BLOCOS], Rectangle sideD, WALL muros[N_MUROS], int lenW, DOOR door, float velocidade)
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
    CheckCollisionDoor (recDown, door, &collision);

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
void CheckCollisionDoor (Rectangle rec, DOOR door, short *collision)
{
    if (!*collision)
    {
        if (CheckCollisionRecs(rec, door.rec))
            *collision=1;
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
    else if (door.status==porta)
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
