#include <stdio.h>
#include <ncurses.h> //terminal manipulation lib
#include <math.h>
#include <time.h>
#include <string.h>

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
        mvwprintw(win, m, n, "%c", mat.ptr[m][n]);
    }
}

void intructionsMenu(){
    clear();
    box(stdscr, '.', '.'); //print borders
    char instrucciones[][255]={
        "Para regresar al menu principal presiona cualquier tecla",
        "",
        "Instrucciones:",
        "El juego consiste en evitar chocar con las plataformas y evitar los obstaculos \'*\'",
        "El estatus del juego se encuentra en la parte superior izquierda, ahi hay datos como:",
        "\t-el puntaje", 
        "\t-el factor de velocidad f[]",
        "\t-las coordenadas del jugador yx[]",
        "\t-si esta brincando j[]",
        "\t-las vidas l[]", 
        "\t-si es invulnerable i[]",
        "Ademas aparecen algunos datos despues de \':\' en la misma ubicacion",
        "Para pausar el juego presiona \'p\' en el juego y para salir presiona \'e\' en el menu o en el menu de pausa"
    };
    for(int i=0; i<sizeof(instrucciones)/sizeof(instrucciones[0]); i++)
        mvprintw(i+1, 1, "%s", instrucciones[i]);
    refresh();
    getchar();
    clear();
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
    
    int selection=1, tick=0;
    box(stdscr, '+', '+'); //print borders
    refresh();
    
    while(1){
        //++ Imprimir el menu
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
        napms(100);
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
                        box(stdscr, '+', '+'); //print borders
                        refresh();
                        break;
                }
                break;
            case 'e':
                endwin();
                printf("\nBye bye\n");
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
            printf("\nBye bye\n");
            exit(0);
        case 'm':
            Menu();
            break;
    }
}
