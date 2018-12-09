#include <graphics.h>

int main()
{
   int gd = DETECT,gm;
   initgraph(&gd,&gm,NULL);
   for(int i=0; i<getmaxx(); i++){
       cleardevice();
       rectangle(i, 10, 10, 10);
       usleep(5000);
       printf("\n\n[%d, %d]\n\n", getmaxx(), getmaxx());
   }

   getchar();
   
   closegraph();
   return 0;
}
