/*
 * File:   dbio.h
 * Author: flo
 *
 * Created on 22. Januar 2015, 9:01
 */

#ifndef DBIO_H
#define DBIO_H

#include "processing.h"

//FileIO
int loadDatabaseFiletoStruct(struct Knoten*[],int AnzahlKnoten);

//Datei Suche/Processing
int findeKnotenByName(struct Knoten *meineKnoten[],int AnzahlKnoten,char *KnotenName,int FindSimmilar);
char** loadAutobahnen(struct Knoten *meineKnoten[],int AnzahlKnoten);
int getNumKnoten();
void ConnectData(struct Knoten *meineKnoten[],int AnzahlKnoten);

//Datei Visualisierung
void printAutobahnVisual(struct Knoten *meineKnoten[], int AnzahlKnoten, char *AutobahnName, char *Suchbegriff,int SortierModus);
void printAutobahnText(struct Knoten *meineKnoten[], int AnzahlKnoten, char *AutobahnName, char *Suchbegriff,int SortierModus);

//BearbeitungsFunktionen
int DeleteKnoten(struct Knoten *meineKnoten[], int Anzahlknoten, int AusfahrtIDinStruct);//muss beim löschen von Kreuzen 2 mal Aufgerufen und löscht damit beide enden eines kreuzes
int NewKnoten(struct Knoten *meineKnoten[],int AnzahlKnoten,char* Name,char* AutobahnName,double AutobahnKM);

#endif