#include <ctype.h>
#include "utils.h"
#include "processing.h"
#include <stdio.h>
#include "../lib/printui/printui.h"
#include <stdlib.h>

#ifdef WIN32

#include <windows.h>

double get_time()
{
    LARGE_INTEGER t, f;
    QueryPerformanceCounter(&t);
    QueryPerformanceFrequency(&f);
    return (double)t.QuadPart/(double)f.QuadPart;
}


const char *skipParam=" \\CMDknowsUTF8";

void RestartInUtf8Cmd(){

    char buffer[MAX_PATH];//always use MAX_PATH for filepaths
    GetModuleFileName(NULL,buffer,sizeof(buffer));

    char *cmd= malloc(sizeof(char)*200);
    sprintf(cmd, "%s%s%s", "start \"Autobahn\" cmd.exe /C ",buffer, skipParam);

    //Einstellungen der autobahn konsole
    system("setlocal enableextensions disabledelayedexpansion");
    system("reg add \"HKCU\\Console\\Autobahn\" /f");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"FaceName\"         /t \"REG_SZ\"     /d \"Consolas\"");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"FontFamily\"       /t \"REG_DWORD\"  /d 0x00000036");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"FontSize\"         /t \"REG_DWORD\"  /d 0x000F0000");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"FontWeight\"       /t \"REG_DWORD\"  /d 0x00000190");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"QuickEdit\"        /t \"REG_DWORD\"  /d 0x00000000");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"WindowSize\"       /t \"REG_DWORD\"  /d 0x0020078");
    system(cmd);

}

void ConfigureCMD(){


    int width;
    width = getTerminalWidth()+1;
    char buffer[50];
    sprintf(buffer,"mode con:cols=%d lines=50",width);
    system(buffer);

    HANDLE StdHandle;
    COORD mycoord;
    mycoord.X=width;
    mycoord.Y=20000;
    StdHandle=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleScreenBufferSize(StdHandle,mycoord);


    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

}

#else

#include <sys/time.h>


void ConfigureCMD(){


    int width;
    width = getTerminalWidth();
    char buffer[50];
    sprintf(buffer,"resize -s 30 %d",width);
    system(buffer);

}

double get_time()
{
    struct timeval t;
    struct timezone tzp;
    gettimeofday(&t, &tzp);
    return t.tv_sec + t.tv_usec*1e-6;
}

#endif


int QsortCompareKM(const void *s1, const void *s2) {
    struct Knoten* K1 = *(struct Knoten **) s1;
    struct Knoten* K2 = *(struct Knoten **) s2;


    return (int) (K1->AutobahnKM - K2->AutobahnKM);
}

int QsortCompareName(const void *s1, const void *s2) {
    struct Knoten* K1 = *(struct Knoten **) s1;
    struct Knoten* K2 = *(struct Knoten **) s2;


    return (int) (K1->AutobahnKM - K2->AutobahnKM);
}

//Zähler muss UTF-8 Konform sein wegen umlauten u.ä ,da funtion in der standartlib dies nichtkann nund umlaute etc. nicht mitzählt
int CountUTF8String(char *s) {
    int i = 0, j = 0;
    while (s[i]) {
        if ((s[i] & 0xc0) != 0x80) j++;
        i++;
    }
    return j;
}

int strcompCaseInsensitive(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }
}

double my_round(double x, unsigned int digits) {
    if (digits > 0) {
        return my_round(x*10.0, digits-1)/10.0;
    }
    else {
        return x;
    }
}