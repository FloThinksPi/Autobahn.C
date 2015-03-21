
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include "processing.h"
#include "../lib/printui/printui.h"
#include "dbio.h"
#include "../lib/levenshtein/levenshtein.h"
#include "utils.h"

#ifdef WIN32
char* FILEPATH="knoten.csv";
#else
char* FILEPATH="knoten_viele.csv";
#endif

const int ZeilenLeange=300;//Maximale länge einer zeile im dokument


//////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// FILE I/O //////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

int getNumKnoten(){//Gibt die anzahl an datensätzen in der datei an

    //Öffnet Datei
    FILE *fp = fopen(FILEPATH, "r");
    if (fp == NULL) {
        return 0;
    }
    //Datei zeilenweise lesen und hochzählen
    int countlines=0;
    int ch;

    while(!feof(fp))
    {
        ch = fgetc(fp);
        if(ch == '\n' || ch == EOF) //Wenn Ein EOF oder Zeilenubruch kommt  hochzählen.
        {
            countlines++;
        }
    }
    fclose(fp);

    //gezählte zeilen ausgeben = anzahl Datensätze
    return countlines;
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
        double dist;
        //SpeicherPlatzt für eintrag reservieren und alle werte initialisieren damit es nichgt zu undef. verhalten kommt.
        meineKnoten[i]= malloc(sizeof(struct Knoten));
        meineKnoten[i]->besucht= false;
        meineKnoten[i]->Name=NULL;
        meineKnoten[i]->AutobahnName=NULL;
        meineKnoten[i]->AutobahnKM=0;
        meineKnoten[i]->entfernungZumUrsprung=0;
        meineKnoten[i]->ID=i;
        meineKnoten[i]->isKreuz=0;
        meineKnoten[i]->numWege=0;
        meineKnoten[i]->Wege[0]=NULL;
        meineKnoten[i]->Wege[1]=NULL;
        meineKnoten[i]->Wege[2]=NULL;
        meineKnoten[i]->knotenZurueck=NULL;
        meineKnoten[i]->entfernungZumUrsprung=0;

        while(1){



            switch(u) {
                case 0: {

                    meineKnoten[i]->Name= malloc(sizeof(char)*(strlen(Data)+1));
                    memcpy(meineKnoten[i]->Name, Data, strlen(Data)+1);
                    meineKnoten[i]->ID=i;
                    break;
                }
                case 1: {

                    meineKnoten[i]->AutobahnName= malloc(sizeof(char)*(strlen(Data)+1));
                    memcpy(meineKnoten[i]->AutobahnName, Data, strlen(Data)+1);
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

    if(fp!=NULL){
        fclose(fp);
    }

    free(line);

    //Bearbeitet nun die eingelesenen Daten (Ausfahrten Verknüpfen und danach Kreuze)
    ConnectData(meineKnoten,AnzahlKnoten);

    return 0;
}


void saveStructToFile(struct Knoten *meineKnoten[],int AnzahlKnoten){//Speichert alle Daten in die DatenbankDatei , Überschreibt diese dabei Komplett.

    FILE * file= fopen(FILEPATH,"w");
    if (file != NULL) {
        for(int i=0;i<AnzahlKnoten-1;i++){
            fprintf(file,"%s,%s,%.2f\n",meineKnoten[i]->Name,meineKnoten[i]->AutobahnName,meineKnoten[i]->AutobahnKM);
        }
        fprintf(file,"%s,%s,%.2f",meineKnoten[AnzahlKnoten-1]->Name,meineKnoten[AnzahlKnoten-1]->AutobahnName,meineKnoten[AnzahlKnoten-1]->AutobahnKM);
        fclose(file);
    }else{}//TODO EXIT MIT FEHLERMEDLING

    return ;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// DATA VERARBEITUNG //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////


void ErstelleWegBidirektional(struct Knoten *meineKnoten[],int Knoten1,int Knoten2,double leange){

    //Verknüpft Knoten1 mit Knoten2 mit leange als Länge , bidirektional d.h. in beide richtungen.

    int numWegezuKnoten1 = meineKnoten[Knoten1]->numWege;
    int numWegezuKnoten2 = meineKnoten[Knoten2]->numWege;


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


void OnlyConnectKreuze(struct Knoten *meineKnoten[],int AnzahlKnoten){//Verbindet Alle Ausfahrten mit gleichem Namen miteinander mit kleinem weg(wird nicht angezeigt und wird Weggerundet)

    for (int i = 0; i < AnzahlKnoten; ++i)
    {

        for (int j = i + 1; j < AnzahlKnoten; ++j) {
            if (i == j)break;

            if (strcmp(meineKnoten[i]->Name, meineKnoten[j]->Name) == 0) {
                ErstelleWegBidirektional(meineKnoten, i, j, 0.00000001);
                meineKnoten[i]->isKreuz = 1;
                meineKnoten[j]->isKreuz = 1;
                break;
            }
        }

    }
}

void OnlyREConnectKreuze(struct Knoten *meineKnoten[],int AnzahlKnoten){//Verbindet Alle Kreuze(Schon einmal durch OnlyConnectKreuze verbinden worden) mit gleichem Namen miteinander mit kleinem weg(wird nicht angezeigt und wird Weggerundet)

    for (int i = 0; i < AnzahlKnoten; ++i)
    {
        if(meineKnoten[i]->isKreuz==1) {

            for (int j = i + 1; j < AnzahlKnoten; ++j) {
                if (i == j)break;

                if (strcmp(meineKnoten[i]->Name, meineKnoten[j]->Name) == 0) {
                    ErstelleWegBidirektional(meineKnoten, i, j, 0.00000001);
                    meineKnoten[i]->isKreuz = 1;
                    meineKnoten[j]->isKreuz = 1;
                    break;
                }
            }

        }
    }
}

void OnlyConnectEinKreuz(struct Knoten *meineKnoten[],int AnzahlKnoten,char *ZeilKreuz){//Verbindet Eine Ausfahrt mit einer gleichen Namens miteinander mit kleinem weg(wird nicht angezeigt und wird Weggerundet)

    for (int i = 0; i < AnzahlKnoten; ++i)
    {
        if(strcmp(meineKnoten[i]->Name, ZeilKreuz) == 0) {

            for (int j = i + 1; j < AnzahlKnoten; ++j) {

                if (strcmp(meineKnoten[i]->Name, meineKnoten[j]->Name) == 0) {
                    ErstelleWegBidirektional(meineKnoten, i, j, 0.00000001);
                    meineKnoten[i]->isKreuz = 1;
                    meineKnoten[j]->isKreuz = 1;
                    break;
                }
            }

        }
    }
}

void ConnectData(struct Knoten *meineKnoten[],int AnzahlKnoten){//Verbindet alle Ausfahrten mit der Nächsten auf der Gleichen Autobahn , geht dann Autobahn für Autobahn durch , ergebniss = voneinander getrennte autobahnen , die Kreuze müssen separat verknüpft werden.

    //Voraussetzung ist ein nach Autobahnkm sortiertes Struct

    //Gleiche IDs der Knoten der richtigen reihenfolge an
    for(int x=0;x<AnzahlKnoten;x++){
        meineKnoten[x]->ID=x;
        meineKnoten[x]->numWege=0;
    }

    //Lese Welche Autobahenen es gibt
    char** Autobahnen= GetAutobahnen(meineKnoten, AnzahlKnoten);

    //Geht auobahn für autobahn durch und verbinden alle knoten eriner autobahn , auch die kreuze.csv , sie haben am anfang nur zwei wege , also wie normale abfahrten
    for(int x=1;x<=atoi(Autobahnen[0]);x++){


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

    OnlyConnectKreuze(meineKnoten, AnzahlKnoten);


}

//////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Daten Bearbeitung///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////


//Editieren wird durch löschen und neu anlegen realisiert , somit sind nur die Grundfunktionen löschen und neu anlegen nötig.

int DeleteKnoten(struct Knoten *meineKnoten[], int AnzahlKnoten, int Ausfahrt){//Löscht einen Dtaensatz

    free(meineKnoten[Ausfahrt]->Name);
    free(meineKnoten[Ausfahrt]->AutobahnName);
    free(meineKnoten[Ausfahrt]->knotenZurueck);
    free(meineKnoten[Ausfahrt]);


    for(int i=Ausfahrt+1;i<AnzahlKnoten;i++){
        meineKnoten[i-1]=meineKnoten[i];
        meineKnoten[i-1]->ID=i-1;
    }

    for(int i=0;i<AnzahlKnoten;i++){//Free(null) ist valid
        if(meineKnoten[i]->Wege[0]!=NULL) free(meineKnoten[i]->Wege[0]); meineKnoten[i]->Wege[0]= NULL;
        if(meineKnoten[i]->Wege[1]!=NULL) free(meineKnoten[i]->Wege[1]); meineKnoten[i]->Wege[1]= NULL;
        if(meineKnoten[i]->Wege[2]!=NULL) free(meineKnoten[i]->Wege[2]); meineKnoten[i]->Wege[2]= NULL;

        meineKnoten[i]->numWege=0;
    }


    //qsort(meineKnoten,AnzahlKnoten,sizeof(struct Knoten*),QsortCompareKM);
    //ConnectData(meineKnoten, AnzahlKnoten-1);//Sortieren nicht nötig da die reihenfolge nicht geändert wird.

    return AnzahlKnoten-1;
}

int NewKnoten(struct Knoten *meineKnoten[],int AnzahlKnoten,char* Name,char* AutobahnName,double AutobahnKM){ // Neuen Datensatz Erstellen


    int zielSpeicher= INT_MAX;

    for(int i=0;i<AnzahlKnoten;i++){
        if(meineKnoten[i]->AutobahnKM>AutobahnKM){
            zielSpeicher=i;
            break;
        }
    }

    if(zielSpeicher== INT_MAX)zielSpeicher=AnzahlKnoten;

    for(int i=AnzahlKnoten;i>zielSpeicher;i--){
        meineKnoten[i]=meineKnoten[i-1];
        meineKnoten[i]->ID=i;
    }



    meineKnoten[zielSpeicher]=malloc(sizeof(struct Knoten));
    meineKnoten[zielSpeicher]->besucht= false;
    meineKnoten[zielSpeicher]->Name=strdup(Name);
    meineKnoten[zielSpeicher]->AutobahnName=strdup(AutobahnName);
    meineKnoten[zielSpeicher]->AutobahnKM=AutobahnKM;
    meineKnoten[zielSpeicher]->entfernungZumUrsprung=0;
    meineKnoten[zielSpeicher]->ID=zielSpeicher;
    meineKnoten[zielSpeicher]->isKreuz=0;
    meineKnoten[zielSpeicher]->numWege=0;
    meineKnoten[zielSpeicher]->Wege[0]=NULL;
    meineKnoten[zielSpeicher]->Wege[1]=NULL;
    meineKnoten[zielSpeicher]->Wege[2]=NULL;
    meineKnoten[zielSpeicher]->knotenZurueck=NULL;
    meineKnoten[zielSpeicher]->entfernungZumUrsprung=0;



    for(int i=0;i<AnzahlKnoten;i++){//Free(null) ist valid
        if(meineKnoten[i]->Wege[0]!=NULL) free(meineKnoten[i]->Wege[0]); meineKnoten[i]->Wege[0]= NULL;
        if(meineKnoten[i]->Wege[1]!=NULL) free(meineKnoten[i]->Wege[1]); meineKnoten[i]->Wege[1]= NULL;
        if(meineKnoten[i]->Wege[2]!=NULL) free(meineKnoten[i]->Wege[2]); meineKnoten[i]->Wege[2]= NULL;

        meineKnoten[i]->numWege=0;
    }

    //qsort(meineKnoten,AnzahlKnoten,sizeof(struct Knoten*),QsortCompareKM);
    //ConnectData(meineKnoten, AnzahlKnoten+1);//Sortieren nicht nötig da die reihenfolge nicht geändert wird.

    return AnzahlKnoten+1;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Daten Abfragen /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////


char**GetAutobahnen(struct Knoten *meineKnoten[], int AnzahlKnoten) { // Gibt eine liste der existierenden Autobahnen zurück.


    //Durchsucht alle knoten , wenn die autobahn des knotens noch nicht in der liste zum ausgeben ist dann wird diese hinzugefügt.

    char** Autobahnen = malloc(sizeof(char*)*AnzahlKnoten+1);//Es können maximal so viele autobahnen wie ausfahrten existieren.
    int AnzahlAutobahnen=0;

    for(int i=0;i<AnzahlKnoten;i++){

        int inArray=0;

        for(int u=1;u<AnzahlAutobahnen+1;u++){
            if( strcmp(Autobahnen[u],meineKnoten[i]->AutobahnName) == 0 ){inArray=1; break;}
        }

        if(inArray==0){
            Autobahnen[AnzahlAutobahnen+1]=malloc(sizeof(char)* (strlen(meineKnoten[i]->AutobahnName)+1));
            memcpy(Autobahnen[AnzahlAutobahnen+1], meineKnoten[i]->AutobahnName, strlen(meineKnoten[i]->AutobahnName)+1);
            AnzahlAutobahnen++;
        }

    }

    Autobahnen[0]=malloc(sizeof(char)*10);//Speichern anzahl autobahnen im ersten feld
    sprintf(Autobahnen[0],"%d",AnzahlAutobahnen);

    Autobahnen = realloc(Autobahnen, sizeof(char*)*AnzahlAutobahnen+1);//Speicher verkleinern auf benötigten speicher

    return Autobahnen;
}





int FindSimilarNode(struct Knoten *meineKnoten[],int AnzahlKnoten,char *KnotenName) { //Findet Ähnlich lautende Knoten (levenshtein)

    int p=0;
    char *buffer= malloc(sizeof(char)*1000);
    for(int i=0;i<AnzahlKnoten;i++)
    {

        if(levenshtein(KnotenName, meineKnoten[i]->Name)<=3){
            if(p==0){
                printf("\n");
                sprintf(buffer,"Ausfahrt/Kreuz \"%s\" Nicht gefunden, meinten sie vieleicht",KnotenName);
                printMenuHeader(buffer);
                p=1;
            }
            printMenuItem(meineKnoten[i]->Name);
        }



    }
    if(p==0){
        puts("\n");
        sprintf(buffer,"Ausfahrt/Kreuz \"%s\" Nicht gefunden",KnotenName);
        printMenuHeader(buffer);
    }else{
        printFooter();
    }
    free(buffer);
    return 0;
}


int findeKnotenByName(struct Knoten *meineKnoten[],int AnzahlKnoten,char *KnotenName,int FindSimmilar){ //Findet die ID eines Knoten durch suche nach Namen


    for(int i=0;i<AnzahlKnoten;i++){
        if(strcompCaseInsensitive(meineKnoten[i]->Name,KnotenName)==0){
            return meineKnoten[i]->ID;
        }

    }

    if(FindSimmilar){
        FindSimilarNode(meineKnoten, AnzahlKnoten, KnotenName);
    }

    return INT_MAX;

};



//////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Datenbank Visualisierung ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////



void printAutobahnRow(char *S,int TotalSize){//Ausgabe Hilfefunktion

    size_t size_S=strlen(S);
    int size_n= CountUTF8String(S);
    char *Buffer=malloc(sizeof(char)*(TotalSize+1));

    memcpy(Buffer, S, size_S);

    int u;
    for(u=0;u<(TotalSize-size_n);u++){
        Buffer[size_n+u]=' ';
    }

    printMenuItem(Buffer);

    free(Buffer);
}

void printAutobahnVisual(struct Knoten *meineKnoten[], int AnzahlKnoten, char *AutobahnName, char *Suchbegriff,int SortierModus){//Gibt Eine art zeichnung einer Autobahn aus (Standart)

    char *Buffer= malloc(sizeof(char)*1000);
    int TotalSize= 70;
    struct Knoten *SearchBuffer = NULL;

    struct Knoten *meineGefiltertenKnoten[AnzahlKnoten];

    //Ausfahrten und Kreuze Errechnen
    int i;
    size_t size;
    int AnzahlGefilterte=0;
    for(i=0;i<AnzahlKnoten;i++){
        if(strcmp(meineKnoten[i]->AutobahnName,AutobahnName)==0){

            meineGefiltertenKnoten[AnzahlGefilterte]= meineKnoten[i];
            AnzahlGefilterte++;

        }
    }

    puts("\n");
    //Überschrift Ausgeben
    if(Suchbegriff!=NULL){
        sprintf(Buffer,"%s Befindet sich auf der Autobahn %s",Suchbegriff,meineGefiltertenKnoten[0]->AutobahnName);

        for(int x=0;x<AnzahlGefilterte;x++){
            if(strcompCaseInsensitive(meineGefiltertenKnoten[x]->Name,Suchbegriff)==0){

                sprintf(Buffer,"<<<%s>>>",meineGefiltertenKnoten[x]->Name);

                SearchBuffer = malloc(sizeof(struct Knoten));
                SearchBuffer->AutobahnName=strdup(meineGefiltertenKnoten[x]->AutobahnName);
                SearchBuffer->Name=strdup(Buffer);
                SearchBuffer->AutobahnKM=meineGefiltertenKnoten[x]->AutobahnKM;
                SearchBuffer->numWege=meineGefiltertenKnoten[x]->numWege;
                SearchBuffer->ID=meineGefiltertenKnoten[x]->ID;
                SearchBuffer->Wege[0] = meineGefiltertenKnoten[x]->Wege[0];
                SearchBuffer->Wege[1] = meineGefiltertenKnoten[x]->Wege[1];
                SearchBuffer->Wege[2] = meineGefiltertenKnoten[x]->Wege[2];
                SearchBuffer->isKreuz=meineGefiltertenKnoten[x]->isKreuz;

                meineGefiltertenKnoten[x]= SearchBuffer;//Zeiger wird umgerichtet
                sprintf(Buffer,"  %s(%.0f Km) ist auf Autobahn: %s  ",meineGefiltertenKnoten[x]->Name,meineGefiltertenKnoten[x]->AutobahnKM,AutobahnName);
            }
        }


    }else{
        sprintf(Buffer,"  Autobahn %s  ",AutobahnName);
    }

    printMenuHeader(Buffer);
    printMenuItem(" ");
    if(meineGefiltertenKnoten[0]->isKreuz==1){

        char *Buffer2;
        //Findet die autobahn zu der das kreuz führt
        for(int u=0;u<meineGefiltertenKnoten[0]->numWege;u++){
            if(AnzahlGefilterte>1) {
                if (meineGefiltertenKnoten[0]->Wege[u]->nach->ID != meineGefiltertenKnoten[0 + 1]->ID) {
                    Buffer2 = meineGefiltertenKnoten[0]->Wege[u]->nach->AutobahnName;
                    break;
                }
            }else{
                Buffer2 = meineGefiltertenKnoten[0]->Wege[u]->nach->AutobahnName;
                break;
            }
        }



        sprintf(Buffer,"Kreuz %s(%.2f Km) --- %s -> %s",meineGefiltertenKnoten[0]->Name,meineGefiltertenKnoten[0]->AutobahnKM,meineGefiltertenKnoten[0]->AutobahnName,Buffer2);
        printAutobahnRow(Buffer, TotalSize);


    }else{
        //Erste Ausfahrt Ausgeben
        sprintf(Buffer,"%s(%.2f Km)",meineGefiltertenKnoten[0]->Name,meineGefiltertenKnoten[0]->AutobahnKM);
        printAutobahnRow(Buffer, TotalSize);
    }

    printAutobahnRow("      |  ", TotalSize);
    printAutobahnRow("    //#\\\\", TotalSize);
    printAutobahnRow("   // # \\\\", TotalSize);
    printAutobahnRow("  //  #  \\\\", TotalSize);

    //AUSFAHRTEN

    int x;
    for(x=1;x<AnzahlGefilterte-1;x++){

        printAutobahnRow("  ||  #  ||", TotalSize);

        if(meineGefiltertenKnoten[x]->isKreuz==1){

            printAutobahnRow("  /|  #  |\\", TotalSize);
            printAutobahnRow(" / |  #  | \\", TotalSize);

            char *Buffer2;
            //Findet die autobahn zu der das kreuz führt
            for(int u=0;u<meineGefiltertenKnoten[x]->numWege;u++){
                if(meineGefiltertenKnoten[x]->Wege[u]->nach->ID!=meineGefiltertenKnoten[x-1]->ID && meineGefiltertenKnoten[x]->Wege[u]->nach->ID!=meineGefiltertenKnoten[x+1]->ID){
                    Buffer2=meineGefiltertenKnoten[x]->Wege[u]->nach->AutobahnName;
                    break;
                }
            }

            sprintf(Buffer,"=  |  #  |  =    Kreuz  %s(%.2f Km) --- %s -> %s",meineGefiltertenKnoten[x]->Name,meineGefiltertenKnoten[x]->AutobahnKM,meineGefiltertenKnoten[x]->AutobahnName,Buffer2);
            printAutobahnRow(Buffer, TotalSize);

            printAutobahnRow(" \\ |  #  | /", TotalSize);
            printAutobahnRow("  \\|  #  |/", TotalSize);


        }else{


            printAutobahnRow("  ||  #  |\\", TotalSize);
            printAutobahnRow("  ||  #  | \\", TotalSize);
            sprintf(Buffer,"  ||  #  |  =    %s(%.2f Km)",meineGefiltertenKnoten[x]->Name,meineGefiltertenKnoten[x]->AutobahnKM);
            printAutobahnRow(Buffer, TotalSize);
            printAutobahnRow("  ||  #  | /", TotalSize);
            printAutobahnRow("  ||  #  |/", TotalSize);
        }

        printAutobahnRow("  ||  #  ||", TotalSize);

    }


//ENDKNOTEN

//Wenn es nur einen Knoten Gibt wird kein Endknoten ausgegeben , es gibt ja keinen.
    if (AnzahlGefilterte>1) {

        printAutobahnRow("  \\\\  #  //", TotalSize);

        printAutobahnRow("   \\\\ # //", TotalSize);

        printAutobahnRow("    \\\\#//", TotalSize);

        printAutobahnRow("      |  ", TotalSize);

        //Letzten Knoten Ausgeben

        if(meineGefiltertenKnoten[x]->isKreuz==1){

            char *Buffer2;
            //Findet die autobahn zu der das kreuz führt
            for(int u=0;u<meineGefiltertenKnoten[x]->numWege;u++){
                if(meineGefiltertenKnoten[x]->Wege[u]->nach->ID!=meineGefiltertenKnoten[x-1]->ID){
                    Buffer2=meineGefiltertenKnoten[x]->Wege[u]->nach->AutobahnName;
                    break;
                }
            }

            sprintf(Buffer,"Kreuz %s(%.2f Km) --- %s -> %s",meineGefiltertenKnoten[x]->Name,meineGefiltertenKnoten[x]->AutobahnKM,meineGefiltertenKnoten[x]->AutobahnName,Buffer2);
            printAutobahnRow(Buffer, TotalSize);


        }else{
            //Erste Ausfahrt Ausgeben
            sprintf(Buffer,"%s(%.2f Km)",meineGefiltertenKnoten[x]->Name,meineGefiltertenKnoten[x]->AutobahnKM);
            printAutobahnRow(Buffer, TotalSize);
        }
    }

    printFooter();
    puts("\n");

    if(SearchBuffer){
        if (SortierModus != INT_MAX) {
            if (SearchBuffer->isKreuz) {

                for (int u = 0; u < SearchBuffer->numWege; u++) {
                    if (strcompCaseInsensitive(SearchBuffer->Wege[u]->nach->Name,Suchbegriff)==0) {
                        printAutobahnVisual(meineKnoten, AnzahlKnoten, SearchBuffer->Wege[u]->nach->AutobahnName, Suchbegriff , INT_MAX);
                        //Aufräumen
                        free(Buffer);
                        free(SearchBuffer);
                        return;
                    }
                }

            }
        }
    }

    //Aufräumen
    free(Buffer);
    free(SearchBuffer);
    return ;
}

void printAutobahnText(struct Knoten *meineKnoten[], int AnzahlKnoten, char *AutobahnName, char *Suchbegriff,int SortierModus) { //Gibt Autobahn in Tabllenform an , Kann sortiert werden nach name , wird ausgelöst durch ls -t , -n zum sortiern

    char *Buffer = malloc(sizeof(char) * 1000);
    struct Knoten *SearchBuffer = NULL;

    struct Knoten *meineGefiltertenKnoten[AnzahlKnoten];

    //Ausfahrten und Kreuze Errechnen
    int i;
    size_t size;
    int AnzahlGefilterte = 0;
    for (i = 0; i < AnzahlKnoten; i++) {
        if (strcmp(meineKnoten[i]->AutobahnName, AutobahnName) == 0) {

            meineGefiltertenKnoten[AnzahlGefilterte] = meineKnoten[i];
            AnzahlGefilterte++;

        }
    }

    puts("\n");
    //Überschrift Ausgeben
    if (Suchbegriff != NULL) {

        for (int x = 0; x < AnzahlGefilterte; x++) {
            if (strcompCaseInsensitive(meineGefiltertenKnoten[x]->Name, Suchbegriff) == 0) {

                sprintf(Buffer, "<<<%s>>>", meineGefiltertenKnoten[x]->Name);

                SearchBuffer = malloc(sizeof(struct Knoten));
                SearchBuffer->AutobahnName = strdup(meineGefiltertenKnoten[x]->AutobahnName);
                SearchBuffer->Name = strdup(Buffer);
                SearchBuffer->AutobahnKM = meineGefiltertenKnoten[x]->AutobahnKM;
                SearchBuffer->numWege = meineGefiltertenKnoten[x]->numWege;
                SearchBuffer->ID = meineGefiltertenKnoten[x]->ID;
                SearchBuffer->Wege[0] = meineGefiltertenKnoten[x]->Wege[0];
                SearchBuffer->Wege[1] = meineGefiltertenKnoten[x]->Wege[1];
                SearchBuffer->Wege[2] = meineGefiltertenKnoten[x]->Wege[2];
                SearchBuffer->isKreuz = meineGefiltertenKnoten[x]->isKreuz;

                meineGefiltertenKnoten[x] = SearchBuffer;//Zeiger wird umgerichtet
                sprintf(Buffer, "  %s(%.0f Km) ist auf Autobahn: %s  ", meineGefiltertenKnoten[x]->Name, meineGefiltertenKnoten[x]->AutobahnKM, AutobahnName);
            }
        }


    } else {
        sprintf(Buffer, "  Autobahn %s  ", AutobahnName);
    }

    printMenuHeader(Buffer);
    printMenuItem(" ");
    printTabelHeader(2, "  Ausfahrts/Kreuz Name  ", "  Autobahn Km  ");


    if (SortierModus == 1) {

        struct OutputBuffer *OBuffer[AnzahlGefilterte];

        for (int x = 0; x < AnzahlGefilterte; x++) {

            OBuffer[x] = malloc(sizeof(struct OutputBuffer *) * 2);


            if (meineGefiltertenKnoten[x]->isKreuz == 1) {


                char *Buffer2;
                //Findet die autobahn zu der das kreuz führt
                for (int u = 0; u < meineGefiltertenKnoten[x]->numWege; u++) {
                    if (meineGefiltertenKnoten[x]->Wege[u]->nach->ID != meineGefiltertenKnoten[x - 1]->ID && meineGefiltertenKnoten[x]->Wege[u]->nach->ID != meineGefiltertenKnoten[x + 1]->ID) {
                        Buffer2 = meineGefiltertenKnoten[x]->Wege[u]->nach->AutobahnName;
                        break;
                    }
                }

                sprintf(Buffer, "%s (%s -> %s)", meineGefiltertenKnoten[x]->Name, meineGefiltertenKnoten[x]->AutobahnName, Buffer2);
                char *TmpKM = malloc(sizeof(char) * 1000);
                sprintf(TmpKM, "%f", meineGefiltertenKnoten[x]->AutobahnKM);

                OBuffer[x]->TextName = strdup(Buffer);
                OBuffer[x]->TextKm = strdup(TmpKM);

                free(TmpKM);


            } else {

                sprintf(Buffer, "%f", meineGefiltertenKnoten[x]->AutobahnKM);

                OBuffer[x]->TextName = strdup(meineGefiltertenKnoten[x]->Name);
                OBuffer[x]->TextKm = strdup(Buffer);


            }

        }

        qsort(OBuffer, AnzahlGefilterte, sizeof(struct OutputBuffer *), QsortCompareName);

        for (int i = 0; i < AnzahlGefilterte; i++) {
            printTabelRow(2, OBuffer[i]->TextName, OBuffer[i]->TextKm);
            free(OBuffer[i]->TextKm);
            free(OBuffer[i]->TextName);
            free(OBuffer[i]);
        }


    } else {


        for (int x = 0; x < AnzahlGefilterte; x++) {


            if (meineGefiltertenKnoten[x]->isKreuz == 1) {


                char *Buffer2;
                //Findet die autobahn zu der das kreuz führt
                for (int u = 0; u < meineGefiltertenKnoten[x]->numWege; u++) {
                    if (meineGefiltertenKnoten[x]->Wege[u]->nach->ID != meineGefiltertenKnoten[x - 1]->ID && meineGefiltertenKnoten[x]->Wege[u]->nach->ID != meineGefiltertenKnoten[x + 1]->ID) {
                        Buffer2 = meineGefiltertenKnoten[x]->Wege[u]->nach->AutobahnName;
                        break;
                    }
                }

                sprintf(Buffer, "\"%s\" Kreuz von %s -> %s", meineGefiltertenKnoten[x]->Name, meineGefiltertenKnoten[x]->AutobahnName, Buffer2);
                char *TmpKM = malloc(sizeof(char) * 1000);
                sprintf(TmpKM, "%f", meineGefiltertenKnoten[x]->AutobahnKM);
                printTabelRow(2, Buffer, TmpKM);
                free(TmpKM);


            } else {

                sprintf(Buffer, "%f", meineGefiltertenKnoten[x]->AutobahnKM);
                printTabelRow(2, meineGefiltertenKnoten[x]->Name, Buffer);

            }

        }
    }

    printFooter();
    puts("\n");

    //Gibt die andere autobahn eines kreuzes aus , wenn ein kreuz abgefragt wurde wird auch die zielautogahn auf die das kreuz führt angezeigt
    if (SearchBuffer) {
        if (SortierModus != INT_MAX) {
            if (SearchBuffer->isKreuz) {

                for (int u = 0; u < SearchBuffer->numWege; u++) {
                    if (strcompCaseInsensitive(SearchBuffer->Wege[u]->nach->Name, Suchbegriff) == 0) {
                        printAutobahnText(meineKnoten, AnzahlKnoten, SearchBuffer->Wege[u]->nach->AutobahnName, Suchbegriff, INT_MAX);
                        //Aufräumen
                        free(Buffer);
                        free(SearchBuffer);
                        return;
                    }
                }

            }
        }
    }

    //Aufräumen
    free(Buffer);
    free(SearchBuffer);


}