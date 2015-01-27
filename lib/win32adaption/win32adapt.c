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
//Default CMD size(Windows) : 80x25 --> Zahlt /n als zeichen , terminal muss also 81 breit sein oder /n löschen
//Default Terminal size(Unix) : 80x24
//INFOS----------------

#ifdef _WIN32

#include <locale.h>
#include <iostream>
#include <windows.h>

void MakeCmdUtf8Compatible(){

    setlocale(LC_ALL, ".65001");
    system("mode con: cols=81 lines=30");//TODO /n auf windows systemen wieder löschen da sie einen extra buchstaben brauchen und das terminal so 81Zeichen breit sein muss damit nicht 2 umbrüche gemacht werden

    HANDLE outcon = GetStdHandle(STD_OUTPUT_HANDLE);//you don't have to call this function every time

    CONSOLE_FONT_INFOEX font;//CONSOLE_FONT_INFOEX is defined in some windows header
    GetCurrentConsoleFontEx(outcon, false, &font);//PCONSOLE_FONT_INFOEX is the same as CONSOLE_FONT_INFOEX*
    font.FaceName = "Lucida Console";
    SetCurrentConsoleFontEx(outcon, false, &font);


}

#endif
