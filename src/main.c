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
#include "actions.h"
#include <ctype.h>


//Wählt die richtige main für UNIX und Windows systeme
#ifdef _WIN32


const char *skipParameter="\\CMDknowsUTF8";

int main (int argc, char *argv[])
{
    if(argv[1]!=NULL){
        if(strcmp(argv[1],skipParameter)){
          // RestartInUtf8Cmd();
           ConfigureCMD();
            StartupMenu(0,NULL);
        }else{
            ConfigureCMD();
            StartupMenu(0,NULL);
        }
    }else{
    //RestartInUtf8Cmd();
       ConfigureCMD();
        StartupMenu(0,NULL);
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
struct Knoten *meineKnoten;

int loaded=0;//Daten Geladen ?
int needReload=0;//Müssen Daten (Teilweise) Neu geladen Werden?
int needFullReload=0;//Müssen Daten Komplett neu Verarbeitet Werden?
int DataChanged=0;//Hat sich die Datenbank Verändert? , dadurch wird entschieden ob der dialog änderungen speichern bei exit kommt.

int Navigate(int argc, char *argv[]){//Funktion die durch Nav im Navigationsmenu getriggert wird , erhält die eingabe und trennt parameter

    char *Typo="   Falsche benutzung der Navigation : \"nav -h\" für hilfe   ";//Kurze Funktionsbeschreibung , soll bei falscheingabe kommen (falsche syntax)

    if(argc==3) {// Stimmt die anzahl der Parameter? 3 Parameter = nav [StartAusfahrt] [ZielAusfahrt]

        char *K2 = argv[2];
        chop(K2);//Zeilenumbruch beim letzten Parameter entfernen

        Nav(meineKnoten,AnzahlKnoten,argv[1],K2);

    }else if(argc==2){//Hilfe (nav -h)
        if(argv[1][0]=='-' && argv[1][1]=='h'){//TODO -hs -hallo -hafaf sind noch möglich
            printMenuHeader("");
            printMenuHeaderContinous(" Hilfe für Navigationsbefehl ");
            printMenuHeaderContinous(" Dieser Befehl Findet den Weg von A nach B");
            printMenuHeaderContinous(" Usage: nav [StartAusfahrt] [ZielAusfahrt] ");
            printMenuHeaderContinous(" Beispiel: nav Heilbronn Stuttgart");
            printFooter();
        }
    }
    else{
        puts("\n");
        printMenuHeaderContinous(Typo);
    }

}


int Search(int argc, char *argv[]){// LS befehl , zeigt ausfahrten bzw autobahnen.

    char *Typo="   Falsche benutzung des List befehls : \"ls -h\" für hilfe   ";

    if(argc>=2&&argc<=4) {

        if(needReload) {// Wird benötigt um Wege neu zu knüpfen , da Wege nur für Nav und Ls wichtig sind wird nicht nach jedem Edit neu Geladen -> Performance
            char *Buffer= malloc(sizeof(char)*1000);

            system(CLEAR);
            sprintf(Buffer,"Einen Moment ,  Änderungen für %d Daten werden verarbeitet.",AnzahlKnoten);
            printMenuHeader("Wende Änderungen an");
            printMenuItem(Buffer);
            printFooter();
            puts("\n");

            ConnectData(meineKnoten,AnzahlKnoten);
            //OnlyConnectKreuze( meineKnoten->meineKnoten, AnzahlKnoten);

            needReload=0;
            needFullReload=1;
            system(CLEAR);

            sprintf(Buffer,"%d Datensätze wurden Erfolgreich verarbeitet.",AnzahlKnoten);
            printMenuHeader("Änderungen Geladen");
            printMenuItem(Buffer);
            printFooter();
            puts("\n");

            free(Buffer);
        }


        int hasparam=0;
        int textonly=0;
        int sortName=0;


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
                    case 'a':

                        SearchAutobahnen( meineKnoten,AnzahlKnoten);

                        return 0;
                    case 'h':
                        printMenuHeader("ls Hilfe");
                        printMenuItem("");
                        printMenuItem("Geben Sie ls [Stadt/Kreuzname] ein (z.B. 'ls Heilbronn'),");
                        printMenuItem("um die Autobahn auf der sich die Stadt oder das Kreuz befindet, anzeigen zu lassen.");
                        printMenuItem("Es werden alle Städte und Kreuze mit dem Abstand zum Anfang der Autobahn angezeigt.");
                        printMenuItem("-t Zeigt die Autobahn in einer Tabelle (Kompakte Ansicht)");
                        printMenuItem("-n Sortiert die Ausgabe nach Namen (Funktioniert nur mit -t)");
                        printMenuItem("ls -a Zeigt alle Autobahnen die es gibt.");
                        printMenuItem("");
                        printFooter();
                        return 0;
                    default:
                        printMenuHeaderContinous(Typo);
                        return 1;
                }
            }
        }

        if(textonly==0 && sortName==1){

            puts("");
            printMenuHeaderContinous("  Das sortieren Nach Namen Funtioniert nur im TabellenModus (mit parameter -t )  ");

            return 1;
        }

        char *K1 = argv[1];
        if(hasparam==0){
            chop(K1);
        }

        SearchNormal( meineKnoten,AnzahlKnoten,K1,textonly,sortName);

    }else{
        puts("\n");
        printMenuHeaderContinous(Typo);
    }

}

