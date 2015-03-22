//
// Created by Florian Braun on 20.03.2015.
//

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "actions.h"
#include "utils.h"
#include "dbio.h"
#include "processing.h"
#include "../lib/printui/printui.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  Read Only Operations  /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int Nav(struct Knoten *meineKnoten, int AnzahlKnoten ,char *K1 ,char *K2){ //Findet Weg von 2 Knoten (K1 und K2), ï¿½berprï¿½ft ob die ï¿½bergebenen Knoten Valid sind. lï¿½st dann den Deijkstra aus.

    if(isValidKnotenName(K2))return 1;//ist Eingabe Ok (erlaubte zeichen etc.)
    if(isValidKnotenName(K1))return 1;

    int K1Nummer=findeKnotenByName(meineKnoten,AnzahlKnoten,K1,1);//Findet den Knoten duirch Namen
    int K2Nummer=findeKnotenByName(meineKnoten,AnzahlKnoten,K2,1);

    if(K1Nummer!= INT_MAX && K2Nummer!= INT_MAX){// INT_MAX = knoten nicht gefunden ~ ERROR
        findeWeg(meineKnoten, AnzahlKnoten, K1Nummer, K2Nummer);// Startet Deijkstra Brerchnung
    }else{return 1;}

    return 0;
}

int SearchNormal(struct Knoten *meineKnoten,int AnzahlKnoten ,char *K1,int textonly,int sortName){// Backend des "ls" befehls der in den Daten sucht

    char** Autobahnen = GetAutobahnen(meineKnoten, AnzahlKnoten);//free Attobahn nicht vergessen
    int AutobahnGefunden=0;

    if(isValidKnotenName(K1))return 1;// ist eingabe OK (erlaubte zeichen etc..)

    for(int x=1;x<=atoi(Autobahnen[0]);x++) {//Finde Autobahn

        if (strcompCaseInsensitive(K1, Autobahnen[x]) == 0) {// Falls wahr ist die eingabe eine Autobahn
            AutobahnGefunden=1;
            if(textonly){
                printAutobahnText(meineKnoten, AnzahlKnoten, Autobahnen[x], NULL,sortName);//Ausgabe der Kompletten Autobahn in Tabellenform
            }else{
                printAutobahnVisual(meineKnoten, AnzahlKnoten, Autobahnen[x], NULL,sortName);//Ausgabe der Kompletten Autobahn "Grafisch"
            }
        }

    }

    if(!AutobahnGefunden) {//Wenn die Autobahn nicht gefunden wurde suche ob es ein Knoten ist

        int K1Nummer = findeKnotenByName(meineKnoten, AnzahlKnoten, K1,1);

        if(textonly){
            if (K1Nummer != INT_MAX) {
                printAutobahnText(meineKnoten, AnzahlKnoten, meineKnoten[K1Nummer].AutobahnName, K1, sortName);// Ausgabe Komplette Autobahn , Ausfahrt wird aber Hervorgehoben und in Ueberschrift gezeigt -> Tabellenform
            }
        }else{

            if (K1Nummer != INT_MAX) {
                printAutobahnVisual(meineKnoten, AnzahlKnoten,meineKnoten[K1Nummer].AutobahnName, K1,sortName);// Ausgabe Komplette Autobahn , Ausfahrt wird aber Hervorgehoben und in Ueberschrift gezeigt -> Grafisch
            }

        }

    }

    //Cleanup
    int AnzAutobahnen=atoi(Autobahnen[0]);
    for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
    free(Autobahnen);
    return 0;
}

