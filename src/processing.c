// A C / C++ program for Dijkstra's single source shortest path algorithm.
// The program is for adjacency matrix representation of the graph

#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "processing.h"
#include "../lib/printui/printui.h"


//Anzahl der Knoten
int AnzahlKnoten = 0;

double my_round(double x, unsigned int digits) {
    if (digits > 0) {
        return my_round(x*10.0, digits-1)/10.0;
    }
    else {
        return x;
    }
}

double gibWegLaenge(struct Knoten *meineKnoten[],int u,int v){

    int i=0;
    while(i < meineKnoten[u]->numWege){


        if(meineKnoten[u]->Wege[i]->nach->ID==meineKnoten[v]->ID){
            double ret = meineKnoten[u]->Wege[i]->laenge;
            return ret;
        }

        i++;

    }

    return 0;
}



//Errechnet den am kürzesten entfernten nächsten knoten aus allen noch nicht besuchten knoten.
//Wählt somit den "Weg" dergegangen wird.
double minDistance(struct Knoten *meineKnoten[])
{
    // Initialize min value
    double min = INT_MAX, min_index ;

    for (int v = 0; v < AnzahlKnoten; v++) {
        if (meineKnoten[v]->besucht == false && meineKnoten[v]->entfernungZumUrsprung <= min) {
            min = meineKnoten[v]->entfernungZumUrsprung;
            min_index = v;
        }

    }

    return min_index;
}


//Hier wird der Algorythmus gestartet und die gegangenen Wege Gespeichert.
void dijkstra(struct Knoten *meineKnoten[], int src)
{


    //Entfernung = 0 da der startknoten zu sich selbst nicht entfernt ist
    meineKnoten[src]->entfernungZumUrsprung = 0;

    // Find shortest path for all vertices (V-1 Möglich da der letzte wert automatisch fest steht , aber nicht gemacht da sonst der letzte wert nicht ausgegeben wird)
    for (int count = 0; count < AnzahlKnoten-1; count++)
    {
        int u=0;

        //U = immer src beim ersten mal , weitere info siehe funktion
        u = minDistance(meineKnoten);

        // Mark the picked vertex as processed
        meineKnoten[u]->besucht = true;



        // Update dist value of the adjacent vertices of the picked vertex.
        for (int v = 0; v < AnzahlKnoten; v++) {

            // Update dist[v] only if is not in sptSet, there is an edge from
            // u to v, and total weight of path from src to  v through u is
            // smaller than current value of dist[v]

            if(!meineKnoten[v]->besucht && gibWegLaenge(meineKnoten,u,v) && meineKnoten[u]->entfernungZumUrsprung != INT_MAX
                    && meineKnoten[u]->entfernungZumUrsprung + gibWegLaenge(meineKnoten,u,v) < meineKnoten[v]->entfernungZumUrsprung){
                meineKnoten[v]->entfernungZumUrsprung = meineKnoten[u]->entfernungZumUrsprung + gibWegLaenge(meineKnoten,u,v);
                meineKnoten[v]->knotenZurueck=meineKnoten[u];
            }


        }



    }



}


// Gibt Alle möglichen Ziele mit gegangenem Weg aus
int printPathToTarget(struct Knoten *meineKnoten[],int StartKnoten,int Endknoten)
{

    char* BewegungsArray[AnzahlKnoten];

    char* buffer = malloc(sizeof(char)*200);

    for (int i = 0; i < AnzahlKnoten; i++) {
        if (meineKnoten[i]->ID == Endknoten) {

            int v = i;
            int AnzahlBewegungen = 0;


            while (meineKnoten[v]->knotenZurueck->ID >0) {



                if (gibWegLaenge(meineKnoten, v, meineKnoten[v]->knotenZurueck->ID)) {//TODO Zeile geht nicht , Kreuzüberfahrten solllten nicht anzeigen werden -> distanz 0km ausbelnden
                    BewegungsArray[AnzahlBewegungen * 3] = malloc(sizeof(char)*(countUTF8String(meineKnoten[v]->knotenZurueck->Name)+1));
                    BewegungsArray[AnzahlBewegungen * 3 + 1] = malloc(sizeof(char)*(countUTF8String(buffer)+1));
                    BewegungsArray[AnzahlBewegungen * 3 + 2] = malloc(sizeof(char)*(countUTF8String(meineKnoten[v]->Name)+1));

                    sprintf(buffer, "------(%4.2f Km)----->", gibWegLaenge(meineKnoten, v, meineKnoten[v]->knotenZurueck->ID));

                    memmove(BewegungsArray[AnzahlBewegungen * 3], meineKnoten[v]->knotenZurueck->Name, countUTF8String(meineKnoten[v]->knotenZurueck->Name)+1);
                    memmove(BewegungsArray[AnzahlBewegungen * 3 + 1], buffer, countUTF8String(buffer)+1);
                    memmove(BewegungsArray[AnzahlBewegungen * 3 + 2], meineKnoten[v]->Name, countUTF8String(meineKnoten[v]->Name)+1);

                    v = meineKnoten[v]->knotenZurueck->ID;
                    AnzahlBewegungen++;
                }

            }



            if (meineKnoten[i]->ID != meineKnoten[StartKnoten]->ID) {
                if (meineKnoten[i]->entfernungZumUrsprung == INT_MAX) {
                    puts("\n");
                    sprintf(buffer, "\"%s\" ist von \"%s\" aus nicht Erreichbar", meineKnoten[i]->Name, meineKnoten[StartKnoten]->Name);
                    printMenuHeader(buffer);
                    puts("\n");
                } else {
                    puts("\n");
                    sprintf(buffer, "Weg von \"%s\" nach \"%s\" ", meineKnoten[StartKnoten]->Name, meineKnoten[i]->Name);
                    printMenuHeader(buffer);
                    printMenuItem("");

                    printTabelHeader(3, "Von", "Strecke", "Nach");
                    for (int x = AnzahlBewegungen - 1; x >= 0; x--) {
                        printTabelRow(3, BewegungsArray[x * 3], BewegungsArray[x * 3 + 1], BewegungsArray[x * 3 + 2]);
                    }

                    sprintf(buffer, "  Gesamt: %0.2f ", meineKnoten[i]->entfernungZumUrsprung);
                    printMenuHeader(buffer);
                    puts("\n");
                }


            }

            break;
        }


    }

    return 1;
}

//Finden und ausgabe des weges zwischen a und b
int findeWeg(struct Knoten *meineKnoten[],int AnzahlNodes,int StartKnoten,int ZielKnoten)
{

    //Anzahl an Knoten wird festgelegt
    AnzahlKnoten=AnzahlNodes;


    //Werte werden Initialisiert im KnotenArray , Dies muss vor dem erstellen der wege passieren da Numwege initialisiert wird.
    for (int i = 0; i < AnzahlKnoten; i++){
        meineKnoten[i]->entfernungZumUrsprung = INT_MAX , meineKnoten[i]->besucht = false ;
    }


    //Algorythmus wird gestartet
    dijkstra(meineKnoten, StartKnoten);

    //TODO Ausgaben schreiben , mit printui tabellen
    printPathToTarget(meineKnoten,StartKnoten,ZielKnoten);


    return 1;
}
