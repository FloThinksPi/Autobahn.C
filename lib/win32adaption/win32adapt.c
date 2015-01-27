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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void RestartInUtf8Cmd(){

    setConsoleCP();
    setConsoleOutputCP();
    system("mode con: cols=81 lines=30");//TODO /n auf windows systemen wieder löschen da sie einen extra buchstaben brauchen und das terminal so 81Zeichen breit sein muss damit nicht 2 umbrüche gemacht werden

    char *befehl = "@echo off"
            "setlocal enableextensions disabledelayedexpansion"
            "set \"consoleName=autobahn.c\""
            ":: http://technet.microsoft.com/en-us/library/cc978570.aspx"
            "(   reg add \"HKCU\\Console\\%consoleName%\" /f"
            "        reg add \"HKCU\\Console\\%consoleName%\" /f /v \"FaceName\"         /t \"REG_SZ\"     /d \"Consolas\""
            "reg add \"HKCU\\Console\\%consoleName%\" /f /v \"FontFamily\"       /t \"REG_DWORD\"  /d 0x00000036"
            "reg add \"HKCU\\Console\\%consoleName%\" /f /v \"FontSize\"         /t \"REG_DWORD\"  /d 0x00080004"
            "reg add \"HKCU\\Console\\%consoleName%\" /f /v \"FontWeight\"       /t \"REG_DWORD\"  /d 0x00000000"
            "reg add \"HKCU\\Console\\%consoleName%\" /f /v \"QuickEdit\"        /t \"REG_DWORD\"  /d 0x00000000"
            "reg add \"HKCU\\Console\\%consoleName%\" /f /v \"ScreenBufferSize\" /t \"REG_DWORD\"  /d 0x00200040"
            "reg add \"HKCU\\Console\\%consoleName%\" /f /v \"WindowSize\"       /t \"REG_DWORD\"  /d 0x00200040"
            ") > nul"
            "start \"%consoleName%\" cmd.exe ";

    system(befehl);

//TODO PFAD ZUR EXE FINDEN UND HINTER CMD:EXE HÄNGEN
}

#endif
