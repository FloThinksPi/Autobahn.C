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


//Default CMD size : 80x25
//Default Terminal size : 80x24


int main(void)
{

    //Beispiel für das anzeigen von spalten
    /*printf("Column1    Column2   Column3\n");
    printf("%7.1lf%11.0lf%10.3lf\n", 100.1, 1221.0, 9348.012);
    printf("%7.1lf%11.0lf%10.3lf\n", 2.3, 211.0, 214.0);*/

    StartupMenu(0,NULL);

    return 0;

}



//Bedienungserklärung zum Programmstart
int StartupMenu(int argc, char *argv[]){

    system(CLEAR);

    AddCMD("easteregg", "easteregg", NULL);
    AddCMD("", "", MainMenu);

    showSystemCMDHelp();

    printf("Diese Befehle funktionieren Überall im Programm\nDrücken sie Enter um zu Starten\n");

    //Alles mus vor CMD Start sein da dort eine endlosschleife restartet wird die erst bei eingabe endet
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

    AddCMD("1", "um den Bearbeitungsmodus für Autobahnen zu öffnen", NULL);
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

