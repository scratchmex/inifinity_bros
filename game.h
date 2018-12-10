#include <math.h>

struct Player{
    int x;
    int y;
    int x_border;
    int y_border;
    int jumpx;
    int lifes;
    int invulerable;
};

int jumpPoly(int x){
    int x0=30;
    if(x>=x0) return -2;
    else if(x>x0/2) return -1;
    else if(x<x0/2-2) return 1;
    return 0;
    int xm=6;
    double fx=-x*(x-x0)*(4*xm/pow(xm,2));
    mvprintw(10, 1, "jmply=%lf", fx);
    return (int)floor(fx);
}

void genChunk(Matrix chunk){
    //fresh chunk
    initMatrix(chunk, '+');
    //generate
    int height=WIN_height-((rand()%((int)floor(WIN_height/2)-OBS_heightspace))+OBS_heightspace);
    int length=OBS_maxlength-(rand()%OBS_lengthvar); //largo del obstaculo
    for(int x=0; x<length && x<chunk.nCols; x++)
        chunk.ptr[height][x]='=';
    if(rand()%5==0)
        chunk.ptr[height-1-rand()%2][rand()%length]='*';
}

void printPlayer(struct Player player){
    for(int i=0; i<player.y_border; i++)
        mvprintw(player.y-i, player.x, "%%");
}

int canDrop(Matrix world, struct Player *player){
    if(player->y+1>=world.nRens) return 0;
    for(int n=0; n<player->x_border && n<world.nCols; n++){
        // mvprintw(player->y+1, player->x+n, "@");
        if(world.ptr[player->y+1][player->x+n]!='+') return 0;
    }
    if(player->y+2>=world.nRens) return 0;
    return 1;
}

int canJump(Matrix world, struct Player *player){
    if(player->y-1>=world.nRens || player->y-1<0) return 0;
    for(int n=0; n<player->x_border && n<world.nCols; n++){
        // mvprintw(player->y-1, player->x+n, "@");
        if(world.ptr[player->y-player->y_border][player->x+n]!='+') return 0;
    }
    return 1;
}

int playerColision(Matrix world, struct Player *player){
    for(int m=0; m<player->y_border && player->y-m<world.nRens; m++) //verifica los bordes del personaje
        if(world.ptr[player->y-m][player->x+player->x_border]=='=' || world.ptr[player->y-m][player->x+player->x_border]=='*')
            return 1;
    return 0;
}

void movePlayer(WINDOW *win, struct Player *player, Matrix world){
    nodelay(win, TRUE);
    char c = getch();
    //nodelay(win, FALSE);
    
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
