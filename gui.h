#include <stdio.h>
#include <ncurses.h> //terminal manipulation lib

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

int Menu(){
    char ch;
    clear();
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

void pauseMenu(WINDOW* win){
    nodelay(win, FALSE);
    mvprintw(5, 1, "Presiona \'s\' para continuar o \'e\' para salir.");
    
    char c=getch();
    if(c=='s') return;
    if(c=='e'){
        endwin();
        printf("Bye bye\n");
        exit(0);
    } 
}
