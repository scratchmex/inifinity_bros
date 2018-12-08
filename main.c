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
    mvprintw(player.y, player.x, "%%");
}

int canDrop(Matrix world, struct Player *player){
    if(player->y+1>=world.nRens) return 0;
    for(int n=0; n<player->x_border && n<world.nCols; n++){
        mvprintw(player->y+1, player->x+n, "@");
        if(world.ptr[player->y+1][player->x+n]!='+') return 0;
    }
    return 1;
}

int canJump(Matrix world, struct Player *player){
    if(player->y-1>=world.nRens || player->y-1<0) return 0;
    for(int n=0; n<player->x_border && n<world.nCols; n++){
        mvprintw(player->y-1, player->x+n, "@");
        if(world.ptr[player->y-1][player->x+n]!='+') return 0;
    }
    return 1;
}

int movePlayer(WINDOW *win, struct Player *player, Matrix world){
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
        if(jumpy==0) //si ya acabo de brincar reinicializa
            player->jumpx=0;
        else if(jumpy>0 && canJump(world, player)){
            mvprintw(7, 1, ":SUBIENDOO");    
            player->y-=jumpy; //subiendo
        }
    }
    else if(canDrop(world, player) && player->y+2!=world.nRens){
        mvprintw(7, 1, ":CAYENDOO");
        player->y++; 
    }
        
    mvprintw(3, 1, "yxj[%d,%d,%d]", player->y, player->x, player->jumpx);
    
    //verificacion que perdio
    //player.x+1<world.nCols siempreeee
    for(int m=0; m<player->y_border && player->y-m<world.nRens; m++)
        if(world.ptr[player->y-m][player->x+1]=='='){
            break_here();
            return 1;
        }
    return 0;
}

void StartGame(Matrix world, Matrix chunk, struct Player player){
    int chunk_offset=0, chunk_col;
    double factor;
    
    while(1){
        clear();
        //++ start verifications ++
        if(chunk_offset%GAME_chunksize==0) genChunk(chunk);        
        
        //++ start drawing ++
        //print world
        mvprintw(1, 1, "[%d]", chunk_offset);
        wprintMatrix(stdscr, world); //print world matrix
        box(stdscr, '|', '-'); //print borders
        //move player
        if(movePlayer(stdscr, &player, world)){
            mvprintw(10, 1, "[PERDIO]");
            printPlayer(player); //perdio si return 1
            break;
        } 
        
        printPlayer(player);
        
        //++ new tick ++
        factor=1/(log2(floor(chunk_offset/GAME_chunksize)+2));
        mvprintw(2, 1, "[f:%lf]", factor);
        refresh();
        
        sleep_ms((int)floor(GAME_mstick*factor));
        shiftColsMatrix(world, -1); //move world left 1
        //copy new column to world
        chunk_col=chunk_offset%GAME_chunksize;
        for(int m=0; m<chunk.nRens && m<world.nRens; m++)    
            world.ptr[m][world.nCols-1]=chunk.ptr[m][chunk_col];
        chunk_offset++;
    }
    
    mvprintw(5, 1, ":GAME OVER\nPresiona enter para salir");
    refresh();
    getchar();
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
        .y_border=1,
        .jumpx=0
    };
    
    sleep_ms(1000);
    if(Menu()==1) StartGame(world, chunk, player);
    else{
        endwin();
        printf("SOMETHING HAPPENED IN MENU\n");
    }
    
    endwin();
    printf("GAME ENDED");
    freeMatrix(world);
    freeMatrix(chunk);
}

int main(){
    resizeWindow(WIN_height, WIN_width);
    initscr(); // Initialize the window
    noecho(); // Don't echo any keypresses
    curs_set(FALSE); // Don't display a cursor
    //raw(); // Interpret raw text input
    
    initGame();

    getchar();
    endwin();
    return 0;
}
