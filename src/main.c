/*
 * File:   main.c
 * Author: flo
 *
 * Created on 13. Januar 2015, 12:11
 */


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "dbio.h"
#include "main.h"
#include "../lib/menu/menu.h"
#include "../lib/printui/printui.h"
#include "utils.h"
#include <math.h>
#include <ctype.h>


//Wählt die richtige main für UNIX und Windows systeme
#ifdef _WIN32


const char *skipParameter="\\CMDknowsUTF8";

int main (int argc, char *argv[])
{
    if(argv[1]!=NULL){
        if(strcmp(argv[1],skipParameter)){
           RestartInUtf8Cmd();
            //ConfigureCMD();
            //StartupMenu(0,NULL);
        }else{
            ConfigureCMD();
            StartupMenu(0,NULL);
        }
    }else{
    RestartInUtf8Cmd();
        //ConfigureCMD();
        //StartupMenu(0,NULL);
    }

  return 0;
}

#else

int main (int argc, char *argv[])
{
    ConfigureCMD();

    StartupMenu(0,NULL);

    return 0;
}
#endif


int AnzahlKnoten;

//Struct Hack dient um ein Globales undefiniert großes array zu schaffen , erst seit C99 Erlaubt.
struct UndefArrayHack {
    int placeholder;
    struct Knoten *meineKnoten[];
};

struct UndefArrayHack *ArrayHack;
int loaded=0;
int needReload=0;

void chop(char *str) {
    size_t p=strlen(str);
    str[p-1] = '\0';
}

int FindWay(int argc, char *argv[]){

    char *Typo="Usage: nav [StartAusfahrt] [ZielAusfahrt] [-h(hilfe)] \n";

    if(argc>=3&&argc<=4) {

        int hasparam=0;
        size_t optind;
        for (optind = 3; optind < argc && argv[optind][0] == '-'; optind++) {
            switch (argv[optind][1]) {
                case 'h':
                    hasparam=1;
                    puts("\n");
                    printMenuItem(Typo);
                    break;
                default:
                    puts("\n");
                    printMenuItem(Typo);
            }
        }

        char *K1 = argv[1];
        char *K2 = argv[2];

        if(hasparam==0){
            chop(K2);
        }

        int K1Nummer=findeKnotenByName(ArrayHack->meineKnoten,AnzahlKnoten,K1,1);
        int K2Nummer=findeKnotenByName(ArrayHack->meineKnoten,AnzahlKnoten,K2,1);

        if(K1Nummer!= INT_MAX && K2Nummer!= INT_MAX){
            findeWeg(ArrayHack->meineKnoten, AnzahlKnoten, K1Nummer, K2Nummer);
        }

    }else{
        puts("\n");
        printMenuItem(Typo);
    }

}

