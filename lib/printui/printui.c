/**************************************************************************************************/
/* Copyright (C) mc2lab.com, SSE@USTC, 2014-2015                                                  */
/*                                                                                                */
/*  FILE NAME             :  printui.C                                                            */
/*  PRINCIPAL AUTHOR      :  Florian Braun                                                        */
/*  SUBSYSTEM NAME        :  printu                                                               */
/*  MODULE NAME           :  printui                                                              */
/*  LANGUAGE              :  C                                                                    */
/*  TARGET ENVIRONMENT    :  ANY (C99)                                                            */
/*  DATE OF FIRST RELEASE :  2015/01/25                                                           */
/*  DESCRIPTION           :  Programm to print nice output in a window                            */
/**************************************************************************************************/

//INFOS----------------
//Default CMD size(Windows) : 80x25
//Default Terminal size(Unix) : 80x24
//INFOS----------------

//TODO Püfen ob ausgame nicht zu lang ist als es dürfte , was zu verschiebung der zeichen führt
//TODO /n löschen wenn win32 da dort /n einen character belegt und deswegen eine neue zeile beginnt.
//TODO Footer spalten zeichen (vertkal) hinzufügen


//Code Begin

#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


//FunktionsDeklaration
int countUTF8String(char *s);

//vc=VerticalCaracter | hc = horizontalCharacter
static const char *hc = "┅";
static const char *vc = "┋";
//EDGES
static const char *lo = "┏";
static const char *ro = "┓";

static const char *lu = "┗";
static const char *ru = "┛";

//Terminal Breite , terminal wird auf win32 und unix systemen resized siehe src/ConsoleAdaption
static const int terminalWidth = 120;

//New Line Char , bei windows muss sie wieder gelöscht werden da sie ein zeichen verbraucht

#ifdef _WIN32
unsigned char *nlc = "\n";
#else
unsigned char *nlc = "\n";
#endif

//##############
//### Formatierungs Berechnung / Ausgabefalse
//##############

int getTerminalWidth(){
    return terminalWidth;
}

void center_print_row_segment(char *s, int width,bool isHeading,bool isLast)
{


    int length = countUTF8String(s);
    int i;
    fputs(vc, stdout);
    for (i=1; i<((width-length)/2); i++) {
        if(isHeading){
            fputs(hc, stdout);
        }else{
            fputs(" ", stdout);
        }
    }

    fputs(s, stdout);
    i += length;
    for (; i<=width-2; i++) {
        if(isHeading){
            fputs(hc, stdout);
        }else{
            fputs(" ", stdout);
        }
    }
    fputs(vc, stdout);



    if(isLast) {
        fputs(nlc, stdout);
    }


}


//MODE 0=Header 1=Footer 2=Durchgehende Linie
void center_print_menu_header(char *s, int width,int Mode)
{

    int length = countUTF8String(s);
    int i;

    if(Mode ==1){  fputs(lu,stdout);  }else if(Mode ==0){fputs(lo,stdout);}else{fputs(hc,stdout);}
    for (i=1; i<=(width-length)/2; i++) {
        fputs(hc, stdout);
    }
    fputs(s, stdout);
    i += length;
    for (; i<=width-2; i++) {

        fputs(hc, stdout);
    }

    if(Mode ==1){  fputs(ru,stdout);  }else if(Mode ==0){fputs(ro,stdout);}else{fputs(hc,stdout);}


}

void center_print_menu(char *s, int width)
{

    int length = countUTF8String(s);
    int i;

    fputs(vc, stdout);
    for (i=1; i<=(width-length)/2; i++) {
        fputs(" ", stdout);
    }
    fputs(s, stdout);
    i += length;

    for (; i <= width - 2; i++) {
        fputs(" ", stdout);
    }


    fputs(vc, stdout);

    fputs(nlc,stdout);


}

//Zähler muss UTF-8 Konform sein wegen umlauten u.ä ,da funtion in der standartlib dies nichtkann nund umlaute etc. nicht mitzählt
int countUTF8String(char *s) {
    int i = 0, j = 0;

    while (s[i]) {
        if ((s[i] & 0xc0) != 0x80) j++;
        i++;

    }
    return j;
}

//##############
//### Menu Aufbau
//##############

//Gibt die Menuüberschrift aus
void printMenuHeader(char *text){

    center_print_menu_header(text, terminalWidth,0);

}
void printMenuHeaderContinous(char *text){

    center_print_menu_header(text, terminalWidth,3);

}

//Gibt einen menueintrag aus
void printMenuItem(char *text){

    center_print_menu(text, terminalWidth);

}

//##############
//### Datenausgabe
//##############

//Gibt eine x Spaltige Tabelle Aus
void printTabelHeader(int numCols,...){


    va_list ap;
    int i;

    va_start(ap, numCols);
    for(i = 0; i < numCols-1; i++) {
        center_print_row_segment(va_arg(ap, char *), terminalWidth / numCols, true, false);
    }
        center_print_row_segment(va_arg(ap, char *),(terminalWidth-i*(terminalWidth / numCols)), true, true);


    va_end(ap);




}
void printTabelRow(char numCols,...){


    va_list ap;
    int i;

    va_start(ap, numCols);
    for(i = 0; i < numCols-1; i++) {
        center_print_row_segment(va_arg(ap, char *), terminalWidth / numCols, false, false);
    }
      center_print_row_segment(va_arg(ap, char *),(terminalWidth-i*(terminalWidth / numCols)), false, true);

    va_end(ap);


}

//Allgemeine schlusszeile
void printFooterContinous(){
    center_print_row_segment("", terminalWidth , true, true);
}

void printFooter(){
    center_print_menu_header("", terminalWidth,1);
}
