#define MATTYPE char

#ifdef __unix__
#define OS_Unix

#elif defined(_WIN32) || defined(WIN32)
#define OS_Windows
#include <windows.h>
#endif

#include "settings.h"
#include "matrixlib.h"
#include "gui.h"
#include "game.h"

#include <stdio.h>
#include <ncurses.h> //terminal manipulation lib
#include <string.h>

#include <math.h>
#include <stdlib.h>
#include <time.h>

void break_here(){
}

/** load text
FILE* fp=fopen("menu.txt", "r");
Matrix bros=fscanMatrixAdvanced(fp, "title");
printMatrix(bros);
fclose(fp);
//freeMatrix(bros);
**/

void sleep_ms(int milliseconds){ // cross-platform sleep function
    #if defined(_WIN32) || defined(WIN32)
        Sleep(milliseconds);
    #elif _POSIX_C_SOURCE >= 199309L
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        nanosleep(&ts, NULL);
    #else
        usleep(milliseconds * 1000);
    #endif
}

//++++++ GAME ++++++
void printPlayer(struct Player player){
    for(int i=0; i<player.y_border; i++)
        mvprintw(player.y-i, player.x, "%%");
}

int canDrop(Matrix world, struct Player *player){
    if(player->y+1>=world.nRens) return 0;
    for(int n=0; n<player->x_border && n<world.nCols; n++){
        mvprintw(player->y+1, player->x+n, "@");
        if(world.ptr[player->y+1][player->x+n]!='+') return 0;
    }
    if(player->y+2>=world.nRens) return 0;
    return 1;
}

int canJump(Matrix world, struct Player *player){
    if(player->y-1>=world.nRens || player->y-1<0) return 0;
    for(int n=0; n<player->x_border && n<world.nCols; n++){
        mvprintw(player->y-1, player->x+n, "@");
        if(world.ptr[player->y-player->y_border][player->x+n]!='+') return 0;
    }
    return 1;
}

void movePlayer(WINDOW *win, struct Player *player, Matrix world){
    nodelay(win, TRUE);
    char c = getch();
    nodelay(win, FALSE);
    
    switch(c) {
        case ' ':
            if(player->jumpx==0) player->jumpx=1;
            //player->jumpx++;
            mvprintw(4, 1, ":UP");
            break;
        case 's':
            mvprintw(4, 1, ":PAUSA");  
            pauseMenu(win);
            break;
        default:
            mvprintw(4, 1, ":");
            break;
    }
    
    if(player->jumpx!=0){//si esta brincando opera
        int jumpy=jumpPoly(player->jumpx);
        player->jumpx++;
        mvprintw(6, 1, ":BRINCANDO");
        if(jumpy==-2 || !canJump(world, player)) //si ya acabo de brincar reinicializa
            player->jumpx=0;
        else if(jumpy>0){
            mvprintw(7, 1, ":SUBIENDOO");    
            player->y-=jumpy; //subiendo
        }
        else if(jumpy<0 && canDrop(world, player)){
            mvprintw(7, 1, ":BAJANDOO");    
            player->y-=jumpy; //bajando
        }
    }
    else if(canDrop(world, player)){
        mvprintw(7, 1, ":CAYENDOO");
        player->y++; 
    }
}

void StartGame(Matrix world, Matrix chunk, struct Player *player){
    int chunk_offset=0, chunk_col;
    double factor=1;
    
    while(1){
        clear();   
        //++ start drawing ++
        //print world
        mvprintw(1, 1, "[%d]", chunk_offset);
        wprintMatrix(stdscr, world); //print world matrix
        box(stdscr, '|', '-'); //print borders
        //move player and print
        mvprintw(3, 1, "yxjli[%d,%d, %d, %d, %d]", player->y, player->x, player->jumpx, player->lifes, player->invulerable);
        movePlayer(stdscr, player, world);
        
        if(player->invulerable!=-1){ //es invulnerable
            mvprintw(5, 1, ":INVULNERABLE");
            if(chunk_offset==PLAYER_invulnerableticks+player->invulerable)
                player->invulerable=-1; //resetea la invulnerabilidad
            else if((chunk_offset-player->invulerable)%2==1){ //decide si lo imprimes para que parpadee
                printPlayer(*player);
            }
        }
        else //si no es invulnerable imprimelo
            printPlayer(*player);
        mvprintw(2, 1, "[f:%lf]", factor);
        refresh(); //refresh screen
        
        //++ start verifications ++
        if(chunk_offset%GAME_chunksize==0) genChunk(chunk);   
        //++ verificacion que perdio
        //player.x+1<world.nCols siempreeee
        if(player->invulerable==-1)//si es vulnerable verifica
            for(int m=0; m<player->y_border && player->y-m<world.nRens; m++)
                if(world.ptr[player->y-m][player->x+player->x_border]=='=' || world.ptr[player->y-m][player->x+player->x_border]=='*'){
                    if(--player->lifes == 0){
                        mvprintw(10, 1, ":PERDIO");
                        return; //se acaba el juego
                    }
                    else{
                        player->invulerable=chunk_offset; //el momento en que empezo a ser invulnerable
                        factor*=1.15; //alenta el juego
                        break;
                    }
                }  
            
        //++ new tick ++
        factor=1/(log10(floor(chunk_offset/GAME_chunksize)+10));
        sleep_ms((int)floor(GAME_mstick*factor));
        
        //++ preparing new frame
        shiftColsMatrix(world, -1); //move world left 1
        //copy new column to world
        chunk_col=chunk_offset%GAME_chunksize;
        for(int m=0; m<chunk.nRens && m<world.nRens; m++)    
            world.ptr[m][world.nCols-1]=chunk.ptr[m][chunk_col];
        chunk_offset++;
    }
}

void initGame(){
    resizeterm(WIN_height, WIN_width);
    clear();
    printw("Starting...");
    refresh();
    
    //start chunk
    Matrix chunk=createMatrix(WIN_height, GAME_chunksize);
    
    //start world
    Matrix world=createMatrix(WIN_height, WIN_width);
    initMatrix(world, '+');
    
    //GAME_mstick in settings.h
    
    //init player
    struct Player player={
        .x=(int)floor(WIN_width/2),
        .y=WIN_height-2,
        .x_border=1,
        .y_border=3,
        .jumpx=0,
        .lifes=2,
        .invulerable=-1,
    };
    
    sleep_ms(100);
    StartGame(world, chunk, &player);
    mvprintw(4, 1, ":GAME OVER\nPresiona enter para salir");
    refresh();
    getchar();
    
    endwin();
    //printf("GAME ENDED");
    freeMatrix(world);
    freeMatrix(chunk);
}

int main(){
    resizeWindow(WIN_height, WIN_width);
    initscr(); // Initialize the window
    noecho(); // Don't echo any keypresses
    curs_set(FALSE); // Don't display a cursor
    //raw(); // Interpret raw text input
    
    Menu();
    while(1) {
        initGame();
    }

    getchar();
    endwin();
    return 0;
}
