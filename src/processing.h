/*
 * File:   processing.h
 * Author: flo
 *
 * Created on 22. Januar 2015, 9:01
 */

#ifndef PROCESSING_H
#define PROCESSING_H

#include <stdbool.h>

struct Knoten {
    int ID;
    double AutobahnKM;
    char *AutobahnName;
    char *Name;
    bool besucht;
    struct Knoten *knotenZurueck;
    double entfernungZumUrsprung;
    int numWege;
    struct Wege *Wege[3];//MAximal 3 Wege sind möglich (2 bei Ausfahrten und 3 bei kreuzen , da sich die kreuze.csv von 2 autobahnen mit einer kante der länge 0 verbinden)
};

struct Wege {
    struct Knoten *nach;
    double laenge;
};

int findeWeg(struct Knoten *meineKnoten[],int AnzahlNodes,int StartKnoten,int ZielKnoten);

#endif
