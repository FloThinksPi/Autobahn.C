
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "processing.h"


const int ZeilenLeange=200;//Maximale länge einer zeile im dokument
const int Datenleange=50;//Maximale länge eines Datensatzes in einer zeile ( durch kommas getrennter bereich)

void ErstelleWegBidirektional(struct Knoten *meineKnoten[],int Knoten1,int Knoten2,int leange){

    int numWegezuKnoten1 = meineKnoten[Knoten1]->numWege;
    int numWegezuKnoten2 = meineKnoten[Knoten2]->numWege;

    //TODO Püfen ob weg schon existiert

    struct Wege *W1 =  malloc(sizeof(struct Wege));
    struct Wege *W2 =  malloc(sizeof(struct Wege));

    meineKnoten[Knoten1]->Wege[numWegezuKnoten1]=W1;
    meineKnoten[Knoten2]->Wege[numWegezuKnoten2]=W2;


    meineKnoten[Knoten1]->Wege[numWegezuKnoten1]->nach=meineKnoten[Knoten2];
    meineKnoten[Knoten1]->Wege[numWegezuKnoten1]->laenge=leange;


    meineKnoten[Knoten2]->Wege[numWegezuKnoten2]->nach=meineKnoten[Knoten1];
    meineKnoten[Knoten2]->Wege[numWegezuKnoten2]->laenge=leange;

    meineKnoten[Knoten1]->numWege++;
    meineKnoten[Knoten2]->numWege++;


}

int getNumKnoten(){

    //ZÄHLT ANZAHL ABFAHRTEN
    FILE *fp = fopen("data/knoten.csv", "r");
    if (fp == NULL) {
        perror("Fehler \"data/knoten.csv");
        return NULL;
    }
    //Datei zeilenweise lesen
    int countlines=0;
    int ch;

    while(!feof(fp))
    {
        ch = fgetc(fp);
        if(ch == '\n' || ch == EOF)
        {
            countlines++;
        }
    }
    fclose(fp);
    printf("Anzahl Knoten: %d \n", countlines);//DEBUG

    return countlines;
}

int loadDatabaseFiletoStruct(struct Knoten *meineKnoten[]){
//TODO DA kreuze doppekl sind , aber speicherplatz reserviert wird , verschwendet man diesen -> fix


    FILE *fp = fopen("data/knoten.csv", "r");
    char *line= malloc(sizeof(char)*ZeilenLeange);
    int i=0;
    while (1) {
        if (fgets(line,ZeilenLeange, fp) == NULL) break;

        char *Data;
        char *Buffer;
        Data=strtok_r(line,",",&Buffer);
        int u= 0;
        int dist;
        meineKnoten[i]= malloc(sizeof(struct Knoten));
        while(1){

            switch(u) {
                case 0: meineKnoten[i]->Name=Data;meineKnoten[i]->ID=i; break;
                case 1: meineKnoten[i]->AutobahnName=Data; break;
                case 2: sscanf(Data, "%d", &dist); meineKnoten[i]=dist; break;
            }


            Data = strtok_r(NULL,",",&Buffer);
            u++;
            if(Data==0) break;
        }
        printf("%s\n",meineKnoten[i]->Name);
        i++;

    }



    return NULL;
}



