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
    int x0=30, xm=6;
    if(x>=x0) return -2;
    else if(x>x0/2) return -1;
    else if(x<x0/2-2) return 1;
    return 0;
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