int SearchAutobahnen(struct Knoten *meineKnoten,int AnzahlKnoten){// Zeigt alle Autobahnen an

    char** Autobahnen = GetAutobahnen(meineKnoten, AnzahlKnoten);//Erstes feld ist anzahl autobahnen in textform

    printMenuHeader("  Alle Autobahnen  ");
    puts("\n");
    for(int x=1;x<=atoi(Autobahnen[0]);x++) {
        printTabelRow(3,"",Autobahnen[x],"");
    }
    puts("\n");
    printFooter();


    //Cleanup
    int AnzAutobahnen=atoi(Autobahnen[0]);
    for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
    free(Autobahnen);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  Datensaetze Veraendern  /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////  Daten Erstellen   /////////////////////////////////////////

int NewAusfahrt(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname,char *Akm,int DoubleNameAllowed){// Neue ausfahrt Erstellen (new mit 4 parametern)

    char *AutobahnBuffer=Aname;
    char** Autobahnen= GetAutobahnen(meineKnoten, AnzahlKnoten);
    int NeueAnzahlKnoten=INT_MAX;

    if(isValidKnotenName(Kname))goto ReturnError;// Ist Eingabe ok (Zeichen erlaubt etc..)
    if(isValidAutobahnName(Aname))goto ReturnError;
    if(isValidAutobahnKM(Akm))goto ReturnError;// Ist Zahl erlaubt (z.b. 9 zeichen maximal 2 nachkomma)


    double f = atof(Akm);
    int AnzAutobahnen;

    if(isNewInValid(meineKnoten,AnzahlKnoten,Kname,Aname,Akm,DoubleNameAllowed))goto ReturnError;// Wuerde die erstellung eines Neuen knotens ok sein ? (prüft auf gleiche namen oder ob an der stelle schon ein knoten existiert)

    for(int x=1;x<=atoi(Autobahnen[0]);x++) {// Den namen der Autobahn Groß kleinbuchstaben anpassen falls die Autobahn schon existiert
        if (strcompCaseInsensitive(Aname, Autobahnen[x]) == 0) {
            AutobahnBuffer = strdup(Autobahnen[x]);
        }
    }

    NeueAnzahlKnoten=NewKnoten(meineKnoten, AnzahlKnoten, Kname, AutobahnBuffer, f);// Alles Ok also Neuen Knoten erstellen , speicherplatz muss schon vor aufruf von NewAusfahrt realloced werden.
    goto ReturnSuccess;

    ReturnError://Goto sorgt für sauberen return mit free
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);
        free(AutobahnBuffer);
        return INT_MAX;

    ReturnSuccess://Goto sorgt für sauberen return mit free
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);
        free(AutobahnBuffer);
        return NeueAnzahlKnoten;
}

int NewKreuz(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname1,char *Akm1,char *Aname2,char *Akm2){// Gleich wie NewAusfahrt , es wird ein Kreuz erstellt statt ausfahrt

    int NeueAnzahlKnoten=0;

    if(isValidKnotenName(Kname))return INT_MAX;
    if(isValidAutobahnName(Aname1))return INT_MAX;
    if(isValidAutobahnName(Aname2))return INT_MAX;
    if(isValidAutobahnKM(Akm1))return INT_MAX;
    if(isValidAutobahnKM(Akm2))return INT_MAX;
    if(isNewInValid(meineKnoten,AnzahlKnoten,Kname,Aname1,Akm1,0))return INT_MAX;
    if(isNewInValid(meineKnoten,AnzahlKnoten,Kname,Aname2,Akm2,0))return INT_MAX;

    //An diesem punkt ist garantiert , dass NewAusfahrt funktionieren muss.

    NeueAnzahlKnoten=NewAusfahrt(meineKnoten,AnzahlKnoten,Kname,Aname1,Akm1,0);
    NeueAnzahlKnoten=NewAusfahrt(meineKnoten,NeueAnzahlKnoten,Kname,Aname2,Akm2,1);// Doppelter name (letzter Parameter) muss erlaubt sein da davor ja schon der andere teil des kreuzes mit gleichem namen

    return NeueAnzahlKnoten;
}

//////////////////////////////////////  Daten Bearbeiten  /////////////////////////////////////////

int EditName(struct Knoten *meineKnoten,int AnzahlKnoten,char *Name,char *NeuName){// Name von Ausfahrt/Kreuz/Autobahn aendern

    char** Autobahnen= GetAutobahnen(meineKnoten, AnzahlKnoten);//Autobahenn holen
    int AnzAutobahnen;
    int SucessAutobahn=0;

    //Validierung
    if(isValidKnotenName(Name) && isValidAutobahnName(Name))goto ReturnError;
    if(isValidKnotenName(NeuName) && isValidAutobahnName(NeuName))goto ReturnError;

    if (isNewInValid(meineKnoten, AnzahlKnoten, NeuName,"Er-Fund-En","99.999", 0))goto ReturnError;//Autobahn und Km sind egal , also etwas nehmen was dur validate regeln nie erstellt werden kann

    //Falls es eine Autobahn ist
    for(int x=1;x<=atoi(Autobahnen[0]);x++) {

        if (strcompCaseInsensitive(Name, Autobahnen[x]) == 0) {

            for(int z=0;z<AnzahlKnoten;z++){
                if(strcompCaseInsensitive(Autobahnen[x],meineKnoten[z].AutobahnName)==0){

                    SucessAutobahn=1;
                    free(meineKnoten[z].AutobahnName);
                    meineKnoten[z].AutobahnName=strdup(NeuName);

                }
            }
        }
    }
    if(SucessAutobahn)goto ReturnSuccess;
    /////////


    //Falls es ein Knoten(Kreuz oder Aussfahrt ist)
    for(int z=0;z<AnzahlKnoten;z++){
        if(strcompCaseInsensitive(Name,meineKnoten[z].Name)==0){

            free(meineKnoten[z].Name);
            meineKnoten[z].Name=strdup(NeuName);

        }
    }
    //////////


    ReturnError://Sauberer Return mit Free
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);
        return INT_MAX;

    ReturnSuccess://Sauberer Return mit Free
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);
        return AnzahlKnoten;
}