int Edit(int argc, char *argv[]){

    char *Typo="   Der Edit Befehl wurde Falsch genutzt , \"edit -h\" für hilfe    ";

    if(argc==2) {// Hilfe anzeigen

        if(argv[1][0]=='-' && argv[1][1]=='h') {//TODO -hs -hallo -hafaf sind noch möglich

            printMenuHeader(" Hilfe für den Edit befehl ");
            printMenuHeaderContinous("");
            printMenuHeaderContinous("Umbenennen: Edit [ObjektName] [NeuerObjektName]    -> Ein objekt kann Ausfahrt,Kreuz,Autobahn sein");
            printMenuHeaderContinous("Ausfahrt Ändern: Edit [AusfahrtName] [NeueAutobahn] [NeuerAtobahnKm]");
            printMenuHeaderContinous("Kreuz Ändern: Edit [KreuzName] [NeueAutobahn_1] [NeuerAtobahnKm_1] [NeueAutobahn_2] [NeuerAtobahnKm_2]");
            printMenuHeaderContinous("");
            printMenuHeaderContinous("Beispiel Umbenennen: \"edit Mosbach MOS\" oder \"edit A1 Aeins\"");
            printMenuHeaderContinous("Beispiel Ausfahrt: \"edit Mosbach A1 10\" oder \"edit Heilbronn A8 99.99\"");
            printMenuHeaderContinous("Beispiel Kreuz: \"edit K1_2 A1 11 A2 93.23\" oder \"edit MeinKreuz A2 93.2 A999 39.2\"");
            printFooter();

            return 0;

        }

    }else if(argc==3) {//Umbenennung "edit [AlterName] [NeuerName]" Kann Kreuz,Ausfahrt,Autobahn sein

        char *K1 = argv[1];
        char *K2 = argv[2];
        chop(K2);

        int ReturnValue = EditName( meineKnoten,AnzahlKnoten,K1,K2);

        if(ReturnValue!=INT_MAX){
            AnzahlKnoten=ReturnValue;
            needReload=1;
            DataChanged=1;
        }else{return 1;}

    } else if(argc==4) {//Ausfahrt Bearbeiten "edit [KnotenName] [NeueAutobahn] [NewKM]"

        char *lastparam = argv[3];
        chop(lastparam);

        int ReturnValue=EditAusfahrt( meineKnoten,AnzahlKnoten,argv[1],argv[2],lastparam);

        if(ReturnValue!=INT_MAX){
            AnzahlKnoten=ReturnValue;
            needReload=1;
            DataChanged=1;
        }else{return 1;}


    }else if(argc==6){//Kreuz Bearbeiten "edit [KreuzName] [NeuAutobahnName1] [NeuKM1] [NeuAutobahnName2] [NeuKM2]"

        char *lastparam = argv[5];
        chop(lastparam);

        int ReturnValue=EditKreuz( meineKnoten,AnzahlKnoten,argv[1],argv[2],argv[3],argv[4],lastparam);

        if(ReturnValue!=INT_MAX){
            AnzahlKnoten=ReturnValue;
            needReload=1;
            DataChanged=1;
        }else{return 1;}

    }else{
        printMenuHeader(Typo);
    }

    return 0;
}

int Delete(int argc, char *argv[]){

    char *Typo="    Der Delete Befehl wurde Falsch angewendet , \"rm -h\" für hilfe";

    if(argc==2) {

        if(argv[1][0]=='-' && argv[1][1]=='h') {//TODO -hs -hallo -hafaf sind noch möglich

            printMenuHeader(" Hilfe für den Remove befehl ");
            printMenuHeaderContinous("");
            printMenuHeaderContinous("Usage: rm [ObjektName]    -> Ein objekt kann Ausfahrt,Kreuz,Autobahn sein");
            printMenuHeaderContinous("Beispiel: rm Heilbronn");
            printMenuHeaderContinous("Beispiel: rm A1");
            printFooter();

            return 0;

        }else{
            char *K1 = argv[1];
            chop(K1);

            int ReturnValue=Remove( meineKnoten,AnzahlKnoten,K1);

            if(ReturnValue!=INT_MAX){
                AnzahlKnoten=ReturnValue;
                meineKnoten = realloc(meineKnoten,sizeof(struct Knoten)*(AnzahlKnoten));
                needReload=1;
                DataChanged=1;
            }else{return 1;}

        }

    }else{
        printMenuHeader(Typo);
        return 1;
    }

    return 0;
}



