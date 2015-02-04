
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "processing.h"
#include "../lib/printui/printui.h"
#include "dbio.h"
#include "../lib/levenshtein/levenshtein.h"

#ifdef _WIN32
char* FILEPATH="data\\knoten.csv";
#elif __linux
char* FILEPATH="data/knoten.csv";//TODO AbsolutePath berechnen
#elif __APPLE__
char* FILEPATH="data/knoten.csv";
#endif

const int ZeilenLeange=200;//Maximale länge einer zeile im dokument
const int Datenleange=50;//Maximale länge eines Datensatzes in einer zeile ( durch kommas getrennter bereich)

void ErstelleWegBidirektional(struct Knoten *meineKnoten[],int Knoten1,int Knoten2,double leange){

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
    FILE *fp = fopen(FILEPATH, "r");
    if (fp == NULL) {
        perror("Fehler \"data/knoten.csv");
        return 0;
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

int compare(const void *s1, const void *s2) {
    struct Knoten* K1 = *(struct Knoten **) s1;
    struct Knoten* K2 = *(struct Knoten **) s2;

    return K1->AutobahnKM - K2->AutobahnKM;
}

char** loadAutobahnen(struct Knoten *meineKnoten[],int AnzahlKnoten) {


    char** Autobahnen = malloc(sizeof(char*));
    int AnzahlAutobahnen=0;

    for(int i=0;i<AnzahlKnoten;i++){

        int inArray=0;

        for(int u=1;u<AnzahlAutobahnen+1;u++){
            if( strcmp(Autobahnen[u],meineKnoten[i]->AutobahnName) == 0 ){inArray=1; break;}
        }

        if(inArray==0){
            Autobahnen[AnzahlAutobahnen+1]=malloc(sizeof(char)* (countUTF8String(meineKnoten[i]->AutobahnName)+1));
            Autobahnen[AnzahlAutobahnen+1]=meineKnoten[i]->AutobahnName;
            AnzahlAutobahnen++;
        }

    }

    Autobahnen[0]=malloc(sizeof(char)*2000);//TODO BAD STATIC LEGTH
    sprintf(Autobahnen[0],"%d",AnzahlAutobahnen);


    return Autobahnen;
}

int loadDatabaseFiletoStruct(struct Knoten *meineKnoten[],int AnzahlKnoten){


    FILE *fp = fopen(FILEPATH, "r");
    char *line= malloc(sizeof(char)*(ZeilenLeange+1));
    int i=0;

    char *Data;
    while (1) {
        if (fgets(line,ZeilenLeange, fp) == NULL) break;


        Data=strtok(line,",");
        int u=0;
        int dist;
        printf("I=%d\n",i);
        meineKnoten[i]= malloc(sizeof(struct Knoten));
        while(1){
            printf("2I=%d\n",i);
            printf("U=%d\n",u);


            switch(u) {
                case 0: {
                        puts("A1");
                        meineKnoten[i]->Name= malloc(sizeof(char)*(countUTF8String(Data)+1));
                        memcpy(meineKnoten[i]->Name, Data, countUTF8String(Data)+1);
                        meineKnoten[i]->ID=i;
                        puts("A2\n");
                        break;
                 }
                case 1: {
                    puts("B1");
                        meineKnoten[i]->AutobahnName= malloc(sizeof(char)*(countUTF8String(Data)+1));
                        memcpy(meineKnoten[i]->AutobahnName, Data, countUTF8String(Data)+1);
                    puts("B2\n");
                    break;
                 }
                case 2: {
                    puts("C1");
                        dist= atoi(Data);
                        meineKnoten[i]->AutobahnKM=dist;
                    puts("C1\n");
                        break;
                }
                default : break;
            }

            printf("Dist=%d\n",dist);
            Data = strtok(NULL,",");
            u++;
            if(Data==0) break;
        }
        i++;
    }



    puts("AOK");
    //Sortiere Struct

    qsort(meineKnoten,AnzahlKnoten,sizeof(struct Knoten*),compare);

    puts("AOK");
    //Gleiche IDs der Knoten der richtigen reihenfolge an
    for(int x=0;x<AnzahlKnoten;x++){
        meineKnoten[x]->ID=x;
        meineKnoten[x]->numWege=0;
    }

    puts("AOK");

    //Lese Welche Autobahenen es gibt
    char** Autobahnen=loadAutobahnen(meineKnoten,AnzahlKnoten);

    //Geht auobahn für autobahn durch und verbinden alle knoten eriner autobahn , auch die kreuze , sie haben am anfang nur zwei wege , also wie normale abfahrten
    for(int x=1;x<=atol(Autobahnen[0]);x++){//TODO Atol depriciated

        printf("%s\n",Autobahnen[x]);
        int lastNode=INT_MAX;


        for(int u=0;u<AnzahlKnoten;u++){

            if(strcmp(meineKnoten[u]->AutobahnName, Autobahnen[x])==0){
                if(lastNode!=INT_MAX){
                    ErstelleWegBidirektional(meineKnoten, lastNode, u, meineKnoten[u]->AutobahnKM - meineKnoten[lastNode]->AutobahnKM);
                }
                lastNode=u;
            }

        }

    }

    //Verbinde alle Knoten mit gleichen namen mitteinander mit der entfernung 0.000001(wird zu 0 wegen ungenauigkeit) , (Kreuze werden verknüpft)

    for (int i = 0; i < AnzahlKnoten; ++i)
    {
        for (int j = i + 1; j < AnzahlKnoten; ++j) if (strcmp(meineKnoten[i]->Name,meineKnoten[j]->Name)==0)
            {
                char *Buffer= malloc(sizeof(char*));
                ErstelleWegBidirektional(meineKnoten, i, j, 0.000001);

                sprintf(Buffer,"%s(%s)",meineKnoten[i]->Name,meineKnoten[i]->AutobahnName);//TODO Anhängen der Autobahn an kreuznamen im output erledigen damit suchfunktion arbeiten kann
                memcpy(meineKnoten[i]->Name, Buffer, countUTF8String(Buffer)+1);

                sprintf(Buffer,"%s(%s)",meineKnoten[j]->Name,meineKnoten[j]->AutobahnName);
                memcpy(meineKnoten[j]->Name, Buffer, countUTF8String(Buffer)+1);

                break;
            }
    }

    //Initialisiere speicher
    for (int i = 0; i < AnzahlKnoten; i++){
        meineKnoten[i]->knotenZurueck= malloc(sizeof(struct Knoten*));
    }

    return 0;
}

int strcompCaseInsensitive(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }
}

int FindSimilarNode(struct Knoten *meineKnoten[],int AnzahlKnoten,char *KnotenName)
{

    int p=0;
    char *buffer= malloc(sizeof(char*));
    for(int i=0;i<AnzahlKnoten;i++)
    {

        if(levenshtein(KnotenName, meineKnoten[i]->Name)<=4 && countUTF8String(meineKnoten[i]->Name)>5){
            if(p==0){
                printf("\n");
                sprintf(buffer,"Ausfahrt/Kreuz \"%s\" Nicht Gefunden, Meinten sie vieleicht",KnotenName);
                printMenuHeader(buffer);
                p=1;
            }
            printMenuItem(meineKnoten[i]->Name);
        }

        if(levenshtein(KnotenName, meineKnoten[i]->Name)<=2 && countUTF8String(meineKnoten[i]->Name)<=5){
            if(p==0){
                printf("\n");
                sprintf(buffer,"Ausfahrt/Kreuz \"%s\" Nicht Gefunden, Meinten sie vieleicht",KnotenName);
                printMenuHeader(buffer);
                p=1;
            }
            printMenuItem(meineKnoten[i]->Name);
        }

    }
    if(p==0){
        puts("\n");
        sprintf(buffer,"Ausfahrt/Kreuz \"%s\" Nicht Gefunden",KnotenName);
        printMenuHeader(buffer);
    }else{
        printFooter();
    }
    return 0;
}


int findeKnotenByName(struct Knoten *meineKnoten[],int AnzahlKnoten,char *KnotenName){


    for(int i=0;i<AnzahlKnoten;i++){
        if(strcompCaseInsensitive(meineKnoten[i]->Name,KnotenName)==0){
            return meineKnoten[i]->ID;
        }

    }

    FindSimilarNode(meineKnoten, AnzahlKnoten, KnotenName);

    return INT_MAX;

};







































