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

int StartupMenu(int argc, char *argv[]);//Solve Warning

//Wählt die richtige main für UNIX und Windows systeme
//Unter Windows Werden Regestry einträge für eine Konsole erstellt und danach das programm in dieser konsole Neu gestartet.
//Zum debuggen muss dieses "Feautre" Ausgestellt werden , siehe komentare unten , da sonst sich das programm neu startet -> Dabugger nutzlos.
#ifdef _WIN32
const char *skipParameter="\\CMDknowsUTF8";//Bei diesem Startparameter weis das programm das es in einer UTF8 Kompatiblen Konsole läuft
int main (int argc, char *argv[])
{
    if(argv[1]!=NULL){
        if(strcmp(argv[1],skipParameter)){
           RestartInUtf8Cmd();//Comment For debugging
           // ConfigureCMD();//Uncomment for debugging
           // StartupMenu(0,NULL);//Uncomment for debugging
        }else{
            ConfigureCMD();
            StartupMenu(0,NULL);
        }
    }else{
    RestartInUtf8Cmd();//Comment For debugging
        // ConfigureCMD();//Uncomment for debugging
        // StartupMenu(0,NULL);//Uncomment for debugging
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


int AnzahlKnoten;//Globale Anzahl von Datensätze (Knoten genannt)
struct Knoten *meineKnoten;//Globale Variable die die "Datenbank" enthält

int loaded=0;//Daten Geladen ?
int needReload=0;//Müssen Daten (Teilweise) Neu geladen Werden?
int needFullReload=0;//Müssen Daten Komplett neu Verarbeitet Werden?
int DataChanged=0;//Hat sich die Datenbank Verändert? , dadurch wird entschieden ob der dialog änderungen speichern bei exit kommt.

int Navigate(int argc, char *argv[]){//Funktion die durch "nav" im Navigationsmenu getriggert wird , erhält die eingabe und trennt parameter , entscheidet was getan wird

    char *Typo="   Falsche benutzung der Navigation : \"nav -h\" für hilfe   ";//Kurze Funktionsbeschreibung , soll bei falscheingabe kommen (falsche syntax)

    if(argc==3) {// Stimmt die anzahl der Parameter? 3 Parameter ~ nav [StartAusfahrt] [ZielAusfahrt]

        char *K2 = argv[2];
        chop(K2);//Zeilenumbruch beim letzten Parameter entfernen

        Nav(meineKnoten,AnzahlKnoten,argv[1],K2);

    }else if(argc==2){//Hilfe (nav -h)
        if(argv[1][0]=='-' && argv[1][1]=='h'){//TODO -hs -hallo -hafaf sind noch möglich
            printTabelHeader(1," Hilfe für Navigationsbefehl ");
            printTabelRow(1," Dieser Befehl Findet den Weg von A nach B (Ausfahrt oder Kreuz möglich)");
            printTabelRow(1," Usage: nav [StartAusfahrt] [ZielAusfahrt] ");
            printTabelRow(1," Beispiel: nav Heilbronn Stuttgart");
            printFooter();
        }
    }
    else{// Falscheingabe
        puts("\n");
        printMenuHeaderContinous(Typo);
    }

}


int Search(int argc, char *argv[]){// LS befehl , zeigt ausfahrten bzw autobahnen.

    char *Typo="   Falsche benutzung des List befehls : \"ls -h\" für hilfe   ";

    if(argc>=2&&argc<=4) {//Erlaubte anzahl von Parametern (-h kann z.b. überall stehen)

        if(needReload) {// Wird benötigt um Wege neu zu knüpfen , da Wege nur für Nav und Ls wichtig sind wird nicht nach jedem Edit neu Geladen -> Performance
            char *Buffer= malloc(sizeof(char)*1000);

            system(CLEAR);
            sprintf(Buffer,"Einen Moment ,  Änderungen für %d Daten werden verarbeitet.",AnzahlKnoten);
            printMenuHeader("Wende Änderungen an");
            printMenuItem(Buffer);
            printFooter();
            puts("\n");

            ConnectData(meineKnoten,AnzahlKnoten);//TODO RELOAD FEHLER BEI NEW KREUZEN
            //OnlyConnectKreuze( meineKnoten->meineKnoten, AnzahlKnoten);

            needReload=0;
            needFullReload=1;//Full reload beim wechseln von edit in nav modus nötig
            system(CLEAR);

            sprintf(Buffer,"%d Datensätze wurden Erfolgreich verarbeitet.",AnzahlKnoten);
            printMenuHeader("Änderungen Geladen");
            printMenuItem(Buffer);
            printFooter();
            puts("\n");

            free(Buffer);
        }


        int hasparam=0;//ist ein Parameter vorhanden z.b. -t
        int textonly=0;// ist -t da
        int sortName=0;// ist -n da


        size_t optind;
        for (optind = 0; optind < argc ; optind++) {//Parameter auswerten
            if(argv[optind][0] == '-') {
                switch (argv[optind][1]) {
                    case 't':// TabellenModus
                        hasparam = 1;
                        textonly=1;
                        break;
                    case 'n':// Sortieren nach Name
                        hasparam = 1;
                        sortName=1;
                        break;
                    case 'a'://Bei -a werden einfach alle atobahnen ausgegeben und die funktion returned.

                        SearchAutobahnen( meineKnoten,AnzahlKnoten);
                        return 0;

                    case 'h':// HIlfe anzeiegen , danach abrechen
                        printTabelHeader(1,"Hilfe für den \"ls\" Befehl");
                        printTabelRow(1,"Geben Sie ls [Stadt/Kreuzname] ein (z.B. 'ls Heilbronn'),");
                        printTabelRow(1,"um die Autobahn auf der sich die Stadt oder das Kreuz befindet, anzeigen zu lassen.");
                        printTabelRow(1,"");
                        printTabelRow(1,"Es werden alle Städte und Kreuze mit dem Abstand zum Anfang der Autobahn angezeigt.");
                        printTabelRow(1,"-t Zeigt die Autobahn in einer Tabelle (Kompakte Ansicht)");
                        printTabelRow(1,"-n Sortiert die Ausgabe nach Namen (Funktioniert nur mit -t)");
                        printTabelRow(1,"ls -a Zeigt alle Autobahnen die es gibt.");
                        printFooter();
                        return 0;
                    default://Falscher Parameter
                        printMenuHeaderContinous(Typo);//Kurzhilfe eingeben
                        return 1;//Return Error
                }
            }
        }

        if(textonly==0 && sortName==1){//Für sorteren nach namen braucht man Tabellenausgabe

            puts("");
            printMenuHeaderContinous("  Das sortieren Nach Namen Funtioniert nur im TabellenModus (mit parameter -t )  ");

            return 1;
        }

        char *K1 = argv[1];
        if(hasparam==0){// das "\n" weglöschen da K1 letzter parameter ist
            chop(K1);
        }

        SearchNormal( meineKnoten,AnzahlKnoten,K1,textonly,sortName);//Suche Starten , Backend der Suche die mit Daten zu tun hat , main ist nur für UI + management

    }else{//Falsche eingabe , anzahl Parameter stimmt nicht
        puts("\n");
        printMenuHeaderContinous(Typo);//Kurzhilfe
    }

}

int Edit(int argc, char *argv[]){// Management des Edit befehls

    char *Typo="   Der Edit Befehl wurde Falsch genutzt , \"edit -h\" für hilfe    ";//Kurzhilfe

    if(argc==2) {// Hilfe anzeigen

        if(argv[1][0]=='-' && argv[1][1]=='h') {//TODO -hs -hallo -hafaf sind noch möglich

            printTabelHeader(1," Hilfe für den Edit befehl ");
            printTabelRow(1,"Umbenennen: Edit [ObjektName] [NeuerObjektName]    -> Ein objekt kann Ausfahrt,Kreuz,Autobahn sein");
            printTabelRow(1,"Ausfahrt Ändern: Edit [AusfahrtName] [NeueAutobahn] [NeuerAtobahnKm]");
            printTabelRow(1,"Kreuz Ändern: Edit [KreuzName] [NeueAutobahn_1] [NeuerAtobahnKm_1] [NeueAutobahn_2] [NeuerAtobahnKm_2]");
            printTabelRow(1,"");
            printTabelRow(1,"Beispiel Umbenennen: \"edit Mosbach MOS\" oder \"edit A1 Aeins\"");
            printTabelRow(1,"Beispiel Ausfahrt: \"edit Mosbach A1 10\" oder \"edit Heilbronn A8 99.99\"");
            printTabelRow(1,"Beispiel Kreuz: \"edit K1_2 A1 11 A2 93.23\" oder \"edit MeinKreuz A2 93.2 A999 39.2\"");
            printFooter();
            return 0;

        }

    }else if(argc==3) {//Umbenennung "edit [AlterName] [NeuerName]" Kann Kreuz,Ausfahrt,Autobahn umbenennen

        char *K1 = argv[1];
        char *K2 = argv[2];
        chop(K2);// "\n" löschen

        int ReturnValue = EditName( meineKnoten,AnzahlKnoten,K1,K2);

        if(ReturnValue!=INT_MAX){// INT_MAX ist ERROR
            AnzahlKnoten=ReturnValue;// Anzahl Knoten sollte Gleich bleiben
            needReload=1;
            DataChanged=1;
        }else{return 1;}

    } else if(argc==4) {//Ausfahrt Bearbeiten "edit [KnotenName] [NeueAutobahn] [NewKM]"

        char *lastparam = argv[3];
        chop(lastparam);

        int ReturnValue=EditAusfahrt( meineKnoten,AnzahlKnoten,argv[1],argv[2],lastparam);

        if(ReturnValue!=INT_MAX){// INT_MAX ist ERROR
            AnzahlKnoten=ReturnValue;// Anzahl Knoten sollte Gleich bleiben , falls aus einem Kreuz ein Knoten gemacht wird kann sich das ändern.
            needReload=1;
            DataChanged=1;
        }else{return 1;}


    }else if(argc==6){//Kreuz Bearbeiten "edit [KreuzName] [NeuAutobahnName1] [NeuKM1] [NeuAutobahnName2] [NeuKM2]"

        char *lastparam = argv[5];
        chop(lastparam);

        int ReturnValue=EditKreuz( meineKnoten,AnzahlKnoten,argv[1],argv[2],argv[3],argv[4],lastparam);

        if(ReturnValue!=INT_MAX){//INT_MAX ist ERROR
            AnzahlKnoten=ReturnValue;// Anzahl Knoten sollte Gleich bleiben , falls aus einem Kreuz ein Knoten gemacht wird kann sich das ändern.
            needReload=1;
            DataChanged=1;
        }else{return 1;}

    }else{//Falsche anzahl Parameter
        printMenuHeader(Typo);//Kurzhilfe
        return 1;
    }

    return 0;
}

int Delete(int argc, char *argv[]){// "rm" befehl , zum löschen von Ausfahrten, Kreuzen und Autobahnen

    char *Typo="    Der Delete Befehl wurde Falsch angewendet , \"rm -h\" für hilfe";

    if(argc==2) {//Nur 1 Parameter ist gültig

        if(argv[1][0]=='-' && argv[1][1]=='h') {//TODO -hs -hallo -hafaf sind noch möglich

            printTabelHeader(1," Hilfe für den Remove befehl ");
            printTabelRow(1,"Usage: rm [ObjektName]    -> Ein objekt kann Ausfahrt,Kreuz,Autobahn sein");
            printTabelRow(1,"Beispiel: rm Heilbronn");
            printTabelRow(1,"Beispiel: rm A1");
            printFooter();
            return 0;

        }else{// Parameter ist nicht -h , soll somit gelöscht werden
            char *K1 = argv[1];
            chop(K1);// "\n" entfernen

            int ReturnValue=Remove( meineKnoten,AnzahlKnoten,K1);// Aufruf des Remove Backends, denn Main nur UI + Management

            if(ReturnValue!=INT_MAX){//INT_MAX = ERROR
                AnzahlKnoten=ReturnValue;// Anzahl An Datensätzen anpassen die nach dem löschen noch da sind)
                meineKnoten = realloc(meineKnoten,sizeof(struct Knoten)*(AnzahlKnoten));// Es sind weniger knoten Geworden -> speicher verkleinern soll nur in Main sein wo Globale variable ist.
                needReload=1;
                DataChanged=1;
            }else{return 1;}

        }

    }else{// Falsche anzahl Parameter
        printMenuHeader(Typo);//Kurzfehler
        return 1;
    }

    return 0;
}



int New(int argc, char *argv[]){// "new" befehl zum erstellen von Ausfahrten und Kreuze , falls Autobahnnicht existiert wird diese angelegt

    char *Typo="   Der New befehl wurde falsch benutzt , \"new -h\" für hilfe";//Kurzhilfe

    if(argc==4) {// new [AusfahrtName] [AutobahnName] [AutobahnKm]

        char *lastparam=argv[3];
        chop(lastparam);// "\n" entfernen


        meineKnoten = realloc(meineKnoten,sizeof(struct Knoten)*(AnzahlKnoten+1));// Mehr Speicher zuweisen (+1 da nur eine Ausfahrt)
        int ReturnValue = NewAusfahrt( meineKnoten,AnzahlKnoten,argv[1],argv[2],lastparam,0);// Backend von New Aufrufen die Daten dann hinzufügrt denn Main ist nur management + UI

        if(ReturnValue!=INT_MAX){
            AnzahlKnoten=ReturnValue;//Anzahl Knoten Anpassen , neue knoten ist dazugekommen
            needReload=1;
            DataChanged=1;
        }else{
            meineKnoten = realloc(meineKnoten,sizeof(struct Knoten)*(AnzahlKnoten));// Da fehlgeschlagen , den speicher wieder verkleinern
            return 1;
        }

        return 0;

    }else if(argc==6){// new [AusfahrtName] [AutobahnName_1] [AutobahnKm_1] [AutobahnName_2] [AutobahnKm_2]

        char *lastparam=argv[5];
        chop(lastparam);

        meineKnoten = realloc(meineKnoten,sizeof(struct Knoten)*(AnzahlKnoten+2));// Mehr Speicher zuweisen (+2 da nur ein Kreuz 2 datensätze benötigt)
        int ReturnValue = NewKreuz( meineKnoten,AnzahlKnoten,argv[1],argv[2],argv[3],argv[4],lastparam);// Backend von New Aufrufen die Daten dann hinzufügrt denn Main ist nur management + UI

        if(ReturnValue!=INT_MAX){
            AnzahlKnoten=ReturnValue;//Anzahl Knoten Anpassen , neue 2 knoten dazugekommen
            needReload=1;
            DataChanged=1;
        }else{
            meineKnoten = realloc(meineKnoten,sizeof(struct Knoten)*(AnzahlKnoten));// Da fehlgeschlagen , den speicher wieder verkleinern
            return 1;
        }

        return 0;

    }else if(argc==2){// new -h

        if(argv[1][0]=='-' && argv[1][1]=='h') {//TODO -hs -hallo -hafaf sind noch möglich
            printTabelHeader(1," Hilfe für den New befehl ");
            printTabelRow(1,"Neue Ausfahrt: new [Name] [Autobahn] [AutobahnKM]");
            printTabelRow(1,"Neues Kreuz: new [Name] [Autobahn1] [AutobahnKM1] [Autobahn2] [AutobahnKM2]");
            printTabelRow(1,"Beispiel Ausfahrt: new Kuenzelsau A6 88.32");
            printTabelRow(1,"Beispiel Kreuz: new Weisberg A6 73.23 A81 123.23");
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


int StartupMenu(int argc, char *argv[]) {// SplashScreen mit Infos

    system(CLEAR);

    AddCMD("", "", MainMenu);// Befehl"Bitte enter drücken" registrieren

    printMenuHeader("   Autobahn.C Startbildschirm   ");
    printMenuItem("Drücken Sie Enter um zu Starten");
    printFooter();

    printMenuItem("Folgende Befehle funktionieren überall im Programm:");
    printMenuItem("\"help\" zeigt alle verfügbaren Befehle an");
    printMenuItem("\"clear\" leert die Konsole");
    printMenuItem("\"exit\" Beendet das Programm Ordnungsgemäs, dadurch wird die Datenbank gespeichert");
    printFooter();

    printMenuItem("Dieses Programm wurde estellt von [Florian Braun][Tobi][Maurice Eble][Lena Hoinkis]");
    printMenuItem("Viel Spaß");
    printFooter();

    //Alles mus vor CMDStart sein da dort eine endlosschleife gestartet wird die erst bei eingabe endet(sucht nach eingabe)

    StartCMDSystem();//Menu System Starten (einmalig nötig)
    return 0;
}


int MainMenu(int argc, char *argv[]){//Hauptmenu , auswahl zwischen Nav und Edit Menu

    char *Buffer= malloc(sizeof(char)*1000);
    if(loaded==0){// Daten wurden noch nicht aus Datei Geladen , also Speicher für unsere Datenstruktur im Ram holen und Daten Einlesen.

        system(CLEAR);//Terminal leeren
        AnzahlKnoten=getNumKnoten();//Anzahl an datensätzen aus datei lesen

        printMenuHeader("Lade Daten");
        sprintf(Buffer,"Einen Moment , %d Datensätze werden verarbeitet.",AnzahlKnoten);
        printMenuItem(Buffer);
        printFooter();
        puts("\n");

        meineKnoten = malloc(sizeof(struct Knoten)*AnzahlKnoten);// Speicher holen
        loadDatabaseFiletoStruct(meineKnoten,AnzahlKnoten);//Datenbank in speicher Laden
        loaded=1;

        system(CLEAR);

        sprintf(Buffer,"%d Datensätze wurden erfolgreich verarbeitet.",AnzahlKnoten);
        printMenuHeader("Daten Geladen");
        printMenuItem(Buffer);
        printFooter();
        puts("\n");

    }else if(needFullReload||needReload) {// Falls änderungen im Edit Menu gemacht Wurden

        system(CLEAR);
        sprintf(Buffer,"Einen Moment ,  Änderungen für %d Daten werden verarbeitet.",AnzahlKnoten);
        printMenuHeader("Wende Änderungen an");
        printMenuItem(Buffer);
        printFooter();
        puts("\n");

        ConnectData( meineKnoten, AnzahlKnoten);// Neu verarbeiten der Daten (Wege erstellen und Kreuze verbinden)
        needReload=0;
        needFullReload=0;

        system(CLEAR);
        sprintf(Buffer,"%d Datensätze wurden Erfolgreich verarbeitet.",AnzahlKnoten);
        printMenuHeader("Änderungen Geladen");
        printMenuItem(Buffer);
        printFooter();
        puts("\n");

    }else{// Keine änderungen , nichts laden
            system(CLEAR);
        }

    ResetAllCMDs();// Alle befehle die registriert wurden freigeben , auf diese werden nicht mehr "gehört"

    SetMenuTitle("Hauptmenu");// Menu Titel
    AddCMD("edit", "um das Bearbeitungs Menu zu öffnen", EditMenu);//Befehl Registrieren
    AddCMD("nav", "um das Navigations Menu zu öffnen", NavMenu);//Befehl Registrieren
    showUserCMDHelp();//User Befehle anzeigen (Nicht hilfe, clear , exit)

    free(Buffer);// Buffer freigeben
    return 0;
}


int EditMenu(int argc, char *argv[]){//BearbeitungsMenu , Kommentare siehe Main da es die selben sind

    system(CLEAR);

    ResetAllCMDs();
    SetMenuTitle("Editier Menu");

    AddCMD("edit", "um Ausfahrten/Kreuze/Autobahnen zu bearbeiten (edit -h für Erklärung des Befehls)", Edit);
    AddCMD("new", "um Ausfahrten/Kreuze/Autobahnen zu Erstellen (new -h für Erklärung des Befehls)", New);
    AddCMD("rm", "um Ausfahrten/Kreuze/Autobahnen zu Löschen (rm -h für Erklärung des Befehls)", Delete);
    AddCMD("ls", "für die Suche nach Daten(ls -h für Erklärung des Befehls)", Search);
    AddCMD("back", "um Zurück zum Hauptmenu zu kommen", MainMenu);

    showUserCMDHelp();

    return 0;
}


int NavMenu(int argc, char *argv[]){//Navigations Menu , Kommentare siehe Main da es die selben sind

    system(CLEAR);

    ResetAllCMDs();
    SetMenuTitle("Navigations Menu");
    AddCMD("nav", "für eine Navigation (nav -h für Erklärung des Befehls)", Navigate);
    AddCMD("ls", "für die Suche nach Daten(ls -h für Erklärung des Befehls)", Search);
    AddCMD("back", "um Zurück zum Hauptmenu zu kommen", MainMenu);

    showUserCMDHelp();

    return 0;
}


int exit_n(int argc, char *argv[]){// Wird ausgeführt falls keine Speicerung von änderungen Nötig oder Gewollt wird.
    exit(0);
}

int runsave(int argc,char *argv[]){// Speicerung wird beim exit gewünscht

    char *Buffer= malloc(sizeof(char)*1000);// Zwischenspeicher

    system(CLEAR);
    sprintf(Buffer,"Einen Moment , %d Datensätze werden gespeichert.",AnzahlKnoten);
    printMenuHeader("Speichere Daten");
    printMenuItem(Buffer);
    printFooter();
    puts("\n");

    saveStructToFile( meineKnoten, AnzahlKnoten);// Aufruf der funktion die die Daten in die Datei Speichert

    system(CLEAR);
    sprintf(Buffer,"%d Datensätze wurden Erfolgreich gespeichert.",AnzahlKnoten);
    printMenuHeader("Daten Erfolgreich Gespeichert");
    printMenuItem(Buffer);
    printFooter();
    puts("\n");

    free(Buffer);
    exit(0);
}

int saveIt(int argc, char *argv[]){// Dialog ob änderungen gespeichert werden sollen. der Exit dialog ob wirklich verlassen werden soll ist in der Menu lib

    if(DataChanged) {// Änderungen sind vorhanden , also speichern oder nicht ?

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

    }else{//Keine änderungen vorhanden , also nichts zu speichern
        exit(0);
    }

}