int Search(int argc, char *argv[]){

    char *Typo="          Usage: ls [KnotenName] [-tn(Optional)]          ";

    if(argc>=2&&argc<=4) {

        int hasparam=0;
        int textonly=0;
        int sortName=0;

        char** Autobahnen=loadAutobahnen(ArrayHack->meineKnoten,AnzahlKnoten);//TODO NUR beim einlesen nötig
        int AutobahnGefunden=0;
        size_t optind;
        for (optind = 0; optind < argc ; optind++) {
            if(argv[optind][0] == '-') {
                switch (argv[optind][1]) {
                    case 't':
                        hasparam = 1;
                        textonly=1;
                        break;
                    case 'n':
                        hasparam = 1;
                        sortName=1;
                        break;
                    case 'h':
                        printMenuHeader("ls Hilfe");
                        printMenuItem("");
                        printMenuItem("Geben Sie ls [Stadt/Kreuzname] ein (z.B. 'ls Heilbronn'),");
                        printMenuItem("um die Autobahn auf der sich die Stadt oder das Kreuz befindet, anzeigen zu lassen.");
                        printMenuItem("Es werden alle Städte und Kreuze mit dem Abstand zum Anfang der Autobahn angezeigt.");
                        printMenuItem("");
                        printFooter();
                        return 1;
                    default:
                        printMenuHeaderContinous(Typo);
                        return 0;
                }
            }
        }

        char *K1 = argv[1];

        if(hasparam==0){
            chop(K1);
        }


        if (strcompCaseInsensitive("autobahn",K1) == 0) {

            printMenuHeader("  Alle Autobahnen  ");
            puts("\n");
            for(int x=1;x<=atoi(Autobahnen[0]);x++) {
                printTabelRow(3,"",Autobahnen[x],"");
            }
            puts("\n");
            printFooter();

            return 0;
        }

        for(int x=1;x<=atoi(Autobahnen[0]);x++) {//TODO Atol depriciated

            if (strcompCaseInsensitive(K1, Autobahnen[x]) == 0) {
                AutobahnGefunden=1;
                if(textonly){
                    printAutobahnText(ArrayHack->meineKnoten, AnzahlKnoten, Autobahnen[x], NULL,sortName);
                }else{
                    printAutobahnVisual(ArrayHack->meineKnoten, AnzahlKnoten, Autobahnen[x], NULL,sortName);
                }
            }

        }

        if(!AutobahnGefunden) {

            int K1Nummer = findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, K1,1);

            if(textonly){
                printAutobahnText(ArrayHack->meineKnoten, AnzahlKnoten, ArrayHack->meineKnoten[K1Nummer]->AutobahnName, K1,sortName);
            }else{

                if (K1Nummer != INT_MAX) {
                    printAutobahnVisual(ArrayHack->meineKnoten, AnzahlKnoten, ArrayHack->meineKnoten[K1Nummer]->AutobahnName, K1,sortName);
                }

            }

        }



    }else{
        puts("\n");
        printMenuHeaderContinous(Typo);
    }

}

