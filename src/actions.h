//
// Created by Florian Braun on 20.03.2015.
//

#ifndef _AUTOBAHN_C_ACTIONS_H_
#define _AUTOBAHN_C_ACTIONS_H_

#include "dbio.h"
#include "main.h"

//Abfragen
int Nav(struct Knoten *meineKnoten, int AnzahlKnoten ,char *K1 ,char *K2); //Findet Weg von 2 Knoten (K1 und K2), �berpr�ft ob die �bergebenen Knoten Valid sind. l�st dann den Deijkstra aus.
int SearchNormal(struct Knoten *meineKnoten,int AnzahlKnoten ,char *K1,int textonly,int sortName); //Gibt Autobahn aus oder Autobahn auf der ein Gew�nschter Knoten liegt , kann somit nach Knoten und Autobahnen suchen.
int SearchAutobahnen(struct Knoten *meineKnoten,int AnzahlKnoten); // Gibt alle Autobahnen aus.

//NEW
int NewAusfahrt(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname,char *Akm,int DoubleNameAllowed);//Erstellt Neue ausfahrt
int NewKreuz(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname1,char *Akm1,char *Aname2,char *Akm2);//Erstellt Neues Kreuz

//EDIT
int EditName(struct Knoten *meineKnoten,int AnzahlKnoten,char *Name,char *NeuName);//Aendert Name von Kreuz/Autobahn/Aussfahrt
int EditAusfahrt(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname,char *Akm);//Editiert Ausfahrt
int EditKreuz(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname1,char *Akm1,char *Aname2,char *Akm2);//Editiert Kreuz

//Remove
int Remove(struct Knoten *meineKnoten,int AnzahlKnoten,char *K1);// Loscht Ausfahrt/Kreuz/Autobahn

//Extras
int isNewInValid(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname,char *Akm,int DoubleNameAllowed);//Zeigt ob New von seiten Konflikte mit anderen Datens�tzen aus funktioniert

#endif //_AUTOBAHN_C_ACTIONS_H_
