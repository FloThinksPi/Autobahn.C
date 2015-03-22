/**************************************************************************************************/
/* Copyright (C) mc2lab.com, SSE@USTC, 2014-2015                                                  */
/*                                                                                                */
/*  FILE NAME             :  printui.h                                                            */
/*  PRINCIPAL AUTHOR      :  Florian Braun                                                        */
/*  SUBSYSTEM NAME        :  printu                                                               */
/*  MODULE NAME           :  printui                                                              */
/*  LANGUAGE              :  C                                                                    */
/*  TARGET ENVIRONMENT    :  ANY (C99)                                                            */
/*  DATE OF FIRST RELEASE :  2015/01/25                                                           */
/*  DESCRIPTION           :  Programm to print nice output in a window                            */
/**************************************************************************************************/

#ifndef PRINTUI_H
#define PRINTUI_H

//X-Spaltige Tabelle Ausgeben ( numCols= Spaltenanzahl , gefolgt von genausovielen strings)
//bsp. printTabelHeader(4,"1. Spalte","2. Spalte","3. Spalte","4.Spalte")
void printTabelHeader(int numCols,...);//Tabellenueberschrift
void printTabelRow(char numCols,...);//TabellenZeile

//Allgemeinen Footer ausgeben
void printFooter();//Endzeile
void printFooterText(char *S);//Endzeile mit Text

//Menu Ausgeben
void printMenuHeader(char *text);//Menu ueberschrift
void printMenuHeaderContinous(char *text);//Durchgehende Menuuberschrift(Ohne Rand)
void printMenuItem(char *text);//Menu eintrag

//Gibt die Terminalbreite zurück die hier gesetzt wird
int getTerminalWidth();

#endif