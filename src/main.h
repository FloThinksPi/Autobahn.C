/* 
 * File:   main.h
 * Author: flo
 *
 * Created on 13. Januar 2015, 12:11
 */

#ifndef MAIN_H
#define MAIN_H

#ifdef _WIN32
    #define CLEAR "cls"
#else // In Nicht Windows Systemen ist CLEAR = clear , in windows ist CLEAR = cls
#define CLEAR "clear"
#endif

#include "dbio.h"

//Struct Hack dient um ein Globales undefiniert dynamisch groﬂes array zu schaffen , erst seit C99 Erlaubt.
struct UndefArrayHack {
    int placeholder;//Muss vorhanden sein
    struct Knoten *meineKnoten[];//Dynamisch groﬂes Array
};

int main (int argc, char *argv[]);


int MainMenu(int argc, char *argv[]);
int EditMenu(int argc, char *argv[]);
int NavMenu(int argc, char *argv[]);

int saveIt(int argc, char *argv[]);

#endif