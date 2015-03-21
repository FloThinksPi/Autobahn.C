// A C / C++ program for Dijkstra's single source shortest path algorithm.
// The program is for adjacency matrix representation of the graph

#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "processing.h"
#include "../lib/printui/printui.h"
#include "utils.h"
#include "dbio.h"


//Anzahl der Knoten
int AnzahlKnoten = 0;

double gibWegLaenge(struct Knoten *meineKnoten[],int u,int v){

    int i=0;
    while(i < meineKnoten[u]->numWege){


        if(meineKnoten[u]->Wege[i]->nach->ID==meineKnoten[v]->ID){
            return meineKnoten[u]->Wege[i]->laenge;
        }

        i++;

    }

    return 0;
}



//Errechnet den am kürzesten entfernten nächsten knoten aus allen noch nicht besuchten knoten.
//Wählt somit den "Weg" dergegangen wird.
int minDistance(struct Knoten *meineKnoten[])
{
    // Initialize min value
    double min = INT_MAX;
    int min_index = 0;

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
int printPathToTarget(struct Knoten *meineKnoten[],int StartKnoten,int Endknoten,double StartZeit)
{

    char* BewegungsArray[AnzahlKnoten];

    char* buffer = malloc(sizeof(char)*200);


    for (int i = 0; i < AnzahlKnoten; i++) {
        if (meineKnoten[i]->ID == Endknoten) {

            int v = i;
            int AnzahlBewegungen = 0;


            while (meineKnoten[v]->knotenZurueck!=NULL) {

                if (gibWegLaenge(meineKnoten, v, meineKnoten[v]->knotenZurueck->ID)>0.001) {

                    sprintf(buffer, "------(%4.2f Km)----->", gibWegLaenge(meineKnoten, v, meineKnoten[v]->knotenZurueck->ID));

                    BewegungsArray[AnzahlBewegungen * 3]=strdup(meineKnoten[v]->knotenZurueck->Name);
                    BewegungsArray[AnzahlBewegungen * 3 + 1]=strdup(buffer);
                    BewegungsArray[AnzahlBewegungen * 3 + 2]=strdup(meineKnoten[v]->Name);

                    v = meineKnoten[v]->knotenZurueck->ID;
                    AnzahlBewegungen++;
                }else{
                    v = meineKnoten[v]->knotenZurueck->ID;
                }


            }



            if (meineKnoten[i]->ID != meineKnoten[StartKnoten]->ID) {
                if (meineKnoten[i]->entfernungZumUrsprung == INT_MAX) {
                    puts("\n");
                    sprintf(buffer, "  \"%s\" ist von \"%s\" aus nicht Erreichbar  ", meineKnoten[i]->Name, meineKnoten[StartKnoten]->Name);
                    printMenuHeaderContinous(buffer);
                    puts("\n");
                } else {
                    puts("\n");
                    sprintf(buffer, "  Weg von \"%s\" nach \"%s\"  ", meineKnoten[StartKnoten]->Name, meineKnoten[i]->Name);
                    printMenuHeader(buffer);
                    printMenuItem("");

                    printTabelHeader(3, " Von ", " Strecke ", " Nach ");
                    for (int x = AnzahlBewegungen - 1; x >= 0; x--) {
                        printTabelRow(3, BewegungsArray[x * 3], BewegungsArray[x * 3 + 1], BewegungsArray[x * 3 + 2]);
                    }

                    sprintf(buffer, " | Gesamt: %0.2f Km | Über %d Knoten | Berechnet in %f Sekunden | ", meineKnoten[i]->entfernungZumUrsprung,AnzahlBewegungen-1, get_time()-StartZeit);
                    printFooterText(buffer);
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

    double StartZeit = get_time();

    //Anzahl an Knoten wird festgelegt
    AnzahlKnoten=AnzahlNodes;


    //Werte werden Initialisiert im KnotenArray , Dies muss vor dem erstellen der wege passieren da Numwege initialisiert wird.
    for (int i = 0; i < AnzahlKnoten; i++){
        meineKnoten[i]->entfernungZumUrsprung = INT_MAX , meineKnoten[i]->besucht = false ,  meineKnoten[i]->knotenZurueck=NULL;
    }


    //Algorythmus wird gestartet
    dijkstra(meineKnoten, StartKnoten);

    //Ausgabe der Wege in einer Tabelle
    printPathToTarget(meineKnoten,StartKnoten,ZielKnoten,StartZeit);


    return 1;
}
