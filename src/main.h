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

int main(void);

int MainMenu(int argc, char *argv[]);
int EditMenu(int argc, char *argv[]);
int NavMenu(int argc, char *argv[]);


#endif