int Edit(int argc, char *argv[]){


    char *Typo="          Usage: ls [KnotenName] [-tn(Optional)]          ";

    if(argc==3) {


        char *K1 = argv[1];
        char *K2 = argv[2];

        chop(K2);

        char* buffer= malloc(sizeof(char)*1000);

        for(int i=0;i< CountUTF8String(K1);i++){

            if(isalnum((int)K1[i])==0 && K1[i]!='_' && K1[i]!='-' && K1[i]!='.'){

                sprintf(buffer,"  Fehler , Der Ausfahrts/Kreuz Name enthält '%c' , ein Verbotenes Zeichen",argv[1][i]);
                printMenuHeader(buffer);
                printMenuItem("Erlaubte zeichen [Aa-Bb] [0-9] '_' '-' '.' ");
                printFooter();

                free(buffer);
                return 1;
            }

        }

        for(int i=0;i< CountUTF8String(K2);i++){

            if(isalnum((int)K2[i])==0 && K2[i]!='_' && K2[i]!='-' && K2[i]!='.'){

                sprintf(buffer,"  Fehler , Der Ausfahrts/Kreuz Name enthält '%c' , ein Verbotenes Zeichen",argv[1][i]);
                printMenuHeader(buffer);
                printMenuItem("Erlaubte zeichen [Aa-Bb] [0-9] '_' '-' '.' ");
                printFooter();

                free(buffer);
                return 1;
            }

        }

        free(buffer);

        char** Autobahnen=loadAutobahnen(ArrayHack->meineKnoten,AnzahlKnoten);

        for(int x=1;x<=atoi(Autobahnen[0]);x++) {//TODO Atol depriciated

            if (strcompCaseInsensitive(K1, Autobahnen[x]) == 0) {

                for(int z=0;z<AnzahlKnoten;z++){
                    if(strcompCaseInsensitive(Autobahnen[x],ArrayHack->meineKnoten[z]->AutobahnName)==0){
                        if(ArrayHack->meineKnoten[z]->isKreuz==1){
                            //Erst löschen dannach nach dem 2. knoten des kreuzes suchen und dies auch löschen


                            struct Knoten *Knoten1 = malloc(sizeof(struct Knoten));
                            Knoten1->ID=ArrayHack->meineKnoten[z]->ID;
                            Knoten1->Name= strdup(ArrayHack->meineKnoten[z]->Name);
                            Knoten1->AutobahnName= strdup(ArrayHack->meineKnoten[z]->AutobahnName);
                            Knoten1->AutobahnKM=ArrayHack->meineKnoten[z]->AutobahnKM;

                            AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, Knoten1->ID);
                            ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

                            int zw = findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, Knoten1->Name, 0);
                            struct Knoten *Knoten2 = malloc(sizeof(struct Knoten));
                            Knoten1->ID=ArrayHack->meineKnoten[zw]->ID;
                            Knoten1->Name= strdup(ArrayHack->meineKnoten[zw]->Name);
                            Knoten1->AutobahnName= strdup(ArrayHack->meineKnoten[zw]->AutobahnName);
                            Knoten1->AutobahnKM=ArrayHack->meineKnoten[zw]->AutobahnKM;

                            AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, Knoten2->ID);
                            ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

                            ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*(AnzahlKnoten+1));
                            AnzahlKnoten= NewKnoten(ArrayHack->meineKnoten, AnzahlKnoten, Knoten1->Name , K2 , Knoten1->AutobahnKM);

                            ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*(AnzahlKnoten+1));
                            AnzahlKnoten= NewKnoten(ArrayHack->meineKnoten, AnzahlKnoten, Knoten2->Name , K2 , Knoten2->AutobahnKM);

                            free(Knoten1->AutobahnName);
                            free(Knoten1->Name);
                            free(Knoten1);

                            free(Knoten2->AutobahnName);
                            free(Knoten2->Name);
                            free(Knoten2);

                            needReload=1;
                        }else{
                            struct Knoten *Knoten1 = malloc(sizeof(struct Knoten));
                            Knoten1->ID=ArrayHack->meineKnoten[z]->ID;
                            Knoten1->Name= strdup(ArrayHack->meineKnoten[z]->Name);
                            Knoten1->AutobahnName= strdup(ArrayHack->meineKnoten[z]->AutobahnName);
                            Knoten1->AutobahnKM=ArrayHack->meineKnoten[z]->AutobahnKM;

                            AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, Knoten1->ID);
                            ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

                            ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*(AnzahlKnoten+1));
                            AnzahlKnoten= NewKnoten(ArrayHack->meineKnoten, AnzahlKnoten, Knoten1->Name , K2 , Knoten1->AutobahnKM);


                            free(Knoten1->AutobahnName);
                            free(Knoten1->Name);
                            free(Knoten1);

                            needReload=1;
                        }
                    }
                }

                return 0;
            }

        }


        int K1Nummer = findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, K1,1);

        if (K1Nummer != INT_MAX) {

            if(ArrayHack->meineKnoten[K1Nummer]->isKreuz==1){
                //Erst löschen dannach nach dem 2. knoten des kreuzes suchen und dies auch löschen
                AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, K1Nummer);
                ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

                AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, K1,1));
                ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

                needReload=1;
                return 0;
            }else{
                struct Knoten *Knoten1 = malloc(sizeof(struct Knoten));
                Knoten1->ID=ArrayHack->meineKnoten[K1Nummer]->ID;
                Knoten1->Name= strdup(ArrayHack->meineKnoten[K1Nummer]->Name);
                Knoten1->AutobahnName= strdup(ArrayHack->meineKnoten[K1Nummer]->AutobahnName);
                Knoten1->AutobahnKM=ArrayHack->meineKnoten[K1Nummer]->AutobahnKM;

                AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, Knoten1->ID);
                ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

                ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*(AnzahlKnoten+1));
                AnzahlKnoten= NewKnoten(ArrayHack->meineKnoten, AnzahlKnoten, K2 , Knoten1->AutobahnName , Knoten1->AutobahnKM);


                free(Knoten1->AutobahnName);
                free(Knoten1->Name);
                free(Knoten1);

                needReload=1;
            }

        }




    }else{
        printMenuHeader(Typo);
    }

    return 1;
}

