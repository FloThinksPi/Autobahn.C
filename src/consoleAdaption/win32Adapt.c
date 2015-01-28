/**************************************************************************************************/
/* Copyright (C) mc2lab.com, SSE@USTC, 2014-2015                                                  */
/*                                                                                                */
/*  FILE NAME             :  win32Adapt.C                                                         */
/*  PRINCIPAL AUTHOR      :  Florian Braun                                                        */
/*  SUBSYSTEM NAME        :  consoleAdaption                                                      */
/*  MODULE NAME           :  win32Adapt                                                           */
/*  LANGUAGE              :  C                                                                    */
/*  TARGET ENVIRONMENT    :  ANY (C99)                                                            */
/*  DATE OF FIRST RELEASE :  2015/01/26                                                           */
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
#include <Windows.h>
#include "win32Adapt.h"

//TODO SET Width from printui var


const char *skipParam=" \\CMDknowsUTF8";

void RestartInUtf8Cmd(){

    char buffer[MAX_PATH];//always use MAX_PATH for filepaths
    GetModuleFileName(NULL,buffer,sizeof(buffer));

    char cmd[1024];
    snprintf(cmd, sizeof cmd, "%s%s%s", "start \"Autobahn\" cmd.exe /C ",buffer, skipParam);

    //Einstellungen der autobahn konsole
    system("setlocal enableextensions disabledelayedexpansion");
    system("reg add \"HKCU\\Console\\Autobahn\" /f");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"FaceName\"         /t \"REG_SZ\"     /d \"Lucida Console\"");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"FontFamily\"       /t \"REG_DWORD\"  /d 0x00000036");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"FontSize\"         /t \"REG_DWORD\"  /d 0x00100004");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"FontWeight\"       /t \"REG_DWORD\"  /d 0x00000000");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"QuickEdit\"        /t \"REG_DWORD\"  /d 0x00000000");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"ScreenBufferSize\" /t \"REG_DWORD\"  /d 0x00200040");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"WindowSize\"       /t \"REG_DWORD\"  /d 0x00200040");
    system(cmd);


}

void ConfigureCMD(){

    int width;
    width = getTerminalWidth();
    char buffer[50];
    sprintf(buffer,"mode con: cols=%d lines=30",width);
    system(buffer);
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

}


#endif
