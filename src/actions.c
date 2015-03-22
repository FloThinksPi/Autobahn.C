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

int Nav(struct Knoten *meineKnoten, int AnzahlKnoten ,char *K1 ,char *K2){ //Findet Weg von 2 Knoten (K1 und K2), �berpr�ft ob die �bergebenen Knoten Valid sind. l�st dann den Deijkstra aus.

    if(isValidKnotenName(K2))return 1;
    if(isValidKnotenName(K1))return 1;

    int K1Nummer=findeKnotenByName(meineKnoten,AnzahlKnoten,K1,1);
    int K2Nummer=findeKnotenByName(meineKnoten,AnzahlKnoten,K2,1);

    if(K1Nummer!= INT_MAX && K2Nummer!= INT_MAX){
        findeWeg(meineKnoten, AnzahlKnoten, K1Nummer, K2Nummer);
    }

}

int SearchNormal(struct Knoten *meineKnoten,int AnzahlKnoten ,char *K1,int textonly,int sortName){

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
            if (K1Nummer != INT_MAX) {
                printAutobahnText(meineKnoten, AnzahlKnoten, meineKnoten[K1Nummer].AutobahnName, K1, sortName);
            }
        }else{

            if (K1Nummer != INT_MAX) {
                printAutobahnVisual(meineKnoten, AnzahlKnoten,meineKnoten[K1Nummer].AutobahnName, K1,sortName);
            }

        }

    }


    int AnzAutobahnen=atoi(Autobahnen[0]);
    for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
    free(Autobahnen);

}

