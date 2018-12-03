#define MATTYPE char

#include <stdio.h>
#include "matrixlib.h"
#include <ncurses.h> //terminal manipulation lib
#include <string.h>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "settings.h"

#ifdef __unix__
#define OS_Unix

#elif defined(_WIN32) || defined(WIN32)
#define OS_Windows
#include <windows.h>
#endif

/** load text
FILE* fp=fopen("menu.txt", "r");
Matrix bros=fscanMatrixAdvanced(fp, "title");
printMatrix(bros);
fclose(fp);
//freeMatrix(bros);
**/

void resizeWindow(int width, int height){
    #ifdef OS_Windows
    SMALL_RECT windowSize = {0, 0, width, height} //change the values
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
    #endif

    #ifdef OS_Unix
    printf("\e[8;%d;%dt", width, height);
    #endif
}

void wprintMatrix(WINDOW* win, Matrix mat){
    int y, x;
    getmaxyx(win, y, x);
    if(mat.nCols!=x || mat.nRens!=y) printf("CUIDADO CON TUS DIMENSIONES DE LA MATRIZ\n");
    for(int m=0; m<mat.nRens; m++) for(int n=0; n<mat.nCols; n++){
        if(mat.ptr[m][n]=='+') continue; //si es el char especial saltalo
        mvprintw(m, n, "%c", mat.ptr[m][n]);
    }
}

void wprintMatrixRange(WINDOW* win, Matrix mat, int y0, int x0){
    int y, x;
    getmaxyx(win, y, x);
    if(mat.nCols!=x || mat.nRens!=y) printf("CUIDADO CON TUS DIMENSIONES DE LA MATRIZ\n");
    for(int m=0; m<mat.nRens && m<=y0; m++) for(int n=0; n<mat.nCols && n<=x0; n++){
        if(mat.ptr[m][n]=='+') continue; //si es el char especial saltalo
        mvprintw(m, n, "%c", mat.ptr[m][n]);
    }
}

void shiftColsMatrix(Matrix mat, int dir){
    for(int n=(mat.nCols-dir)%(mat.nCols); n<mat.nCols; n++)
        for(int m=0; m<mat.nRens; m++){
            mat.ptr[m][(n+dir)%(mat.nCols)]=mat.ptr[m][n];
    }
}

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
void genChunk(Matrix chunk){
    //fresh chunk
    initMatrix(chunk, '+');
    //generate
    int height=(rand()%(WIN_height-OBS_heightspace))+OBS_heightspace;
    int length=OBS_maxlength-(rand()%OBS_lengthvar); //largo del obstaculo
    for(int x=0; x<length && x<chunk.nCols; x++)
        chunk.ptr[height][x]='=';
}

void StartGame(Matrix chunk){
    int chunk_offset=0, chunk_col;
    double factor;
    clear();
    printw("Starting...");
    refresh();
    
    Matrix world=createMatrix(WIN_height, WIN_width);
    initMatrix(world, '+');
    
    sleep_ms(1000);
    
    int running=1;
    while(running){
        //++ start verifications ++
        if(chunk_offset%GAME_chunksize==0) genChunk(chunk);        
        
        //++ start drawing ++
        //print world
        clear();
        mvprintw(1, 1, "[%d]", chunk_offset);
        wprintMatrix(stdscr, world); //print world matrix
        box(stdscr, '|', '-'); //print borders
        //print player
        mvprintw(WIN_height-2, (int)floor(WIN_width/2), "%%");
        
        refresh();
        
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
    
    getchar();
    freeMatrix(world);
}

int Menu(){
    char ch;
    //mas funcionalidad, seleccion con colores
    
    // start_color();
    // init_pair(1, COLOR_BLACK, COLOR_RED);
    // init_pair(2, COLOR_BLACK, COLOR_GREEN);
    // attron(A_BOLD);
    // attron(COLOR_PAIR(0));
    // printw("%c\n", ch);
    // attroff(A_BOLD);
    // attroff(COLOR_PAIR(0));
    // if(ch=='q') break;
    printw("Presiona <espacio> para iniciar el juego");
    refresh();
    while(1){
        ch = getch(); //wait
        switch(ch){
            case ' ':
                return 1;
        }
	}
}

void initGame(){
    //start chunk
    Matrix chunk=createMatrix(WIN_height, GAME_chunksize);
    
    //GAME_mstick in settings.h
    
    if(Menu()==1) StartGame(chunk);
    else{
        endwin();
        printf("SOMETHING HAPPENED IN MENU\n");
    }
    
    endwin();
    printMatrix(chunk);
    printf("GAME ENDED");
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
