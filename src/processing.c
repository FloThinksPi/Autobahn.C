/* 
 * File:   processing.c
 * Author: flo
 *
 * Created on 22. Januar 2015, 9:01
 */

/*
 * File:   main.c
 * Author: lenahoinkis
 *
 * Created on 27. Januar 2015, 20:14
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include "processing.h"

const int n = 20;

const int g[n][n] = { //  A    B    C    D    E    F    G    H    I    J    K    L    M    N    O    P    Q    R    S    T
        /* A */ {   0, 115,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },
        /* B */ { 115,   0,  64,  73,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },
        /* C */ {  -1,  64,   0,  84,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },
        /* D */ {  -1,  73,  84,   0,  75, 176, 140,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },
        /* E */ {  -1,  -1,  -1,  75,   0,  -1,  77,  93,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },
        /* F */ {  -1,  -1,  -1, 176,  -1,   0,  90,  -1,  -1, 107,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },
        /* G */ {  -1,  -1,  -1, 140,  77,  90,   0,  79,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },
        /* H */ {  -1,  -1,  -1,  -1,  93,  -1,  79,   0,  -1, 180,  96,  -1,  -1, 191,  -1,  -1,  -1,  -1,  -1,  -1 },
        /* I */ {  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   0,  -1,  75,  -1,  77,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },
        /* J */ {  -1,  -1,  -1,  -1,  -1, 107,  -1, 180,  -1,   0,  -1,  40,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },
        /* K */ {  -1,  -1,  -1,  -1,  -1,  -1,  -1,  96,  75,  -1,   0,  -1,  29, 147,  -1,  -1,  -1,  -1,  -1,  -1 },
        /* L */ {  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  40,  -1,   0,  -1,  94,  -1,  -1,  -1,  -1,  -1,  -1 },
        /* M */ {  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  77,  -1,  29,  -1,   0,  -1,  -1,  -1, 120,  -1,  -1,  -1 },
        /* N */ {  -1,  -1,  -1,  -1,  -1,  -1,  -1, 191,  -1,  -1, 147,  94,  -1,   0,  81,  -1,  -1, 141,  -1,  -1 },
        /* O */ {  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  81,   0,  -1,  -1, 117,  -1,  -1 },
        /* P */ {  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   0,  27,  -1,  76,  -1 },
        /* Q */ {  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 120,  -1,  -1,  27,   0,  -1,  65,  -1 },
        /* R */ {  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 141, 117,  -1,  -1,   0,  68,  39 },
        /* S */ {  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  76,  65,  68,   0,  55 },
        /* T */ {  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  1,  39,  0 } };

void init(bool *quere, int start,double *distance,int *pre, int n){
    for(int i = 0;i<n;i++){
        distance[i]= FLT_MAX;
        pre[i]= 0;
        quere[i]=true;
    }
    distance[start]=0;

}

int minDistance(double *distance, int n, bool *quere){
    double smallest= DBL_MAX;
    for(int i = 0;i<n;i++){
        if (distance[i]<smallest && quere[i]==true) smallest = i;
    }
    return smallest;
}



void distance_update(int u, int v, double *distance, int *pre){
    double newDistance = distance[u] + g[u][v];
    //printf("distance: %d \n point: %d \n new Distance: %d \n u: %d \n v: %d \n\n",distance[u],g[u][v],newDistance,u,v);
    if(newDistance<distance[v] || distance[v]==-1){
        distance[v]=newDistance;
        pre[v]=u;
    }

}

int allSelected(bool *quere){
    for(int i = 0;i<n;i++){
        if (quere[i] == true) return 0;
    }
    return 1;
}

void shortestPath(int goal, int *pre, int *way){
    int u=goal, i=1;
    way[0]=goal;
    while (pre[u]!=0){
        u = pre[u];
        way[i]=u;
        printf("\n pre: %d",way[i]);
        i++;
    }

}

void Dijkstra(int start, int *pre, double *distance){
    int  u;
    bool quere[n];

    init(quere,start,distance,pre,n);

    while(allSelected(quere)==0){
        u = minDistance(distance,n, quere);//u = Knoten in quere mit dem kleinsten Abstand
        quere[u]=false; //u von quere löschen
        for(int i = 0;i<n;i++){ //für alle Nachbarn von u:
            if (g[u][i] > 0){
                int v = i;
                if (quere[v]){
                    distance_update(u,v,distance,pre);

                }
            }
        }

    }
}

int start(int argc, char** argv) {


    int way[n]= {-1};
    int pre[n];
    double distance[n];
    Dijkstra(0,pre,distance);

    shortestPath(19,pre,way);
    printf("\n Weg: ");
    for(int i=n;i>0;i--){
        if (way[i]) printf("%d  ", way[i]);
    }
    printf("\n Distanz: %f  ", distance[19]);


    return (EXIT_SUCCESS);
}
