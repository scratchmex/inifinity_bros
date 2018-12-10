#include <stdio.h>
#include <ncurses.h> //terminal manipulation lib
#include <math.h>
#include <time.h>

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

//++ GUI functions
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
    if(mat.nCols!=x || mat.nRens!=y) printf("CUIDADO CON TUS DIMENSIONES DE LA MATRIZ -- NO REDIMENSIONES LA VENTANA\n");
    for(int m=0; m<mat.nRens; m++) for(int n=0; n<mat.nCols; n++){
        if(mat.ptr[m][n]=='+') continue; //si es el char especial saltalo
        mvwprintw(win, m, n, "%c", mat.ptr[m][n]);
    }
}

void wprintMatrixRange(WINDOW* win, Matrix mat, int y0, int x0){
    int y, x;
    getmaxyx(win, y, x);
    if(mat.nCols!=x || mat.nRens!=y) printf("CUIDADO CON TUS DIMENSIONES DE LA MATRIZ -- NO REDIMENSIONES LA VENTANA\n");
    for(int m=0; m<mat.nRens && m<=y0; m++) for(int n=0; n<mat.nCols && n<=x0; n++){
        if(mat.ptr[m][n]=='+') continue; //si es el char especial saltalo
        mvprintw(m, n, "%c", mat.ptr[m][n]);
    }
}

void intructionsMenu(){
    
}

int Menu(){
    int c;
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    clear();
    
    //mas funcionalidad, seleccion con colores
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    // attron(A_BOLD);
    // attron(COLOR_PAIR(0));
    // printw("%c\n", ch);
    // attroff(A_BOLD);
    // attroff(COLOR_PAIR(0));
    
    //++ Load panels and GUI files
    FILE *fp;
    fp=fopen("menu.txt", "r");
    Matrix title=fscanMatrixAdvanced(fp, "title");
    Matrix iniciar=fscanMatrixAdvanced(fp, "iniciar");
    Matrix instrucciones=fscanMatrixAdvanced(fp, "instrucciones");
    fclose(fp);
    
    WINDOW *panels[3];
    //getmaxyx(stdscr, y, x)
    int height=10, space=5;
    //titulo
    panels[0]=newwin(title.nRens, title.nCols, height, (int)floor(WIN_width-title.nCols)/2);
    height+=title.nRens+space;
    //iniciar
    panels[1]=newwin(iniciar.nRens, iniciar.nCols, height, (int)floor(WIN_width-iniciar.nCols)/2);
    height+=iniciar.nRens+space;
    //instrucciones
    panels[2]=newwin(instrucciones.nRens, instrucciones.nCols, height, (int)floor(WIN_width-instrucciones.nCols)/2);
    height+=instrucciones.nRens+space;
    
    //++ Imprimir el menu
    box(stdscr, '+', '+'); //print borders
    refresh();
    
    int selection=1, tick=0;
    
    while(1){
        //++ Colorear texto y ver <enter>
        //titulo
        wattron(panels[0], COLOR_PAIR(tick+1));
        wprintMatrix(panels[0], title);
        wattroff(panels[0], COLOR_PAIR(tick+1));
        wrefresh(panels[0]);
        tick++;
        tick %= 7;
        //paneles
        wprintMatrix(panels[1], iniciar);
        wprintMatrix(panels[2], instrucciones);
        switch(selection) {
            case 1://iniciar
                wattron(panels[1], COLOR_PAIR(1));
                wprintMatrix(panels[1], iniciar);
                wattroff(panels[1], COLOR_PAIR(1));
                break;
            case 2://instrucciones
                wattron(panels[2], COLOR_PAIR(2));
                wprintMatrix(panels[2], instrucciones);
                wattroff(panels[2], COLOR_PAIR(2));
                break;
        }
        wrefresh(panels[1]);
        wrefresh(panels[2]);
        
        //Ver tecla
        //nodelay(stdscr, FALSE);
        sleep_ms(100);
        c=getch();
        // nodelay(stdscr, TRUE);
        
        switch(c){
            case KEY_UP:
                if(selection>1) selection--;
                break;
            case KEY_DOWN:
                if(selection<2) selection++;
                break;
            case 10://<enter> ascii value
                switch(selection) {
                    case 1://iniciar juego
                        return 1;
                    case 2://menu instrucciones
                        intructionsMenu();
                        break;
                }
                break;
            case 'e':
                endwin();
                printf("Bye bye\n");
                exit(0);
        }
    }
    
    freeMatrix(title);
    freeMatrix(iniciar);
    freeMatrix(instrucciones);
    delwin(panels[0]);
    delwin(panels[1]);
    delwin(panels[2]);
}

void pauseMenu(WINDOW* win){
    mvprintw(5, 1, "Presiona \'s\' para continuar, \'m\' para el menu o \'e\' para salir.");
    nodelay(win, FALSE);
    char c=getch();
    nodelay(win, TRUE);
    switch(c) {
        case 's':
            return;
        case 'e':
            endwin();
            printf("Bye bye\n");
            exit(0);
        case 'm':
            Menu();
            break;
    }
}
