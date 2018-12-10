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

//++++++ GAME ++++++
void playGame(Matrix world, Matrix chunk, struct Player *player){
    nodelay(stdscr, TRUE); //sin esperar 
    int chunk_offset=0, chunk_col, logbase=5;
    double factor=1;
    
    while(1){        
        //++ start drawing ++
        erase();
        //print world
        mvprintw(1, 1, "[%d]", chunk_offset);
        wprintMatrix(stdscr, world); //print world matrix
        box(stdscr, '|', '-'); //print borders
        //print player status
        mvprintw(3, 1, "yx[%d,%d] j[%d] l[%d] i[%d]", player->y, player->x, player->jumpx, player->lifes, player->invulerable);
        //print factor of speed
        mvprintw(2, 1, "[f:%lf]", factor);
        //print player
        if(player->invulerable!=-1){ //es invulnerable
            mvprintw(5, 1, ":INVULNERABLE");
            if(chunk_offset>=PLAYER_invulnerableticks+player->invulerable && !playerColision(world, player)) //si ya se le acabo la invulnerabilidad y no esta colisionando
                player->invulerable=-1; //resetea la invulnerabilidad
            else if((chunk_offset-player->invulerable)%2==1){ //decide si lo imprimes para que parpadee
                printPlayer(*player);
            }
        }
        else //si no es invulnerable imprimelo
            printPlayer(*player);
        //refresh screen
        refresh();
        
        //++ start verifications ++
        //chunk ver
        if(chunk_offset%GAME_chunksize==0) genChunk(chunk);   
        //verificacion que perdio
        //player.x+1<world.nCols siempreeee
        if(player->invulerable==-1 && playerColision(world, player)){//si es vulnerable y colisiono verifica
            if(--player->lifes == 0){
                mvprintw(10, 1, ":PERDIO");
                refresh();
                return; //se acaba el juego
            }
            else{
                player->invulerable=chunk_offset; //el momento en que empezo a ser invulnerable
                factor*=1.5; //alenta el juego
            }
        }
        //mueve al jugador para el siguiente tick
        movePlayer(stdscr, player, world);
        //espera al siguiente tick
        sleep_ms((int)floor(GAME_mstick*factor));
        
        //++ new tick ++ (calcula el siguiente tick del juego)
        //calcula el nuevo factor de velocidad
        factor=1/(log(floor(chunk_offset/GAME_chunksize)+logbase)/log(logbase));
        //preparing new frame
        shiftColsMatrix(world, -1); //move world left 1
        //copy new column to world
        chunk_col=chunk_offset%GAME_chunksize;
        for(int m=0; m<chunk.nRens && m<world.nRens; m++)    
            world.ptr[m][world.nCols-1]=chunk.ptr[m][chunk_col];
        chunk_offset++;
    }
}

void initGame(){
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
    playGame(world, chunk, &player);
    mvprintw(4, 1, ":GAME OVER\tPresiona <enter> para reiniciar");
    refresh();
    getchar();
    
    endwin();
    //printf("GAME ENDED");
    freeMatrix(world);
    freeMatrix(chunk);
}

int main(){
    resizeWindow(WIN_height, WIN_width);
    printf("Hecho por Ivan Gonzalez [NUA: 424667] como proyecto final para Elementos de la Computacion\n\nPresiona cualquier tecla para continuar con el juego...");
    getchar();
    initscr(); // Initialize the window
    resizeterm(WIN_height, WIN_width);
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