int Delete(int argc, char *argv[]){

    char *Typo="          Usage: ls [KnotenName] [-tn(Optional)]          ";
    if(argc==2) {

        char** Autobahnen=loadAutobahnen(ArrayHack->meineKnoten,AnzahlKnoten);

        char *K1 = argv[1];

        chop(K1);

        char* buffer= malloc(sizeof(char)*1000);

        for(int i=0;i< CountUTF8String(K1);i++){

            if(isalnum((int)K1[i])==0 && K1[i]!='_' && K1[i]!='-' && K1[i]!='.'){

                sprintf(buffer,"  Fehler , Der Ausfahrts/Kreuz Name enthält '%c' , ein Verbotenes Zeichen",argv[1][i]);
                printMenuHeader(buffer);
                printMenuItem("Erlaubte zeichen [Aa-Bb] [0-9] '_' '-' '.' ");
                printFooter();

                free(buffer);
                return 1;
            }

        }

        free(buffer);

        for(int x=1;x<=atoi(Autobahnen[0]);x++) {//TODO Atol depriciated

            if (strcompCaseInsensitive(K1, Autobahnen[x]) == 0) {

                for(int z=0;z<AnzahlKnoten;z++){
                    if(strcompCaseInsensitive(Autobahnen[x],ArrayHack->meineKnoten[z]->AutobahnName)==0){
                        if(ArrayHack->meineKnoten[z]->isKreuz==1){
                            //Erst löschen dannach nach dem 2. knoten des kreuzes suchen und dies auch löschen

                            char *KreuzName=ArrayHack->meineKnoten[z]->Name;

                            AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, z);
                            ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

                            AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, KreuzName,1));
                            ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

                            needReload=1;
                        }else{
                            AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, z);
                            ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);
                            needReload=1;
                        }
                    }
                }

                return 0;
            }

        }


        int K1Nummer = findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, K1,1);

        if (K1Nummer != INT_MAX) {

           if(ArrayHack->meineKnoten[K1Nummer]->isKreuz==1){
               //Erst löschen dannach nach dem 2. knoten des kreuzes suchen und dies auch löschen
               AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, K1Nummer);
               ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

               AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, K1,1));
               ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

               needReload=1;
               return 0;
           }else{
               AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, K1Nummer);
               ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);
               needReload=1;
               return 0;
           }

        }




    }else{
        printMenuHeader(Typo);
    }

    return 1;
}