int SearchAutobahnen(struct Knoten *meineKnoten,int AnzahlKnoten){// Zeigt alle Autobahnen an

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
//////////////////////////////////////  Datens�tze Ver�ndern  /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////  Daten Erstellen   /////////////////////////////////////////

int NewAusfahrt(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname,char *Akm,int DoubleNameAllowed){

    char *AutobahnBuffer=Aname;
    char** Autobahnen= GetAutobahnen(meineKnoten, AnzahlKnoten);
    int NeueAnzahlKnoten=INT_MAX;

    if(isValidKnotenName(Kname))goto ReturnError;
    if(isValidAutobahnName(Aname))goto ReturnError;
    if(isValidAutobahnKM(Akm))goto ReturnError;


    double f = atof(Akm);
    int AnzAutobahnen;


    if(isNewInValid(meineKnoten,AnzahlKnoten,Kname,Aname,Akm,DoubleNameAllowed))goto ReturnError;


    for(int x=1;x<=atoi(Autobahnen[0]);x++) {

        if (strcompCaseInsensitive(Aname, Autobahnen[x]) == 0) {

            AutobahnBuffer=strdup(Autobahnen[x]);

        }

    }

    NeueAnzahlKnoten=NewKnoten(meineKnoten, AnzahlKnoten, Kname, AutobahnBuffer, f);


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

int NewKreuz(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname1,char *Akm1,char *Aname2,char *Akm2){

    int NeueAnzahlKnoten=0;

    if(isValidKnotenName(Kname))return INT_MAX;
    if(isValidAutobahnName(Aname1))return INT_MAX;
    if(isValidAutobahnName(Aname2))return INT_MAX;
    if(isValidAutobahnKM(Akm1))return INT_MAX;
    if(isValidAutobahnKM(Akm2))return INT_MAX;
    if(isNewInValid(meineKnoten,AnzahlKnoten,Kname,Aname1,Akm1,0))return INT_MAX;
    if(isNewInValid(meineKnoten,AnzahlKnoten,Kname,Aname2,Akm2,0))return INT_MAX;

    NeueAnzahlKnoten=NewAusfahrt(meineKnoten,AnzahlKnoten,Kname,Aname1,Akm1,0);
    NeueAnzahlKnoten=NewAusfahrt(meineKnoten,NeueAnzahlKnoten,Kname,Aname2,Akm2,1);

    return NeueAnzahlKnoten;
}

//////////////////////////////////////  Daten Bearbeiten  /////////////////////////////////////////

int EditName(struct Knoten *meineKnoten,int AnzahlKnoten,char *Name,char *NeuName){

    char** Autobahnen= GetAutobahnen(meineKnoten, AnzahlKnoten);
    int AnzAutobahnen;
    int SucessAutobahn=0;

    if(isValidKnotenName(Name) && isValidAutobahnName(Name))goto ReturnError;
    if(isValidKnotenName(NeuName) && isValidAutobahnName(NeuName))goto ReturnError;

    if (isNewInValid(meineKnoten, AnzahlKnoten, NeuName,"Er-Fund-En","99.999", 0))goto ReturnError;


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

int EditAusfahrt(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname,char *Akm){

    int NeueAnzahlKnoten=AnzahlKnoten;

    if(isValidKnotenName(Kname))goto ReturnError;
    if(isValidAutobahnName(Aname))goto ReturnError;
    if(isValidAutobahnKM(Akm))goto ReturnError;
    if(isNewInValid(meineKnoten,AnzahlKnoten,Kname,Aname,Akm,1))goto ReturnError;

    int ReturnValue=Remove(meineKnoten,AnzahlKnoten,Kname);

    if(ReturnValue!=INT_MAX){
        NeueAnzahlKnoten=ReturnValue;
        NeueAnzahlKnoten=NewAusfahrt(meineKnoten,NeueAnzahlKnoten,Kname,Aname,Akm,0);
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

int EditKreuz(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname1,char *Akm1,char *Aname2,char *Akm2){



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

//////////////////////////////////////  Daten L�schen   /////////////////////////////////////////

int Remove(struct Knoten *meineKnoten,int AnzahlKnoten,char *K1){

    char** Autobahnen= GetAutobahnen(meineKnoten, AnzahlKnoten);
    int AnzAutobahnen;

    if(isValidAutobahnName(K1) && isValidKnotenName(K1)) goto ReturnError;

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

int isNewInValid(struct Knoten *meineKnoten,int AnzahlKnoten,char *Kname,char *Aname,char *Akm,int DoubleNameAllowed){//Zeigt ob NewAusfahrt() fehlschlagen w�rde aufgrund von schon existierenden Autobahnen , Knote, etc..

    char *AutobahnBuffer=Aname;
    char** Autobahnen= GetAutobahnen(meineKnoten, AnzahlKnoten);
    char *buffer = malloc(sizeof(char)*1000);
    int AnzAutobahnen;
    double f = atof(Akm);

    for(int x=1;x<=atoi(Autobahnen[0]);x++) {

        if (strcompCaseInsensitive(Kname, Autobahnen[x]) == 0) {

            puts("");
            sprintf(buffer,"  Fehler , Es gibt Bereits ein Autobahn die so heist wie die gew�nschte Ausfahrt/Kreuz(%s)",Kname);
            printMenuHeader(buffer);

            goto ReturnError;
        }

    }


    if(findeKnotenByName(meineKnoten, AnzahlKnoten, Kname, 0) == INT_MAX){

    }else if(DoubleNameAllowed==0){
        puts("");
        sprintf(buffer,"  Fehler , Es gibt Bereits eine Ausfahrt/Kreuz Namens \"%s\"  ",Kname);
        printMenuHeaderContinous(buffer);

        goto ReturnError;
    }

    for(int i=0;i<AnzahlKnoten;i++){
        if(meineKnoten[i].AutobahnKM==f && strcompCaseInsensitive(meineKnoten[i].AutobahnName,AutobahnBuffer)==0 ){

            puts("");
            sprintf(buffer,"  Fehler , Bei Km '%.2f' auf Autobahn '%s' ist bereits Ausfahrt/Kreuz '%s' ",f,meineKnoten[i].AutobahnName,meineKnoten[i].Name);
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