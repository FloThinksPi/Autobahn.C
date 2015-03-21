/*
 * File:   dbio.h
 * Author: flo
 *
 * Created on 22. Januar 2015, 9:01
 */

#ifndef DBIO_H
#define DBIO_H

#include <stdbool.h>

struct OutputBuffer
{
    char *TextName;
    char *TextKm;
};

struct Knoten {
    int ID;
    double AutobahnKM;
    char *AutobahnName;
    char *Name;
    int isKreuz;
    bool besucht;
    struct Knoten *knotenZurueck;
    double entfernungZumUrsprung;
    int numWege;
    struct Wege *Wege[3];//Maximal 3 Wege sind möglich (2 bei Ausfahrten und 3 bei kreuzen , da sich die kreuze.csv von 2 autobahnen mit einer kante der länge 0 verbinden)
};
struct Wege {
    struct Knoten *nach;
    double laenge;
};

//FileIO
int loadDatabaseFiletoStruct(struct Knoten*[],int AnzahlKnoten);
void saveStructToFile(struct Knoten *meineKnoten[],int AnzahlKnoten);
int getNumKnoten();

//Daten Suche
int findeKnotenByName(struct Knoten *meineKnoten[],int AnzahlKnoten,char *KnotenName,int FindSimmilar);
char**GetAutobahnen(struct Knoten *meineKnoten[], int AnzahlKnoten);

//Daten Verarbeitung
void ConnectData(struct Knoten *meineKnoten[],int AnzahlKnoten);
void OnlyConnectKreuze(struct Knoten *meineKnoten[],int AnzahlKnoten);
void OnlyREConnectKreuze(struct Knoten *meineKnoten[],int AnzahlKnoten);//Nur bereits existente Knoten Werden verknüpft
void OnlyConnectEinKreuz(struct Knoten *meineKnoten[],int AnzahlKnoten,char *ZeilKreuz);
void OnlyConnectEineAutobahn(struct Knoten *meineKnoten[],int AnzahlKnoten,char *ZeilAutobahn);

//Datei Visualisierung
void printAutobahnVisual(struct Knoten *meineKnoten[], int AnzahlKnoten, char *AutobahnName, char *Suchbegriff,int SortierModus);
void printAutobahnText(struct Knoten *meineKnoten[], int AnzahlKnoten, char *AutobahnName, char *Suchbegriff,int SortierModus);

//BearbeitungsFunktionen
int DeleteKnoten(struct Knoten *meineKnoten[], int Anzahlknoten, int AusfahrtIDinStruct);//muss beim löschen von Kreuzen 2 mal Aufgerufen und löscht damit beide enden eines kreuzes
int NewKnoten(struct Knoten *meineKnoten[],int AnzahlKnoten,char* Name,char* AutobahnName,double AutobahnKM);

#endif