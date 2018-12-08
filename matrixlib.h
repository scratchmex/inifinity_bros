#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// this converts to string
#define STR_(X) #X

// this makes sure the argument is expanded before converting to string
#define STR(X) STR_(X)

#ifndef MATTYPE
#define MATTYPE char
#endif

#ifndef VALTYPE
#define VALTYPE double
#endif

VALTYPE randType(){
    return rand()/(VALTYPE)RAND_MAX*10;
}

//################### TYPE DEF CODE ###################
typedef struct MATRIX{
    MATTYPE** ptr;
    int nRens;
    int nCols;
} Matrix;

//################### MATRIX CODE ###################
void randMatrix(Matrix mat){
    for(int i=0; i<mat.nRens; i++) for(int j=0; j<mat.nCols; j++)
        mat.ptr[i][j]=randType();
}

Matrix createMatrix(int rens, int cols){
    MATTYPE** ptr=(MATTYPE**)malloc(rens*sizeof(MATTYPE*));
    if(ptr==NULL) printf("No hay suficiente memoria, hubo error en malloc\n");
    //printf("ALLOCATING=[%d,%d]\n", rens, cols);
    for(int i=0; i<rens; i++){
        ptr[i]=(MATTYPE*)malloc(cols*sizeof(MATTYPE));
        if(ptr[i]==NULL) printf("No hay suficiente memoria, hubo error en malloc\n");
    }
    Matrix mat={ptr, rens, cols};

    return mat;
}

void freeMatrix(Matrix mat){
    for(int i=0; i<mat.nRens; i++) free(mat.ptr[i]);
    free(mat.ptr);
}

void initMatrix(Matrix mat, char c){
    for(int m=0; m<mat.nRens; m++) for(int n=0; n<mat.nCols; n++)
        mat.ptr[m][n]=c;
}

void printMatrix(Matrix mat){
    #if MATTYPE == char
    for(int m=0; m<mat.nRens; m++) printf("%s\n", mat.ptr[m]);
    #elif MATTYPE == double || MATTYPE == float
    for(int m=0; m<mat.nRens; m++){
        for(int n=0; n<mat.nCols; n++)
            printf("%+06.2lf ", mat.ptr[m][n]);
        printf("\n");
    }
    #else
    printf("Error in printMat: need to define the way to treat this type ["STR_(MATTYPE)"]\n");
    #endif
}

void fprintMatrix(FILE* fp, Matrix mat){
    fprintf(fp, "%d %d\n", mat.nRens, mat.nCols);

    #if MATTYPE == char
    for(int m=0; m<mat.nRens; m++) fprintf(fp, "%s\n", mat.ptr[m]);
    #elif MATTYPE == double || MATTYPE == float
    for(int m=0; m<mat.nRens; m++){
        for(int n=0; n<mat.nCols; n++) fprintf(fp, "%lf ", mat.ptr[m][n]);
        fprintf(fp, "\n");
    }
    #else
    printf("Error in fprintMat: need to define the way to treat this type ["STR_(MATTYPE)"]\n");
    #endif
}

Matrix fscanMatrix(FILE* fp){
    int Y, X;
    fscanf(fp, "%d %d", &Y, &X);
    Matrix mat=createMatrix(Y, X);

    for(int m=0; m<mat.nRens; m++) for(int n=0; n<mat.nCols; n++){
    #if MATTYPE == char
        fscanf(fp, "%c", &mat.ptr[m][n]);
        if(mat.ptr[m][n]=='\n') n--; //if line break ignore it
    #elif MATTYPE == double || MATTYPE == float
        fscanf(fp, "%lf", &mat.ptr[m][n]);
    #else
        printf("Error in fprintMat: need to define the way to treat this type ["STR_(MATTYPE)"]\n");
    return;
    #endif
    }


    return mat;
}

Matrix fscanMatrixAdvanced(FILE* fp, char prefix[]){
    int Y=0, X=0;
    char param[50]="@";
    strcat(param, prefix);
    strcat(param, " [%d,%d]");

    char line[256];
    for(int i=0; fgets(line, sizeof(line), fp); i++){
        //printf("LINE[%d]--searching: %s\n", i+1, param);
        //printf("%s\n", line);
        if(sscanf(line, param, &Y, &X)==2) break;
    }

    Matrix mat=createMatrix(Y, X);
    printf("%s=[%d,%d]\n", param, mat.nRens, mat.nCols);

    #if MATTYPE == char
    for(int m=0; m<mat.nRens; m++){
        fgets(line, sizeof(line), fp);
        strcpy(mat.ptr[m], line);
        mat.ptr[m][mat.nCols]='\0';
        printf("%d~ %s\n", m, mat.ptr[m]);
        //if(mat.ptr[m][n]=='\n') n--; //if line break ignore it
    }

    #elif MATTYPE == double || MATTYPE == float
    for(int m=0; m<mat.nRens; m++) for(int n=0; n<mat.nCols; n++)
        fscanf(fp, "%lf", &mat.ptr[m][n]);

    #else
    printf("Error in fprintMat: need to define the way to treat this type ["STR_(MATTYPE)"]\n");
    #endif

    return mat;
}

void shiftColsMatrix(Matrix mat, int dir){
    for(int n=(mat.nCols-dir)%(mat.nCols); n<mat.nCols; n++)
        for(int m=0; m<mat.nRens; m++){
            mat.ptr[m][(n+dir)%(mat.nCols)]=mat.ptr[m][n];
    }
}
