//
// Created by Florian Braun on 20.03.2015.
//

#ifndef _AUTOBAHN_C_ACTIONS_H_
#define _AUTOBAHN_C_ACTIONS_H_

#include "dbio.h"
#include "main.h"

int Nav(struct Knoten *meineKnoten[], int AnzahlKnoten ,char *K1 ,char *K2); //Findet Weg von 2 Knoten (K1 und K2), überprüft ob die übergebenen Knoten Valid sind. löst dann den Deijkstra aus.

int SearchNormal(struct Knoten *meineKnoten[],int AnzahlKnoten ,char *K1,int textonly,int sortName); //Gibt Autobahn aus oder Autobahn auf der ein Gewünschter Knoten liegt , kann somit nach Knoten und Autobahnen suchen.
int SearchAutobahnen(struct Knoten *meineKnoten[],int AnzahlKnoten); // Gibt alle Autobahnen aus.

int Remove(struct UndefArrayHack *ArrayHack,int AnzahlKnoten,char *K1);
int NewAusfahrt(struct UndefArrayHack *ArrayHack,int AnzahlKnoten,char *Kname,char *Aname,char *Akm,int DoubleNameAllowed);
int NewKreuz(struct UndefArrayHack *ArrayHack,int AnzahlKnoten,char *Kname,char *Aname1,char *Akm1,char *Aname2,char *Akm2);

int EditName(struct UndefArrayHack *ArrayHack,int AnzahlKnoten,char *Name,char *NeuName);
int EditKreuz(struct UndefArrayHack *ArrayHack,int AnzahlKnoten,char *Kname,char *Aname1,char *Akm1,char *Aname2,char *Akm2);
int EditAusfahrt(struct UndefArrayHack *ArrayHack,int AnzahlKnoten,char *Kname,char *Aname,char *Akm);

//Extras
int isNewInValid(struct UndefArrayHack *ArrayHack,int AnzahlKnoten,char *Kname,char *Aname,char *Akm,int DoubleNameAllowed);//Zeigt ob New von seiten Konflikte mit anderen Datensätzen aus funktioniert

#endif //_AUTOBAHN_C_ACTIONS_H_
