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
//Default CMD size : 80x25
//Default Terminal size : 80x24
//INFOS----------------


//Deklarationen

#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

//vc=VerticalCaracter | hc = horizontalCharacter
static const char hc[] = "-";
static const char vc[] = "|";

//Terminal Breite
static const int terminalWidth = 80;

//##############
//### Formatierungs Berechnung / Ausgabefalse
//##############


void center_print_row_segment(const char *s, int width,bool isHeading,bool isLast)
{


    int length = countUTF8String(s);
    int i;
    fputs(vc, stdout);
    for (i=1; i<=((width-length)/2); i++) {
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
        fputs("\n", stdout);
    }


}



void center_print_menu_header(const char *s, int width)
{

    int length = countUTF8String(s);
    int i;


    fputs(vc,stdout);
    for (i=1; i<=(width-length)/2; i++) {
        fputs(hc, stdout);
    }
    fputs(s, stdout);
    i += length;
    for (; i<=width-2; i++) {

        fputs(hc, stdout);
    }
    fputs(vc,stdout);
    fputs("\n", stdout);


}

void center_print_menu(const char *s, int width)
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
    //printf("%d",(width-length)%2);
    fputs("\n",stdout);


}

//Zähler muss UTF-8 Konform sein wegen umlauten u.ä
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

    center_print_menu_header(text, terminalWidth);

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
    if((numCols%3)>0){
        center_print_row_segment(va_arg(ap, char *), terminalWidth / numCols, true, true);
    }else{
        center_print_row_segment(va_arg(ap, char *), (terminalWidth / numCols)+2, true, true);
    }

    va_end(ap);




}
void printTabelRow(char numCols,...){


    va_list ap;
    int i;

    va_start(ap, numCols);
    for(i = 0; i < numCols-1; i++) {
        center_print_row_segment(va_arg(ap, char *), terminalWidth / numCols, false, false);
    }
    if((numCols%3)>0){
        center_print_row_segment(va_arg(ap, char *), terminalWidth / numCols, false, true);
    }else{
        center_print_row_segment(va_arg(ap, char *), (terminalWidth / numCols)+2, false, true);
    }

    va_end(ap);



}

//Allgemeine schlusszeile
void printFooter(){
    center_print_row_segment("", terminalWidth , true, true);
}


