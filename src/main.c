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
#include "processing.h"
#include "../lib/menu/menu.h"
#include "../lib/printui/printui.h"
#include "consoleAdaption/unixAdapt.h"


//Wählt die richtige main für UNIX und Windows systeme
#ifdef _WIN32

#include "consoleAdaption/win32Adapt.h"
const char *skipParameter="\\CMDknowsUTF8";

int main (int argc, char *argv[])
{
    if(argv[1]!=NULL){
        if(strcmp(argv[1],skipParameter)){
           RestartInUtf8Cmd();
        }else{
            ConfigureCMD();
            StartupMenu(0,NULL);
        }
    }else{
    RestartInUtf8Cmd();
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

void chop(char *str) {
    size_t p=strlen(str);
    str[p-1] = '\0';
}

int FindWay(int argc, char *argv[]){

    char *Typo="Usage: Search [StartAusfahrt] [ZielAusfahrt] [-h(hilfe)] \n";

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

    char *Typo="          Usage: ls [KnotenName] [-auk(Optional)]          ";

    if(argc>=2&&argc<=3) {

        int hasparam=0;
        char** Autobahnen=loadAutobahnen(ArrayHack->meineKnoten,AnzahlKnoten);//TODO NUR beim einlesen nötig
        int AutobahnGefunden=0;
        size_t optind;
        for (optind =0; optind < argc ; optind++) {
            if(argv[optind][0] == '-') {
                switch (argv[optind][1]) {
                    case 'a':
                        hasparam = 1;
                        break;
                    case 'u':
                        hasparam = 1;
                        break;
                    case 'k':
                        hasparam = 1;
                        break;
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

        for(int x=1;x<=atoi(Autobahnen[0]);x++) {//TODO Atol depriciated

            if (strcompCaseInsensitive(K1, Autobahnen[x]) == 0) {
                AutobahnGefunden=1;
                printf("Autobahn: %s", Autobahnen[x]);//TODO Func zum Autobahn Ausgeben , func(meineKnoten,Anzahlk,Autobahn,Knotenname(kann NUll sein)
            }

        }

        if(!AutobahnGefunden) {

            int K1Nummer = findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, K1);

            if (K1Nummer != INT_MAX) {
                printAutobahn(ArrayHack->meineKnoten, AnzahlKnoten, ArrayHack->meineKnoten[K1Nummer]->AutobahnName,K1);
            }

        }



    }else{
        puts("\n");
        printMenuHeaderContinous(Typo);
    }

}


int StartupMenu(int argc, char *argv[]){

    system(CLEAR);

    SetMenuTitle("Autobahn.C Startbildschirm");
    AddCMD("easteregg", "um das Easteregg zu öffnen.", NULL);
    AddCMD("", "", MainMenu);

    showSystemCMDHelp();

    printMenuItem("Diese Befehle funktionieren  Überall im Programm");
    printMenuItem("Drücken sie Enter um zu  Starten");
    printFooter();

    printMenuItem("Dieses Programm wurde estellt von [...]");
    printMenuItem("Es kommt in der menutzung dem terminal eines UNIX systems nahe");
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

        sprintf(Buffer,"%d Datensätze Wurden Erfolgreich verarbeitet.",AnzahlKnoten);
        printMenuHeader("Daten Geladen");
        printMenuItem(Buffer);
        printFooter();
        puts("\n");
    }else{
        system(CLEAR);
    }

    ResetAllCMDs();

    SetMenuTitle("Hauptmenu");

    AddCMD("edit", "um das Bearbeitungs Menu zu öffnen", EditMenu);
    AddCMD("nav", "um das Navigations Menu zu öffnen", NavMenu);

    showUserCMDHelp();

    //free(Buffer);TODO FREE ERROR

    return 0;

}

//BearbeitungsMenu
int EditMenu(int argc, char *argv[]){

    system(CLEAR);

    ResetAllCMDs();
    SetMenuTitle("Editier Menu");

    AddCMD("edit", "um Ausfahrten/Kreuze/Autobahnen zu bearbeiten (edit -h für Erklärung des Befehls)", NULL);
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

