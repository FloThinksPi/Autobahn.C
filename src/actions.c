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

int Nav(struct Knoten *meineKnoten[], int AnzahlKnoten ,char *K1 ,char *K2){ //Findet Weg von 2 Knoten (K1 und K2), überprüft ob die übergebenen Knoten Valid sind. löst dann den Deijkstra aus.

    if(isValidKnotenName(K2))return 1;
    if(isValidKnotenName(K1))return 1;

    int K1Nummer=findeKnotenByName(meineKnoten,AnzahlKnoten,K1,1);
    int K2Nummer=findeKnotenByName(meineKnoten,AnzahlKnoten,K2,1);

    if(K1Nummer!= INT_MAX && K2Nummer!= INT_MAX){
        findeWeg(meineKnoten, AnzahlKnoten, K1Nummer, K2Nummer);
    }

}

int SearchNormal(struct Knoten *meineKnoten[],int AnzahlKnoten ,char *K1,int textonly,int sortName){

    char** Autobahnen = GetAutobahnen(meineKnoten, AnzahlKnoten);//free Attobahn nicht vergessen
    int AutobahnGefunden=0;

    if(isValidKnotenName(K1))return 1;

    for(int x=1;x<=atoi(Autobahnen[0]);x++) {//Finde Autobahn

        if (strcompCaseInsensitive(K1, Autobahnen[x]) == 0) {
            AutobahnGefunden=1;
            if(textonly){
                printAutobahnText(meineKnoten, AnzahlKnoten, Autobahnen[x], NULL,sortName);
            }else{
                printAutobahnVisual(meineKnoten, AnzahlKnoten, Autobahnen[x], NULL,sortName);
            }
        }

    }

    if(!AutobahnGefunden) {//Wenn die Autobahn nicht gefunden wurde suche ob es ein Knoten ist , wenn nicht

        int K1Nummer = findeKnotenByName(meineKnoten, AnzahlKnoten, K1,1);

        if(textonly){
            printAutobahnText(meineKnoten, AnzahlKnoten,meineKnoten[K1Nummer]->AutobahnName, K1,sortName);
        }else{

            if (K1Nummer != INT_MAX) {
                printAutobahnVisual(meineKnoten, AnzahlKnoten,meineKnoten[K1Nummer]->AutobahnName, K1,sortName);
            }

        }

    }


    int AnzAutobahnen=atoi(Autobahnen[0]);
    for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
    free(Autobahnen);

}

int SearchAutobahnen(struct Knoten *meineKnoten[],int AnzahlKnoten){// Zeigt alle Autobahnen an

    char** Autobahnen = GetAutobahnen(meineKnoten, AnzahlKnoten);//free Attobahn nicht vergessen

    printMenuHeader("  Alle Autobahnen  ");
    puts("\n");
    for(int x=1;x<=atoi(Autobahnen[0]);x++) {
        printTabelRow(3,"",Autobahnen[x],"");
    }
    puts("\n");
    printFooter();


    int AnzAutobahnen=atoi(Autobahnen[0]);
    for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
    free(Autobahnen);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  Datensätze Verändern  /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////  Daten Erstellen   /////////////////////////////////////////

int NewAusfahrt(struct UndefArrayHack *ArrayHack,int AnzahlKnoten,char *Kname,char *Aname,char *Akm,int DoubleNameAllowed){

    char *AutobahnBuffer;
    char** Autobahnen= GetAutobahnen(ArrayHack->meineKnoten, AnzahlKnoten);
    int NeueAnzahlKnoten=INT_MAX;

    if(isValidKnotenName(Kname))goto ReturnError;
    if(isValidAutobahnName(Aname))goto ReturnError;
    if(isValidAutobahnKM(Akm))goto ReturnError;


    double f = atof(Akm);
    int AnzAutobahnen;


    if(isNewInValid(ArrayHack,AnzahlKnoten,Kname,Aname,Akm,DoubleNameAllowed))goto ReturnError;


    for(int x=1;x<=atoi(Autobahnen[0]);x++) {

        if (strcompCaseInsensitive(Aname, Autobahnen[x]) == 0) {

            AutobahnBuffer=strdup(Autobahnen[x]);

        }

    }

    ArrayHack = realloc(ArrayHack,sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);
    ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*(AnzahlKnoten+1));
    NeueAnzahlKnoten=NewKnoten(ArrayHack->meineKnoten, AnzahlKnoten, Kname, AutobahnBuffer, f);
    struct Knoten *meinKnoten=ArrayHack->meineKnoten[0];

    goto ReturnSuccess;

    ReturnError:
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);
        free(AutobahnBuffer);
        return INT_MAX;

    ReturnSuccess:
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);
        free(AutobahnBuffer);
        return NeueAnzahlKnoten;
}

