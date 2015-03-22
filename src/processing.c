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

double gibWegLaenge(struct Knoten *meineKnoten,int u,int v){//Gibt den Wert des Wegs von u->v bzw meineKnoten[u]->meineKnoten[v]
    int i=0;
    while(i < meineKnoten[u].numWege){
        if(meineKnoten[u].Wege[i]->nach->ID==meineKnoten[v].ID){
            return meineKnoten[u].Wege[i]->laenge;
        }
        i++;
    }
    return 0;//Kein Weg gefunden
}



int minDistance(struct Knoten *meineKnoten){//Errechnet den am kürzesten entfernten nächsten knoten aus allen noch nicht besuchten knoten. //Wählt somit den "Weg" dergegangen wird.

    double min = INT_MAX;//Initialisiere min mit Maximalwert (Jeder weg ist kleiner sein)
    int min_index = 0;

    for (int v = 0; v < AnzahlKnoten; v++) {//Wahlt den Knoten aus der die kleinste entfernung zum startknotenn hat
        if (meineKnoten[v].besucht == false && meineKnoten[v].entfernungZumUrsprung <= min) {
            min = meineKnoten[v].entfernungZumUrsprung;
            min_index = v;
        }
    }
    return min_index;//Return den besten naechsten Knoten
}


void dijkstra(struct Knoten *meineKnoten, int src)//Hier wird der Algorythmus gestartet und die gegangenen Wege Gespeichert.
{
    //Entfernung = 0 da der startknoten zu sich selbst nicht entfernt ist
    meineKnoten[src].entfernungZumUrsprung = 0;

    for (int count = 0; count < AnzahlKnoten-1; count++){//Kuerzesten Pfad finden

        int u=0;

        //U = immer src beim ersten mal , findet besten neachsten knoten, siehe funktionskommentare
        u = minDistance(meineKnoten);

        // Den gewahlten Knoten als besucht markieren
        meineKnoten[u].besucht = true;

        // Entfernungen zum Ursprung neu Errechnen
        for (int v = 0; v < AnzahlKnoten; v++) {

            // Entfernung zum Ursprung Errechnen wenn
            // 1. Zwischen U und V eine verbindung besteht
            // 2. V noch nicht Besucht Wurde
            // 3. Der Weg von V ueber U zum Ursprung kleiner ist als der von V zum Ursprung

            if(!meineKnoten[v].besucht && gibWegLaenge(meineKnoten,u,v) && meineKnoten[u].entfernungZumUrsprung != INT_MAX &&  meineKnoten[u].entfernungZumUrsprung + gibWegLaenge(meineKnoten,u,v) < meineKnoten[v].entfernungZumUrsprung){
                meineKnoten[v].entfernungZumUrsprung = meineKnoten[u].entfernungZumUrsprung + gibWegLaenge(meineKnoten,u,v);//Benachbarte Knoten bekommen Weg zum Ursprung
                meineKnoten[v].knotenZurueck=&meineKnoten[u];//Vorherige Knoten wird gespeichert zum Backtracing -> Ausgabe
            }
        }
    }

}

