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

int StartupMenu(int argc, char *argv[]);

//Wählt die richtige main für UNIX und Windows systeme
#ifdef _WIN32

#include "consoleAdaption/win32Adapt.h"
const char *skipParameter="\\CMDknowsUTF8";

int main (int argc, char *argv[])
{
    if(argv[1]!=NULL){
        if(strcmp(argv[1],skipParameter)){
            ConfigureCMD();
            StartupMenu(0,NULL);
            //RestartInUtf8Cmd();
        }else{
            ConfigureCMD();
            StartupMenu(0,NULL);
        }
    }else{
        ConfigureCMD();
        StartupMenu(0,NULL);
     //RestartInUtf8Cmd();
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
    struct Knoten *meineKnoten[100];
};

struct UndefArrayHack *ArrayHack;


//Funktionen müssen vor dem aufruf im menu stehen da sie sonst nicht gefunden werden.
int showTestTabel(int argc, char *argv[]){


    printf("\n");

    printTabelHeader(6,"Col1","Col2","Col3","Colü4","Col5","6.");
    printTabelRow(6,"Erste Den","1203","Heilbrn","123","NULL","6.");
    printFooter();

    printf("\n");

    printTabelHeader(3,"Col1","Col2","Col3");
    printTabelRow(3,"Erste Daten","Zweiter Wert","FIX THAT BIT");
    printTabelRow(3,"Erste Daten","Zweiter Wert","FIX THAT BIT");
    printFooter();

    printTabelHeader(2,"Spalte 1","Spalte 2");
    printTabelRow(2,"123","dasd");
    printTabelRow(2,"123","dasd");
    printTabelRow(2,"123","dasd");
    printTabelRow(2,"123","dasd");
    printFooter();

    return 0;
}

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

    return 1;
}

int Search(int argc, char *argv[]){

    char *Typo="Usage: ls [KnotenName] [-auk(Optional)]";

    if(argc>=2&&argc<=3) {

        int hasparam=0;
        char** Autobahnen=loadAutobahnen(ArrayHack->meineKnoten,AnzahlKnoten);
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
                        puts("\n");
                        printMenuItem(Typo);
                        return 0;
                }
            }
        }

        char *K1 = argv[1];


        if(hasparam==0){
            chop(K1);
        }

        for(int x=1;x<=atol(Autobahnen[0]);x++) {//TODO Atol depriciated

            if (strcompCaseInsensitive(K1, Autobahnen[x]) == 0) {
                AutobahnGefunden=1;
                printf("%s", Autobahnen[x]);//TODO Func zum Autobahn Ausgeben , func(meineKnoten,Anzahlk,Autobahn,Knotenname(kann NUll sein)
            }
        }

        if(!AutobahnGefunden) {

            int K1Nummer = findeKnotenByName(ArrayHack->meineKnoten, AnzahlKnoten, K1);

            if (K1Nummer != INT_MAX) {
                printf("%s", ArrayHack->meineKnoten[K1Nummer]->AutobahnName);//TODO AUSGABE DER GESAMTEN AUTOBAHN HIER
            }

        }



    }else{
        puts("\n");
        printMenuItem(Typo);
    }

    return 1;
}


int StartupMenu(int argc, char *argv[]){

    system(CLEAR);
    //ä,ö,ü,§,%
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

    system(CLEAR);

    ResetAllCMDs();

    AddCMD("edit", "um das Bearbeitungs Menu zu öffnen", EditMenu);
    AddCMD("nav", "um das Navigations Menu zu öffnen", NavMenu);

    showUserCMDHelp();

    return 0;

}

//BearbeitungsMenu
int EditMenu(int argc, char *argv[]){

    system(CLEAR);

    ResetAllCMDs();

    AddCMD("edit", "um Ausfahrten/Kreuze/Autobahnen zu bearbeiten (edit -h für Erklärung des Befehls)", showTestTabel);
    AddCMD("ls", "für Navigationsbefehle (ls -h für Erklärung des Befehls)", Search);
    AddCMD("back", "um Zurück zum Hauptmenu zu kommen", MainMenu);

    showUserCMDHelp();

    return 0;
}

//Navigations / Benutzungs Menu
int NavMenu(int argc, char *argv[]){



    AnzahlKnoten=getNumKnoten();
    ArrayHack =malloc(sizeof(struct UndefArrayHack));

    //ArrayHack->meineKnoten= malloc(sizeof(struct Knoten**));
    loadDatabaseFiletoStruct(ArrayHack->meineKnoten,AnzahlKnoten);

    system(CLEAR);

    ResetAllCMDs();

    AddCMD("nav", "für eine Navigation (search -h für Erklärung des Befehls)", FindWay);
    AddCMD("ls", "für Navigationsbefehle (ls -h für Erklärung des Befehls)", Search);
    AddCMD("back", "um Zurück zum Hauptmenu zu kommen", MainMenu);

    showUserCMDHelp();

    return 0;
}