int EditAusfahrt(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname,char *Akm){// Autobahn oder KM einer Ausfahrt abaendern

    //Kein realloc noetig da die anzahl an Daten gleich bleibt

    int NeueAnzahlKnoten=AnzahlKnoten;

    if(isValidKnotenName(Kname))goto ReturnError;
    if(isValidAutobahnName(Aname))goto ReturnError;
    if(isValidAutobahnKM(Akm))goto ReturnError;
    if(isNewInValid(meineKnoten,AnzahlKnoten,Kname,Aname,Akm,1))goto ReturnError;// Ist gewünschtr neuer "Ort" möglich (Name ist Egal wegen letztem Parameter)

    int ReturnValue=Remove(meineKnoten,AnzahlKnoten,Kname);// Remove Ausführen

    if(ReturnValue!=INT_MAX){//Wenn Remove erfolgreich
        NeueAnzahlKnoten=ReturnValue;
        NeueAnzahlKnoten=NewAusfahrt(meineKnoten,NeueAnzahlKnoten,Kname,Aname,Akm,0);
        goto ReturnSuccess;

    }else{// Remove war nicht erfolgreich
        //TODO NICE OUTPUT
        printMenuHeaderContinous("      Ausfahrt nicht Gefunden      ");
        goto ReturnError;
    }

    ReturnError:
        return INT_MAX;
    ReturnSuccess:
        return NeueAnzahlKnoten;
}

int EditKreuz(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname1,char *Akm1,char *Aname2,char *Akm2){//Gleiches wie EditAusfahrt blos fuer Kreuze

    //Kein realloc noetig da die anzahl an Daten gleich bleibt

    if(isValidKnotenName(Kname))goto ReturnError;
    if(isValidAutobahnName(Aname1))goto ReturnError;
    if(isValidAutobahnKM(Akm1))goto ReturnError;
    if(isValidAutobahnName(Aname2))goto ReturnError;
    if(isValidAutobahnKM(Akm2))goto ReturnError;
    if(isNewInValid(meineKnoten,AnzahlKnoten,Kname,Aname1,Akm1,1))goto ReturnError;
    if(isNewInValid(meineKnoten,AnzahlKnoten,Kname,Aname2,Akm2,1))goto ReturnError;

    int ReturnValue=Remove(meineKnoten,AnzahlKnoten,Kname);

    if(ReturnValue!=INT_MAX){
        AnzahlKnoten=ReturnValue;
        NewKreuz(meineKnoten,AnzahlKnoten,Kname,Aname1,Akm1,Aname2,Akm2);
        goto ReturnSuccess;

    }else{

        //TODO NICE OUTPUT
        printMenuHeaderContinous("  Kreuz Name nicht Gefunden   ");

        goto ReturnError;
    }

    ReturnError:
    return INT_MAX;

    ReturnSuccess:
    return AnzahlKnoten;
}

//////////////////////////////////////  Daten Lï¿½schen   /////////////////////////////////////////

