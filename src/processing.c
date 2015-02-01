// A C / C++ program for Dijkstra's single source shortest path algorithm.
// The program is for adjacency matrix representation of the graph

#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include "processing.h"



//Anzahl der Knoten
int AnzahlKnoten = NULL;



double gibWegLaenge(struct Knoten *meineKnoten[],int u,int v){

    int i=0;
    while(i < meineKnoten[u]->numWege){


        if(meineKnoten[u]->Wege[i]->nach==meineKnoten[v]){
            return meineKnoten[u]->Wege[i]->laenge;
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
        int u=NULL;

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
int printAllPaths(struct Knoten *meineKnoten[],int StartKnoten)
{
    for (int i = 0; i < AnzahlKnoten; i++){
        int v=i;
        char Verlauf[AnzahlKnoten*500];//TODO AUSGABE MIT PRINTUI LIB , Verlauf muss anderst speicher belegen!

        while(meineKnoten[v]->knotenZurueck->Name!=NULL){



            sprintf(Verlauf,"%s | %s -> %s (%.2f)",Verlauf,meineKnoten[v]->knotenZurueck->Name,meineKnoten[v]->Name, gibWegLaenge(meineKnoten, v, meineKnoten[v]->knotenZurueck->ID));
            v=meineKnoten[v]->knotenZurueck->ID;




        }

        printf("%s",Verlauf);


        if(meineKnoten[i]->ID!=meineKnoten[StartKnoten]->ID){
            if(meineKnoten[i]->entfernungZumUrsprung== INT_MAX){
                printf("\"%s\" ist nicht Erreichbar\n",meineKnoten[i]->Name);
            }else {
                printf("  Gesamt: %.2f \n",meineKnoten[i]->entfernungZumUrsprung);
            }
        }



    }
}

//Finden und ausgabe des weges zwischen a und b
int findeWeg(struct Knoten *meineKnoten[],int AnzahlNodes,int StartKnoten,int ZielKnoten)//TODO PARAMETER Verarbeitung und verknüpfung mit menu , Dateneingabe muss in ladedaten();
{

    //Anzahl an Knoten wird festgelegt
    AnzahlKnoten=AnzahlNodes;



    //Werte werden Initialisiert im KnotenArray , Dies muss vor dem erstellen der wege passieren da Numwege initialisiert wird.
    for (int i = 0; i < AnzahlKnoten; i++){
        meineKnoten[i]->entfernungZumUrsprung = INT_MAX , meineKnoten[i]->besucht = false ;
        meineKnoten[i]->knotenZurueck= malloc(sizeof(struct Knoten));
    }



    //Algorythmus wird gestartet
    dijkstra(meineKnoten, StartKnoten);

    //TODO Ausgaben schreiben , mit printui tabellen
    printAllPaths(meineKnoten,StartKnoten);



    return 1;
}