int New(int argc, char *argv[]){

    char *Typo="          Usage: new [ObjektName] [AutobahnName AutobahnKM] [[AutobahnName AutobahnKM]WennKreuz]          ";

    if(argc==4) {

        char *lastparam=argv[3];
        chop(lastparam);

        char *buffer = malloc(sizeof(char)*1000);

        for(int i=0;i< CountUTF8String(argv[1]);i++){

            if(isalnum((int)argv[1][i])==0 && argv[1][i]!='_' && argv[1][i]!='-' && argv[1][i]!='.'){

                sprintf(buffer,"  Fehler , Der Ausfahrts/Kreuz Name enthält '%c' , ein Verbotenes Zeichen",argv[1][i]);
                printMenuHeader(buffer);
                printMenuItem("Erlaubte zeichen [Aa-Bb] [0-9] '_' '-' '.' ");
                printFooter();

                free(buffer);
                return 1;
            }

        }

        for(int i=0;i<CountUTF8String(argv[2]);i++){

            if(isalnum((int)argv[2][i])==0 && argv[2][i]!='_' && argv[2][i]!='-' && argv[2][i]!='.'){

                sprintf(buffer,"  Fehler , Der Autobahn Name enthält '%c' , ein Verbotenes Zeichen",argv[2][i]);
                printMenuHeader(buffer);
                printMenuItem("Erlaubte zeichen [Aa-Bb] [0-9] '_' '-' '.' ");
                printFooter();

                free(buffer);
                return 1;
            }

        }

        double f = atof(lastparam);
        sprintf(buffer,"%.2f", f);

        while(buffer[strlen(buffer)-1]=='0' || buffer[strlen(buffer)-1]=='.'){
            if(buffer[strlen(buffer)-1]=='.'){
                chop(buffer);
                break;
            }else{
                chop(buffer);
            }

        }

        if(strcmp(buffer, lastparam)==0){

            if(f>=1000000000 || f<=-1000000000){
                puts("");
                printMenuHeader("Fehler , AutobahnKM dürfen maximal 9 stellig mit 2 Nachkommastellen sein");
                printMenuItem(Typo);
                printFooter();
                free(buffer);
                return 1;
            }

        }else{
            puts("");
            printMenuHeader("Fehler , AutobahnKM ist keine Gültige Zahl[Maximal 9 stellig mit 2 Nachkommastellen]");
            printMenuItem(Typo);
            printFooter();
            free(buffer);
            return 1;
        }


       char *AutobahnBuffer=argv[2];

        char** Autobahnen=loadAutobahnen(ArrayHack->meineKnoten,AnzahlKnoten);

        for(int x=1;x<=atoi(Autobahnen[0]);x++) {

            if (strcompCaseInsensitive(argv[1], Autobahnen[x]) == 0) {


                puts("");
                sprintf(buffer,"  Fehler , Es gibt Bereits ein Autobahn mit gleichem Namen(%s)",argv[1]);
                printMenuHeader(buffer);

                free(buffer);
                int AnzAutobahnen=atoi(Autobahnen[0]);
                for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
                free(Autobahnen);
                return 1;
            }

        }


        if(findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, argv[1], 0) == INT_MAX || strcmp("new", argv[0])!=0){


                for(int x=1;x<=atoi(Autobahnen[0]);x++) {

                    if (strcompCaseInsensitive(argv[2], Autobahnen[x]) == 0) {

                        AutobahnBuffer=Autobahnen[x];

                    }

                }


        }else{
            puts("");
            sprintf(buffer,"  Fehler , Es gibt Bereits einen Knoten Namens \"%s\"  ",argv[1]);
            printMenuHeader(buffer);

            int AnzAutobahnen=atoi(Autobahnen[0]);
            for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
            free(Autobahnen);
            free(buffer);
            return 1;
        }



        ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*(AnzahlKnoten+1));
        AnzahlKnoten=NewKnoten(ArrayHack->meineKnoten, AnzahlKnoten, argv[1], AutobahnBuffer, f);

        int AnzAutobahnen=atoi(Autobahnen[0]);
        for(int x=0;x<=AnzAutobahnen;x++) free(Autobahnen[x]);
        free(Autobahnen);
        free(buffer);


    }else if(argc==6){

        char *buffer= malloc(sizeof(char)*100);

        char *FirstParam[4]={"new",argv[1],argv[2],argv[3]};
        char *SecondParam[4]={"NewIgnoreDouble",argv[1],argv[4],argv[5]};
        sprintf(buffer,"%s\n",argv[1]);
        char *DeleteParam[2]={"Delete",buffer};

        if(New(4, FirstParam)==0){

            if(New(4, SecondParam )!=0){

                Delete(2,  DeleteParam );

            }

        }

        free(buffer);

    }else{
        puts("\n");
        printMenuHeaderContinous(Typo);
    }


    return 1;
}