int Remove(struct Knoten *meineKnoten,int AnzahlKnoten,char *K1){ // DAten loeschen

    char** Autobahnen= GetAutobahnen(meineKnoten, AnzahlKnoten);
    int AnzAutobahnen;

    if(isValidAutobahnName(K1) && isValidKnotenName(K1)) goto ReturnError; // Stimmt die eingabe (Zeichen etc.)?

    /////// FALLS AUTOBAHN  ////////
    for(int x=1;x<=atoi(Autobahnen[0]);x++) {

        if (strcompCaseInsensitive(K1, Autobahnen[x]) == 0) {

            for(int z=0;z<AnzahlKnoten;z++){
                if(strcompCaseInsensitive(Autobahnen[x],meineKnoten[z].AutobahnName)==0){
                    if(meineKnoten[z].isKreuz==1){

                        char *KreuzName= strdup(meineKnoten[z].Name);

                        AnzahlKnoten= DeleteKnoten(meineKnoten, AnzahlKnoten, z);
                        AnzahlKnoten= DeleteKnoten(meineKnoten, AnzahlKnoten, findeKnotenByName(meineKnoten, AnzahlKnoten, KreuzName,0));

                        free(KreuzName);

                    }else{
                        AnzahlKnoten= DeleteKnoten(meineKnoten, AnzahlKnoten, z);

                    }
                }
            }

            goto ReturnSuccess;

        }

    }
    ///////////////////////////


    /////// FALLS AUSFAHRT/KREUZ /////////
    int K1Nummer = findeKnotenByName(meineKnoten, AnzahlKnoten, K1,1);

    if (K1Nummer != INT_MAX) {

        if(meineKnoten[K1Nummer].isKreuz==1){

            AnzahlKnoten= DeleteKnoten(meineKnoten, AnzahlKnoten, K1Nummer);
            AnzahlKnoten= DeleteKnoten(meineKnoten, AnzahlKnoten, findeKnotenByName(meineKnoten, AnzahlKnoten, K1,1));

            goto ReturnSuccess;

        }else{
            AnzahlKnoten = DeleteKnoten(meineKnoten, AnzahlKnoten, K1Nummer);

            goto ReturnSuccess;
        }

    }
    ////////////////////////////////////


    ReturnError://Clean Return mit Free
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);
        return INT_MAX;

    ReturnSuccess://Clean Return mit Free
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);
        return AnzahlKnoten;
}

///////////////////////////////////////////////// ZUSATZFUNKTIONEN /////////////////////////////////////////////////////

int isNewInValid(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname,char *Akm,int DoubleNameAllowed){//Zeigt ob NewAusfahrt() fehlschlagen wuerde aufgrund von schon existierenden Autobahnen , Knote, etc..

    char *AutobahnBuffer=Aname;
    char** Autobahnen= GetAutobahnen(meineKnoten, AnzahlKnoten);
    char *buffer = malloc(sizeof(char)*1000);
    int AnzAutobahnen;
    double f = atof(Akm);

    for(int x=1;x<=atoi(Autobahnen[0]);x++) {//Gibt es bereits eine Autobahn die so heist wie Kname?
        if (strcompCaseInsensitive(Kname, Autobahnen[x]) == 0) {
            puts("");
            sprintf(buffer,"  Fehler , Es gibt Bereits ein Autobahn die so heist wie die gewï¿½nschte Ausfahrt/Kreuz(%s)",Kname);
            printMenuHeader(buffer);
            goto ReturnError;
        }
    }


    if(findeKnotenByName(meineKnoten, AnzahlKnoten, Kname, 0) != INT_MAX && DoubleNameAllowed==0){//Gibt es bereits ein Ausfahrt/Kreuz die so heist wie Kname?
        puts("");
        sprintf(buffer,"  Fehler , Es gibt Bereits eine Ausfahrt/Kreuz Namens \"%s\"  ",Kname);
        printMenuHeaderContinous(buffer);

        goto ReturnError;
    }

    for(int i=0;i<AnzahlKnoten;i++){// Gibt es an der Geuenschten stelle schon eine Ausfahrt / Kreuz ?
        if(meineKnoten[i].AutobahnKM==f && strcompCaseInsensitive(meineKnoten[i].AutobahnName,AutobahnBuffer)==0 ){

            puts("");
            sprintf(buffer,"  Fehler , Bei Km '%.2f' auf Autobahn '%s' ist bereits Ausfahrt/Kreuz '%s' ",f,meineKnoten[i].AutobahnName,meineKnoten[i].Name);
            printMenuHeaderContinous(buffer);

            goto ReturnError;
        }
    }


    //RETURN SUCCES (Wird nur 1 mal hier gebraucht also kein goto)
    AnzAutobahnen=atoi(Autobahnen[0]);
    for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
    free(Autobahnen);
    free(buffer);
    return 0;

    ReturnError://Clean return mit Free
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);
        free(buffer);
        return 1;
}