int printPathToTarget(struct Knoten *meineKnoten,int StartKnoten,int Endknoten,double StartZeit){//Gibt gegangenen Weg aus

    char* BewegungsArray[AnzahlKnoten];//Weg wird mit lastnode zuruckverfolgt , deshalb muss die Ausgabe gedreht werden damit nicht das ende zuerst kommt sondern der Anfang , der "gedrehte" weg wird hier gespeichert

    char* buffer = malloc(sizeof(char)*1000);// Zwischenspeicer


    for (int i = 0; i < AnzahlKnoten; i++){
        if (meineKnoten[i].ID == Endknoten){//ENDKNOTEN Finden
            int v = i;
            int AnzahlBewegungen = 0;

            ////Zurueckgelegten Weg (Trace) in Bewegungsarray speichern///////
            while (meineKnoten[v].knotenZurueck!=NULL) {

                if (gibWegLaenge(meineKnoten, v, meineKnoten[v].knotenZurueck->ID)>0.001) {//Kreuzuebergange haben den weg 0.00001 werden somit ignoriert in der ausgabe (z.b. K1_2(A1) ---0.00001km--> K1_2(A2) wird ignoriert)

                    sprintf(buffer, "------(%4.2f Km)----->", gibWegLaenge(meineKnoten, v, meineKnoten[v].knotenZurueck->ID));

                    BewegungsArray[AnzahlBewegungen * 3]=strdup(meineKnoten[v].knotenZurueck->Name);//Ausgangspunkt der "Bewegung" -> Jeder 3. wert (0,3,6,...)
                    BewegungsArray[AnzahlBewegungen * 3 + 1]=strdup(buffer);//Entfernung der "Bewegung"                            -> Jeder 3.+1 Wert (1,4,7...)
                    BewegungsArray[AnzahlBewegungen * 3 + 2]=strdup(meineKnoten[v].Name);//Endpunkt der "Bewegung"                 -> Jeder 3.+2 Wert (2,5,8,..)

                    v = meineKnoten[v].knotenZurueck->ID;//Einen Knoten Weiter (zuruck)gehen im nachsten durchlauf
                    AnzahlBewegungen++;
                }else{// falls es ein Kreuzuebergang ist eifach weiter gehen ohne ausgabe zu generieren
                    v = meineKnoten[v].knotenZurueck->ID;
                }
            }
            /////////////////////////////////////////////////////////////////


            //////////////////Bewegungsarray Rueckwarts Lesen und Ausgeben -> resultiert in richtiger logischer Reihenfolge///////////////////////////
            if (meineKnoten[i].ID != meineKnoten[StartKnoten].ID) {//Wenn Start!=Endknoten

                if (meineKnoten[i].entfernungZumUrsprung == INT_MAX) {//Wenn Kein Weg gefunden
                    puts("\n");
                    sprintf(buffer, "  \"%s\" ist von \"%s\" aus nicht Erreichbar  ", meineKnoten[i].Name, meineKnoten[StartKnoten].Name);
                    printMenuHeaderContinous(buffer);
                    puts("\n");

                } else {//Weg gefunden , ausgabe beginnen

                    //Ueberschrift
                    puts("\n");
                    sprintf(buffer, "  Weg von \"%s\" nach \"%s\"  ", meineKnoten[StartKnoten].Name, meineKnoten[i].Name);
                    printMenuHeader(buffer);
                    printMenuItem("");

                    //BEWEGUNGSARRAY Rueckwarts Ausgeben
                    printTabelHeader(3, " Von ", " Strecke ", " Nach ");
                    for (int x = AnzahlBewegungen - 1; x >= 0; x--) {
                        printTabelRow(3, BewegungsArray[x * 3], BewegungsArray[x * 3 + 1], BewegungsArray[x * 3 + 2]);
                    }

                    //Endzeile
                    sprintf(buffer, " | Gesamt: %0.2f Km | Über %d Knoten | Berechnet in %f Sekunden | ", meineKnoten[i].entfernungZumUrsprung,AnzahlBewegungen-1, get_time()-StartZeit);
                    printFooterText(buffer);
                    puts("\n");
                }
            }
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            break; // Ausgabe erfolgte , Engknoten wurde gefunden , eine weitere suche nach endknoten macht keinen sinn
        }


    }

    return 1;
}

//Finden und ausgabe des weges zwischen a und b , bereitet den Deijkstra vor und managed die berechnung und ausgabe.
int findeWeg(struct Knoten *meineKnoten,int AnzahlNodes,int StartKnoten,int ZielKnoten)
{

    double StartZeit = get_time();//Zeitmessung

    //Anzahl an Knoten wird festgelegt
    AnzahlKnoten=AnzahlNodes;

    //Werte werden Initialisiert im KnotenArray , Dies muss vor dem erstellen der wege passieren da Numwege initialisiert wird.
    for (int i = 0; i < AnzahlKnoten; i++){
        meineKnoten[i].entfernungZumUrsprung = INT_MAX , meineKnoten[i].besucht = false ,  meineKnoten[i].knotenZurueck=NULL;
    }


    //Algorythmus wird gestartet
    dijkstra(meineKnoten, StartKnoten);

    //Ausgabe der Wege in einer Tabelle
    printPathToTarget(meineKnoten,StartKnoten,ZielKnoten,StartZeit);


    return 1;
}