int NewKreuz(struct UndefArrayHack *ArrayHack,int AnzahlKnoten,char *Kname,char *Aname1,char *Akm1,char *Aname2,char *Akm2){

    int NeueAnzahlKnoten=0;

    if(isValidKnotenName(Kname))return INT_MAX;
    if(isValidAutobahnName(Aname1))return INT_MAX;
    if(isValidAutobahnName(Aname2))return INT_MAX;
    if(isValidAutobahnKM(Akm1))return INT_MAX;
    if(isValidAutobahnKM(Akm2))return INT_MAX;
    if(isNewInValid(ArrayHack,AnzahlKnoten,Kname,Aname1,Akm1,0))return INT_MAX;
    if(isNewInValid(ArrayHack,AnzahlKnoten,Kname,Aname2,Akm2,0))return INT_MAX;

    NeueAnzahlKnoten=NewAusfahrt(ArrayHack,AnzahlKnoten,Kname,Aname1,Akm1,0);
    NeueAnzahlKnoten=NewAusfahrt(ArrayHack,NeueAnzahlKnoten,Kname,Aname2,Akm2,1);

    OnlyConnectEineAutobahn(ArrayHack->meineKnoten,NeueAnzahlKnoten,Aname1);
    OnlyConnectEineAutobahn(ArrayHack->meineKnoten,NeueAnzahlKnoten,Aname2);
    OnlyConnectEinKreuz(ArrayHack->meineKnoten,NeueAnzahlKnoten,Kname);

    return NeueAnzahlKnoten;
}

//////////////////////////////////////  Daten Bearbeiten  /////////////////////////////////////////

int EditName(struct UndefArrayHack *ArrayHack,int AnzahlKnoten,char *Name,char *NeuName){

    char** Autobahnen= GetAutobahnen(ArrayHack->meineKnoten, AnzahlKnoten);
    int AnzAutobahnen;
    int SucessAutobahn=0;

    if(isValidKnotenName(Name) && isValidAutobahnName(Name))goto ReturnError;
    if(isValidKnotenName(NeuName) && isValidAutobahnName(NeuName))goto ReturnError;

    if (isNewInValid(ArrayHack, AnzahlKnoten, NeuName,"Er-Fund-En","99.999", 0))goto ReturnError;


    //Falls es eine Autobahn ist
    for(int x=1;x<=atoi(Autobahnen[0]);x++) {

        if (strcompCaseInsensitive(Name, Autobahnen[x]) == 0) {

            for(int z=0;z<AnzahlKnoten;z++){
                if(strcompCaseInsensitive(Autobahnen[x],ArrayHack->meineKnoten[z]->AutobahnName)==0){

                    SucessAutobahn=1;
                    free(ArrayHack->meineKnoten[z]->AutobahnName);
                    ArrayHack->meineKnoten[z]->AutobahnName=strdup(NeuName);

                }
            }
        }
    }
    if(SucessAutobahn)goto ReturnSuccess;
    /////////


    //Falls es ein Knoten(Kreuz oder Aussfahrt ist)
    for(int z=0;z<AnzahlKnoten;z++){
        if(strcompCaseInsensitive(Name,ArrayHack->meineKnoten[z]->Name)==0){

            free(ArrayHack->meineKnoten[z]->Name);
            ArrayHack->meineKnoten[z]->Name=strdup(NeuName);

        }
    }
    //////////


    ReturnError:
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);
        return INT_MAX;

    ReturnSuccess:
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);
        return AnzahlKnoten;
}

int EditAusfahrt(struct UndefArrayHack *ArrayHack,int AnzahlKnoten,char *Kname,char *Aname,char *Akm){

    int NeueAnzahlKnoten=AnzahlKnoten;

    if(isValidKnotenName(Kname))goto ReturnError;
    if(isValidAutobahnName(Aname))goto ReturnError;
    if(isValidAutobahnKM(Akm))goto ReturnError;
    if(isNewInValid(ArrayHack,AnzahlKnoten,Kname,Aname,Akm,1))goto ReturnError;

    int ReturnValue=Remove(ArrayHack,AnzahlKnoten,Kname);

    if(ReturnValue!=INT_MAX){
        NeueAnzahlKnoten=ReturnValue;
        NeueAnzahlKnoten=NewAusfahrt(ArrayHack,NeueAnzahlKnoten,Kname,Aname,Akm,0);
        goto ReturnSuccess;

    }else{
        //TODO NICE OUTPUT
        printMenuHeaderContinous("   Ausfahrt nicht Gefunden   ");
        goto ReturnError;
    }


    ReturnError:
        return INT_MAX;

    ReturnSuccess:
        return NeueAnzahlKnoten;
}

