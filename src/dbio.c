
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
char* FILEPATH="data/knoten_viele.csv";//TODO AbsolutePath berechnen
#elif __APPLE__
char* FILEPATH="data/knoten.csv";
#endif

const int ZeilenLeange=300;//Maximale länge einer zeile im dokument
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

int getNumKnoten(){//Muss vor dem laden der daten geschegen , damit der speicherplatz Arrayhack->meineknoten richtig belegt wird

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

    return countlines;
}

int compare(const void *s1, const void *s2) {
    struct Knoten* K1 = *(struct Knoten **) s1;
    struct Knoten* K2 = *(struct Knoten **) s2;

    return (int) (K1->AutobahnKM - K2->AutobahnKM);
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

    Autobahnen[0]=malloc(sizeof(char)*10);//TODO BAD STATIC LEGTH
    sprintf(Autobahnen[0],"%d",AnzahlAutobahnen);


    return Autobahnen;
}

int loadDatabaseFiletoStruct(struct Knoten *meineKnoten[],int AnzahlKnoten){


    FILE *fp = fopen(FILEPATH, "r");
    char *line= malloc(sizeof(char)*(ZeilenLeange+1));


    char *Data;
    int i;
    for(i=0;i<AnzahlKnoten;i++) {
        if (fgets(line,ZeilenLeange,fp) == NULL) break;

        //fgets(line,ZeilenLeange,fp);
        Data=strtok(line,",");
        int u=0;
        int dist;
        meineKnoten[i]= malloc(sizeof(struct Knoten));
        while(1){



            switch(u) {
                case 0: {

                        meineKnoten[i]->Name= malloc(sizeof(char)*(countUTF8String(Data)+1));
                        memcpy(meineKnoten[i]->Name, Data, countUTF8String(Data)+1);
                        meineKnoten[i]->ID=i;
                        break;
                 }
                case 1: {

                        meineKnoten[i]->AutobahnName= malloc(sizeof(char)*(countUTF8String(Data)+1));
                        memcpy(meineKnoten[i]->AutobahnName, Data, countUTF8String(Data)+1);
                    break;
                 }
                case 2: {

                        dist= atof(Data);
                        meineKnoten[i]->AutobahnKM=dist;
                        break;
                }
                default : break;
            }


            Data = strtok(NULL,",");
            u++;
            if(Data==0) break;
        }
    }

    fclose(fp);

    //Sortiere Struct

    //size_t Knoten_len = AnzahlKnoten / sizeof(struct Knoten);
    qsort(meineKnoten,AnzahlKnoten,sizeof(struct Knoten*),compare);


    //Gleiche IDs der Knoten der richtigen reihenfolge an
    for(int x=0;x<AnzahlKnoten;x++){
        meineKnoten[x]->ID=x;
        meineKnoten[x]->numWege=0;
    }


    //Lese Welche Autobahenen es gibt
    char** Autobahnen=loadAutobahnen(meineKnoten,AnzahlKnoten);

    //Geht auobahn für autobahn durch und verbinden alle knoten eriner autobahn , auch die kreuze.csv , sie haben am anfang nur zwei wege , also wie normale abfahrten
    for(int x=1;x<=atol(Autobahnen[0]);x++){//TODO Atol depriciated


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
                char *Buffer= malloc(sizeof(char)*1000);
                ErstelleWegBidirektional(meineKnoten, i, j, 0.000001);

                meineKnoten[i]->isKreuz=1;
                //sprintf(Buffer,"%s(%s)",meineKnoten[i]->Name,meineKnoten[i]->AutobahnName);//TODO Anhängen der Autobahn an kreuznamen im output erledigen damit suchfunktion arbeiten kann
                //memcpy(meineKnoten[i]->Name, Buffer, countUTF8String(Buffer)+1);

                meineKnoten[j]->isKreuz=1;
                //sprintf(Buffer,"%s(%s)",meineKnoten[j]->Name,meineKnoten[j]->AutobahnName);
                //memcpy(meineKnoten[j]->Name, Buffer, countUTF8String(Buffer)+1);

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
    char *buffer= malloc(sizeof(char)*1000);
    for(int i=0;i<AnzahlKnoten;i++)
    {

        if(levenshtein(KnotenName, meineKnoten[i]->Name)<=3){
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


void printAutobahn(struct Knoten *meineKnoten[],int AnzahlKnoten,char *AutobahnName,char *Suchbegriff){//TODO QuickA and Dirty print remove

    char *Buffer= malloc(sizeof(char)*1000);
    int TotalSize= 70;
    char *SearchBuffer;

    struct Knoten *meineGefiltertenKnoten[AnzahlKnoten];

    //Ausfahrten und Kreuze Errechnen
    int i;
    int size;
    int AnzahlGefilterte=0;
    for(i=0;i<AnzahlKnoten;i++){
        if(strcmp(meineKnoten[i]->AutobahnName,AutobahnName)==0){

            meineGefiltertenKnoten[AnzahlGefilterte]=meineKnoten[i];//zeigt auf den gleichen bereich wie meinknoten
            AnzahlGefilterte++;

        }
    }

    puts("\n");
    //Überschrift Ausgeben
    if(Suchbegriff!=NULL){
        sprintf(Buffer,"%s Befindet sich auf Autobahn %s",Suchbegriff,meineGefiltertenKnoten[0]->AutobahnName);

        for(int x=0;x<AnzahlGefilterte;x++){
            if(strcompCaseInsensitive(meineGefiltertenKnoten[x]->Name,Suchbegriff)==0){
                sprintf(Buffer,"<<<%s>>>",meineGefiltertenKnoten[x]->Name);
                size = countUTF8String(Buffer);
                SearchBuffer = malloc(sizeof(char)*size+1);
                memcpy(SearchBuffer, Buffer, size+1);
                meineGefiltertenKnoten[x]->Name=SearchBuffer;//Zeiger wird umgerichtet
                sprintf(Buffer,"%s(%.0f Km) ist auf Autobahn: %s",meineGefiltertenKnoten[x]->Name,meineGefiltertenKnoten[x]->AutobahnKM,AutobahnName);
            }
        }


    }else{
        sprintf(Buffer,"Autobahn %s",AutobahnName);
    }
    printMenuHeader(Buffer);
    printMenuItem(" ");
    if(meineGefiltertenKnoten[0]->isKreuz==1){

        char *Buffer2= malloc(sizeof(char)*1000);
        //Findet die autobahn zu der das kreuz führt
        for(int u=0;u<meineGefiltertenKnoten[0]->numWege;u++){
            if(meineGefiltertenKnoten[0]->Wege[u]->nach->ID!=meineGefiltertenKnoten[0+1]->ID){
                Buffer2=meineGefiltertenKnoten[0]->Wege[u]->nach->AutobahnName;
                break;
            }
        }

        sprintf(Buffer,"Kreuz %s(%.2f Km) --- %s -> %s",meineGefiltertenKnoten[0]->Name,meineGefiltertenKnoten[0]->AutobahnKM,meineGefiltertenKnoten[0]->AutobahnName,Buffer2);
        size= countUTF8String(Buffer);
        for(int u=0;u<(TotalSize-size);u++){
            Buffer[size+u]=' ';
        }
        printMenuItem(Buffer);


        free(Buffer2);

    }else{
        //Erste Ausfahrt Ausgeben
        sprintf(Buffer,"%s(%.2f Km)",meineGefiltertenKnoten[0]->Name,meineGefiltertenKnoten[0]->AutobahnKM);
        size = countUTF8String(Buffer);
        for(int u=0;u<(TotalSize-size);u++){
            Buffer[size+u]=' ';
        }
        printMenuItem(Buffer);
    }


    sprintf(Buffer,"      |  ");
    size= countUTF8String(Buffer);
    for(int u=0;u<(TotalSize-size);u++){
        Buffer[size+u]=' ';
    }
    printMenuItem(Buffer);

    sprintf(Buffer,"    //#\\\\");
    size= countUTF8String(Buffer);
    for(int u=0;u<(TotalSize-size);u++){
        Buffer[size+u]=' ';
    }
    printMenuItem(Buffer);

    sprintf(Buffer,"   // # \\\\");
    size= countUTF8String(Buffer);
    for(int u=0;u<(TotalSize-size);u++){
        Buffer[size+u]=' ';
    }
    printMenuItem(Buffer);

    sprintf(Buffer,"  //  #  \\\\");
    size= countUTF8String(Buffer);
    for(int u=0;u<(TotalSize-size);u++){
        Buffer[size+u]=' ';
    }
    printMenuItem(Buffer);

    //AUSFAHRTEN

    int x;
    for(x=1;x<AnzahlGefilterte-1;x++){



        sprintf(Buffer,"  ||  #  ||");
        size= countUTF8String(Buffer);
        for(int u=0;u<(TotalSize-size);u++){
            Buffer[size+u]=' ';
        }
        printMenuItem(Buffer);


        if(meineGefiltertenKnoten[x]->isKreuz==1){
            sprintf(Buffer,"  /|  #  |\\");
            size= countUTF8String(Buffer);
            for(int u=0;u<(TotalSize-size);u++){
                Buffer[size+u]=' ';
            }
            printMenuItem(Buffer);
            sprintf(Buffer," / |  #  | \\");
            size= countUTF8String(Buffer);
            for(int u=0;u<(TotalSize-size);u++){
                Buffer[size+u]=' ';
            }
            printMenuItem(Buffer);

            char *Buffer2= malloc(sizeof(char)*1000);
            //Findet die autobahn zu der das kreuz führt
            for(int u=0;u<meineGefiltertenKnoten[x]->numWege;u++){
                if(meineGefiltertenKnoten[x]->Wege[u]->nach->ID!=meineGefiltertenKnoten[x-1]->ID && meineGefiltertenKnoten[x]->Wege[u]->nach->ID!=meineGefiltertenKnoten[x+1]->ID){
                    Buffer2=meineGefiltertenKnoten[x]->Wege[u]->nach->AutobahnName;
                    break;
                }
            }

            sprintf(Buffer,"=  |  #  |  =    Kreuz  %s(%.2f Km) --- %s -> %s",meineGefiltertenKnoten[x]->Name,meineGefiltertenKnoten[x]->AutobahnKM,meineGefiltertenKnoten[x]->AutobahnName,Buffer2);
            size= countUTF8String(Buffer);
            for(int u=0;u<(TotalSize-size);u++){
                Buffer[size+u]=' ';
            }
            printMenuItem(Buffer);


            free(Buffer2);



            sprintf(Buffer," \\ |  #  | /");
            size= countUTF8String(Buffer);
            for(int u=0;u<(TotalSize-size);u++){
                Buffer[size+u]=' ';
            }
            printMenuItem(Buffer);
            sprintf(Buffer,"  \\|  #  |/");
            size= countUTF8String(Buffer);
            for(int u=0;u<(TotalSize-size);u++){
                Buffer[size+u]=' ';
            }
            printMenuItem(Buffer);
        }else{

            sprintf(Buffer,"  ||  #  |\\");
            size= countUTF8String(Buffer);
            for(int u=0;u<(TotalSize-size);u++){
                Buffer[size+u]=' ';
            }
            printMenuItem(Buffer);
            sprintf(Buffer,"  ||  #  | \\");
            size= countUTF8String(Buffer);
            for(int u=0;u<(TotalSize-size);u++){
                Buffer[size+u]=' ';
            }
            printMenuItem(Buffer);



            sprintf(Buffer,"  ||  #  |  =    %s(%.2f Km)",meineGefiltertenKnoten[x]->Name,meineGefiltertenKnoten[x]->AutobahnKM);
            size= countUTF8String(Buffer);
            for(int u=0;u<(TotalSize-size);u++){
                Buffer[size+u]=' ';
            }
            printMenuItem(Buffer);




            sprintf(Buffer,"  ||  #  | /");
            size= countUTF8String(Buffer);
            for(int u=0;u<(TotalSize-size);u++){
                Buffer[size+u]=' ';
            }
            printMenuItem(Buffer);
            sprintf(Buffer,"  ||  #  |/");
            size= countUTF8String(Buffer);
            for(int u=0;u<(TotalSize-size);u++){
                Buffer[size+u]=' ';
            }
            printMenuItem(Buffer);
        }


        sprintf(Buffer,"  ||  #  ||");
        size= countUTF8String(Buffer);
        for(int u=0;u<(TotalSize-size);u++){
            Buffer[size+u]=' ';
        }
        printMenuItem(Buffer);

    }


//ENDKNOTEN

    sprintf(Buffer,"  \\\\  #  //");
    size= countUTF8String(Buffer);
    for(int u=0;u<(TotalSize-size);u++){
        Buffer[size+u]=' ';
    }
    printMenuItem(Buffer);

    sprintf(Buffer,"   \\\\ # //");
    size= countUTF8String(Buffer);
    for(int u=0;u<(TotalSize-size);u++){
        Buffer[size+u]=' ';
    }
    printMenuItem(Buffer);

    sprintf(Buffer,"    \\\\#//");
    size= countUTF8String(Buffer);
    for(int u=0;u<(TotalSize-size);u++){
        Buffer[size+u]=' ';
    }
    printMenuItem(Buffer);

    sprintf(Buffer,"      |  ");
    size= countUTF8String(Buffer);
    for(int u=0;u<(TotalSize-size);u++){
        Buffer[size+u]=' ';
    }
    printMenuItem(Buffer);

    //Letzten Knoten Ausgeben


    if(meineGefiltertenKnoten[0]->isKreuz==1){

        char *Buffer2= malloc(sizeof(char)*1000);
        //Findet die autobahn zu der das kreuz führt
        for(int u=0;u<meineGefiltertenKnoten[x]->numWege;u++){
            if(meineGefiltertenKnoten[x]->Wege[u]->nach->ID!=meineGefiltertenKnoten[x-1]->ID){
                Buffer2=meineGefiltertenKnoten[x]->Wege[u]->nach->AutobahnName;
                break;
            }
        }

        sprintf(Buffer,"Kreuz %s(%.2f Km) --- %s -> %s",meineGefiltertenKnoten[x]->Name,meineGefiltertenKnoten[x]->AutobahnKM,meineGefiltertenKnoten[x]->AutobahnName,Buffer2);
        size= countUTF8String(Buffer);
        for(int u=0;u<(TotalSize-size);u++){
            Buffer[size+u]=' ';
        }
        printMenuItem(Buffer);

        free(Buffer2);

    }else{
        //Erste Ausfahrt Ausgeben
        sprintf(Buffer,"%s(%.2f Km)",meineGefiltertenKnoten[x]->Name,meineGefiltertenKnoten[x]->AutobahnKM);
        size = countUTF8String(Buffer);
        for(int u=0;u<(TotalSize-size);u++){
            Buffer[size+u]=' ';
        }
        printMenuItem(Buffer);
    }

    printFooter();
    puts("\n");

    //Aufräumen
    free(Buffer);
    free(*meineGefiltertenKnoten);
}




































