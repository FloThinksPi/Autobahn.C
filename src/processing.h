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
    int AID;
    char *AutobahnName;
    char *Name;
    bool besucht;
    struct Knoten *knotenZurueck;
    double entfernungZumUrsprung;
    int numWege;
    struct Wege *Wege[20];
};

struct Wege {
    struct Knoten *nach;
    double laenge;
};

int findeWeg(int StartKnoten,int ZielKnoten);

#endif
