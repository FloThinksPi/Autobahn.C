// A C / C++ program for Dijkstra's single source shortest path algorithm.
// The program is for adjacency matrix representation of the graph

#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>



//Anzahl der Knoten
int AnzahlKnoten = NULL;


struct Knoten {
    int ID;
    char *Name;
    bool besucht;
    struct Knoten *knotenZurueck;
    double entfernungZumUrsprung;
    int numWege;
    struct Wege *Wege[20];
};

struct Wege {
    struct Knoten *von; //TODO von nicht nötig da info in dem vorhandensein des eintrags beim knoten steckt -> löschen
    struct Knoten *nach;
    double laenge;
};


void ErstelleWegBidirektional(struct Knoten *meineKnoten[],int Knoten1,int Knoten2,int leange){

    int numWegezuKnoten1 = meineKnoten[Knoten1]->numWege;
    int numWegezuKnoten2 = meineKnoten[Knoten2]->numWege;

    //TODO Püfen ob weg schon existiert

    struct Wege *W1 =  malloc(sizeof(struct Wege));
    struct Wege *W2 =  malloc(sizeof(struct Wege));

    meineKnoten[Knoten1]->Wege[numWegezuKnoten1]=W1;
    meineKnoten[Knoten2]->Wege[numWegezuKnoten2]=W2;

    meineKnoten[Knoten1]->Wege[numWegezuKnoten1]->von=meineKnoten[Knoten1];
    meineKnoten[Knoten1]->Wege[numWegezuKnoten1]->nach=meineKnoten[Knoten2];
    meineKnoten[Knoten1]->Wege[numWegezuKnoten1]->laenge=leange;

    meineKnoten[Knoten2]->Wege[numWegezuKnoten2]->von=meineKnoten[Knoten2];
    meineKnoten[Knoten2]->Wege[numWegezuKnoten2]->nach=meineKnoten[Knoten1];
    meineKnoten[Knoten2]->Wege[numWegezuKnoten2]->laenge=leange;

    meineKnoten[Knoten1]->numWege++;
    meineKnoten[Knoten2]->numWege++;


}

void setzeKnotenName(struct Knoten *meineKnoten[], int KnotenNummer, char *KnotenName){


    struct Knoten *K =  malloc(sizeof(struct Knoten));

    meineKnoten[KnotenNummer]=K;
    meineKnoten[KnotenNummer]->Name=KnotenName;
    meineKnoten[KnotenNummer]->ID=KnotenNummer;


}


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
        char *Verlauf[AnzahlKnoten];
        int CountWays=0;
        while(meineKnoten[v]->knotenZurueck->Name!=NULL){

            char *buffer=malloc(sizeof(LONG_MAX));

            sprintf(buffer,"%s -> %s (%.2f)",meineKnoten[v]->knotenZurueck->Name,meineKnoten[v]->Name, gibWegLaenge(meineKnoten, v, meineKnoten[v]->knotenZurueck->ID));
            v=meineKnoten[v]->knotenZurueck->ID;
            Verlauf[CountWays]= malloc(sizeof(LONG_MAX));
            Verlauf[CountWays]=buffer;

            CountWays++;

        }

        for(int u=CountWays;u>0;u--){
                printf("%s | ",Verlauf[u-1]);
        }

        if(meineKnoten[i]->ID!=meineKnoten[StartKnoten]->ID){
            if(meineKnoten[i]->entfernungZumUrsprung== INT_MAX){
                printf("\"%s\" ist nicht Erreichbar\n",meineKnoten[i]->Name);
            }else {
                printf("  Gesamt: %.2f \n",meineKnoten[i]->entfernungZumUrsprung);
            }
        }



    }
}


void ladeDaten();//TODO Hier wird von dbio die daten geholt und in das struct geschrieben , wird schon beim öffnen des menus ausgeführt.

int start()//TODO PARAMETER Verarbeitung und verknüpfung mit menu , Dateneingabe muss in ladedaten();
{

    //Anzahl an Knoten wird festgelegt
    AnzahlKnoten=10;
    //Lege Startknoten Fest
    int StartKnoten=5;

    //Ein array mit "AnzahlKnoten" einträgen , aus Knoten wird erstellt und somit speicher zugewiesen
    struct Knoten *meineKnoten[AnzahlKnoten];


    //TESTDATEN VON http://www.geeksforgeeks.org/greedy-algorithms-set-6-dijkstras-shortest-path-algorithm/

    //Es müssen "AnzahlKnoten" mal Knoten erstellt werden sonst kommt es zum Speicherzugriffsfehler parameter(PointerZumKnotenarray,int ID des Knotens, Name des Knotens)
    setzeKnotenName(meineKnoten, 0, "Null");
    setzeKnotenName(meineKnoten, 1, "Eins");
    setzeKnotenName(meineKnoten, 2, "Zwei");
    setzeKnotenName(meineKnoten, 3, "Drei");
    setzeKnotenName(meineKnoten, 4, "Vier");
    setzeKnotenName(meineKnoten, 5, "Fünf");
    setzeKnotenName(meineKnoten, 6, "Sechs");
    setzeKnotenName(meineKnoten, 7, "Sieben");
    setzeKnotenName(meineKnoten, 8, "Acht");
    setzeKnotenName(meineKnoten, 9, "N/R Knoten");

    //Werte werden Initialisiert im KnotenArray , Dies muss vor dem erstellen der wege passieren da Numwege initialisiert wird.
    for (int i = 0; i < AnzahlKnoten; i++){
        meineKnoten[i]->entfernungZumUrsprung = INT_MAX , meineKnoten[i]->besucht = false ,meineKnoten[i]->numWege=0;
        meineKnoten[i]->knotenZurueck= malloc(sizeof(struct Knoten));
    }

    //Hier werden die verbindungen zwischen den knoten erstellt , sie sind immmer bidirektional d.h. in beide richtungen ,die reihenfolge der knoten ist somit egal.
    //parameter(PointerZumKnotenarray,int ID des ersten Knotens,int ID des zweiten Knotens,entfernung zwischen den knoten)
    ErstelleWegBidirektional(meineKnoten,0,1,4);
    ErstelleWegBidirektional(meineKnoten,0,7,8);
    ErstelleWegBidirektional(meineKnoten,1,7,11);
    ErstelleWegBidirektional(meineKnoten,1,2,8);
    ErstelleWegBidirektional(meineKnoten,2,8,2);
    ErstelleWegBidirektional(meineKnoten,2,3,7);
    ErstelleWegBidirektional(meineKnoten,2,5,4);
    ErstelleWegBidirektional(meineKnoten,3,4,9);
    ErstelleWegBidirektional(meineKnoten,3,5,14);
    ErstelleWegBidirektional(meineKnoten,4,5,10);
    ErstelleWegBidirektional(meineKnoten,5,6,2);
    ErstelleWegBidirektional(meineKnoten,6,8,6);
    ErstelleWegBidirektional(meineKnoten,6,7,1);
    ErstelleWegBidirektional(meineKnoten,7,8,7);

    //Algorythmus wird gestartet
    dijkstra(meineKnoten, StartKnoten);

    //TODO Ausgaben schreiben , mit printui tabellen
    printAllPaths(meineKnoten,StartKnoten);

    return 0;
}