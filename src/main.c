/*
 * File:   main.c
 * Author: flo
 *
 * Created on 13. Januar 2015, 12:11
 */


#include <stdio.h>
#include <stdlib.h>
#include "dbio.h"
#include "main.h"
#include "processing.h"
#include "../lib/menu/menu.h"
#include "../lib/printui/printui.h"


int main(void)
{


    StartupMenu(0,NULL);

    return 0;

}

//Funktionen müssen vor dem aufruf im menu stehen da sie sonst nicht gefunden werden.
int showTestTabel(int argc, char *argv[]){

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


int StartupMenu(int argc, char *argv[]){

    system(CLEAR);
    //ä,ö,ü,§,%
    SetMenuTitle("Autobahn.C Startbildschirm");
    AddCMD("easteregg", "easteregg", NULL);
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

    AddCMD("1", "um den Bearbeitungsmodus für Autobahnen zu öffnen", showTestTabel);
    AddCMD("2", "um den Bearbeitungsmodus für Ausfahrten zu öffnen", NULL);
    AddCMD("3", "um den Bearbeitungsmodus für Kreuze zu öffnen", NULL);
    AddCMD("back", "um Zurück zum Hauptmenu zu kommen", MainMenu);

    showUserCMDHelp();

    return 0;
}

//Navigations / Benutzungs Menu
int NavMenu(int argc, char *argv[]){

    system(CLEAR);

    ResetAllCMDs();

    AddCMD("search", "für Suchfunktion (search --help für mehr infos)", NULL);
    AddCMD("nav", "für Navigationsbefehle (nav --help für mehr infos)", NULL);
    AddCMD("back", "um Zurück zum Hauptmenu zu kommen", MainMenu);

    showUserCMDHelp();

    return 0;
}

