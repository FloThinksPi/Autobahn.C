// A C / C++ program for Dijkstra's single source shortest path algorithm.
// The program is for adjacency matrix representation of the graph

#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

// Number of vertices in the graph
#define AnzahlKnoten 4


struct Karte {
    char placeholder;
    struct Knoten *Knoten[AnzahlKnoten];//TODO Ohne Präprozessor arbeiten , alles zu pointern machen
};

struct Knoten {
    char *Name;
    bool besucht;
    double entfernungZumUrsprung;
    int numWege;
    struct Wege *Wege[20];
};

struct Wege {
    struct Knoten *von;
    struct Knoten *nach;
    double laenge;
};


struct Karte ErstelleWegBidirektional(struct Karte meineKarte,int Knoten1,int Knoten2,int leange){

    int numWegezuKnoten1 = meineKarte.Knoten[Knoten1]->numWege;
    int numWegezuKnoten2 = meineKarte.Knoten[Knoten2]->numWege;

    //TODO Püfen ob weg schon existiert

    struct Wege *W1 =  malloc(sizeof(struct Wege));
    struct Wege *W2 =  malloc(sizeof(struct Wege));

    meineKarte.Knoten[Knoten1]->Wege[numWegezuKnoten1]=W1;
    meineKarte.Knoten[Knoten2]->Wege[numWegezuKnoten2]=W2;

    meineKarte.Knoten[Knoten1]->Wege[numWegezuKnoten1]->von=meineKarte.Knoten[Knoten1];
    meineKarte.Knoten[Knoten1]->Wege[numWegezuKnoten1]->nach=meineKarte.Knoten[Knoten2];
    meineKarte.Knoten[Knoten1]->Wege[numWegezuKnoten1]->laenge=leange;

    meineKarte.Knoten[Knoten2]->Wege[numWegezuKnoten2]->von=meineKarte.Knoten[Knoten2];
    meineKarte.Knoten[Knoten2]->Wege[numWegezuKnoten2]->nach=meineKarte.Knoten[Knoten1];
    meineKarte.Knoten[Knoten2]->Wege[numWegezuKnoten2]->laenge=leange;

    meineKarte.Knoten[Knoten1]->numWege++;
    meineKarte.Knoten[Knoten2]->numWege++;

    return meineKarte;
}

struct Karte setzeKnotenName(struct Karte meineKarte, int KnotenNummer, char *KnotenName){


    struct Knoten *K =  malloc(sizeof(struct Knoten));

    meineKarte.Knoten[KnotenNummer]=K;
    meineKarte.Knoten[KnotenNummer]->Name=KnotenName;

    return meineKarte;
}


double gibWegLaenge(struct Karte meineKarte,int u,int v){

    int i=0;
    while(i < meineKarte.Knoten[u]->numWege){




        if(meineKarte.Knoten[u]->Wege[i]->nach==meineKarte.Knoten[v]){
            return meineKarte.Knoten[u]->Wege[i]->laenge;
        }

        i++;

    }

    return 0;
}


// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
double minDistance(struct Karte meineKarte)
{
    // Initialize min value
    double min = INT_MAX, min_index ;

    for (int v = 0; v < AnzahlKnoten; v++) {
        if (meineKarte.Knoten[v]->besucht == false && meineKarte.Knoten[v]->entfernungZumUrsprung <= min) {
            min = meineKarte.Knoten[v]->entfernungZumUrsprung;
            min_index = v;
        }

    }


    //printf("%1.0f-->%1.0f(%1.0f)\n",keepPreeviousNode,min_index,min);
    return min_index;
}

// A utility function to print the constructed distance array
int printSolution(struct Karte meineKarte)
{
    printf("Knoten   Distance from Source\n");
    for (int i = 0; i < AnzahlKnoten; i++)
        printf("%s \t\t %f\n", meineKarte.Knoten[i]->Name, meineKarte.Knoten[i]->entfernungZumUrsprung);
}

// Funtion that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
void dijkstra(struct Karte meineKarte, int src)
{


    // Distance of source vertex from itself is always 0
    meineKarte.Knoten[src]->entfernungZumUrsprung = 0;

    // Find shortest path for all vertices (V-1 Möglich da der letzte wert automatisch fest steht , aber nicht gemacht da sonst der letzte wert nicht ausgegeben wird)
    for (int count = 0; count < AnzahlKnoten; count++)
    {
        int u=NULL;

        // Pick the minimum distance vertex from the set of vertices not
        // yet processed. u is always equal to src in first iteration.
        u = minDistance(meineKarte);

        // Mark the picked vertex as processed
        meineKarte.Knoten[u]->besucht = true;



        // Update dist value of the adjacent vertices of the picked vertex.
        for (int v = 0; v < AnzahlKnoten; v++) {

            // Update dist[v] only if is not in sptSet, there is an edge from
            // u to v, and total weight of path from src to  v through u is
            // smaller than current value of dist[v]

            if(!meineKarte.Knoten[v]->besucht && gibWegLaenge(meineKarte,u,v) && meineKarte.Knoten[u]->entfernungZumUrsprung != INT_MAX
                    && meineKarte.Knoten[u]->entfernungZumUrsprung + gibWegLaenge(meineKarte,u,v) < meineKarte.Knoten[v]->entfernungZumUrsprung){
                meineKarte.Knoten[v]->entfernungZumUrsprung = meineKarte.Knoten[u]->entfernungZumUrsprung + gibWegLaenge(meineKarte,u,v);
            }


        }



    }

    //TODO print the constructed distance array

    printSolution(meineKarte);

}

// driver program to test above function
int start()
{

    //Karte mit Knoten und Wegen wird geladen

    struct Karte meineKarte;

    //Speicher für die Knoten Belegen
    for (int i = 0; i < AnzahlKnoten; i++){
       void *K = malloc(sizeof(struct Knoten));
        meineKarte.Knoten[i]=K;
    }


    meineKarte = setzeKnotenName(meineKarte, 0, "Eins");
    meineKarte = setzeKnotenName(meineKarte, 1, "Zwei");
    meineKarte = setzeKnotenName(meineKarte, 2, "Drei");
    meineKarte = setzeKnotenName(meineKarte, 3, "Vier");

    //restlichen Werte werden definiert (UNBEDINGT BEVOR WEGE VERKNÜPFT WERDEN DALLE NUMWEGE =0 GESETZT WERDEN
    for (int i = 0; i < AnzahlKnoten; i++){
        meineKarte.Knoten[i]->entfernungZumUrsprung = INT_MAX , meineKarte.Knoten[i]->besucht = false ,meineKarte.Knoten[i]->numWege=0;
    }



    meineKarte=ErstelleWegBidirektional(meineKarte,0,1,4);
    meineKarte=ErstelleWegBidirektional(meineKarte,1,2,8);






    dijkstra(meineKarte, 0);

    return 0;
}