int StartupMenu(int argc, char *argv[]){

    system(CLEAR);

    SetMenuTitle("Autobahn.C Startbildschirm");
    AddCMD("easteregg", "um das Easteregg zu öffnen.", NULL);
    AddCMD("", "", MainMenu);

    showSystemCMDHelp();

    printMenuItem("Diese Befehle funktionieren überall im Programm");
    printMenuItem("Drücken Sie Enter um zu Starten");
    printFooter();

    printMenuItem("Dieses Programm wurde estellt von [...]");
    printMenuItem("Es kommt in der Benutzung dem Terminal eines UNIX-Systems nahe");
    printMenuItem("Viel Spaß");
    printFooter();

    //Alles mus vor CMD Start sein da dort eine endlosschleife gestartet wird die erst bei eingabe endet
    StartCMDSystem();

    return 0;

}

//Hauptmenu
int MainMenu(int argc, char *argv[]){

    char *Buffer= malloc(sizeof(char)*1000);
    if(loaded==0){
        system(CLEAR);
        AnzahlKnoten=getNumKnoten();
        sprintf(Buffer,"Einen Moment , %d Datensätze werden verarbeitet.",AnzahlKnoten);
        printMenuHeader("Lade Daten");
        printMenuItem(Buffer);
        printFooter();
        puts("\n");
        ArrayHack = malloc(sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

        loadDatabaseFiletoStruct(ArrayHack->meineKnoten,AnzahlKnoten);
        loaded=1;

        system(CLEAR);

        sprintf(Buffer,"%d Datensätze wurden erfolgreich verarbeitet.",AnzahlKnoten);
        printMenuHeader("Daten Geladen");
        printMenuItem(Buffer);
        printFooter();
        puts("\n");
    }else if(needReload) {
        system(CLEAR);
        sprintf(Buffer,"Einen Moment ,  Änderungen für %d Daten werden verarbeitet.",AnzahlKnoten);
        printMenuHeader("Wende Änderungen an");
        printMenuItem(Buffer);
        printFooter();
        puts("\n");

        ConnectData(ArrayHack->meineKnoten, AnzahlKnoten);

        needReload=0;
        system(CLEAR);

        sprintf(Buffer,"%d Datensätze wurden Erfolgreich verarbeitet.",AnzahlKnoten);
        printMenuHeader("Änderungen Geladen");
        printMenuItem(Buffer);
        printFooter();
        puts("\n");

    }else{
            system(CLEAR);
        }


    free(Buffer);

    ResetAllCMDs();

    SetMenuTitle("Hauptmenu");

    AddCMD("edit", "um das Bearbeitungs Menu zu öffnen", EditMenu);
    AddCMD("nav", "um das Navigations Menu zu öffnen", NavMenu);

    showUserCMDHelp();

    return 0;

}

//BearbeitungsMenu
int EditMenu(int argc, char *argv[]){

    system(CLEAR);

    ResetAllCMDs();
    SetMenuTitle("Editier Menu");

    AddCMD("edit", "um Ausfahrten/Kreuze/Autobahnen zu bearbeiten (edit -h für Erklärung des Befehls)", Edit);
    AddCMD("new", "um Ausfahrten/Kreuze/Autobahnen zu Erstellen (new -h für Erklärung des Befehls)", New);
    AddCMD("rm", "um Ausfahrten/Kreuze/Autobahnen zu Löschen (rm -h für Erklärung des Befehls)", Delete);
    AddCMD("ls", "für Navigationsbefehle (ls -h für Erklärung des Befehls)", Search);
    AddCMD("back", "um Zurück zum Hauptmenu zu kommen", MainMenu);

    showUserCMDHelp();

    return 0;
}

//Navigations / Benutzungs Menu
int NavMenu(int argc, char *argv[]){

    system(CLEAR);

    ResetAllCMDs();
    SetMenuTitle("Navigations Menu");
    AddCMD("nav", "für eine Navigation (search -h für Erklärung des Befehls)", FindWay);
    AddCMD("ls", "für Navigationsbefehle (ls -h für Erklärung des Befehls)", Search);
    AddCMD("back", "um Zurück zum Hauptmenu zu kommen", MainMenu);

    showUserCMDHelp();

    return 0;
}