int EditKreuz(struct UndefArrayHack *ArrayHack,int AnzahlKnoten,char *Kname,char *Aname1,char *Akm1,char *Aname2,char *Akm2){



    if(isValidKnotenName(Kname))goto ReturnError;
    if(isValidAutobahnName(Aname1))goto ReturnError;
    if(isValidAutobahnKM(Akm1))goto ReturnError;
    if(isValidAutobahnName(Aname2))goto ReturnError;
    if(isValidAutobahnKM(Akm2))goto ReturnError;
    if(isNewInValid(ArrayHack,AnzahlKnoten,Kname,Aname1,Akm1,1))goto ReturnError;
    if(isNewInValid(ArrayHack,AnzahlKnoten,Kname,Aname2,Akm2,1))goto ReturnError;

    int ReturnValue=Remove(ArrayHack,AnzahlKnoten,Kname);

    if(ReturnValue!=INT_MAX){
        AnzahlKnoten=ReturnValue;
        NewKreuz(ArrayHack,AnzahlKnoten,Kname,Aname1,Akm1,Aname2,Akm2);
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

//////////////////////////////////////  Daten Löschen   /////////////////////////////////////////

int Remove(struct UndefArrayHack *ArrayHack,int AnzahlKnoten,char *K1){

    char** Autobahnen= GetAutobahnen(ArrayHack->meineKnoten, AnzahlKnoten);
    int AnzAutobahnen;

    if(isValidAutobahnName(K1) && isValidKnotenName(K1)) goto ReturnError;

    for(int x=1;x<=atoi(Autobahnen[0]);x++) {

        if (strcompCaseInsensitive(K1, Autobahnen[x]) == 0) {

            for(int z=0;z<AnzahlKnoten;z++){
                if(strcompCaseInsensitive(Autobahnen[x],ArrayHack->meineKnoten[z]->AutobahnName)==0){
                    if(ArrayHack->meineKnoten[z]->isKreuz==1){
                        //Erst löschen dannach nach dem 2. knoten des kreuzes suchen und dies auch löschen

                        char *KreuzName= strdup(ArrayHack->meineKnoten[z]->Name);

                        AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, z);
                        AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, KreuzName,0));

                        ArrayHack = realloc(ArrayHack,sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

                        free(KreuzName);

                    }else{
                        AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, z);
                        ArrayHack = realloc(ArrayHack,sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

                    }
                }
            }

            goto ReturnSuccess;

        }

    }


    int K1Nummer = findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, K1,1);

    if (K1Nummer != INT_MAX) {

        if(ArrayHack->meineKnoten[K1Nummer]->isKreuz==1){
            //Erst löschen dannach nach dem 2. knoten des kreuzes suchen und dies auch löschen

            AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, K1Nummer);
            ArrayHack = realloc(ArrayHack,sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

            AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, K1,1));
            ArrayHack = realloc(ArrayHack,sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

            goto ReturnSuccess;

        }else{
            AnzahlKnoten = DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, K1Nummer);
            ArrayHack = realloc(ArrayHack,sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

            goto ReturnSuccess;
        }

    }


    ReturnError:
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);

        return INT_MAX;

    ReturnSuccess:
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);

        return AnzahlKnoten;

}

///////////////////////////////////////////////// ZUSATZFUNKTIONEN /////////////////////////////////////////////////////

int isNewInValid(struct UndefArrayHack *ArrayHack,int AnzahlKnoten,char *Kname,char *Aname,char *Akm,int DoubleNameAllowed){//Zeigt ob NewAusfahrt() fehlschlagen würde aufgrund von schon existierenden Autobahnen , Knote, etc..

    char *AutobahnBuffer=Aname;
    char** Autobahnen= GetAutobahnen(ArrayHack->meineKnoten, AnzahlKnoten);
    char *buffer = malloc(sizeof(char)*1000);
    int AnzAutobahnen;
    double f = atof(Akm);

    for(int x=1;x<=atoi(Autobahnen[0]);x++) {

        if (strcompCaseInsensitive(Kname, Autobahnen[x]) == 0) {

            puts("");
            sprintf(buffer,"  Fehler , Es gibt Bereits ein Autobahn die so heist wie die gewünschte Ausfahrt/Kreuz(%s)",Kname);
            printMenuHeader(buffer);

            goto ReturnError;
        }

    }


    if(findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, Kname, 0) == INT_MAX){

    }else if(DoubleNameAllowed==0){
        puts("");
        sprintf(buffer,"  Fehler , Es gibt Bereits eine Ausfahrt/Kreuz Namens \"%s\"  ",Kname);
        printMenuHeaderContinous(buffer);

        goto ReturnError;
    }

    for(int i=0;i<AnzahlKnoten;i++){
        if(ArrayHack->meineKnoten[i]->AutobahnKM==f && strcompCaseInsensitive(ArrayHack->meineKnoten[i]->AutobahnName,AutobahnBuffer)==0 ){

            puts("");
            sprintf(buffer,"  Fehler , Bei Km '%.2f' auf Autobahn '%s' ist bereits Ausfahrt/Kreuz '%s' ",f,ArrayHack->meineKnoten[i]->AutobahnName,ArrayHack->meineKnoten[i]->Name);
            printMenuHeaderContinous(buffer);

            goto ReturnError;
        }
    }

    goto ReturnSuccess;

    ReturnError:
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);
        free(buffer);
        return 1;

    ReturnSuccess:
        AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);
        free(buffer);
        return 0;
}