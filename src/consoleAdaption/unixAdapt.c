/**************************************************************************************************/
/* Copyright (C) mc2lab.com, SSE@USTC, 2014-2015                                                  */
/*                                                                                                */
/*  FILE NAME             :  unixAdapt.c                                                          */
/*  PRINCIPAL AUTHOR      :  Florian Braun                                                        */
/*  SUBSYSTEM NAME        :  consoleAdaption                                                      */
/*  MODULE NAME           :  unixAdapt                                                            */
/*  LANGUAGE              :  C                                                                    */
/*  TARGET ENVIRONMENT    :  ANY (C99)                                                            */
/*  DATE OF FIRST RELEASE :  2015/01/28                                                           */
/*  DESCRIPTION           :  Setup Terminal                                                       */
/**************************************************************************************************/



//INFOS----------------
//Default CMD size(Windows) : 80x25 --> Zahlt /n als zeichen , terminal muss also 81 breit sein oder /n löschen
//Default Terminal size(Unix) : 80x24
//INFOS----------------

#ifdef _WIN32
//No Win32 Code
#else


#include <stdlib.h>
#include <stdio.h>
#include "../../lib/printui/printui.h"


void ConfigureCMD(){


    int width;
    width = getTerminalWidth();
    char buffer[50];
    sprintf(buffer,"resize -s 30 %d",width);
    system(buffer);

}


#endif
