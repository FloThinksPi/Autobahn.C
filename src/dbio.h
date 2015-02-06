/*
 * File:   dbio.h
 * Author: flo
 *
 * Created on 22. Januar 2015, 9:01
 */

#ifndef DBIO_H
#define DBIO_H

#include "processing.h"

int loadDatabaseFiletoStruct(struct Knoten*[],int AnzahlKnoten);
int findeKnotenByName(struct Knoten *meineKnoten[],int AnzahlKnoten,char *KnotenName);
char** loadAutobahnen(struct Knoten *meineKnoten[],int AnzahlKnoten);
int strcompCaseInsensitive(char const *a, char const *b);

int getNumKnoten();


void printAutobahn(struct Knoten *meineKnoten[],int AnzahlKnoten,char *AutobahnName,char *Suchbegriff);

#endif