int New(int argc, char *argv[]){

    char *Typo="   Der New befehl wurde falsch benutzt , \"new -h\" für hilfe";

    if(argc==4) {// new [AusfahrtName] [AutobahnName] [AutobahnKm]

        char *lastparam=argv[3];
        chop(lastparam);


        meineKnoten = realloc(meineKnoten,sizeof(struct Knoten)*(AnzahlKnoten+1));
        int ReturnValue = NewAusfahrt( meineKnoten,AnzahlKnoten,argv[1],argv[2],lastparam,0);

        if(ReturnValue!=INT_MAX){
            AnzahlKnoten=ReturnValue;
            needReload=1;
            DataChanged=1;
        }else{
            meineKnoten = realloc(meineKnoten,sizeof(struct Knoten)*(AnzahlKnoten));
            return 1;
        }

        return 0;

    }else if(argc==6){// new [AusfahrtName] [AutobahnName_1] [AutobahnKm_1] [AutobahnName_2] [AutobahnKm_2]

        char *lastparam=argv[5];
        chop(lastparam);

        meineKnoten = realloc(meineKnoten,sizeof(struct Knoten)*(AnzahlKnoten+2));
        int ReturnValue = NewKreuz( meineKnoten,AnzahlKnoten,argv[1],argv[2],argv[3],argv[4],lastparam);

        if(ReturnValue!=INT_MAX){
            AnzahlKnoten=ReturnValue;
            needReload=1;
            DataChanged=1;
        }else{
            meineKnoten = realloc(meineKnoten,sizeof(struct Knoten)*(AnzahlKnoten+1));
            return 1;
        }

        return 0;

    }else if(argc==2){// new -h

        if(argv[1][0]=='-' && argv[1][1]=='h') {//TODO -hs -hallo -hafaf sind noch möglich
            printMenuHeader(" Hilfe für den Remove befehl ");
            printMenuHeaderContinous("");
            printMenuHeaderContinous("Usage: rm [ObjektName]    -> Ein objekt kann Ausfahrt,Kreuz,Autobahn sein");
            printMenuHeaderContinous("Beispiel: rm Heilbronn");
            printMenuHeaderContinous("Beispiel: rm A1");
            printFooter();
            return 0;
        }else{
            puts("\n");
            printMenuHeaderContinous(Typo);
        }

    }else{//Falscheingabe
        puts("\n");
        printMenuHeaderContinous(Typo);
    }

    return 1;
}


int StartupMenu(int argc, char *argv[]){

    system(CLEAR);

    SetMenuTitle("Autobahn.C Startbildschirm");
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
        
        
        meineKnoten = malloc(sizeof(struct Knoten)*AnzahlKnoten);

        loadDatabaseFiletoStruct(meineKnoten,AnzahlKnoten);
        loaded=1;

        system(CLEAR);

        sprintf(Buffer,"%d Datensätze wurden erfolgreich verarbeitet.",AnzahlKnoten);
        printMenuHeader("Daten Geladen");
        printMenuItem(Buffer);
        printFooter();
        puts("\n");
    }else if(needFullReload||needReload) {
        system(CLEAR);
        sprintf(Buffer,"Einen Moment ,  Änderungen für %d Daten werden verarbeitet.",AnzahlKnoten);
        printMenuHeader("Wende Änderungen an");
        printMenuItem(Buffer);
        printFooter();
        puts("\n");

        ConnectData( meineKnoten, AnzahlKnoten);

        needReload=0;
        needFullReload=0;
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
    AddCMD("nav", "für eine Navigation (search -h für Erklärung des Befehls)", Navigate);
    AddCMD("ls", "für Navigationsbefehle (ls -h für Erklärung des Befehls)", Search);
    AddCMD("back", "um Zurück zum Hauptmenu zu kommen", MainMenu);

    showUserCMDHelp();

    return 0;
}


int exit_n(int argc, char *argv[]){
    exit(0);
}

int runsave(int argc,char *argv[]){

    char *Buffer= malloc(sizeof(char)*1000);

    system(CLEAR);
    sprintf(Buffer,"Einen Moment , %d Datensätze werden gespeichert.",AnzahlKnoten);
    printMenuHeader("Speichere Daten");
    printMenuItem(Buffer);
    printFooter();
    puts("\n");

    saveStructToFile( meineKnoten, AnzahlKnoten);

    system(CLEAR);

    sprintf(Buffer,"%d Datensätze wurden Erfolgreich gespeichert.",AnzahlKnoten);
    printMenuHeader("Daten Erfolgreich Gespeichert");
    printMenuItem(Buffer);
    printFooter();
    puts("\n");

    free(Buffer);
    exit(0);
}

int saveIt(int argc, char *argv[]){

    if(DataChanged) {

        system(CLEAR);

        ResetAllCMDs();
        SetMenuTitle("Wollen sie ihre Änderungen Speichern?");

        AddCMD("y", "Ja Speichern", runsave);
        AddCMD("n", "Nicht Speichern", exit_n);
        AddCMD("ja", "Ja Speichern", runsave);
        AddCMD("nein", "Nicht Speichern", exit_n);
        AddCMD("yes", "Ja Speichern", runsave);
        AddCMD("no", "Nicht Speichern", exit_n);
        AddCMD("j", "Ja Speichern", runsave);
        AddCMD("n", "Nicht Speichern", exit_n);

        printMenuHeader("  Wollen sie ihre Änderungen Speichern?  ");
        printMenuItem("  y oder n für Ja oder Nein  ");
        printFooter();
        return 0;
    }else{
        exit(0);
    }

}
