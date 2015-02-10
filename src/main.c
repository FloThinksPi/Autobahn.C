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

        int K1Nummer=findeKnotenByName(ArrayHack->meineKnoten,AnzahlKnoten,K1);
        int K2Nummer=findeKnotenByName(ArrayHack->meineKnoten,AnzahlKnoten,K2);

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

            int K1Nummer = findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, K1);

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

    return 0;
}

int Delete(int argc, char *argv[]){

    char *Typo="          Usage: ls [KnotenName] [-tn(Optional)]          ";
    int AutobahnGefunden=0;
    if(argc==2) {

        char** Autobahnen=loadAutobahnen(ArrayHack->meineKnoten,AnzahlKnoten);//TODO NUR beim einlesen nötig

        char *K1 = argv[1];

        chop(K1);

        for(int x=1;x<=atoi(Autobahnen[0]);x++) {//TODO Atol depriciated

            if (strcompCaseInsensitive(K1, Autobahnen[x]) == 0) {

                for(int z=0;z<AnzahlKnoten;z++){
                    if(strcompCaseInsensitive(Autobahnen[x],ArrayHack->meineKnoten[z]->AutobahnName)==0){
                        if(ArrayHack->meineKnoten[z]->isKreuz==1){
                            //Erst löschen dannach nach dem 2. knoten des kreuzes suchen und dies auch löschen

                            char *KreuzName=ArrayHack->meineKnoten[z]->Name;

                            AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, z);
                            ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

                            AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, KreuzName));
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


        int K1Nummer = findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, K1);

        if (K1Nummer != INT_MAX) {

           if(ArrayHack->meineKnoten[K1Nummer]->isKreuz==1){
               //Erst löschen dannach nach dem 2. knoten des kreuzes suchen und dies auch löschen
               AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, K1Nummer);
               ArrayHack = realloc(ArrayHack, sizeof(struct UndefArrayHack)+sizeof(struct Knoten*)*AnzahlKnoten);

               AnzahlKnoten= DeleteKnoten(ArrayHack->meineKnoten, AnzahlKnoten, findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, K1));
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

        NewKnoten(ArrayHack->meineKnoten, AnzahlKnoten, argv[1], argv[2], atof(argv[3]));

    }else if(argc==6){
        //heilbronn k1 120 k2 320

    }else{
        puts("\n");
        printMenuHeaderContinous(Typo);
    }


    return 0;
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

